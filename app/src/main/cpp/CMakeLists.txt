cmake_minimum_required(VERSION 3.22.1)

project(PhotoFX)

#common directory K_DIR
set(K_DIR ${CMAKE_SOURCE_DIR}/../../../..)

set(KSAPP_DIR ${K_DIR}/KSApplication)

set(CORE_DIR ${K_DIR}/Core)

include_directories(${KSAPP_DIR} ${CORE_DIR} ${KSAPP_DIR}/Graphics ${KSAPP_DIR}/KSLogger)

add_subdirectory(${KSAPP_DIR}/KSApp ./ksapp)


#ksmain Application
set(ANDROID_SRCS
        AMain.cpp
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
        PhotoFXApp.cpp
        )

add_library(main SHARED ${ANDROID_SRCS})


#This project deps

target_link_libraries(main ksapp)

#link_directories(main PUBLIC ${FT_DIR}/CMAKE_ANDROID_ARCH_ABI)






