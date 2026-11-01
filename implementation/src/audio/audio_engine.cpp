#include "audio_engine.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace independent_fighter {
namespace {

struct CueDefinition {
    AudioCue cue;
    const char* key;
    double frequency_hz;
    std::uint32_t duration_ms;
};

constexpr CueDefinition cue_definitions[]{
    {AudioCue::Hit, "hit", 95.0, 90U},
    {AudioCue::Punch, "punch", 190.0, 75U},
    {AudioCue::Kick, "kick", 140.0, 110U},
    {AudioCue::Jump, "jump", 440.0, 130U},
    {AudioCue::Landing, "landing", 75.0, 100U},
    {AudioCue::SpecialAttack, "special", 330.0, 240U},
    {AudioCue::Block, "block", 260.0, 80U},
    {AudioCue::RoundStart, "round-start", 600.0, 300U},
    {AudioCue::RoundWin, "round-win", 500.0, 220U},
    {AudioCue::RoundLoss, "round-loss", 125.0, 220U},
    {AudioCue::Victory, "victory", 523.25, 420U},
    {AudioCue::Defeat, "defeat", 110.0, 420U}
};

} // namespace

AudioEngine::AudioEngine(AudioConfig config, std::unique_ptr<AudioOutput> output)
    : config_(config),
      output_(output ? std::move(output) : std::make_unique<NullAudioOutput>())
{
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

bool AudioEngine::initialize()
{
    if (initialized_) {
        return true;
    }
    if (config_.sample_rate == 0U || config_.output_channels != 2U
        || config_.effect_channels == 0U || output_ == nullptr
        || !output_->initialize(config_.sample_rate, config_.output_channels)) {
        return false;
    }

    effect_voices_.assign(config_.effect_channels, Voice{});
    initialized_ = true;
    install_procedural_placeholders();
    return true;
}

void AudioEngine::shutdown() noexcept
{
    if (output_ != nullptr) {
        output_->shutdown();
    }
    effect_voices_.clear();
    effects_.clear();
    state_music_.clear();
    music_voice_ = {};
    resources_.clear();
    next_effect_channel_ = 0U;
    initialized_ = false;
}

bool AudioEngine::initialized() const noexcept
{
    return initialized_;
}

AudioResourceManager& AudioEngine::resources() noexcept
{
    return resources_;
}

bool AudioEngine::unload_clip(AudioHandle handle) noexcept
{
    for (Voice& voice : effect_voices_) {
        if (voice.clip == handle) {
            voice = {};
        }
    }
    if (music_voice_.clip == handle) {
        music_voice_ = {};
    }
    for (auto entry = effects_.begin(); entry != effects_.end();) {
        if (entry->second == handle) {
            entry = effects_.erase(entry);
        } else {
            ++entry;
        }
    }
    for (auto entry = state_music_.begin(); entry != state_music_.end();) {
        if (entry->second == handle) {
            entry = state_music_.erase(entry);
        } else {
            ++entry;
        }
    }
    return resources_.unload(handle);
}

bool AudioEngine::register_effect(AudioCue cue, AudioHandle handle)
{
    if (resources_.clip(handle) == nullptr) {
        return false;
    }
    effects_.insert_or_assign(cue, handle);
    return true;
}

bool AudioEngine::register_state_music(GameState state, AudioHandle handle)
{
    if (resources_.clip(handle) == nullptr) {
        return false;
    }
    state_music_.insert_or_assign(state, handle);
    return true;
}

bool AudioEngine::play_effect(AudioCue cue, float volume)
{
    if (!initialized_ || effect_voices_.empty()) {
        return false;
    }
    const auto effect = effects_.find(cue);
    if (effect == effects_.end() || resources_.clip(effect->second) == nullptr) {
        return false;
    }

    auto channel = std::find_if(effect_voices_.begin(), effect_voices_.end(),
        [](const Voice& voice) { return !voice.active; });
    if (channel == effect_voices_.end()) {
        channel = effect_voices_.begin()
            + static_cast<std::ptrdiff_t>(next_effect_channel_ % effect_voices_.size());
        next_effect_channel_ = (next_effect_channel_ + 1U) % effect_voices_.size();
    }
    *channel = Voice{effect->second, 0.0, normalized_volume(volume), false, true, false};
    return true;
}

bool AudioEngine::play_music(AudioHandle handle, bool looping)
{
    if (!initialized_ || resources_.clip(handle) == nullptr) {
        return false;
    }
    if (music_voice_.active && music_voice_.clip == handle) {
        music_voice_.looping = looping;
        music_voice_.paused = false;
        return true;
    }
    music_voice_ = Voice{handle, 0.0, 1.0F, looping, true, false};
    return true;
}

void AudioEngine::stop_music() noexcept
{
    music_voice_ = {};
}

void AudioEngine::pause_music() noexcept
{
    if (music_voice_.active) {
        music_voice_.paused = true;
    }
}

void AudioEngine::resume_music() noexcept
{
    if (music_voice_.active) {
        music_voice_.paused = false;
    }
}

void AudioEngine::set_master_volume(float volume) noexcept
{
    master_volume_ = normalized_volume(volume);
}

void AudioEngine::set_effect_volume(float volume) noexcept
{
    effect_volume_ = normalized_volume(volume);
}

void AudioEngine::set_music_volume(float volume) noexcept
{
    music_volume_ = normalized_volume(volume);
}

void AudioEngine::consume_gameplay_events(std::span<const GameplayEvent> events)
{
    for (const GameplayEvent& event : events) {
        switch (event.type) {
        case GameplayEventType::StateChanged:
            handle_state_change(event.state);
            break;
        case GameplayEventType::JumpStarted:
            play_effect(AudioCue::Jump);
            break;
        case GameplayEventType::Landed:
            play_effect(AudioCue::Landing);
            break;
        case GameplayEventType::HitConfirmed:
            play_effect(AudioCue::Hit);
            break;
        case GameplayEventType::RoundStarted:
            play_effect(AudioCue::RoundStart);
            break;
        case GameplayEventType::RoundWon:
            play_effect(AudioCue::RoundWin);
            break;
        case GameplayEventType::RoundLost:
            play_effect(AudioCue::RoundLoss);
            break;
        case GameplayEventType::CharacterSelected:
        case GameplayEventType::GameOver:
            break;
        }
    }
}

std::vector<std::int16_t> AudioEngine::mix_frames(std::size_t frame_count)
{
    if (frame_count > std::numeric_limits<std::size_t>::max() / config_.output_channels) {
        throw std::length_error("audio block is too large");
    }
    std::vector<std::int16_t> output(frame_count * config_.output_channels, 0);
    if (!initialized_) {
        return output;
    }

    for (std::size_t frame = 0; frame < frame_count; ++frame) {
        double left = 0.0;
        double right = 0.0;
        mix_voice(music_voice_, music_volume_, left, right);
        for (Voice& voice : effect_voices_) {
            mix_voice(voice, effect_volume_, left, right);
        }
        left *= master_volume_;
        right *= master_volume_;
        const double lower = static_cast<double>(std::numeric_limits<std::int16_t>::min());
        const double upper = static_cast<double>(std::numeric_limits<std::int16_t>::max());
        output[frame * 2U] = static_cast<std::int16_t>(std::lround(std::clamp(left, lower, upper)));
        output[frame * 2U + 1U] = static_cast<std::int16_t>(std::lround(std::clamp(right, lower, upper)));
    }
    return output;
}

bool AudioEngine::render_frames(std::size_t frame_count)
{
    if (!initialized_) {
        return false;
    }
    const std::vector<std::int16_t> block = mix_frames(frame_count);
    output_->submit(block);
    return true;
}

bool AudioEngine::music_playing() const noexcept
{
    return music_voice_.active;
}

bool AudioEngine::music_paused() const noexcept
{
    return music_voice_.active && music_voice_.paused;
}

AudioHandle AudioEngine::music_handle() const noexcept
{
    return music_voice_.clip;
}

std::size_t AudioEngine::active_effect_channels() const noexcept
{
    return static_cast<std::size_t>(std::count_if(effect_voices_.begin(), effect_voices_.end(),
        [](const Voice& voice) { return voice.active; }));
}

std::size_t AudioEngine::effect_channel_count() const noexcept
{
    return effect_voices_.size();
}

std::uint32_t AudioEngine::sample_rate() const noexcept
{
    return config_.sample_rate;
}

std::uint16_t AudioEngine::output_channels() const noexcept
{
    return config_.output_channels;
}

const AudioOutput& AudioEngine::output() const noexcept
{
    return *output_;
}

float AudioEngine::normalized_volume(float volume) noexcept
{
    if (!std::isfinite(volume)) {
        return 0.0F;
    }
    return std::clamp(volume, 0.0F, 1.0F);
}

void AudioEngine::install_procedural_placeholders()
{
    for (const CueDefinition& definition : cue_definitions) {
        const AudioHandle handle = resources_.create_tone(
            definition.key, config_.sample_rate, definition.frequency_hz,
            definition.duration_ms);
        effects_.insert_or_assign(definition.cue, handle);
    }

    const AudioHandle title = resources_.create_music_placeholder(
        "training-title", config_.sample_rate, false);
    const AudioHandle fight = resources_.create_music_placeholder(
        "training-fight", config_.sample_rate, true);
    const AudioHandle result = resources_.create_music_placeholder(
        "training-result", config_.sample_rate, false);
    state_music_.insert_or_assign(GameState::Title, title);
    state_music_.insert_or_assign(GameState::CharacterSelect, title);
    state_music_.insert_or_assign(GameState::FightIntro, fight);
    state_music_.insert_or_assign(GameState::Fighting, fight);
    state_music_.insert_or_assign(GameState::RoundResult, result);
    state_music_.insert_or_assign(GameState::Victory, result);
    state_music_.insert_or_assign(GameState::Defeat, result);
    state_music_.insert_or_assign(GameState::GameOver, title);
    play_music(title, true);
}

void AudioEngine::handle_state_change(GameState state)
{
    if (state == GameState::Paused) {
        pause_music();
        return;
    }
    if (state == GameState::Victory) {
        play_effect(AudioCue::Victory);
    } else if (state == GameState::Defeat) {
        play_effect(AudioCue::Defeat);
    }

    const auto track = state_music_.find(state);
    if (track == state_music_.end()) {
        stop_music();
        return;
    }
    if (music_voice_.active && music_voice_.clip == track->second) {
        resume_music();
        return;
    }
    play_music(track->second, true);
}

void AudioEngine::mix_voice(
    Voice& voice,
    float bus_volume,
    double& left,
    double& right)
{
    if (!voice.active || voice.paused) {
        return;
    }
    const AudioClip* source = resources_.clip(voice.clip);
    if (source == nullptr || !source->valid()) {
        voice = {};
        return;
    }

    const std::size_t source_frames = source->frame_count();
    if (voice.cursor >= static_cast<double>(source_frames)) {
        if (!voice.looping) {
            voice.active = false;
            return;
        }
        voice.cursor = std::fmod(voice.cursor, static_cast<double>(source_frames));
    }
    const std::size_t source_frame = static_cast<std::size_t>(voice.cursor);
    const float gain = voice.volume * bus_volume;
    const std::size_t source_offset = source_frame * source->channels;
    const double sample_left = static_cast<double>(source->interleaved_samples[source_offset]);
    const double sample_right = source->channels == 1U
        ? sample_left
        : static_cast<double>(source->interleaved_samples[source_offset + 1U]);
    left += sample_left * static_cast<double>(gain);
    right += sample_right * static_cast<double>(gain);
    voice.cursor += static_cast<double>(source->sample_rate)
        / static_cast<double>(config_.sample_rate);
    if (!voice.looping && voice.cursor >= static_cast<double>(source_frames)) {
        voice.active = false;
    }
}

} // namespace independent_fighter
