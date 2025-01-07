#include <TFT_eSPI.h>
#include <SdFat.h>
#include <AnimatedGIF.h>

TFT_eSPI tft = TFT_eSPI();
SdFat SD;
AnimatedGIF gif;

void setup() {
  Serial.begin(115200);

  // Initialize TFT
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Initialize SD card
  if (!SD.begin()) {  // Replace 5 with your SD CS pin
    Serial.println("SD card initialization failed!");
    while (1);
  }

  // Initialize GIF decoder
  gif.begin();

  // Open GIF file
  gif.open(
    "/example.gif",
    [](const char *filename, int32_t *size) -> void * {
      FsFile *file = new FsFile(SD.open(filename));
      if (file->isOpen()) {
        *size = file->size();
        return (void *)file;
      } else {
        delete file;
        return nullptr;
      }
    },
    [](void *handle) {
      FsFile *file = (FsFile *)handle;
      file->close();
      delete file;
    },
    [](GIFFILE *gifFile, uint8_t *buffer, int32_t len) -> int32_t {
      FsFile *file = (FsFile *)gifFile->fHandle;
      return file->read(buffer, len);
    },
    [](GIFFILE *gifFile, int32_t position) -> int32_t {
      FsFile *file = (FsFile *)gifFile->fHandle;
      return file->seek(position);
    },
    [](GIFDRAW *draw) {
      tft.pushImage(draw->iX, draw->iY, draw->iWidth, draw->iHeight, draw->pPixels);
    }
  );
}

void loop() {
  int delayMs;
  if (gif.playFrame(true, &delayMs)) {
    delay(delayMs);
  } else {
    gif.reset();
  }
}
