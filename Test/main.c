#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>


int main (int argc, char** argv)
 {
  if ( SDL_Init (SDL_INIT_VIDEO) != 0 )
   {
    fprintf (stderr, "Erreur d'initialisation du m�canisme SDL : %s\n", SDL_GetError() );

    return EXIT_FAILURE;
   }

  /*
      A value of 22050 (22.05KHz) is ideal for most games. 22.05KHz is not quite CD quality (you may pass a value of 44100 for CD quality sound),
      but it is quite a bit easier on your CPU. If sound quality is more important to you than CPU cycles, then by all means, pass 44100.
      On most modern computers you won't notice the CPU hit.
  */

  int    audio_rate     = 22050;

  /*

    There are a number of possible sample format constants that we can use here:

        AUDIO_U8 - Unsigned 8-bit samples
        AUDIO_S8 - Signed 8-bit samples
        AUDIO_U16LSB - Unsigned 16-bit samples in little-endian byte order
        AUDIO_S16LSB - Signed 16-bit samples in little-endian byte order
        AUDIO_U16MSB - Unsigned 16-bit samples in big-endian byte order
        AUDIO_S16MSB - Signed 16-bit samples in big-endian byte order
        AUDIO_U16 - Same as AUDIO_U16LSB
        AUDIO_S16 - Same as AUDIO_S16LSB
        AUDIO_U16SYS - Unsigned 16-bit samples in the system's byte order
        AUDIO_S16SYS - Signed 16-bit samples in the system's byte order

  */

  Uint16 audio_format   = AUDIO_S16SYS;
  int    audio_channels = 2;
  int    audio_buffers  = 4096;

  if ( Mix_OpenAudio (audio_rate, audio_format, audio_channels, audio_buffers) != 0)
   {
    fprintf (stderr, "Unable to initialize audio: %s\n", Mix_GetError());
    exit(1);
   }

  Mix_Chunk *sound = Mix_LoadWAV ("sons/magie.wav");

  if ( !sound )
   {
    fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
   }

  if ( SDL_Init (SDL_INIT_VIDEO) != 0 )
   {
    fprintf (stderr, "Erreur d'initialisation du mecanisme SDL : %s\n", SDL_GetError() );

    return EXIT_FAILURE;
   }

  SDL_Window* fenetre = SDL_CreateWindow ("SDL 2.0.14 - Exemple 09" ,
                                          SDL_WINDOWPOS_UNDEFINED   ,
                                          SDL_WINDOWPOS_UNDEFINED   ,
                                          640                       ,
                                          480                       ,
                                          SDL_WINDOW_SHOWN          );

  if ( fenetre == NULL )
   {
    fprintf (stderr, "Erreur de creation de la fenetre : %s\n",SDL_GetError());

    return EXIT_FAILURE;
   }



  int channel = Mix_PlayChannel(-1, sound, 0);

  if(channel == -1)
   {
    fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
   }

  while (Mix_Playing(channel) != 0);

  Mix_FreeChunk(sound);

  Mix_CloseAudio();

  SDL_DestroyWindow (fenetre);
  SDL_Quit();

  return EXIT_SUCCESS;
 }

 // https://moddb.fandom.com/wiki/SDL_mixer:Tutorials:Playing_a_WAV_Sound_File
