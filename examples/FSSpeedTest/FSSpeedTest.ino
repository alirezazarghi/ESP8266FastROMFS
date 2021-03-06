#include <Arduino.h>
#include <ESP8266FastROMFS.h>
#include <FS.h> // SPIFFS

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

  uint8_t data[257];
  for (int i=0; i<257; i++) data[i] = (uint8_t) i;

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

  f = fs->open("testwrite.bin", "r");
  Serial.printf("==> Created file size = %d\n", f->size());
  f->close();

  Serial.printf("Reading %dKB file sequentially in 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = fs->open("testwrite.bin", "r");
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++)
      f->read(data, 256);
  }
  f->close();
  stop = millis();
  Serial.printf("==> Time to read %dKB sequentially in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);

  Serial.printf("Reading %dKB file MISALIGNED in flash and RAM sequentially in 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = fs->open("testwrite.bin", "r");
  f->read();
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++)
      f->read(data+1, 256);
  }
  f->close();
  stop = millis();
  Serial.printf("==> Time to read %dKB sequentially MISALIGNED in flash and RAM in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);


  
  Serial.printf("Reading %dKB file in reverse by 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = fs->open("testwrite.bin", "r");
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++) {
      if (!f->seek(-256 -256 * j * i, SEEK_END)) {
        Serial.printf("Unable to seek to %d, aborting\n", -256 -256 * j * i);
        return;
      }
      if (256 != f->read(data, 256)) {
        Serial.printf("Unable to read 256 bytes, aborting\n");
        return;
      }
    }
  }
  f->close();
  stop = millis();
  Serial.printf("==> Time to read %dKB in reverse in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);


  Serial.printf("Writing 64K file in 1-byte chunks\n");
  start = millis();
  f = fs->open("test1b.bin", "w");
  for (int i=0; i<65536; i++) {
    f->write((uint8_t *)&i, 1);
  }
  f->close();
  stop = millis();
  Serial.printf("==> Time to write 64KB in 1b chunks = %ld milliseconds = %ld bytes/s\n", stop - start, 65536/ (stop-start) * 1000);
  
  Serial.printf("Reading 64K file in 1-byte chunks\n");
  start = millis();
  f = fs->open("test1b.bin", "r");
  for (int i=0; i<65536; i++) {
    char c;
    f->read(&c, 1);
  }
  f->close();
  stop = millis();
  Serial.printf("==> Time to read 64KB in 1b chunks = %ld milliseconds = %ld bytes/s\n", stop - start, 65536/ (stop-start) * 1000);


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

  f = SPIFFS.open("/testwrite.bin", "r");
  Serial.printf("==> Created file size = %d\n", f.size());
  f.close();

  Serial.printf("Reading %dKB file sequentially in 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = SPIFFS.open("/testwrite.bin", "r");
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++)
      f.read(data, 256);
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read %dKB sequentially in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);

  Serial.printf("Reading %dKB file MISALIGNED in flash and RAM sequentially in 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = SPIFFS.open("/testwrite.bin", "r");
  f.read();
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++)
      f.read(data+1, 256);
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read %dKB sequentially MISALIGNED in flash and RAM in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);


  Serial.printf("Reading %dKB file in reverse by 256b chunks\n", TESTSIZEKB);
  start = millis();
  f = SPIFFS.open("/testwrite.bin", "r");
  for (int i=0; i<TESTSIZEKB; i++) {
    for (int j=0; j<4; j++) {
      if (!f.seek(-256 - 256 * j * i, SeekEnd)) {
        Serial.printf("Unable to seek to %d, aborting\n", -256 -256 * j * i);
        return;
      }
      if (256 != f.read(data, 256)) {
        Serial.printf("Unable to read 256 bytes, aborting\n");
        return;
      }
    }
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read %dKB in reverse in 256b chunks = %ld milliseconds = %ld bytes/s\n", TESTSIZEKB, stop - start, TESTSIZEKB*1024 / (stop-start) * 1000);
  

  Serial.printf("Writing 64K file in 1-byte chunks\n");
  start = millis();
  f = SPIFFS.open("/test1b.bin", "w");
  for (int i=0; i<65536; i++) {
    f.write((uint8_t*)&i, 1);
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to write 64KB in 1b chunks = %ld milliseconds = %ld bytes/s\n", stop - start, 65536/ (stop-start) * 1000);
  
  Serial.printf("Reading 64K file in 1-byte chunks\n");
  start = millis();
  f = SPIFFS.open("test1b.bin", "r");
  for (int i=0; i<65536; i++) {
    char c;
    f.read((uint8_t*)&c, 1);
  }
  f.close();
  stop = millis();
  Serial.printf("==> Time to read 64KB in 1b chunks = %ld milliseconds = %ld bytes/s\n", stop - start, 65536/ (stop-start) * 1000);


}

extern void RunFSTest();
void setup()
{
  Serial.begin(115200);
  
  DoFastROMFS();
  DoSPIFFS();
}

void loop()
{

  
}

