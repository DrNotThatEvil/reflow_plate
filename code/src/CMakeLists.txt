set(REFLOW_PLATE_HEADER_LIST 
    "${reflow_plate_firmware_SOURCE_DIR}/include/t_sensor.h"
)

add_executable(reflow_plate
    main.c
    t_sensor.c
    ${REFLOW_PLATE_HEADER_LIST}
)

target_include_directories(reflow_plate
    PUBLIC ../include ../u8g2/csrc 
)

file(GLOB U8G2_SRC ../u8g2/csrc/*.c)
add_library(u8g2 ${U8G2_SRC})

target_link_libraries(reflow_plate pico_stdlib hardware_adc hardware_spi u8g2)
target_link_options(reflow_plate PUBLIC -Xlinker --print-memory-usage)

pico_add_extra_outputs(reflow_plate)

pico_enable_stdio_usb(reflow_plate 0)
pico_enable_stdio_uart(reflow_plate 1)