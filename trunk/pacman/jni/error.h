#pragma once

#include <cstdint>

#include "base.h"

namespace Pacman {

#define PACMAN_CHECK_GL_ERROR()\
	ErrorHandler::CheckGLError(__FILE__, __LINE__);

#define PACMAN_CHECK_ERROR(err, errorCode)\
	ErrorHandler::CheckError((err), (errorCode), __FILE__, __LINE__);

#ifdef NDEBUG
    #define PACMAN_CHECK_ERROR2(err, errorCode, msg)\
	    ErrorHandler::CheckError((err), (errorCode), __FILE__, __LINE__, "");
#else
    #define PACMAN_CHECK_ERROR2(err, errorCode, msg)\
        ErrorHandler::CheckError((err), (errorCode), __FILE__, __LINE__, (msg));
#endif

enum class ErrorCode : uint32_t
{
	CompileShader = 0,
	CreateShader,
	AssignShaderSource,
	CreateShaderProgram,
	LinkShaderProgram,
	ShaderLocationSearch,
	BadPixelFormat,

	InvalidResult,
    InvalidState,
	BadArgument,
    BadCast,
	ContainerInsert,
	JClassNotFound,
	JFuncNotFound,
	JNICallFailed,
	AndroidAPICallFailed,
	TimerFailed,
	MutexError,
	BadFormat
};

struct ErrorHandler
{
	static void CheckGLError(const char* file, const size_t line);
	static void CleanGLErrors();
	static void CheckError(const bool err, const ErrorCode errorCode, const char* file, const size_t line,
						   const char* message = nullptr);
	static void Terminate();
};

} // Pacman namespace
