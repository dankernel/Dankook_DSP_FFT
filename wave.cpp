#include "wave.h"

/*
 * Audio info
 */
struct audio {

  RiffHeader R;
  FormatChunk F;
  DataChunk D;
  
  long SamplesPerSec;
  char *path;

};

struct audio *copy_audio(struct audio *ap)
{
  struct audio *new_ap = NULL;

  char new_file[1024] = "new_";

  if (ap == NULL)
    return NULL;
  
  new_ap = (struct audio*)malloc(sizeof(struct audio));

  new_ap->R = ap->R;
  new_ap->F = ap->F;
  new_ap->F.field = ap->F.field;
  new_ap->D = ap->D;

  new_ap->SamplesPerSec = ap->F.field.dwSamplesPerSec;
  strcat(new_file, ap->path);
  strcpy(new_ap->path, new_file);

  return new_ap;
}

void LongToString(long H_chunkID, char *chunkID)
{
  long MASK = 0x000000FF;

  chunkID[0] = (char)(H_chunkID & MASK);
  chunkID[1] = (char)((H_chunkID >> 8) & MASK);
  chunkID[2] = (char)((H_chunkID >> 16) & MASK);
  chunkID[3] = (char)((H_chunkID >> 24) & MASK);
  chunkID[4] = '\0';
}

int ReadWave(char *filename, RiffHeader *R, FormatChunk *F, DataChunk *D) //wav2dat
{
  FILE *fp;
  HEADER H;
  char chunkID[5];

  if ((fp = fopen(filename, "rb")) == NULL) {
    printf("\nCannot read file.\n\n");
    return 0;
  }

  //청크는 chunkID 와 chunkSize 로 시작함
  // 청크의 시작이 없을 때 까지 반복
  while (0 != fread(&H, sizeof(HEADER), 1, fp))
  {
    LongToString(H.chunkID, chunkID);
    // RIFF chunk 인지 확인
    if (strcmp(chunkID, "RIFF") == 0) {
      R->chunkID = H.chunkID;
      R->chunkSize = H.chunkSize;
      fread(&(R->wFormat), sizeof(R->wFormat), 1, fp);
      LongToString(R->wFormat, chunkID);

      // WAVE chunk 인지 확인
      if (strcmp(chunkID, "WAVE") != 0) {
        printf("\nNot supported format.\n\n");
        return 0;
      }
    }
    // format chunk 처리
    else if (strcmp(chunkID, "fmt ") == 0) {
      F->chunkID = H.chunkID;
      F->chunkSize = H.chunkSize;
      fread(&(F->field), sizeof(F->field), 1, fp);
      // 8bit, 16bit가 아닐때 예외처리
      if (!(F->field.wBitsPerSample == 8) && !(F->field.wBitsPerSample == 16)) {
        printf("\n%d bits per sample is not supported.\n\n", F->field.wBitsPerSample);
        return -1;
      }

      // chunk size 가 16 이상인 경우
      if (H.chunkSize > sizeof(F->field)) {
        // 다음 chunk ID 위치로 이동
        fseek(fp, (long)(H.chunkSize - sizeof(F->field)), SEEK_CUR);
      }
    }
    // data chunk 처리
    else if (strcmp(chunkID, "data") == 0) {
      D->chunkID = H.chunkID;
      D->chunkSize = H.chunkSize;
      D->waveformData
        = (unsigned char *)malloc(sizeof(char) * H.chunkSize);

      if (D->waveformData == NULL)
        return -1;
      fread(D->waveformData, 1, H.chunkSize, fp);
    }
    // RIFF, fmt, data 이외에는 처리하지 않음
    else {
      fseek(fp, (long)H.chunkSize, SEEK_CUR);
    }
  }

  fclose(fp);
  return 1;
}

