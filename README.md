# ge-fangame

English version available in README_EN.md file. **Nhóm chỉ chịu trách nhiệm với những nội dung trong file README tiếng Việt.**

## Giới thiệu project

Project là một fangame 2D đơn giản về một câu chuyện vượt qua đại dương để gặp được "người tri kỷ". Trong game này, người chơi cần thực hiện:
- Lái thuyền sao cho đạt được độ xa bờ (tọa độ Y, coi là điểm số chính của game) cao nhất.
- Tránh các chướng ngại vật xuất hiện trong thời điểm biển động (tầm 1PM-6PM mỗi ngày).
- Câu cá để hồi phục các thanh thức ăn và thể lực (để lái được thuyền).
- Dự báo các cơn bão (chướng ngại vật chết người) bằng các dụng cụ trên thuyền và lái tránh các cơn bão này (*bỏ vì không đủ thời gian*).
- Nâng cấp thuyền, dụng cụ, "stat vật lý" của người chơi, chuẩn bị lương thực thực phẩm trước mỗi chuyến đi thông qua hệ thống currency trong game (*bỏ vì không đủ thời gian*).

Phân công công việc:
- Nguyễn Hoàng Xuân Sơn: viết driver cho DMA2D, SDRAM, thiết kế các menu giao diện game.
- Nguyễn Hồng Đăng: enable FPU, implement chức năng lái thuyền, câu cá trong game.
- Ngô Duy Anh: viết driver cho ILI9341, LTDC, UART, backend SDL, thiết kế chính game.

## Hướng dẫn cài đặt

Game sử dụng CMake và GCC để biên dịch (arm-none-eabi toolchain trên backend STM32 hoặc native toolchain trên PC), Python để bundle assets. Ngoài ra OpenOCD được sử dụng để flash lên MCU  (có thể dùng st-link nhưng OpenOCD tiện hơn nhiều do bỏ qua bước objcopy) và debug (cùng với GDB).

Cách đơn giản nhất để cài đặt các công cụ cần thiết là sử dụng Nix direnv. File `flake.nix` trong project khai báo tất cả các công cụ cần sử dụng, và file này cũng được sử dụng để cài đặt môi trường trên CI.

Để build cho PC, chạy CMake mà không cần thêm lựa chọn gì:
```sh
# configure
cmake -S. -Bbuild/pc
# build
cmake --build build/pc -j
# executable nằm ở build/pc/ge-app/(Debug/Release nếu Ninja Multi-Config)/ge-app
```
Để build cho STM, pass thêm option `-DGE_HAL_STM32=ON`trong bước configure. Ngoài ra nếu GCC native và cross-compiling toolchain đều available thì cũng phải set lại môi trường để trỏ đến cross-compiler, cách đơn giản nhất là sử dụng file toolchain trong project `cmake/arm-none-eabi.cmake`.
```sh
# configure
cmake -S. -Bbuild/stm -DGE_HAL_STM32=ON\
        -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake
# build
cmake --build build/stm -j
# ELF executable nằm ở build/stm/ge-app/(Debug/Release nếu Ninja Multi-Config)/ge-app(.exe)
```
Để flash/debug, sử dụng OpenOCD (`PATH_TO_THE_ELF` là đường dẫn đến ELF executable):
```sh
# Flash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program PATH_TO_THE_ELF verify reset exit"

# Debug
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# in another terminal
gdb PATH_TO_THE_ELF
    # in gdb prompt
    (gdb) target extended-remote :3333
    (gdb) monitor reset halt # software reset
    (gdb) load # reflash the ELF
```

## Thiết kế phần cứng

Thiết kế phần cứng của project không có gì đặc biệt do chỉ sử dụng các peripheral có sẵn trên kit và một số linh kiện đơn giản (2 push button, 1 joystick và một bộ phát âm thanh (MAX98357A + 1 loa khuếch đại)).

