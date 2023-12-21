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

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <memory>

#include <glm/glm.hpp>
#include <opengl/src/glew.h>
#include <glfw/src/glfw3.h>

#include "factory/definitions.hpp"

#include "factory/engine.hpp"
#pragma comment (lib, "opengl32.lib")
//#pragma comment (lib, "glew32.lib")
//#pragma comment (lib, "glew32s.lib")
#pragma comment (lib, "glfw3dll.lib")