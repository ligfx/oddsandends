#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	uint16_t channels;
	uint16_t bits;
	uint32_t sample_rate;
	void *data;
	uint32_t size;
} MngSample;

typedef struct {
	uint32_t position, length;
} MngSamplePointer;

typedef struct {
	char *script;
	MngSample *samples;
	size_t samples_count;
} MngFile;

void mng_scramble(unsigned char *data, unsigned int length);

bool mng_file_init(MngFile *mng, FILE *file) {
	mng->samples_count = freadu32(file);
	uint32_t
		script_position = freadu32(file),
		script_size = freadu32(file);
	
	MngSamplePointer *pointers = malloc(sizeof((*pointers)) * mng->samples_count);
	for (uint32_t i = 0; i < mng->samples_count; i++) {
		pointers[i].position = freadu32(file);
		pointers[i].length = freadu32(file);
	}
	
	fseek(file, script_position, SEEK_SET);
	mng->script = malloc(script_size);
	fread(mng->script, 1, script_size, file);
	mng_scramble(mng->script, script_size);
	
	mng->samples = malloc(sizeof(*(mng->samples)) * mng->samples_count);
	memset(mng->samples, 0, sizeof(*(mng->samples)) * mng->samples_count);
	for (uint32_t i = 0; i < mng->samples_count; i++) {
		fseek(file, pointers[i].position, SEEK_SET);
		
		MngSample *sample = &(mng->samples[i]);
		
		uint32_t fmtchunk_size = freadu32(file);
		uint16_t audio_format = freadu16(file);
		assert(audio_format == 1);
		sample->channels = freadu16(file);
		sample->sample_rate = freadu32(file);
		uint32_t byte_rate = freadu32(file);
		uint16_t block_align = freadu16(file);
		sample->bits = freadu16(file);
		
		char buf[5] = {0, 0, 0, 0, 0};
		fread(buf, 4, 1, file);
		assert(strcmp(buf, "data") == 0);
		sample->size = freadu32(file);
		
		sample->data = malloc(sample->size);
		fread(sample->data, 1, sample->size, file);
	}
	
}

void mng_file_destroy(MngFile *mng) {
	free(mng->script); mng->script = NULL;	
	for (int i = 0; i < mng->samples_count; i++) {
		free(mng->samples[i].data); mng->samples[i].data = NULL;
	}
	free(mng->samples); mng->samples = NULL;
}

typedef struct {
	void *data;
	void (*play)(void*);
	void (*set_pan)(void*, float);
	bool (*is_done)(void*);
	void (*destroy)(void*);
} MngAudio;
void mng_audio_play(MngAudio iface) {
	(*iface.play)(iface.data);
}
void mng_audio_set_pan(MngAudio iface, float pan) {
	(*iface.set_pan)(iface.data, pan);
}
bool mng_audio_is_done(MngAudio iface) {
	return (*iface.is_done)(iface.data);
}
void mng_audio_destroy(MngAudio *iface) {
	(*iface->destroy)(iface->destroy);
	free(iface->data);
	iface->data = NULL;
}

typedef struct {
	ALuint buffer;
	ALuint source;
} MngAudioAL;
void mng_audio_al_play(MngAudioAL *al) {
	alSourcePlay(al->source);
}
void mng_audio_al_set_pan(MngAudioAL *al, float pan) {
	ALfloat position[] = { pan, 0.0, 0.0 };
	alSourcefv(al->source, AL_POSITION, position);
	AL_ERROR("alSourcefv");
}
bool mng_audio_al_is_done(MngAudioAL *al) {
	ALint val;
	alGetSourcei(al->source, AL_SOURCE_STATE, &val);
	return val != AL_PLAYING;
}
void mng_audio_al_destroy(MngAudioAL *al) {
	alDeleteSources(1, &al->source);
	alDeleteBuffers(1, &al->buffer);
}
MngAudio mng_audio_al (MngSample sample) {
	ALenum format = 0;
	if (sample.bits == 8) {
		if (sample.channels == 1) format = AL_FORMAT_MONO8;
		if (sample.channels == 2) format = AL_FORMAT_STEREO8;
	}
	if (sample.bits == 16) {
		if (sample.channels == 1) format = AL_FORMAT_MONO16;
		if (sample.channels == 2) format = AL_FORMAT_STEREO16;
	}
	if (!format) {
		fprintf(stderr, "Incompatible format (%d, %d)\n", sample.channels, sample.bits);
		exit(1);
	}
	
	MngAudioAL *al = malloc(sizeof(*al));	
	alGenBuffers(1, &al->buffer);
	AL_ERROR("alGenBuffer");
	alBufferData(al->buffer, format, sample.data, sample.size, sample.sample_rate);
	alGenSources(1, &al->source);
	AL_ERROR("alGenSource");
	alSourceQueueBuffers(al->source, 1, &al->buffer);
	
	MngAudio interface = {
		.data = al,
		.play = &mng_audio_al_play,
		.set_pan = &mng_audio_al_set_pan,
		.is_done = &mng_audio_al_is_done,
		.destroy = &mng_audio_al_destroy,
	};
	return interface;
}