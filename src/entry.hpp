#pragma once

#define STB_IMAGE_IMPLEMENTATION

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

#include <slm/slmath.h>
#include <glfw/src/glfw3.h>

#include "factory/engine.hpp"

#include "entity/base_entity.hpp"
#include "entity/base_effect.hpp"

#pragma once (lib, "glfw3dll.lib")