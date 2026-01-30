# ge-fangame

English version available in README_EN.md file. **Nhóm chỉ chịu trách nhiệm với những nội dung trong file README tiếng Việt.**

## Demo Videos

- [SDL Backend Demo](demo/sdl_backend.mp4)
- [STM Backend Demo](demo/stm_backend.mp4)

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

### Tổng quan
Thiết kế phần cứng của project tương đối đơn giản do hệ thống chỉ sử dụng các peripheral có sẵn trên kit vi điều khiển cùng một số linh kiện ngoại vi cơ bản.

Các thành phần phần cứng chính bao gồm:
- Kit phát triển STM32F429ZIT6 Discovery
- 01 joystick analog
- 02 nút nhấn (push button)
- 01 buzzer phát âm thanh

Hệ thống được tổ chức theo mô hình vi điều khiển trung tâm, trong đó STM32 đóng vai trò xử lý chính và giao tiếp với các thiết bị ngoại vi thông qua GPIO và ADC.

- Joystick cung cấp tín hiệu analog cho vi điều khiển thông qua bộ chuyển đổi ADC
- Các nút nhấn được kết nối tới các chân GPIO và có thể sử dụng ngắt ngoài
- Buzzer được điều khiển trực tiếp bởi vi điều khiển để phát tín hiệu âm thanh

### Mô tả kết nối phần cứng

#### Joystick
Joystick được sử dụng để cung cấp hai trục điều khiển analog (X và Y).

- GND: nối với GND của kit
- +5V: nối với nguồn 5V
- URX: nối vào chân PC4 (ADC)
- URY: nối vào chân PC5 (ADC)

Hai chân PC4 và PC5 hỗ trợ ADC, cho phép đọc giá trị analog liên tục từ joystick nhằm phục vụ các thao tác điều khiển mượt mà.

#### Push Button
Hai nút nhấn được sử dụng để nhận lệnh từ người dùng.

- GND: nối với GND
- Button A: nối với chân PA0
- Button B: nối với chân PC13

Các chân PA0 và PC13 đều hỗ trợ ngắt ngoài (EXTI), cho phép hệ thống phản hồi nhanh khi có thao tác nhấn nút. Các chân GPIO được cấu hình với điện trở kéo lên nội bộ (pull-up) để đảm bảo trạng thái ổn định khi nút không được nhấn.

#### Buzzer
Buzzer được sử dụng để phát tín hiệu âm thanh đơn giản như thông báo hoặc phản hồi thao tác.

- Cực âm: nối với GND
- Cực dương: nối với chân PB12

Chân PB12 được cấu hình ở chế độ GPIO Output, cho phép bật/tắt buzzer theo yêu cầu của ứng dụng.

### Bảng tổng hợp kết nối chân (Pin Mapping)

| Thiết bị     | Tín hiệu | Chân MCU |
|--------------|----------|----------|
| Joystick     | URX      | PC4      |
| Joystick     | URY      | PC5      |
| Push Button A| Input    | PA0      |
| Push Button B| Input    | PC13     |
| Buzzer       | Output   | PB12 |

### Nguồn và mức điện áp
Vi điều khiển STM32 hoạt động ở mức logic 3.3V. Các chân GPIO được cấu hình phù hợp với mức điện áp của từng thiết bị ngoại vi. Joystick được cấp nguồn 5V, trong khi các tín hiệu analog đầu ra được đảm bảo nằm trong giới hạn cho phép của ADC trên vi điều khiển.


## Thiết kế phần mềm

