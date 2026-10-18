# FreeRTOS Kernel V11.1.0 build target
#
# Sources are trimmed down to what this project needs:
#   - the kernel core sources
#   - the GCC Cortex-M3 port (STM32F103)
#   - the heap_4 allocator
#
# FreeRTOSConfig.h is supplied by the application through the
# freertos_config INTERFACE target defined in the top-level CMakeLists.txt.

add_library(freertos_kernel STATIC
    croutine.c
    event_groups.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c

    portable/GCC/ARM_CM3/port.c
    portable/MemMang/heap_4.c
)

# Defines freertos_kernel_include (FreeRTOS/include + freertos_config)
add_subdirectory(include)

target_include_directories(freertos_kernel
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/portable/GCC/ARM_CM3
)

target_link_libraries(freertos_kernel
    PUBLIC
        freertos_kernel_include
)
