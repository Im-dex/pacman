#pragma once

#include <cstdint>

#include "base.h"

namespace Pacman {

#ifdef NDEBUG
    #define PACMAN_CHECK_GL_ERROR()
    #define PACMAN_CHECK_ERROR(err)
    #define PACMAN_CHECK_ERROR2(err, msg)
#else
    #define PACMAN_CHECK_GL_ERROR()\
        ErrorHandler::CheckGLError(__FILE__, __LINE__);

    #define PACMAN_CHECK_ERROR(err)\
        ErrorHandler::CheckError((err), __FILE__, __LINE__);

    #define PACMAN_CHECK_ERROR2(err, msg)\
        ErrorHandler::CheckError((err), __FILE__, __LINE__, (msg));
#endif

struct ErrorHandler
{
	static void CheckGLError(const char* file, const size_t line);
	static void CleanGLErrors();
	static void CheckError(const bool err, const char* file, const size_t line,
						   const char* message = nullptr);
	static void Terminate();
};

} // Pacman namespace
