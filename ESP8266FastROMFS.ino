#include "fastromfs.h"
#include "FS.h"

#define TESTSIZEKB 512

void DoFastROMFS()
{
  Serial.printf("------Using FastROMFS\n");
  
  FastROMFilesystem *fs = new FastROMFilesystem;
  if (!fs->mkfs()) {
    Serial.printf("Unable to mkfs(), aborting\n");
    return;
  }
  if (!fs->mount()) {
    Serial.printf("Unable to mount(), aborting\n");
    return;
  }

  uint8_t data[256];
  for (int i=0; i<256; i++) data[i] = (uint8_t) i;

  Serial.printf("Creating %dKB file, may take a while...\n", TESTSIZEKB);
  long start = millis();
  FastROMFile *f = fs->open("testwrite.bin", "w");
  if (!f) {
    Serial.printf("Unable to open file for writing, aborting\n");
    return;
  }
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++)
      f->write(data, 256);
  }
  f->close();
  long stop = millis();
  Serial.printf("==> Time to write %dKB in 256b chunks = %ld milliseconds\n", TESTSIZEKB, stop - start);

  Serial.printf("Reading %dKB file sequentially in 256b chunks", TESTSIZEKB);
  start = millis();
  f = fs->open("testwrite.bin", "r");
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++)
      f->read(data, 256);
  }
  f->close();
  stop = millis();
  Serial.printf("==> Time to read %dKB sequentially in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);

  Serial.printf("Reading %dKB file in reverse by 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = fs->open("testwrite.bin", "r");
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++) {
      f->seek(-256 -256 * j * i, SEEK_END);
      f->read(data, 256);
    }
  }
  f->close();
  stop = millis();
  Serial.printf("==> Time to read %d in reverse in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);

  fs->umount();
}



void DoSPIFFS()
{
  Serial.printf("------Using SPIFFS\n");
  
  if (!SPIFFS.format()) {
    Serial.printf("Unable to format(), aborting\n");
    return;
  }
  if (!SPIFFS.begin()) {
    Serial.printf("Unable to begin(), aborting\n");
    return;
  }

  uint8_t data[256];
  for (int i=0; i<256; i++) data[i] = (uint8_t) i;

  Serial.printf("Creating %dKB file, may take a while...\n", TESTSIZEKB);
  long start = millis();
  File f = SPIFFS.open("/testwrite.bin", "w");
  if (!f) {
    Serial.printf("Unable to open file for writing, aborting\n");
    return;
  }
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++)
      f.write(data, 256);
  }
  f.close();
  long stop = millis();
  Serial.printf("==> Time to write %dKB in 256b chunks = %ld milliseconds\n", TESTSIZEKB, stop - start);

  Serial.printf("Reading %dKB file sequentially in 256b chunks", TESTSIZEKB);
  start = millis();
  f = SPIFFS.open("/testwrite.bin", "r");
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++)
      f.read(data, 256);
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read %dKB sequentially in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);

  Serial.printf("Reading %dKB file in reverse by 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = SPIFFS.open("testwrite.bin", "r");
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++) {
      f.seek(-256 -256 * j * i, SeekEnd);
      f.read(data, 256);
    }
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read %d in reverse in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);
}


void setup()
{
  Serial.begin(115200);
  DoFastROMFS();
  DoSPIFFS();
}

void loop()
{

  
}

