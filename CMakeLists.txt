cmake_minimum_required(VERSION 3.29)
project(HID_Descriptor C)

set(CMAKE_C_STANDARD 11)

add_executable(HID_Descriptor main.c
        HID_Interpreter.c
        HID_Interpreter.h)
