#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <stdio.h>

#define AL_ERROR(name) do { \
	if (alGetError() != AL_NO_ERROR) { \
		fprintf(stderr, "%s: %d\n", name, alGetError()); \
		exit(1); \
	}\
} while (0)

ALuint alGenSource() { 
	ALuint source;
	alGenSources(1, &source);
	return source;
}

ALuint alGenBuffer() { 
	ALuint buffer;
	alGenBuffers(1, &buffer);
	return buffer;
}

int main (int argc, char **argv) {
	ALCdevice *device = alcOpenDevice(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	if (device == NULL) {
		fprintf(stderr, "Couldn't open device\n");
		exit(1);
	}
	
	ALCcontext *context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	
	alGetError();
	
	ALenum format;
	ALsizei size;
	ALsizei freq;
	ALboolean loop;
	ALvoid* data;
	alutLoadWAVFile(argv[1], &format, &data, &size, &freq, &loop);
	AL_ERROR("alutLoadWAVFile");

	ALuint buffer = alGenBuffer();
	AL_ERROR("alGenBuffer");
	
	alBufferData(buffer, format, data, size, freq);
	AL_ERROR("alBufferData");
	
	alutUnloadWAV(format, data, size, freq);
	AL_ERROR("alutUnloadWAV");
	
	ALuint source = alGenSource();
	AL_ERROR("alGenSource");
	
	alSourcei(source, AL_BUFFER, buffer);
	alSourcei(source, AL_LOOPING, AL_TRUE);
	AL_ERROR("alSourcei");
	
	alSourcePlay(source);
	while(1) { sleep(1); }
	
	/*
		alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);
		
		alcMakeCurrentContext(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);
	*/
}
