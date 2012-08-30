LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := nativelib
LOCAL_CFLAGS    := -Werror -std=gnu++0x -Wno-psabi -fexceptions
LOCAL_SRC_FILES := main.cpp\
                   renderer.cpp\
                   sprite.cpp\
                   spritesheet.cpp\
                   color.cpp\
                   vertex_buffer.cpp\
                   error.cpp\
                   shader.cpp\
                   shader_program.cpp\
                   texture.cpp\
                   asset_manager.cpp\
                   scene_node.cpp\
                   scene_manager.cpp\
                   timer.cpp\
                   update_thread.cpp\
                   frame_animator.cpp\
                   map.cpp\
                   font_manager.cpp\
                   jni_utility.cpp\
                   json_helper.cpp\
                   engine.cpp\
				   mutex.cpp\
                   input_manager.cpp\
                   json/json_reader.cpp\
                   json/json_value.cpp\
                   json/json_writer.cpp\
				   game/game.cpp\
				   game/actor.cpp\
				   game/actors_manager.cpp\
				   game/pacman.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
