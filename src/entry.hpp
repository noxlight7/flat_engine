#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define NOMINMAX

#if defined (_MSC_VER)
#define ALWAYS_INLINE __forceinline
#elif defined (__GNUC__) && __GNUC__ > 3
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#define ALWAYS_INLINE inline
#endif

#ifdef _DEBUG
#define PUSH_EXCEPTION(err,...) \
	{\
		printf(err, __VA_ARGS__);\
		return;\
	}
#else
#define PUSH_EXCEPTION(err,...) \
	return;
#endif

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <memory>
#include <sstream>
#include <fstream>

#include <glm/glm.hpp>
#include <opengl/src/glew.h>
#include <glfw/src/glfw3.h>

#include "factory/definitions.hpp"

#include "utils/hash.hpp"

//#include "factory/engine.hpp"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glfw3dll.lib")