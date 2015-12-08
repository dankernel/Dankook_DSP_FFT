#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <memory.h>

#define ID long
#define CLIPPING(x) x < 0 ? 0 : x > 255 ? 255 : x
#define PI 3.14159265358979323846

typedef struct {
  ID chunkID;
  long chunkSize;
} HEADER;

typedef struct {
  ID chunkID; // RIFF, 식별자
  long chunkSize; // 현재부터 파일 끝까지의 바이트 수
  long wFormat;
} RiffHeader;

typedef struct {
  short wFormatTag; // PCM = 1 , Values other than 1 indicate some form of compression 
  unsigned short wChannels; // the number of audio channels (Mono = 1, Stereo = 2, etc)
  unsigned long dwSamplesPerSec; //샘플링 레이트
  unsigned long dwAvgBytesPerSec; //전송률 (stereo의 경우, 2*dwSamplesPerSec)
  unsigned short wBlockAlign; // Block Align
  unsigned short wBitsPerSample; // bits per sample
} subFormatChunk;

typedef struct {
  ID chunkID; //ASCII코드로 "fmt" (0x666d7420 big-endian form). 
  long chunkSize; //포맷 청크의 크기 (byte), wFormat 이 1일때 (PCM 일때)는 16
  subFormatChunk field;
} FormatChunk;

typedef struct {
  ID chunkID; //ASCII코드로 "data" (0x64617461 big-endian form). 
  long chunkSize; //데이터 청크에 포함된 데이터의 크기
  unsigned char *waveformData; // PCM 데이터
} DataChunk;

