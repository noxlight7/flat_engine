//
// Created by curo on 07.02.2025.
//

#ifndef DISPLAY_OBJECTS_H
#define DISPLAY_OBJECTS_H
#include <unordered_map>

#include "texture_manager.hpp"

class DisplayObjects {
    std::unordered_map<uint32_t, TextureID> buffer;

public:
    void add(uint32_t type_id, const TextureID texture) {
        buffer[type_id] = texture;
    }

    TextureID get(const uint32_t type_id) const {
        if (const auto it = buffer.find(type_id); it != buffer.end())
            return it->second;
        return 0;
    }

    TextureID operator[](const uint32_t type_id) const {
        return get(type_id);
    }
};

#endif //DISPLAY_OBJECTS_H
