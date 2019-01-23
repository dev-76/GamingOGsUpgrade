#include "Database.h"

namespace GamingOGsUpgrade {

	nlohmann::json config;

	bool ParseConfig(nlohmann::json& items) {
		auto& db = DB();

		try {
			db << "drop table if exists Items;";

			db << "create table if not exists Players ("
				"Id integer primary key autoincrement not null,"
				"SteamId integer not null,"
				"Points integer default 0,"
				"Vouchers integer default 0);";
			db << "create table Items ("
				"Id integer primary key autoincrement not null,"
				"ItemID integer default 0,"
				"Tier integer default 0,"
				"Points integer default 0);";
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
			return false;
		}

		for(const auto& item : items) {
			InsertItem(item["ItemID"], item["Tier"], item["Points"]);
		}

		return true;
	}

	bool RegisterPlayer(uint64 steamID) {
		if(IsPlayerIntoDB(steamID)) {
			return true;
		}
		auto& db = DB();
		try {
			db << "INSERT INTO Players (SteamID) VALUES (?);" << steamID;
			return true;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
			return false;
		}
	}

	void AddPlayerPoints(uint64 steamID, uint32 points) {
		auto& db = DB();
		try {
			db << "UPDATE Players SET Points = Points + ? WHERE SteamID = ?;" << points << steamID;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
		}
	}

	void SetPlayerVouchers(uint64 steamID, uint32 amount) {
		auto& db = DB();
		try {
			db << "UPDATE Players SET Vouchers = ? WHERE SteamID = ?;" << amount << steamID;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
		}
	}

	void AddPlayerVouchers(uint64 steamID, uint32 amount) {
		auto& db = DB();
		try {
			db << "UPDATE Players SET Vouchers = Vouchers + ? WHERE SteamID = ?;" << amount << steamID;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
		}
	}

	void UsePoints(uint64 steamID, uint32 points) {
		auto& db = DB();
		try {
			db << "UPDATE Players SET Points = Points - ? WHERE SteamID = ?;" << points << steamID;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
		}
	}

	void UseVouchers(uint64 steamID, uint32 amount) {
		auto& db = DB();
		try {
			db << "UPDATE Players SET Points = Vouchers - ? WHERE SteamID = ?;" << amount << steamID;
		}
		catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
		}
	}

	bool IsPlayerIntoDB(uint64 steamID) {
		auto& db = DB();
		try {
			int count = 0;
			db << "SELECT count(1) FROM Players WHERE SteamID = ?;" << steamID >> count;
			return count != 0;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
			return false;
		}
	}

	bool IsItemExists(uint64 itemID, uint32 tier) {
		auto& db = DB();
		try {
			int count = 0;
			db << "SELECT count(1) FROM Items WHERE ItemID = ? AND Tier = ?;" << itemID << tier >> count;
			return count != 0;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
			return false;
		}
	}

	uint32 GetPlayerPoints(uint64 steamID) {
		if(!IsPlayerIntoDB(steamID)) {
			return 0;
		}

		auto& db = DB();
		try {
			int points = 0;
			db << "SELECT Points FROM Players WHERE SteamID = ?;" << steamID >> points;
			return points;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
			return 0;
		}
	}

	uint32 GetPlayerVouchers(uint64 steamID) {
		if(!IsPlayerIntoDB(steamID)) {
			return 0;
		}

		auto& db = DB();
		try {
			int vouchers = 0;
			db << "SELECT Vouchers FROM Players WHERE SteamID = ?;" << steamID >> vouchers;
			return vouchers;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
			return 0;
		}
	}

	uint32 GetPointsToUpgrade(uint64 itemID, uint32 tier) {
		if(!IsItemExists(itemID, tier)) {
			return 0;
		}

		auto& db = DB();
		try {
			int points = 0;
			db << "SELECT Points FROM Items WHERE ItemID = ? AND Tier = ?;" << itemID << tier >> points;
			return points;
		} catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
			return 0;
		}
	}

	void InsertItem(uint64 itemID, uint32 tier, uint32 pointsToUpgrade) {
		auto& db = DB();
		try {
			db << "INSERT INTO Items (ItemID, Tier, Points) VALUES (?, ?, ?);" << itemID << tier << pointsToUpgrade;
		}
		catch(const sqlite::sqlite_exception& e) {
			Log::GetLog()->error("[{}:{}] Database error: {}", __FILE__, __FUNCTION__, e.what());
		}
	}

	sqlite::database& DB() {
		static sqlite::database db(ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/GamingOGsUpgrade/GamingOGsUpgrade.db");
		return db;
	}

}
