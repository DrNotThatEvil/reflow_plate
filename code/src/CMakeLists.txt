set(REFLOW_PLATE_HEADER_LIST 
    "${reflow_plate_firmware_SOURCE_DIR}/include/t_sensor.h"
)

add_executable(reflow_plate
    main.c
    t_sensor.c
    ${REFLOW_PLATE_HEADER_LIST}
)

pico_enable_stdio_usb(reflow_plate 0)
pico_enable_stdio_uart(reflow_plate 1)

target_include_directories(reflow_plate PUBLIC ../include)
target_link_libraries(reflow_plate pico_stdlib hardware_adc)

pico_add_extra_outputs(reflow_plate)