unsigned char* WaveToPCM(char* name, long* data_size)
{
  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  ReadWave(name, &R, &F, &D);

  *data_size = D.chunkSize;
  if (F.field.wBitsPerSample != 8){
    printf("\n BitsPerSample of input wave file must be 8 bits !!!");
  }

  return(D.waveformData);
}


void read()
{
  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  char name[100] = "Demo.wav";
  double PlayTime;

  ReadWave(name, &R, &F, &D);

  PlayTime = (double)D.chunkSize / (F.field.dwSamplesPerSec * F.field.wChannels * F.field.wBitsPerSample / 8);

  printf("\n[] Data chunk size = %ld", D.chunkSize);
  printf("\n[] The number of channel = %d (mono = 1, stereo = 2)", F.field.wChannels);
  printf("\n[] Sampling rate = %ld [Hz or samples/sec]", F.field.dwSamplesPerSec);
  printf("\n[] Sample resolution = %d [bits/sample]", F.field.wBitsPerSample);
  printf("\n[] Play time = %lf [sec]\n", PlayTime);
}


int WriteWave2(char *filename, RiffHeader R, FormatChunk F, DataChunk D)
{
  FILE *fp;

  if ((fp = fopen(filename, "wb")) == NULL) {
    printf("\t File Open Failure\n");
    return(0);
  }

  fwrite(&R, sizeof(R), 1, fp);

  fwrite(&F, sizeof(F), 1, fp);

  fwrite(&(D.chunkID), sizeof(D.chunkID), 1, fp);
  fwrite(&(D.chunkSize), sizeof(D.chunkSize), 1, fp);
  fwrite(D.waveformData, sizeof(char), D.chunkSize, fp);

  fclose(fp);

  return(1);
}

int WriteWave(char *name,
    short BitsPerSample,   // 8bit과 16bit만 허용 
    long SamplesPerSec, // sampling rate
    short Channel, // channels
    unsigned char *waveform_data, // PCM data
    long waveform_data_size // PCM data 의 길이
    )
{
  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  R.chunkID = 0x46464952; // "RIFF"
  R.chunkSize = 16 + waveform_data_size + 20;
  // PCM data 길이 + RIFF, Format 청크의 길이 - 8 Byte
  R.wFormat = 0x45564157; // "WAVE"

  F.chunkID = 0x20746d66;  // "fmt "
  F.chunkSize = 16; // PCM 이므로 추가 사항 없음
  F.field.wFormatTag = 1; // PCM
  F.field.wChannels = Channel;
  F.field.dwAvgBytesPerSec = SamplesPerSec * Channel * (BitsPerSample / 8);
  F.field.dwSamplesPerSec = SamplesPerSec;
  F.field.wBitsPerSample = BitsPerSample;
  F.field.wBlockAlign = Channel*(BitsPerSample / 8);

  D.chunkID = 0x61746164;   // "data"
  D.chunkSize = waveform_data_size;

  D.waveformData = waveform_data;

  WriteWave2(name, R, F, D);

  return(1);
}

/*
 * Creating do file
 */
void write_do()
{
  char name[100] = "pa.wav";

  static double freq[] = {264.0, 297.0, 330.0, 352.0, 396.0, 440.0, 495.0, 528.0}; // 도 의 주파수
  // static double freq[] = {264.0, 264.0, 264.0, 264.0, 264.0, 264.0, 264.0, 264.0}; // 도 의 주파수

  long SamplesPerSec = 22050; // 22kHz sampling
  short BitsPerSample = 8; // 8bits
  short Channels = 1; // mono
  double SamplesPeriod = 1 / SamplesPerSec;
  double PlayTime = 1;
  long waveformDataSize;
  unsigned char *waveformData;
  double t, f;
  long index;

  waveformDataSize = (long)(PlayTime*SamplesPerSec*Channels*(BitsPerSample / 8));
  waveformData = (unsigned char *)malloc(sizeof(char)*waveformDataSize);

  t = 0.0;
  f = freq[3];
  for (index = 0; index < waveformDataSize; index++, t += 1.0 / SamplesPerSec) {
    waveformData[index] = (int)(128.0 + 100.0 * sin(2.0 * PI * f * t) + 0.5);
    // printf("%02X \n", waveformData[index]);
  }

  // printf("%10ld %10lf\n", index, f);
  WriteWave(name, BitsPerSample, SamplesPerSec, Channels, waveformData, waveformDataSize);

  free(waveformData);
}

