#pragma once

#include <API/ARK/Ark.h>
#include <json.hpp>
#include <sqllite/sqlite_modern_cpp.h>

#pragma comment(lib, "sqlite3.lib")

namespace GamingOGsUpgrade {

	extern nlohmann::json config;

	bool ParseConfig(nlohmann::json& config);

	bool RegisterPlayer(uint64 steamID);
	void AddPlayerPoints(uint64 steamID, uint32 points);
	void SetPlayerVouchers(uint64 steamID, uint32 amount);
	void AddPlayerVouchers(uint64 steamID, uint32 amount);
	void UsePoints(uint64 steamID, uint32 points);
	void UseVouchers(uint64 steamID, uint32 amount);
	void InsertItem(uint64 itemID, uint32 tier, uint32 pointsToUpgrade);

	bool IsPlayerIntoDB(uint64 steamID);
	bool IsItemExists(uint64 itemID, uint32 tier);

	uint32 GetPlayerPoints(uint64 steamID);
	uint32 GetPlayerVouchers(uint64 steamID);
	uint32 GetPointsToUpgrade(uint64 itemID, uint32 tier);

	inline sqlite::database& DB();

}