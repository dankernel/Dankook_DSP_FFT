#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/wait.h>

#include "wave.cpp"
#include "fft/fft.c"

#define KB 1024
#define MB KB * 1024
#define GB MB * 1024

char *mp3_decode(char *path) 
{
  int ret = 0;
  int status = 0;
  char *new_path = NULL;

  if (path == NULL)
    goto fail;

  if (strstr(path, ".mp3") == NULL) {
    printf("No Mp3 File\n");
    goto fail;
  }

  /* Make new_path */
  new_path = (char *)malloc(sizeof(char) * strlen(path) + 1);
  strcpy(new_path, path);
  strcpy(&new_path[strlen(path) - 3], "wav");

  if ((ret = fork()) == 0) {
    /* Lame */
    execlp("lame", "lame", "--decode", path, NULL);
    exit(0);
  } else {
    wait(&status);
  }
  
ok:
  return new_path;

fail:
  return path;
}

int main(int argc, char *argv[]) {
  
  int flags = 0;
  int ret = 0;
  struct audio *ap;
  char *new_path = NULL;
  
  if (argc < 2) {
    printf("Arg error\n");
    return 0;
  }

  // MP3 File Encode
  new_path = mp3_decode(argv[1]);
  printf("[OK] MP3 encode. New file name : %s \n", new_path);

  /* Read */
  ap = read_audio(new_path);

  /* Print info */
  print_audio_info(ap);

  /* Get wav array */
  int *tmp = NULL;
  tmp = get_bin(ap);

  /* Make twinddle_factor struct */
  int size = get_waveformDataSize(ap);
  struct twinddle_factor *tf = NULL;
  tf = tf_init(tmp, size);

  /* RUN FFT */
  printf("==== FFT start... ==== \n");
  tf = Main_FFT(tf, size, 1);
  printf("==== FFT END ==== \n");

  /* Print */
  tf_bit_reverse(tf, size);
  tf_print(tf, size);

  /* Decimation */
  // struct audio *new_ap;
  // if ((new_ap = Decimation(ap)) == NULL) {
  //   printf("Error 1\n");
  //   goto fail;
  // }
  // if (write_audio(new_ap, "mono_") < 0) {
  //   printf("Error 2\n");
  //   goto fail;
  // }

  // print_audio_info(new_ap);

  printf("[OK] End\n");
  return 0;

fail:
  printf("[Fail]\n");
  return 0;

}

