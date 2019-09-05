#pragma once
#include "digital_controller.h"

namespace peripherals {
struct AnalogController : public DigitalController {
   protected:
    enum class Command {
        None,
        Read,
        EnterConfiguration,
        ExitConfiguration,
        SetLed,
        GetLed,
        UnlockRumble,
        Unknown_46,
        Unknown_47,
        Unknown_4c
    };

    struct Stick {
        /**
         * 0x00 - left/up
         * 0x80 - center
         * 0xff - right/down
         */
        uint8_t x;
        uint8_t y;
        Stick() : x(0x80), y(0x80) {}
    };

    uint8_t _handle(uint8_t byte);  // Wrapper for handler for catching return value
    uint8_t handleReadAnalog(uint8_t byte);
    uint8_t handleEnterConfiguration(uint8_t byte);
    uint8_t handleExitConfiguration(uint8_t byte);
    uint8_t handleSetLed(uint8_t byte);
    uint8_t handleLedStatus(uint8_t byte);
    uint8_t handleUnlockRumble(uint8_t byte);
    uint8_t handleUnknown46(uint8_t byte);
    uint8_t handleUnknown47(uint8_t byte);
    uint8_t handleUnknown4c(uint8_t byte);

    Stick left, right;
    Command command = Command::None;
    bool analogEnabled = false;
    bool ledEnabled = false;
    bool configurationMode = false;

   public:
    AnalogController(int Port);
    uint8_t handle(uint8_t byte) override;
    void update() override;
};
};  // namespace peripherals