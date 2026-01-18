#include "ge-hal/stm/framebuffer.hpp"

#include "ge-hal/app.hpp"
#include "ge-hal/stm/gpio.hpp"
#include "ge-hal/stm/sdram.hpp"
#include "ge-hal/stm/spi.hpp"
#include "ge-hal/stm/time.hpp"
#include "stm32f429xx.h"
#include <cstdio>
#include <initializer_list>

namespace ge {
namespace hal {
namespace stm {

static GE_SDRAM u16 framebuffer_storage[2][ge::App::WIDTH * ge::App::HEIGHT];

u16 *pixel_buffer(int buffer_index) {
  return framebuffer_storage[buffer_index & 1];
}

enum class ILI9341Commands : u8 {
  eRESET = 0x01,
  eSLEEP_OUT = 0x11,
  eGAMMA = 0x26,
  eDISPLAY_OFF = 0x28,
  eDISPLAY_ON = 0x29,
  eCOLUMN_ADDR = 0x2A,
  ePAGE_ADDR = 0x2B,
  eGRAM = 0x2C,
  eMAC = 0x36,
  ePIXEL_FORMAT = 0x3A,
  eWDB = 0x51,
  eWCD = 0x53,
  eRGB_INTERFACE = 0xB0,
  eFRC = 0xB1,
  eBPC = 0xB5,
  eDFC = 0xB6,
  ePOWER1 = 0xC0,
  ePOWER2 = 0xC1,
  eVCOM1 = 0xC5,
  eVCOM2 = 0xC7,
  ePOWERA = 0xCB,
  ePOWERB = 0xCF,
  ePGAMMA = 0xE0,
  eNGAMMA = 0xE1,
  eDTCA = 0xE8,
  eDTCB = 0xEA,
  ePOWER_SEQ = 0xED,
  e3GAMMA_EN = 0xF2,
  eINTERFACE = 0xF6,
  ePRC = 0xF7
};

void init_ltdc() {
  // config pins
  Pin csx{'C', 2}, wrx{'D', 13};
  SPIHandle spi = SPI5_CONFIG.init();

  for (auto pin : {csx, wrx}) {
    pin.set_mode(GPIOMode::Output);
    pin.set_otype(GPIOOType::PushPull);
    pin.set_pupd(GPIOPuPd::NoPull);
    pin.set_speed(GPIOSpeed::Medium);
  }

  csx.write(true);

  auto send_command = [&](ILI9341Commands cmd,
                          std::initializer_list<u8> data = {}) {
    // select the current peripheral
    csx.write(false);

    // command mode
    wrx.write(false);
    spi.send_blocking(static_cast<u8>(cmd));

    // data mode
    if (data.size() > 0) {
      wrx.write(true);
      for (u8 d : data) {
        spi.send_blocking(d);
      }
    }

    // deselect the current peripheral
    csx.write(true);
  };

  // ILI9341 initialization sequence
  send_command(ILI9341Commands::eRESET); // software reset
  delay_spin(50000);
  send_command(ILI9341Commands::ePOWERA, {0x39, 0x2C, 0x00, 0x34, 0x02});
  send_command(ILI9341Commands::ePOWERB, {0x00, 0xC1, 0x30});
  send_command(ILI9341Commands::eDTCA, {0x85, 0x00, 0x78});
  send_command(ILI9341Commands::eDTCB, {0x00, 0x00});
  send_command(ILI9341Commands::ePOWER_SEQ, {0x64, 0x03, 0x12, 0x81});
  send_command(ILI9341Commands::ePRC, {0x20});
  send_command(ILI9341Commands::ePOWER1, {0x23});
  send_command(ILI9341Commands::ePOWER2, {0x10});
  send_command(ILI9341Commands::eVCOM1, {0x3E, 0x28});
  send_command(ILI9341Commands::eVCOM2, {0x86});
  send_command(ILI9341Commands::eMAC, {0x48});
  send_command(ILI9341Commands::ePIXEL_FORMAT, {0x55});
  send_command(ILI9341Commands::eFRC, {0x00, 0x18});
  send_command(ILI9341Commands::eDFC, {0x08, 0x82, 0x27});
  send_command(ILI9341Commands::e3GAMMA_EN, {0x00});
  send_command(ILI9341Commands::eCOLUMN_ADDR, {0x00, 0x00, 0x00, 0xEF});
  send_command(ILI9341Commands::ePAGE_ADDR, {0x00, 0x00, 0x01, 0x3F});
  send_command(ILI9341Commands::eGAMMA, {0x01});

  send_command(ILI9341Commands::ePGAMMA,
               {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07,
                0x10, 0x03, 0x0E, 0x09, 0x00});
  send_command(ILI9341Commands::eNGAMMA,
               {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08,
                0x0F, 0x0C, 0x31, 0x36, 0x0F});
  send_command(ILI9341Commands::eSLEEP_OUT);

  delay_spin(1000000); // Wait for wake up

  send_command(ILI9341Commands::eDISPLAY_ON);
  auto start_time = systick_get();

  send_command(ILI9341Commands::eGRAM);

  {
    auto spi16 = spi.switch_to_16bit();
    csx.write(false);
    wrx.write(true);
    for (int i = 0; i < 240 * 320; i++) {
      spi16.send_blocking(0xF800);
    }
    csx.write(true);
  }

  auto end_time = systick_get();
  std::printf("LTDC initialized in %u ms\r\n", end_time - start_time);
}

} // namespace stm
} // namespace hal
} // namespace ge
