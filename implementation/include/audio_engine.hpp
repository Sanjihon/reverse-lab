#pragma once

#include "audio_output.hpp"
#include "audio_resource_manager.hpp"
#include "game_types.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <unordered_map>
#include <vector>

namespace independent_fighter {

struct AudioConfig {
    std::uint32_t sample_rate{48000U};
    std::uint16_t output_channels{2U};
    std::size_t effect_channels{8U};
};

class AudioEngine {
public:
    explicit AudioEngine(
        AudioConfig config = {},
        std::unique_ptr<AudioOutput> output = {});
    ~AudioEngine();

    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    bool initialize();
    void shutdown() noexcept;
    bool initialized() const noexcept;

    AudioResourceManager& resources() noexcept;
    bool unload_clip(AudioHandle handle) noexcept;
    bool register_effect(AudioCue cue, AudioHandle handle);
    bool register_state_music(GameState state, AudioHandle handle);
    bool play_effect(AudioCue cue, float volume = 1.0F);
    bool play_music(AudioHandle handle, bool looping = true);
    void stop_music() noexcept;
    void pause_music() noexcept;
    void resume_music() noexcept;
    void set_master_volume(float volume) noexcept;
    void set_effect_volume(float volume) noexcept;
    void set_music_volume(float volume) noexcept;
    void consume_gameplay_events(std::span<const GameplayEvent> events);

    std::vector<std::int16_t> mix_frames(std::size_t frame_count);
    bool render_frames(std::size_t frame_count);
    bool music_playing() const noexcept;
    bool music_paused() const noexcept;
    AudioHandle music_handle() const noexcept;
    std::size_t active_effect_channels() const noexcept;
    std::size_t effect_channel_count() const noexcept;
    std::uint32_t sample_rate() const noexcept;
    std::uint16_t output_channels() const noexcept;
    const AudioOutput& output() const noexcept;

private:
    struct Voice {
        AudioHandle clip{};
        double cursor{};
        float volume{1.0F};
        bool looping{};
        bool active{};
        bool paused{};
    };

    static float normalized_volume(float volume) noexcept;
    void install_procedural_placeholders();
    void handle_state_change(GameState state);
    void mix_voice(
        Voice& voice,
        float bus_volume,
        double& left,
        double& right);

    AudioConfig config_{};
    std::unique_ptr<AudioOutput> output_{};
    AudioResourceManager resources_{};
    std::unordered_map<AudioCue, AudioHandle> effects_{};
    std::unordered_map<GameState, AudioHandle> state_music_{};
    std::vector<Voice> effect_voices_{};
    Voice music_voice_{};
    std::size_t next_effect_channel_{};
    float master_volume_{1.0F};
    float effect_volume_{1.0F};
    float music_volume_{1.0F};
    bool initialized_{};
};

} // namespace independent_fighter
