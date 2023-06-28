SET(REFLOW_PLATE_HEADER_LIST 
    "${reflow_plate_firmware_SOURCE_DIR}/include/sensor.h"
)

add_executable(reflow_plate
    main.c
    ${REFLOW_PLATE_HEADER_LIST}
)

target_include_directories(reflow_plate PUBLIC ../include)
target_link_libraries(reflow_plate pico_stdlib)