#include "audio_engine.hpp"
#include "game_session.hpp"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

using namespace independent_fighter;

#define CHECK(...) \
    do { \
        if (!(__VA_ARGS__)) { \
            throw std::runtime_error("check failed: " #__VA_ARGS__); \
        } \
    } while (false)

void write_u16(std::ostream& output, std::uint16_t value)
{
    const char bytes[]{static_cast<char>(value & 0xffU),
        static_cast<char>((value >> 8U) & 0xffU)};
    output.write(bytes, 2);
}

void write_u32(std::ostream& output, std::uint32_t value)
{
    const char bytes[]{
        static_cast<char>(value & 0xffU),
        static_cast<char>((value >> 8U) & 0xffU),
        static_cast<char>((value >> 16U) & 0xffU),
        static_cast<char>((value >> 24U) & 0xffU)
    };
    output.write(bytes, 4);
}

struct TemporaryWav {
    std::filesystem::path path;

    ~TemporaryWav()
    {
        std::error_code error;
        std::filesystem::remove(path, error);
    }
};

TemporaryWav write_test_wav()
{
    const auto path = std::filesystem::temp_directory_path()
        / "independent_fighter_phase15_test.wav";
    constexpr std::uint16_t channels = 1U;
    constexpr std::uint32_t sample_rate = 48000U;
    constexpr std::uint16_t block_align = 2U;
    constexpr std::uint16_t bits_per_sample = 16U;
    constexpr std::int16_t sample_value = 10000;
    constexpr std::uint32_t data_size = 256U * 2U;
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("could not create temporary WAV");
    }
    output.write("RIFF", 4);
    write_u32(output, 36U + data_size);
    output.write("WAVEfmt ", 8);
    write_u32(output, 16U);
    write_u16(output, 1U);
    write_u16(output, channels);
    write_u32(output, sample_rate);
    write_u32(output, sample_rate * block_align);
    write_u16(output, block_align);
    write_u16(output, bits_per_sample);
    output.write("data", 4);
    write_u32(output, data_size);
    const std::uint16_t bits = std::bit_cast<std::uint16_t>(sample_value);
    for (std::uint32_t index = 0; index < data_size / 2U; ++index) {
        write_u16(output, bits);
    }
    output.close();
    return TemporaryWav{path};
}

void test_audio_initialization_and_placeholder_effects()
{
    auto sink = std::make_unique<NullAudioOutput>();
    NullAudioOutput* sink_view = sink.get();
    AudioEngine engine{AudioConfig{48000U, 2U, 4U}, std::move(sink)};
    CHECK(engine.initialize());
    CHECK(engine.initialized());
    CHECK(engine.effect_channel_count() == 4U);
    CHECK(engine.resources().size() == 15U);
    CHECK(engine.music_playing());
    CHECK(engine.play_effect(AudioCue::Punch));
    const std::vector<std::int16_t> block = engine.mix_frames(128U);
    CHECK(block.size() == 256U);
    CHECK(std::any_of(block.begin(), block.end(), [](std::int16_t value) {
        return value != 0;
    }));
    CHECK(engine.render_frames(64U));
    CHECK(sink_view->submitted_frames() == 64U);
    CHECK(sink_view->submitted_blocks() == 1U);
}

void test_wav_loading_and_volume_buses()
{
    const TemporaryWav file = write_test_wav();
    AudioEngine engine{};
    CHECK(engine.initialize());
    const AudioHandle clip = engine.resources().load_wav_pcm16(file.path);
    CHECK(engine.resources().clip(clip) != nullptr);
    CHECK(engine.resources().clip(clip)->origin == AudioContentOrigin::LoadedPcmFile);
    CHECK(engine.resources().clip(clip)->frame_count() == 256U);
    CHECK(engine.register_effect(AudioCue::Punch, clip));
    engine.set_music_volume(0.0F);
    engine.set_effect_volume(0.5F);
    engine.set_master_volume(0.5F);
    CHECK(engine.play_effect(AudioCue::Punch));
    const std::vector<std::int16_t> block = engine.mix_frames(1U);
    CHECK(block[0] == 2500);
    CHECK(block[1] == 2500);

    engine.set_master_volume(0.0F);
    CHECK(engine.play_effect(AudioCue::Punch));
    const std::vector<std::int16_t> silent = engine.mix_frames(1U);
    CHECK(silent[0] == 0 && silent[1] == 0);
}

void test_simultaneous_effect_channels_and_clipping()
{
    AudioEngine engine{AudioConfig{48000U, 2U, 2U}};
    CHECK(engine.initialize());
    CHECK(engine.play_effect(AudioCue::Punch));
    CHECK(engine.play_effect(AudioCue::Kick));
    CHECK(engine.active_effect_channels() == 2U);
    CHECK(engine.play_effect(AudioCue::SpecialAttack));
    CHECK(engine.active_effect_channels() == 2U);
    engine.set_music_volume(0.0F);
    const std::vector<std::int16_t> block = engine.mix_frames(512U);
    for (const std::int16_t sample : block) {
        CHECK(sample >= std::numeric_limits<std::int16_t>::min());
        CHECK(sample <= std::numeric_limits<std::int16_t>::max());
    }
}

