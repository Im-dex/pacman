#include "error.h"
#include "base.h"

#include <GLES2/gl2.h>
#include <exception>
#include <vector>
#include <string>
#include <sstream>

namespace Pacman {

const char* kErrorDescriptions[] =
{
	"Shader compilation failed",
	"Shader creation failed",
	"Shader source assign failed",
	"Shader program creation failed",
	"Shader program linking failed",
	"Shader attribute or uniform location search failed",
	"Bad FVF format",
	"Bad pixel format",

	"Invalid result",
	"Bad argument",
	"Insertion into the container failed",
	"Java class not found",
	"Java function not found",
	"Android API call failed"
};

class BaseException : public std::exception
{
protected:
	BaseException() throw() = delete;
	BaseException(const char* file, const size_t line) throw()
				: std::exception(), mFile(file), mLine(line) {}

	BaseException(const BaseException&) = default;
	virtual ~BaseException() throw() {}

	BaseException& operator= (const BaseException&) = default;

	const std::string GetErrorFileLine() const
	{
		std::stringstream result("");
		result << "at: " << mFile << ":" << mLine;
		return result.str();
	}

private:

	std::string mFile;
	size_t mLine;
};

class Exception : public BaseException
{
public:
	Exception() throw() = delete;
	Exception(const ErrorCode errorCode, const char* file, const size_t line, const char* message) throw()
			: BaseException(file, line), mErrorCode(errorCode), mMessage(message) {}

	Exception(const Exception&) = default;
	virtual ~Exception() throw() {}

	Exception& operator= (const Exception&) = default;

	virtual const char* what() const throw()
	{
		std::stringstream result("");
		result << kErrorDescriptions[static_cast<uint32_t>(mErrorCode)] << " " << GetErrorFileLine() << std::endl;
		result << mMessage;
		return result.str().c_str();
	}

private:

	ErrorCode mErrorCode;
	std::string mMessage;
};

class GLException : public BaseException
{
public:
	GLException() throw() = delete;
	GLException(const std::vector<GLint>& errorCodes, const char* file, const size_t line) throw()
			: BaseException(file, line), mErrorCodes(errorCodes) {}

	GLException(const GLException&) = default;
	virtual ~GLException() throw() {}

	GLException& operator= (const GLException&) = default;

	virtual const char* what() const throw()
	{
		std::stringstream result("");
		for (GLint error : mErrorCodes)
		{
			result << GetGLErrorString(error) << std::endl;
		}

		result << GetErrorFileLine() << std::endl;
		return result.str().c_str();
	}

private:

	const char* GetGLErrorString(const GLint errorCode) const
	{
		switch(errorCode)
		{
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		default:
			return "NOT_A_GL_ERROR";
		}
	}

	std::vector<GLint> mErrorCodes;
};

//===========================================================================================================================

const char* FixEmptyString(const char* string)
{
	return (string == nullptr) ? "" : string;
}

void ErrorHandler::CheckGLError(const char* file, const size_t line)
{
	std::vector<GLint> errors;
	for (GLint err = glGetError(); err != GL_NO_ERROR; err = glGetError())
	{
		errors.push_back(err);
	}

	if (errors.size() > 0)
	{
		throw GLException(errors, file, line);
	}
}

void ErrorHandler::CleanGLErrors()
{
	while (glGetError() != GL_NO_ERROR);
}

void ErrorHandler::CheckError(const bool err, const ErrorCode errorCode, const char* file, const size_t line, const char* message)
{
	if (!err)
	{
		throw Exception(errorCode, file, line, FixEmptyString(message));
	}
}

void ErrorHandler::Terminate(JNIEnv* env)
{
	jclass cls = env->FindClass("com/imdex/pacman/NativeLib");
	if (cls != nullptr)
	{
		jmethodID mid = env->GetStaticMethodID(cls, "terminateApplication", "()V");
		if (mid != nullptr)
		{
			env->CallStaticVoidMethod(cls, mid);
			return;
		}
	}

	LOGE("Can't terminate application");
}

} // Pacman namespace
