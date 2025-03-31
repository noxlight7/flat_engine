//
// Created by curo on 22.03.2025.
//

#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include <cstdint>

#include "session.h"

namespace flat_engine::network {
	class ISessionController {
	public:
		virtual ~ISessionController() = default;
		virtual void onConnect(uint32_t session_id) = 0;
		virtual void onDisconnect(uint32_t session_id) = 0;

		virtual std::unique_ptr<IGameData> generateStartData() = 0;

		// [[nodiscard]] GameData& getData(uint32_t session_id) {}
	};
}
#endif //GAME_SESSION_H
