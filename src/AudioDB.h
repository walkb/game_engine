#pragma once
#include "AudioHelper.h"
#include "SDL2_mixer/SDL_mixer.h"
#include <unordered_map>
#include <string>
class AudioDB
{
public:
	inline static std::unordered_map<std::string, Mix_Chunk*> audios;
	inline static std::string gameplay_audio = "";
	inline static std::string loss_audio = "";
	inline static std::string win_audio = "";
	inline static std::string player_score_sfx = "";
	inline static bool hasAudio;
	inline static Mix_Chunk* chunk;

	// Allocates 0-49 channels, opens audio, and stores all sound bytes
	static void Initialize();

	// --- [LUA FUNCTION] ---

	// Plays the specified clip on specified channel with optional loop
	static void Play(int channel, std::string clip_name, bool does_loop);

	// Stops sound on channel [channel]
	static void Halt(int channel);

	// Sets volume on channel to [volume], casted to int from 0, 128
	static void SetVolume(int channel, float volume);
};

