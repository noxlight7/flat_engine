//
// Created by curo on 01.02.2025.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include <cstdint>
#include <string>

class ITexture {
public:
    virtual ~ITexture() = default;

    [[nodiscard]] virtual int getWidth() const = 0;
    [[nodiscard]] virtual int getHeight() const = 0;
    [[nodiscard]] virtual std::string getName() const = 0;
};
#endif //TEXTURE_H