void test_music_transport_volume_and_resources()
{
    AudioEngine engine{};
    CHECK(engine.initialize());
    const AudioHandle music = engine.resources().create_music_placeholder(
        "test-music", 48000U, false);
    CHECK(engine.play_music(music, true));
    CHECK(engine.music_playing());
    CHECK(engine.music_handle() == music);
    engine.pause_music();
    CHECK(engine.music_paused());
    const std::vector<std::int16_t> paused = engine.mix_frames(32U);
    CHECK(std::all_of(paused.begin(), paused.end(), [](std::int16_t value) {
        return value == 0;
    }));
    engine.resume_music();
    CHECK(!engine.music_paused());
    engine.set_music_volume(0.0F);
    const std::vector<std::int16_t> muted = engine.mix_frames(32U);
    CHECK(std::all_of(muted.begin(), muted.end(), [](std::int16_t value) {
        return value == 0;
    }));
    engine.set_music_volume(1.0F);
    CHECK(engine.unload_clip(music));
    CHECK(!engine.music_playing());
    CHECK(engine.resources().clip(music) == nullptr);
    engine.stop_music();
    CHECK(!engine.music_playing());
}

void test_game_state_and_gameplay_event_routing()
{
    GameSession session{};
    AudioEngine engine{};
    CHECK(engine.initialize());
    const AudioHandle title_track = engine.music_handle();

    FrameInput input{};
    input.confirm = true;
    session.process_frame(input);
    auto events = session.drain_events();
    engine.consume_gameplay_events(events);
    CHECK(engine.music_handle() == title_track);

    input = {};
    input.player_one.confirm = true;
    input.player_two.confirm = true;
    session.process_frame(input);
    events = session.drain_events();
    engine.consume_gameplay_events(events);
    const AudioHandle fight_track = engine.music_handle();
    CHECK(fight_track != title_track);

    input = {};
    input.confirm = true;
    session.process_frame(input);
    events = session.drain_events();
    engine.consume_gameplay_events(events);
    CHECK(engine.music_handle() == fight_track);

    input = {};
    input.player_one.jump = true;
    session.process_frame(input);
    events = session.drain_events();
    engine.consume_gameplay_events(events);
    CHECK(engine.active_effect_channels() > 0U);

    session.apply_damage(PlayerId::Two, session.player(PlayerId::Two).health);
    events = session.drain_events();
    engine.consume_gameplay_events(events);
    CHECK(engine.music_handle() != fight_track);
}

void test_music_state_mapping_and_pause()
{
    AudioEngine engine{};
    CHECK(engine.initialize());
    const AudioHandle title_track = engine.music_handle();
    const AudioHandle alternate = engine.resources().create_music_placeholder(
        "alternate-title", 48000U, true);
    CHECK(engine.register_state_music(GameState::CharacterSelect, alternate));
    const GameplayEvent selection{GameplayEventType::StateChanged,
        std::nullopt, GameState::CharacterSelect};
    engine.consume_gameplay_events(std::span<const GameplayEvent>(&selection, 1U));
    CHECK(engine.music_handle() == alternate);
    CHECK(engine.music_handle() != title_track);

    const GameplayEvent pause{GameplayEventType::StateChanged,
        std::nullopt, GameState::Paused};
    engine.consume_gameplay_events(std::span<const GameplayEvent>(&pause, 1U));
    CHECK(engine.music_paused());
    const GameplayEvent resume{GameplayEventType::StateChanged,
        std::nullopt, GameState::Fighting};
    engine.consume_gameplay_events(std::span<const GameplayEvent>(&resume, 1U));
    CHECK(!engine.music_paused());
}

void test_shutdown_releases_resources_and_output()
{
    auto sink = std::make_unique<NullAudioOutput>();
    NullAudioOutput* sink_view = sink.get();
    AudioEngine engine{AudioConfig{}, std::move(sink)};
    CHECK(engine.initialize());
    CHECK(engine.resources().size() > 0U);
    engine.render_frames(64U);
    CHECK(sink_view->submitted_frames() == 64U);
    engine.shutdown();
    CHECK(!engine.initialized());
    CHECK(engine.resources().size() == 0U);
    CHECK(engine.effect_channel_count() == 0U);
    CHECK(!engine.music_playing());
    CHECK(sink_view->last_block().empty());
}

void test_wav_file_output_round_trip()
{
    const auto path = std::filesystem::temp_directory_path()
        / "independent_fighter_phase15_output.wav";
    const TemporaryWav file{path};
    auto output = std::make_unique<WavFileAudioOutput>(file.path);
    AudioEngine engine{AudioConfig{48000U, 2U, 4U}, std::move(output)};
    CHECK(engine.initialize());
    CHECK(engine.render_frames(256U));
    engine.shutdown();
    CHECK(std::filesystem::file_size(file.path) == 44U + 256U * 2U * 2U);

    AudioResourceManager verification{};
    const AudioHandle rendered = verification.load_wav_pcm16(file.path);
    const AudioClip* clip = verification.clip(rendered);
    CHECK(clip != nullptr);
    CHECK(clip->sample_rate == 48000U);
    CHECK(clip->channels == 2U);
    CHECK(clip->frame_count() == 256U);
}

struct TestCase {
    std::string_view name;
    void (*run)();
};

} // namespace

int main()
{
    constexpr TestCase tests[]{
        {"audio initialization and procedural cues", test_audio_initialization_and_placeholder_effects},
        {"WAV loading and volume buses", test_wav_loading_and_volume_buses},
        {"simultaneous effects and clipping", test_simultaneous_effect_channels_and_clipping},
        {"music transport and resource release", test_music_transport_volume_and_resources},
        {"gameplay event routing", test_game_state_and_gameplay_event_routing},
        {"music state mapping and pause", test_music_state_mapping_and_pause},
        {"shutdown releases resources", test_shutdown_releases_resources_and_output},
        {"WAV output round trip", test_wav_file_output_round_trip}
    };

    int passed = 0;
    for (const TestCase& test : tests) {
        try {
            test.run();
            ++passed;
            std::cout << "PASS " << test.name << '\n';
        } catch (const std::exception& error) {
            std::cerr << "FAIL " << test.name << ": " << error.what() << '\n';
            return 1;
        }
    }

    std::cout << passed << " audio tests passed\n";
    return 0;
}