void write_mod_samplingrate()
{
  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  long SamplesPerSec;
  char name_input[100] = "headset2.wav";
  char name_half[100] = "half.wav";
  char name_double[100] = "double.wav";
  ReadWave(name_input, &R, &F, &D);

  SamplesPerSec = F.field.dwSamplesPerSec / 2;
  WriteWave(name_half, F.field.wBitsPerSample, SamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);

  SamplesPerSec = F.field.dwSamplesPerSec * 2;
  WriteWave(name_double, F.field.wBitsPerSample, SamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);
}

void interpolation(unsigned char *input, long size, unsigned char *output)
{
  long i;
  unsigned char *zeropad = (unsigned char *)malloc(sizeof(char) * size * 2);
  unsigned char value;

  // Zero padding 
  for (i = 0; i < size; i++) {
    zeropad[2 * i] = input[i];
    zeropad[2 * i + 1] = 0;
  }

  // Conv
  for (i = 1; i < size * 2 - 1; i++) {
    value = (int)((zeropad[i - 1] + 2 * zeropad[i] + zeropad[i + 1]) / 2.0);
    output[i] = (unsigned char)CLIPPING(value);
  }

}

void subsmapling2to1(unsigned char *input, long size, unsigned char *output)
{
  long i;

  for (i = 0; i < size; i += 2) {
    output[i / 2] = input[i];
  }

}


void NoiseGen(int *noise, long size, int range)
{
  long i;

  for (i = 0; i < size; i++) {
    noise[i] = (rand() % (2 * range + 1) - range);
    /* code */
  }
}

void write_noise()
{
  char name_in[100] = "headset2.wav";
  char name_out[100] = "test_demo+noise.wav";
  char name1[100] = "test_noise.wav";
  long SamplesPerSec = 11025; // 11kHz
  short BitsPerSample = 8; // 8bits
  short Channels = 1; // mono
  double SamplesPeriod = 1 / SamplesPerSec;
  double PlayTime = 5;
  long waveformDataSize;
  unsigned char * waveformData;
  int * noise;
  long i;

  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  waveformDataSize = (long)(5 /*  playtime = 5 */ * 11025 * 1 *(8 / 8));
  waveformData = (unsigned char *)malloc(sizeof(char)*waveformDataSize);
  noise = (int *)malloc(sizeof(int) * waveformDataSize);

  // 잡음 생성
  NoiseGen(noise, waveformDataSize, 30); // range = 30;
  for (i = 0; i < waveformDataSize; i++)
    waveformData[i] = CLIPPING(noise[i] + 128);
  WriteWave(name1, BitsPerSample, SamplesPerSec, Channels, waveformData, waveformDataSize);

  // 잡음 + 다른 사운드 데이터
  ReadWave(name_in, &R, &F, &D);
  free(noise);
  noise = (int *)malloc(sizeof(int) * D.chunkSize);

  NoiseGen(noise, D.chunkSize, 5);
  for (i = 0; i < D.chunkSize; i++)
    D.waveformData[i] = CLIPPING(D.waveformData[i] + noise[i]);

  WriteWave(name_out, F.field.wBitsPerSample, F.field.dwSamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);

  free(waveformData);
  free(noise);
}

struct audio *read_audio (char *path)
{
  struct audio *ap;

  ap = (struct audio*)malloc(sizeof(struct audio));
  ap->path = (char*)malloc(sizeof(char) * 1024);

