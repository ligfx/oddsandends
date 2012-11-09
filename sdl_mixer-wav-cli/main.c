#include <SDL/SDL.h>
#include <SDL_mixer/SDL_mixer.h>

#include <stdio.h>

#undef main

int main (int argc, char **argv) {
	if (SDL_Init(SDL_INIT_AUDIO) == -1) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(-1);
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}
	
	Mix_Chunk *sample;
	sample = Mix_LoadWAV(argv[1]);
	if (!sample) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
		exit(3);
	}

	if (Mix_PlayChannel(-1, sample, 0) == -1) {
		fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		exit(4);
	}

	while (1) {
		int channels_playing = Mix_Playing(-1);
		if (channels_playing <= 0) exit(0);
		sleep(1);
	}
}
