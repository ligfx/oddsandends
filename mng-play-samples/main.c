#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define AL_ERROR(name) do { \
	if (alGetError() != AL_NO_ERROR) { \
		fprintf(stderr, "%s: %d\n", name, alGetError()); \
		exit(1); \
	}\
} while (0)

uint16_t freadu16(FILE *ptr) {
	uint16_t ret;
	fread(&ret, 2, 1, ptr);
	return ret;
}

uint32_t freadu32(FILE *ptr) {
	uint32_t ret;
	fread(&ret, 4, 1, ptr);
	return ret;
}

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

unsigned char* mng_file_get_script(MngFile *mng) { return mng->script; }

void mng_scramble(unsigned char *data, unsigned int length) {
	unsigned char key = 0x5;
	while (length--) {
		*(data++) ^= key;
		key += 0xC1;
	}
}

bool mng_file_init_from_filename(MngFile *mng, char *filename) {
	mng_file_init_and_close(mng, fopen(filename, "rb"));
	return mng;
}

bool mng_file_init_and_close(MngFile *mng, FILE *file) {
	bool ret = mng_file_init(mng, file);
	fclose(file);
	return ret;
}

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
	mng->script = malloc(script_size+1);
	fread(mng->script, 1, script_size, file);
	mng_scramble(mng->script, script_size);
	mng->script[script_size] = '\0';
	
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
void mng_audio_wait(MngAudio *audio, time_t interval) {
	while (!mng_audio_is_done(audio)) {
		sleep(timeout);
	}
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

typedef struct {
	ALCdevice *device;
	ALCcontext *context;
} MngAL;

MngAL mng_al_enter() {
	ALCdevice *device = alcOpenDevice(NULL);
	if (device == NULL) {
		fprintf(stderr, "Couldn't open device\n");
		exit(1);
	}
	ALCcontext *context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	alGetError();
	MngAL al = {.device = device, .context = context};
	return al;
}

void mng_al_exit(MngAL al) {
	alcMakeContextCurrent(NULL);
	alcDestroyContext(al.context);
	alcCloseDevice(al.device);
}

typedef struct {
	MngAudio value;
	MngAudioStack *next;
} MngAudioStack;

void mng_audio_stack_new() { return NULL; }
MngAudioStack* mng_audio_stack_push(MngAudioStack *self, MngAudio value) {
	MngAudioStack *push = malloc(sizeof(*push));
	push->value = value;
	push->next = self;
	return push;
}
bool mng_audio_stack_empty(MngAudioStack *self) {
	return self == NULL;
}
MngAudio mng_audio_stack_peek(MngAudioStack *self) {
	return self->value;
}
MngAudioStack* mng_audio_stack_pop(MngAudioStack *self) {
	MngAudioStack *next = self->next;
	free(self);
	return next;
}
MngAudioStack* mng_audio_stack_next(MngAudioStack *self) {
	return self->next;
}

typedef struct {
	MngAudioStack *stack;
} MngAudioPool;

void mng_audio_pool_init(MngAudioPool *self) {
	self->stack = mng_audio_stack_new();
}
void mng_audio_pool_add(MngAudioPool *self, MngAudio audio) {
	self->stack = mng_audio_stack_push(self->stack, audio);
}
void mng_audio_pool_wait(MngAudioPool *self, time_t interval) {
	MngAudioStack *stack = self->stack;
	while (stack != NULL) {
		mng_audio_wait(stack->value, interval);
		stack = mng_audio_stack_next(stack);
	}
}
void mng_audio_pool_clear(MngAudioPool *self) {
	MngAudioStack *stack = self->stack;
	while (stack != NULL) {
		mng_audio_destroy(&stack->value);
		stack = mng_audio_stack_pop(stack);
	}
}

int main (int argc, char **argv) {
	MngAL al = mng_al_enter();
	
	assert(argc > 1);
	MngFile mng;
	mng_file_init_from_filename(&mng, argv[1]);
	
	printf("%s\n", mng.script);
	
	MngAudio
		uno = mng_audio_al(mng.samples[0]),
		dos = mng_audio_al(mng.samples[1]),
		tres = mng_audio_al(mng.samples[2]);
	
	MngAudioPool pool;
	mng_audio_pool_init(pool);
	mng_audio_pool_add(pool, uno);
	mng_audio_pool_add(pool, dos);
	mng_audio_pool_add(pool, tres);

	mng_audio_set_pan(dos, 1.0);
	mng_audio_set_pan(uno, -1.0);
	
	mng_audio_play(uno);
	sleep(0.5);
	mng_audio_play(dos);
	sleep(2);
	mng_audio_play(tres);

	mng_audio_pool_wait(pool);
	mng_audio_pool_clear(pool);
	
	mng_file_destroy(&mng);

	mng_al_exit(al);
}
