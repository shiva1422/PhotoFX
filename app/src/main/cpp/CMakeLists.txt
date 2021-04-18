cmake_minimum_required(VERSION 3.10.2)
add_library(glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
find_library(log-lib log)
set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
#freetype
set(FREETYPE E:/androidlibs/freetype)
include_directories(${FREETYPE}/include)
#oboe
#set(OBOE  F:/androidlibs/oboe)
#add_subdirectory(${OBOE} ./oboe)
#include_directories(${OBOE}/include)
#firebase
#set(FIREBASE_CPP_SDK_DIR F:/androidlibs/firebasecppsdk)
#add_subdirectory(${FIREBASE_CPP_SDK_DIR} bin/ EXCLUDE_FROM_ALL)
#include_directories(${FIREBASE_CPP_SDK_DIR}/include ${FIREBASE_CPP_SDK_DIR}/libs/android/${CMAKE_ANDROID_ARCH_ABI}/C++)
#set(firebase_libs firebase_analytics firebase_admob firebase_app)
#
add_library(main SHARED Main.cpp PhotoApp.cpp Commons.cpp CustomLooperEvents.cpp AppUI.cpp)
add_library(graphics STATIC Graphics.cpp UI.cpp  RecyclerView.cpp TextView.cpp gpgpu.cpp Shapes.cpp )
add_library(offscreenrenders STATIC OffScreenRender.cpp OffScreenRender.h )
add_library(events STATIC Touch.cpp Callbacks.cpp)
add_library(javacalls STATIC JavaCalls.cpp JavaCalls.h)
#add_library(jnimeths STATIC JniMethods.cpp)
add_library(editing STATIC Editing.cpp Layers.cpp EditableImage.cpp Histogram.cpp FilterProperties.cpp)
add_library(eventhandling STATIC EventHandling.cpp)
add_library(text STATIC Text.cpp)
add_library(time STATIC Time.cpp)

#Image Processing
add_library(auto STATIC Auto.cpp)

#text
add_library(ft2 STATIC IMPORTED)
set_target_properties(ft2 PROPERTIES IMPORTED_LOCATION ${FREETYPE}/${CMAKE_ANDROID_ARCH_ABI}/libft.a)
#
target_link_libraries(main events glue text javacalls graphics  eventhandling editing offscreenrenders auto ft2 jnigraphics GLESv3 EGL android ${log-lib})
#add_library(jnilib SHARED JniMethods.cpp Commons.cpp)
#target_link_libraries(jnilib ${log-lib} graphics GLESv3 EGL glue android)
