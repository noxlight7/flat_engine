//
// Created by curo on 10.01.2025.
//

#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <player_avatar_generated.h>
#include <text_message_generated.h>
#include <flatbuffers/flatbuffers.h>

namespace flat_engine::network {
    class SharedSerializer {
        static std::vector<uint8_t> serializeBuilder(
            const flatbuffers::FlatBufferBuilder &builder) {
            return {
                builder.GetBufferPointer(),
                builder.GetBufferPointer() + builder.GetSize()
            };
        }
    public:
        static flatbuffers::FlatBufferBuilder& getBuilder() {
            thread_local flatbuffers::FlatBufferBuilder builder(1024);
            builder.Clear();
            return builder;
        }

        static std::vector<uint8_t> serializePlayerAvatar(
            const float x, const float y,
            const int cell_x, const int cell_y,
            const float speed_x, float speed_y
            ) {
            auto& builder = getBuilder();
            const auto _in_cell_coords = Game::Vec2F(x, y);
            const auto _cell_coords = Game::Vec2I(cell_x, cell_y);
            const auto _position = Game::Position(_in_cell_coords, _cell_coords);
            const auto _speed = Game::Vec2F(speed_x, speed_y);
            const auto _district_object = Game::DistrictObject(_position, _speed);
            const auto playerAvatar = CreatePlayerAvatarT(builder, &_district_object);
            builder.Finish(playerAvatar);
            return serializeBuilder(builder);
        }

        static std::vector<uint8_t> serializeTextMessage(const std::string& text) {
            auto& builder = getBuilder();
            const auto _text = builder.CreateString(text);
            const auto textMessage = Game::CreateTextMessage(builder, _text);
            builder.Finish(textMessage);
            return serializeBuilder(builder);
        }
    };
}


#endif //SERIALIZER_H