Hướng dẫn cắm mạch:
- Joystick: Nối GND và +5V đến các pin tương ứng, URX và URY lần lượt vào PC4 và PC5.
- Push button: Nối GND, hai button A và B nối với PA0 và PC13.
- Buzzer TMAB12A05: Nối GND và cực dương tương ứng, pin còn lại nối vào PB12.

## Thiết kế phần mềm

Game hỗ trợ hai backend: STM32 backend (hiện chỉ hỗ trợ kit STM32F429ZIT6) và một cross-platform desktop backend dựa trên SDL. Trên backend STM32, project chỉ sử dụng các thư viện header CMSIS ([CMSIS](https://github.com/ARM-software/CMSIS_5), [cmsis_device_f4](https://github.com/STMicroelectronics/cmsis_device_f4/)) trên runtime. SDL backend cần có một gamepad để điều khiển, và mục đích chính là để thử nghiệm tiện lợi trên PC mà không cần phải dùng MCU.

Project **không** hỗ trợ và **không** phụ thuộc vào STM32CubeIDE, STM32CubeMX, TouchGFX và FreeRTOS. Quá trình biên dịch được thực hiện trực tiếp sử dụng GCC cross-compiler toolchain kết hợp với CMake. Project *có thể* hỗ trợ Clang (vì Clang phần lớn tương thích với GCC) nhưng điều này chưa được thử nghiệm.

Project gồm hai module chính. `ge-hal`là thư viện trừu tượng hóa phần cứng (HAL) của PC/STM32, cung cấp một API chung cho cả hai backend. `ge-app` là executable game, được viết một cách cross-platform dựa trên API của `ge-hal`.

### ge-hal

Do trên backend STM, project chỉ phụ thuộc vào các thư viện header CMSIS, phần lớn thư viện tương tác trực tiếp với các register trên STM. Phần code cho backend SDL khá đơn giản do SDL đã làm phần lớn công việc. API `ge-hal` expose một API chính bằng `ge::App` wrap cả chương trình và một API phụ trợ `ge::hal::gpu`, abstract cho DMA2D của thiết bị.

Quá trình khởi tạo của `ge-hal` trên backend STM chủ yếu gồm có:
- Enable FPU để hỗ trợ fp32.
- Setup Clock để đạt được 180MHz (cần thiết cho performance cao).
- Setup interrupt cho hai push button.
- Setup SDRAM (để có đủ bộ nhớ cho các framebuffer).
- Setup màn hình TFT9342 và LTDC.
- Setup DMA2D (để blit nhanh hơn bằng phần cứng, tăng tốc các operation đồ họa).
- Setup ADC (sử dụng DMA) cho Joystick
- Setup Clock và DMA cho I2S audio.

### ge-app

#### Asset bundling

Quá trình bundle asset được thực hiện bằng các script Python (kèm một số thư viện đọc ảnh). Để đơn giản thì các file asset (ảnh, âm thanh, v.v.) được convert thành một cặp file header-source bằng C để có thể dễ dàng bundle trong project. Việc generate các file này được tự động hóa bằng CMake. Linker script sẽ đặt các mảng này trong đúng vị trí trên flash.

#### Kiến trúc hệ thống

Hệ thống được tổ chức thành các Scene (màn) để organize code. Các scene có thể chứa lẫn nhau, giúp cho việc compose các Scene dễ dàng hơn. Mỗi scene thừa kế từ lớp `Scene` gốc hoặc `ContainerScene`, và override các hàm tick (update), render, và các hàm nhận input.

Ngoài ra, `ge-app` còn có các hàm thư viện high-level như để render text, sinh số ngẫu nhiên (từ một seed khởi tạo lấy từ phần cứng), đo thời gian (cho phép tăng tốc hoặc dừng đồng hồ). blend màu, v.v. Những chức năng này không low-level để nằm trong `ge-hal`, do đó được đưa trực tiếp vào `ge-app`.
