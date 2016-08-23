#pragma once
#include "device.h"
#include <SDL.h>

struct SDL_Renderer;

namespace device {
namespace gpu {

// Draw Mode setting
union GP0_E1 {
    enum class SemiTransparency : uint32_t {
        Bby2plusFby2 = 0,  // B/2+F/2
        BplusF = 1,        // B+F
        BminusF = 2,       // B-F
        BplusFby4 = 3      // B+F/4
    };
    enum class TexturePageColors : uint32_t { bit4 = 0, bit8 = 1, bit15 = 2 };
    enum class DrawingToDisplayArea : uint32_t { prohibited = 0, allowed = 1 };
    struct {
        uint32_t texturePageBaseX : 4;  // N * 64
        uint32_t texturePageBaseY : 1;  // N * 256
        SemiTransparency semiTransparency : 2;
        TexturePageColors texturePageColors : 2;
        Bit dither24to15 : 1;
        DrawingToDisplayArea drawingToDisplayArea : 1;
        Bit textureDisable : 1;          // 0=Normal, 1=Disable if GP1(09h).Bit0=1)
        Bit texturedRectangleXFlip : 1;  // (BIOS does set this bit on power-up...?)
        Bit texturedRectangleYFlip : 1;  // (BIOS does set it equal to GPUSTAT.13...?)

        uint32_t : 10;
        uint8_t command;  // 0xe1
    };
    struct {
        uint32_t _reg : 24;
        uint32_t _command : 8;
    };

    GP0_E1() : _reg(0) {}
};

// Display mode
union GP1_08 {
    enum class HorizontalResolution : uint8_t { r256 = 0, r320 = 1, r512 = 2, r640 = 3 };
    enum class VerticalResolution : uint8_t {
        r240 = 0,
        r480 = 1,  // if VerticalInterlace
    };
    enum class VideoMode : uint8_t {
        ntsc = 0,  // 60hz
        pal = 1,   // 50hz
    };
    enum class ColorDepth : uint8_t { bit15 = 0, bit24 = 1 };
    enum class HorizontalResolution2 : uint8_t {
        normal = 0,
        r386 = 1,
    };
    enum class TexturePageColors : uint8_t { bit4 = 0, bit8 = 1, bit15 = 2 };
    enum class DrawingToDisplayArea : uint8_t { prohibited = 0, allowed = 1 };
    enum class ReverseFlag : uint8_t { normal = 0, distorted = 1 };
    struct {
        HorizontalResolution horizontalResolution1 : 2;
        VerticalResolution verticalResolution : 1;
        VideoMode videoMode : 1;
        ColorDepth colorDepth : 1;
        Bit interlace : 1;
        HorizontalResolution2 horizontalResolution2 : 1;  // (0=256/320/512/640, 1=368)
        ReverseFlag reverseFlag : 1;

        uint32_t : 16;
        uint8_t command;  // 0x08
    };
    struct {
        uint32_t _reg : 24;
        uint32_t _command : 8;
    };

    GP1_08() : _reg(0) {}
};

class GPU : public Device {
    void *pixels;
    int stride;

    uint16_t VRAM[512][1024];
    uint32_t fifo[16];
    uint32_t tmpGP0 = 0;
    uint32_t tmpGP1 = 0;
    int whichGP0Byte = 0;
    int whichGP1Byte = 0;

    /* 0 - nothing
       1 - GP0(0xc0) - VRAM to CPU transfer
       2 - GP1(0x10) - Get GPU Info
    */
    int startX = 0;
    int startY = 0;
    int endX = 0;
    int endY = 0;
    int currX = 0;
    int currY = 0;
    int gpuReadMode = 0;
    uint32_t GPUREAD = 0;
    uint32_t GPUSTAT = 0;

    const float WIDTH = 640.f;
    const float HEIGHT = 480.f;

    // GP0(0xc0)
    bool readyVramToCpu = false;

    GP0_E1 gp0_e1;

    // GP0(0xe2)
    int textureWindowMaskX = 0;
    int textureWindowMaskY = 0;
    int textureWindowOffsetX = 0;
    int textureWindowOffsetY = 0;

    // GP0(0xe3)
    int drawingAreaX1 = 0;
    int drawingAreaY1 = 0;

    // GP0(0xe4)
    int drawingAreaX2 = 0;
    int drawingAreaY2 = 0;

    // GP0(0xe5)
    int drawingOffsetX = 0;
    int drawingOffsetY = 0;

    // GP0(0xe6)
    int setMaskWhileDrawing = 0;
    int checkMaskBeforeDraw = 0;

    // GP1(0x02)
    Bit irqAcknowledge;

    // GP1(0x03)
    Bit displayDisable;

    // GP(0x04)
    int dmaDirection = 0;

    // GP1(0x05)
    int displayAreaStartX = 0;
    int displayAreaStartY = 0;

    // GP1(0x06)
    int displayRangeX1 = 0;
    int displayRangeX2 = 0;

    // GP1(0x07)
    int displayRangeY1 = 0;
    int displayRangeY2 = 0;

    GP1_08 gp1_08;

    // GP1(0x09)
    bool textureDisableAllowed = false;

    SDL_Renderer *renderer;
    void drawPolygon(int x[3], int y[3], int color[3]);
    void writeGP0(uint32_t data);
    void writeGP1(uint32_t data);

   public:
	   SDL_Texture *texture;
	   SDL_Texture *SCREEN;
	   SDL_Texture *output;
    bool odd = false;
    void step();
    uint8_t read(uint32_t address);
    void write(uint32_t address, uint8_t data);

    void render();
    void setRenderer(SDL_Renderer *renderer) {
        this->renderer = renderer;
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 1024, 512);
        SCREEN = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
		output = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, 640, 480);
    }
};
}
}