  strcpy(ap->path, path);

  ReadWave(path, &ap->R, &ap->F, &ap->D);
  ap->SamplesPerSec = ap->F.field.dwSamplesPerSec;

  return ap;
}

int print_audio_info(struct audio *ap)
{
  DataChunk D = ap->D;
  FormatChunk F = ap->F;
  double PlayTime;

  PlayTime = (double)D.chunkSize / (F.field.dwSamplesPerSec * F.field.wChannels * F.field.wBitsPerSample / 8);

  printf("\n===== %s =====", ap->path);
  printf("\n[] Data chunk size = %ld", D.chunkSize);
  printf("\n[] The number of channel = %d (mono = 1, stereo = 2)", F.field.wChannels);
  printf("\n[] Sampling rate = %ld [Hz or samples/sec]", F.field.dwSamplesPerSec);
  printf("\n[] Sample resolution = %d [bits/sample]", F.field.wBitsPerSample);
  printf("\n[] Play time = %lf [sec]\n", PlayTime);

  return 0;
}

long write_mod_samplingrate(struct audio *ap, float rate)
{
  long ret;
  if (ap == NULL)
    return 0;

  ret = ap->SamplesPerSec = ap->F.field.dwSamplesPerSec * rate;

  // WriteWave(name_double, F.field.wBitsPerSample, ap->SamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);
  
  return ret;

}

long get_waveformDataSize(struct audio *ap)
{
  if (ap == NULL)
    return 0;

  double PlayTime;
  PlayTime = (double)ap->D.chunkSize / 
    (ap->F.field.dwSamplesPerSec * ap->F.field.wChannels * ap->F.field.wBitsPerSample / 8);

  long waveformDataSize = 0;
  waveformDataSize = (long)(PlayTime * ap->SamplesPerSec * ap->F.field.wChannels * (ap->F.field.wBitsPerSample / 8));
  return waveformDataSize;
}

int *get_bin(struct audio *ap)
{
  long waveformDataSize = 0;
  double PlayTime;
  PlayTime = (double)ap->D.chunkSize / 
    (ap->F.field.dwSamplesPerSec * ap->F.field.wChannels * ap->F.field.wBitsPerSample / 8);

  if (ap == NULL)
    return NULL;

  waveformDataSize = (long)(PlayTime * ap->SamplesPerSec * ap->F.field.wChannels * (ap->F.field.wBitsPerSample / 8));

  int *array = NULL;
  array = (int *)malloc(sizeof(array) * waveformDataSize);

  for (int i = 0; i < waveformDataSize; i++) {
    array[i] = ap->D.waveformData[i];
    // printf("%d \n", ap->D.waveformData[i]);
  }
  printf("[OK] Print bin\n");

  return array;
}

int write_audio(struct audio *ap, const char *tmp)
{
  int ret = 0;
  char new_file[1024];

  if (ap == NULL)
    return -1;

  strcpy(new_file, tmp);
  strcat(new_file, ap->path);
  strcpy(ap->path, new_file);

  WriteWave(new_file, ap->F.field.wBitsPerSample, ap->SamplesPerSec, 
      ap->F.field.wChannels, ap->D.waveformData, ap->D.chunkSize);

  return 0;
}

struct audio *Decimation(struct audio *ap)
{
  if(ap->F.field.wChannels == 1)
    return NULL;

  int i=0;
  struct audio *tmp = copy_audio(ap);

  if (tmp == NULL)
    return NULL;

  tmp->R.chunkSize -= ap->D.chunkSize / 2;
  tmp->F.field.wChannels = 1;
  tmp->D.chunkSize /= 2;

  for(i = 0 ; i < ap->D.chunkSize; i += 4) {
      tmp->D.waveformData[i/2] = ap->D.waveformData[i];
      tmp->D.waveformData[(i/2)+1] = ap->D.waveformData[i+1];
  }

  return tmp;
}
