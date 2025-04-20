#include "AudioDB.h"
#include "EngineUtils.h"
#include <string>

void AudioDB::Initialize() {
	// search for title_music.wav or title_music.ogg
	const std::string audio_directory_path = "resources/audio/";
	if (EngineUtils::CheckPathExists(audio_directory_path)) {
		AudioHelper::Mix_AllocateChannels(50);
		AudioHelper::Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 2048);
		for (const auto& audio : std::filesystem::directory_iterator(audio_directory_path)) {
			std::string name = audio.path().filename().stem().string();
			std::string extension = audio.path().filename().extension().string();
			if (extension == ".ogg" || extension == ".wav") {
				std::string audio_location = audio_directory_path + name + extension;
				audios[name] = AudioHelper::Mix_LoadWAV(audio_location.c_str());
			}
		}
	}
}

// ----- [ LUA FUNCTIONS ] ------

void AudioDB::Play(int channel, std::string clip_name, bool does_loop) {
	if (audios.find(clip_name) == audios.end()) {
		std::cout << "error: failed to play audio clip " << clip_name;
		exit(0);
	}
	Mix_Chunk* chunk = audios[clip_name];
	int loop = does_loop ? -1 : 0;
	if (AudioHelper::Mix_PlayChannel(channel, chunk, loop) == -1) {
		std::cout << "error: failed to play to channel";
		exit(0);
	}
}

void AudioDB::Halt(int channel) {
	AudioHelper::Mix_HaltChannel(channel);
}

void AudioDB::SetVolume(int channel, float volume) {
	int vol = static_cast<int>(volume);
	vol = vol < 0 ? 0 : vol;
	vol = vol > 128 ? 128 : vol;
	AudioHelper::Mix_Volume(channel, vol);
}