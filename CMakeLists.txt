cmake_minimum_required(VERSION 3.28)

#set(PICO_BOARD pico2_w CACHE STRING "Board type")
#set(PICO_PLATFORM rp2350-riscv CACHE STRING "Pico Platform")
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts -fcommon -fno-rtti -fno-exceptions")

include(pico_sdk_import.cmake)

project(pio_a4988_stepper C CXX ASM)

pico_sdk_init()

include(FetchContent)

FetchContent_Declare(stepper
  GIT_REPOSITORY "https://github.com/jancumps/pio_stepper_lib.git"
  GIT_TAG "origin/main"
)
FetchContent_MakeAvailable(stepper)

# start DRIVER IC specific
FetchContent_Declare(stepper_driver
  GIT_REPOSITORY "https://github.com/jancumps/stepper_driver_lib.git"
  GIT_TAG "origin/main"
)
FetchContent_MakeAvailable(stepper_driver)

FetchContent_Declare(pico_a4988
  GIT_REPOSITORY "https://github.com/jancumps/pico_a4988_lib.git"
  GIT_TAG "origin/main"
  # prevent makefile execution (if any)
  SOURCE_SUBDIR =
)
FetchContent_MakeAvailable(pico_a4988)

add_library(driver)
target_sources(driver
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${stepper_driver_SOURCE_DIR}/source/stepper_driver.cpp
        ${pico_a4988_SOURCE_DIR}/source/a4988_pico.cpp
)
target_link_libraries(driver $<COMPILE_ONLY:hardware_gpio>)
# end DRIVER IC specific

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
)
target_sources(${CMAKE_PROJECT_NAME}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
)

target_link_libraries( ${CMAKE_PROJECT_NAME}
        pico_stdlib
        hardware_gpio
# start DRIVER IC specific
        driver
# end DRIVER IC specific
        stepper
)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME} )