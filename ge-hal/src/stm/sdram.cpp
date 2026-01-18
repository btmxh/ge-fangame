#include "ge-hal/stm/sdram.hpp"
#include "ge-hal/core.hpp"
#include "ge-hal/stm/gpio.hpp"
#include <initializer_list>

#include "ge-hal/stm/time.hpp"
#include "stm32f429xx.h"

namespace ge {
namespace hal {
namespace stm {
void init_sdram() {
  // Initialize pins
  auto init_pins = [](char bank, std::initializer_list<u8> nums) {
    for (const auto num : nums) {
      Pin pin{bank, num};
      pin.set_mode(GPIOMode::AlternateFunction);
      pin.set_otype(GPIOOType::PushPull);
      pin.set_pupd(GPIOPuPd::NoPull);
      // FIX 2: Use VeryHigh speed for SDRAM (Essential for >50MHz)
      pin.set_speed(GPIOSpeed::VeryHigh);
      pin.set_af(12);
    }
  };

  init_pins('B', {5, 6});
  init_pins('C', {0});
  init_pins('D', {0, 1, 8, 9, 10, 14, 15});
  init_pins('E', {0, 1, 7, 8, 9, 10, 11, 12, 13, 14, 15});
  init_pins('F', {0, 1, 2, 3, 4, 5, 11, 12, 13, 14, 15});
  init_pins('G', {0, 1, 4, 5, 8, 15});

  RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;

  auto fmc = FMC_Bank5_6;

  // FIX 4: Use direct assignment (=) instead of OR (|=) for a clean state

  // SDCR[0] (SDCR1) holds COMMON settings for both banks
  fmc->SDCR[0] = (2UL << FMC_SDCR1_SDCLK_Pos) | // HCLK / 2
                 (2UL << FMC_SDCR1_RPIPE_Pos) | // 2 HCLK delay
                 (0UL << FMC_SDCR1_RBURST_Pos); // Burst disabled

  // SDCR[1] (SDCR2) holds BANK 2 specific settings
  fmc->SDCR[1] = (3UL << FMC_SDCR2_CAS_Pos) |  // CAS Latency 3
                 (1UL << FMC_SDCR2_NB_Pos) |   // 4 Internal Banks
                 (1UL << FMC_SDCR2_MWID_Pos) | // 16-bit width
                 (1UL << FMC_SDCR2_NR_Pos);    // 12-bit Row Address

  // FIX 3: TRP and TRC are Common timings, must be in SDTR[0] / SDTR1
  fmc->SDTR[0] = (6UL << FMC_SDTR1_TRC_Pos) | // Row cycle delay
                 (1UL << FMC_SDTR1_TRP_Pos);  // Row precharge delay

  // SDTR[1] (SDTR2) holds BANK 2 specific timings
  fmc->SDTR[1] = (1UL << FMC_SDTR1_TWR_Pos) |  // Recovery delay
                 (3UL << FMC_SDTR2_TRAS_Pos) | // Self refresh time
                 (6UL << FMC_SDTR2_TXSR_Pos) | // Exit self-refresh
                 (1UL << FMC_SDTR2_TMRD_Pos);  // Load Mode Register delay

  auto wait_until_not_busy = [&]() {
    while (fmc->SDSR & FMC_SDSR_BUSY)
      delay_spin(1);
  };

  // FIX 1: Add mrd_value parameter to pass Mode Register config
  auto exec_cmd = [&](u8 mode, u8 num_refreshs = 0, u16 mrd_value = 0) {
    wait_until_not_busy();
    fmc->SDCMR = FMC_SDCMR_CTB2 | (mode << FMC_SDCMR_MODE_Pos) |
                 ((num_refreshs - 1) << FMC_SDCMR_NRFS_Pos) |
                 (mrd_value << FMC_SDCMR_MRD_Pos); // Shift MRD into position
  };

  exec_cmd(1);      // Clock Config Enable
  delay_spin(1000); // Wait >100us
  exec_cmd(2);      // PALL
  exec_cmd(3, 8);   // Auto-refresh (8 cycles)

  // FIX 1 (Implementation): Configure the SDRAM chip's internal Mode Register
  // Value = Burst Length 1 (0x0) | Burst Type Sequential (0x0) | CAS Latency 3
  // (0x30) 0x030 = 0b00110000. (Bits 4-6 are CAS latency. 3 << 4 = 48 = 0x30)
  u16 mode_register = 0x0230; // Example: CAS 3, Burst Length 1, Single Write
                              // bit set (if supported)
  // Or simply for standard read/write:
  mode_register = (3 << 4) | (0 << 0); // CAS 3, Burst Length 1

  exec_cmd(4, 1, mode_register); // Load Mode Register command

  fmc->SDRTR = (680UL << FMC_SDRTR_COUNT_Pos); // Set refresh rate
  wait_until_not_busy();
}
} // namespace stm
} // namespace hal
} // namespace ge
