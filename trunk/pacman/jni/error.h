#pragma once

#include <cstdint>

namespace Pacman {

#define PACMAN_CHECK_GL_ERROR()\
	ErrorHandler::CheckGLError(__FILE__, __LINE__);

#define PACMAN_CHECK_ERROR(err, errorCode)\
	ErrorHandler::CheckError((err), (errorCode), __FILE__, __LINE__);

#define PACMAN_CHECK_ERROR2(err, errorCode, msg)\
	ErrorHandler::CheckError((err), (errorCode), __FILE__, __LINE__, (msg));

enum class ErrorCode : uint32_t
{
	CompileShader = 0,
	CreateShader,
	AssignShaderSource,
	CreateShaderProgram,
	LinkShaderProgram,
	ShaderLocationSearch,
	BadFVF,
	BadPixelFormat,

	InvalidResult,
	BadArgument,
	ContainerInsert,
	JClassNotFound,
	JFuncNotFound,
	JNICallFailed,
	AndroidAPICallFailed,
	TimerFailed,
	CreateThreadFailed,
	JoinThreadFailed,
	MutexError,
	BadFormat,
	UnsupportedDevice
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
