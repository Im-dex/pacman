LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := nativelib
LOCAL_CFLAGS    := -Werror -std=gnu++0x -Wno-psabi -fexceptions
LOCAL_SRC_FILES := main.cpp\
				   renderer.cpp\
				   sprite.cpp\
				   color.cpp\
				   vertex_buffer.cpp\
				   error.cpp\
				   shader.cpp\
				   shader_program.cpp\
				   texture.cpp\
				   asset_manager.cpp\
				   drawable.cpp\
				   scene_node.cpp\
				   scene_manager.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