Game hỗ trợ hai backend: STM32 backend (hiện chỉ hỗ trợ kit STM32F429ZIT6) và một cross-platform desktop backend dựa trên SDL. Trên backend STM32, project chỉ sử dụng các thư viện header CMSIS ([CMSIS](https://github.com/ARM-software/CMSIS_5), [cmsis_device_f4](https://github.com/STMicroelectronics/cmsis_device_f4/)) trên runtime. SDL backend cần có một gamepad để điều khiển, và mục đích chính là để thử nghiệm tiện lợi trên PC mà không cần phải dùng MCU.

Project **không** hỗ trợ và **không** phụ thuộc vào STM32CubeIDE, STM32CubeMX, TouchGFX và FreeRTOS. Quá trình biên dịch được thực hiện trực tiếp sử dụng GCC cross-compiler toolchain kết hợp với CMake. Project *có thể* hỗ trợ Clang (vì Clang phần lớn tương thích với GCC) nhưng điều này chưa được thử nghiệm.

Project gồm hai module chính. `ge-hal`là thư viện trừu tượng hóa phần cứng (HAL) của PC/STM32, cung cấp một API chung cho cả hai backend. `ge-app` là executable game, được viết một cách cross-platform dựa trên API của `ge-hal`.

### ge-hal

Do trên backend STM, project chỉ phụ thuộc vào các thư viện header CMSIS, phần lớn thư viện tương tác trực tiếp với các register trên STM. Phần code cho backend SDL khá đơn giản do SDL đã làm phần lớn công việc. API `ge-hal` expose một API chính bằng `ge::App` wrap cả chương trình và một API phụ trợ `ge::hal::gpu`, abstract cho DMA2D của thiết bị.

#### ge::App

`ge::App` đóng vai trò là điểm vào (entry point) của ứng dụng, chịu trách nhiệm:
- Khởi tạo toàn bộ hệ thống
- Điều phối vòng lặp chính (main loop)
- Xử lý input, update logic và render frame

Vòng lặp chính của ứng dụng được thiết kế theo mô hình game loop cổ điển:

1. Đọc input (joystick, button hoặc gamepad)
2. Cập nhật trạng thái game
3. Render framebuffer
4. Đồng bộ frame nếu cần thiết

Thiết kế này đảm bảo hành vi nhất quán giữa backend STM32 và backend SDL.

#### ge::hal::gpu

API `ge::hal::gpu` trừu tượng hóa các thao tác đồ họa cơ bản như:
- Clear framebuffer (`ge::hal::gpu::fill`)
- Blit vùng nhớ (`ge::hal::gpu::blit(_blend)`) kết hợp blending (nếu cần thiết)
- Load LUT cho texture dạng indexed (hiện không dùng, các hàm này chưa được thử nghiệm bao giờ)

Trên backend STM32, các thao tác này được ánh xạ trực tiếp sang phần cứng DMA2D, cho phép:
- Tăng tốc đáng kể các phép toán đồ họa
- Giảm tải cho CPU
- Đảm bảo tốc độ khung hình ổn định dù game render khá nhiều (`ge-app` chưa có dirty rect nên render hiện đang hơi kém hiệu quả)

Trên backend SDL, các thao tác tương ứng được triển khai bằng các primitive của SDL renderer, đảm bảo hành vi tương đương về mặt chức năng (nhưng không có GPU acceleration)

#### Quá trình khởi tạo trên backend STM32

##### 1. Enable FPU

Trước khi khởi tạo các thành phần còn lại, bộ xử lý dấu phẩy động (FPU) được kích hoạt thông qua các thanh ghi CPACR.

FPU cho phép tính toán với kiểu dữ liệu `float`, nhưng cần lưu ý rằng trên một hệ thống nhúng nên tránh sử dụng `float` quá nhiều.

##### 2. Cấu hình Clock hệ thống

Hệ thống clock được cấu hình để vi điều khiển hoạt động ở tần số 180 MHz bằng cách:
- Kích hoạt nguồn clock ngoài (HSE)
- Thiết lập PLL để nhân tần số lên mức mong muốn
- Cấu hình các prescaler cho bus AHB, APB1 và APB2

Việc cấu hình clock ở tần số cao nhằm:
- Đảm bảo hiệu năng đủ cho xử lý đồ họa
- Cung cấp băng thông cần thiết cho LTDC và DMA2D
- Đáp ứng yêu cầu timing của SDRAM ngoài

##### 3. Cấu hình Interrupt cho push button
Các chân GPIO kết nối với push button được cấu hình ở chế độ input kết hợp với ngắt ngoài (EXTI).

Quy trình gồm:
- Cấu hình GPIO ở chế độ input với pull-up nội bộ
- Ánh xạ chân GPIO tới line EXTI tương ứng
- Cấu hình trigger cạnh (falling edge)
- Enable ngắt trong NVIC

Cách tiếp cận này giúp hệ thống phản hồi với push button theo event thay vì polling, dẫn đến hiệu quả tốt hơn và code clean hơn.

##### 4. Khởi tạo SDRAM ngoài

Do đồ họa cần sử dụng nhiều tương đối bộ nhớ, trong khi trên STM chỉ có SRAM là 256 KB, cần activate thêm SDRAM (8MB) thông qua cấu hình FMC.

SDRAM ngoài được khởi tạo thông qua FMC với các bước:
- Enable clock cho FMC
- Cấu hình timing (CAS latency, refresh rate, row/column)
- Thực hiện chuỗi lệnh khởi tạo SDRAM theo datasheet
- Kiểm tra khả năng đọc/ghi bộ nhớ

SDRAM được sử dụng để:
- Lưu framebuffer chính
- Lưu các buffer trung gian cho quá trình render
- Giảm áp lực lên SRAM trong chip

---

##### 5. Khởi tạo LTDC và màn hình TFT
LTDC được cấu hình để xuất hình ảnh từ framebuffer trong SDRAM ra màn hình TFT (ILI9341/ILI9342).

Các bước chính bao gồm:
- Cấu hình các thông số timing (HSYNC, VSYNC, porch)
- Thiết lập độ phân giải và định dạng pixel
- Gán địa chỉ framebuffer trong SDRAM
- Enable layer hiển thị (hiện chỉ dùng 1 layer, thực chất nên dùng 2 layer compositing lên nhau thì sẽ hợp lý hơn).
- Gửi các lệnh SPI đến màn LCD để khởi động màn hình.

LTDC hoạt động độc lập với CPU, cho phép hiển thị liên tục mà không cần can thiệp từ phần mềm.

Để render, chương trình check vblank (một biến `volatile bool`, được set thành true khi LTDC thông báo qua ngắt).

##### 6. Khởi tạo DMA2D
DMA2D được cấu hình như một bộ tăng tốc đồ họa phần cứng.

Quá trình khởi tạo gồm: enable clock cho DMA2D

Mỗi hàm trong `ge::hal::gpu` gốm có các bước chính:
- Cấu hình chế độ hoạt động (memory-to-memory, fill)
- Thiết lập định dạng pixel nguồn và đích
- Đợi đến khi thực hiện xong (đơn giản hóa, thực chất có thể dùng ngắt/polling nhưng cái này gần như là sẽ làm code async)

DMA2D được sử dụng cho các thao tác đồ họa cơ bản như blit và fill, giúp giảm tải CPU và tăng tốc quá trình render.

##### 7. Khởi tạo ADC cho joystick
ADC được cấu hình để đọc tín hiệu analog từ joystick kết hợp với DMA.

Các bước thực hiện:
- Cấu hình các pin GPIO ở chế độ analog
- Cấu hình ADC ở chế độ continuous conversion
- Thiết lập sequence cho hai kênh ADC
- Cấu hình DMA để tự động ghi kết quả vào bộ nhớ
- Enable ADC và DMA

Cách cấu hình này cho phép đọc liên tục vị trí joystick mà không làm gián đoạn vòng lặp chính của game. Việc đọc input joystick chỉ gồm đọc từ 2 biến `volatile u16`.

##### 8. Khởi tạo audio (buzzer)
Buzzer được điều khiển một cách đơn giản thông qua GPIO pin. Quá trình khởi tạo như khởi tạo một pin bình thường (set output mode qua API `ge::hal::stm::Pin`).

### ge-app

#### Asset bundling

Quá trình bundle asset được thực hiện bằng các script Python (kèm một số thư viện đọc ảnh). Để đơn giản thì các file asset (ảnh, âm thanh, v.v.) được convert thành một cặp file header-source bằng C để có thể dễ dàng bundle trong project. Việc generate các file này được tự động hóa bằng CMake. Linker script sẽ đặt các mảng này trong đúng vị trí trên flash.

#### Kiến trúc hệ thống

Hệ thống được tổ chức thành các Scene (màn) để organize code. Các scene có thể chứa lẫn nhau, giúp cho việc compose các Scene dễ dàng hơn. Mỗi scene thừa kế từ lớp `Scene` gốc hoặc `ContainerScene`, và override các hàm tick (update), render, và các hàm nhận input.

Ngoài ra, `ge-app` còn có các hàm thư viện high-level như để render text, sinh số ngẫu nhiên (từ một seed khởi tạo lấy từ phần cứng), đo thời gian (cho phép tăng tốc hoặc dừng đồng hồ). blend màu, v.v. Những chức năng này không low-level để nằm trong `ge-hal`, do đó được đưa trực tiếp vào `ge-app`.
