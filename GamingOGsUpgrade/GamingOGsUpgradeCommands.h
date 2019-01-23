#pragma once

#include <fstream>
#include <API\ARK\Ark.h>
#include "Database.h"

namespace GamingOGsUpgrade {

	void LoadConfig() {
		const std::string filename = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/GamingOGsUpgrade/config.json";
		std::ifstream file(filename);
		if(!file.is_open()) {
			throw std::runtime_error("Can't open config file");
		}
		file >> config;
		ParseConfig(config["GamingOGsUpgrade"]["Items"]);
		file.close();
	}

	inline FString MakeMessage(const std::string& label) {
		std::string msg = config["GamingOGsUpgrade"]["Messages"][label];
		return FString(ArkApi::Tools::Utf8Decode(msg).c_str());
	}

	void ChatUpgradeCommand(AShooterPlayerController* shooter_controller, FString* message, int mode) {
	}

	void ChatRedeemVoucherCommand(AShooterPlayerController* shooter_controller, FString* message, int mode) {
	}

	void ChatCheckVoucherCommand(AShooterPlayerController* shooter_controller, FString* message, int mode) {
		uint64 steamID = ArkApi::GetApiUtils().GetSteamIdFromController(shooter_controller);
		uint32 vouchers = GetPlayerVouchers(steamID);
		ArkApi::GetApiUtils().SendChatMessage(shooter_controller, MakeMessage("ServerName"), *MakeMessage("CheckVouchersReply"), vouchers);
	}

	void ChatGambleCommand(AShooterPlayerController* shooter_controller, FString* message, int mode) {
	}

	void RCONReloadCommand(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/) {
		FString reply;
		try {
			LoadConfig();
		}
		catch(const std::runtime_error& e) {
			reply = FString::Format("Reload failed, error: {}", e.what());
			rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
			return;
		}
		reply = "OK";
		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
	}

	void RCONGiveVoucherCommand(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/) {
		TArray<FString> args;
		FString reply;

		auto len = rcon_packet->Body.ParseIntoArray(args, L" ", true);
		if(len < 3) {
			reply = "Invalid command arguments";
			rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
			return;
		}

		uint64 steamID = std::stoull(args[1].ToString());
		uint32 vouchers = std::stoul(args[2].ToString());

		auto* player = ArkApi::GetApiUtils().FindPlayerFromSteamId(steamID);
		if(player == nullptr) {
			reply = "Player not found";
			rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
			return;
		}

		if(!IsPlayerIntoDB(steamID)) {
			RegisterPlayer(steamID);
		}
		AddPlayerVouchers(steamID, vouchers);
		reply = "OK";
		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
		ArkApi::GetApiUtils().SendChatMessage(player, MakeMessage("ServerName"), *MakeMessage("AddVouchers"), vouchers);
	}

	void RCONSetVoucherCommand(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/) {
		TArray<FString> args;
		FString reply;

		auto len = rcon_packet->Body.ParseIntoArray(args, L" ", true);
		if(len < 3) {
			reply = "Invalid command arguments";
			rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
			return;
		}

		uint64 steamID = std::stoull(args[1].ToString());
		uint32 vouchers = std::stoul(args[2].ToString());

		auto* player = ArkApi::GetApiUtils().FindPlayerFromSteamId(steamID);
		if(player == nullptr) {
			reply = "Player not found by steamID";
			rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
			return;
		}

		if(!IsPlayerIntoDB(steamID)) {
			RegisterPlayer(steamID);
		}
		SetPlayerVouchers(steamID, vouchers);
		reply = "OK";
		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
		ArkApi::GetApiUtils().SendChatMessage(player, MakeMessage("ServerName"), *MakeMessage("SetVouchers"), vouchers);
	}

	void RCONAddPointsCommand(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/) {
		TArray<FString> args;
		FString reply;

		int32 len = rcon_packet->Body.ParseIntoArray(args, L" ", true);
		if(len < 3) {
			reply = "Invalid command arguments";
			rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
			return;
		}

		uint64 steamID = std::stoull(args[1].ToString());
		uint32 points = std::stoul(args[2].ToString());

		auto* player = ArkApi::GetApiUtils().FindPlayerFromSteamId(steamID);
		if(player == nullptr) {
			reply = "Player not found by steamID";
			rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
			return;
		}

		if(!IsPlayerIntoDB(steamID)) {
			RegisterPlayer(steamID);
		}
		AddPlayerPoints(steamID, points);
		reply = "OK";
		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
		ArkApi::GetApiUtils().SendChatMessage(player, MakeMessage("ServerName"), *MakeMessage("AddPoints"), points);
	}

	void AddCommands() {
		ArkApi::GetCommands().AddChatCommand("upgrade", &ChatUpgradeCommand);
		ArkApi::GetCommands().AddChatCommand("reedemvoucher", &ChatRedeemVoucherCommand);
		ArkApi::GetCommands().AddChatCommand("checkvoucher", &ChatCheckVoucherCommand);
		ArkApi::GetCommands().AddChatCommand("gamble", &ChatGambleCommand);

		ArkApi::GetCommands().AddRconCommand("upgrade.reload", &RCONReloadCommand);
		ArkApi::GetCommands().AddRconCommand("givevoucher", &RCONGiveVoucherCommand);
		ArkApi::GetCommands().AddRconCommand("setvoucher", &RCONSetVoucherCommand);
		ArkApi::GetCommands().AddRconCommand("addpoints", &RCONAddPointsCommand);
	}

	void RemoveCommands() {
		ArkApi::GetCommands().RemoveChatCommand("upgrade");
		ArkApi::GetCommands().RemoveChatCommand("reedemvoucher");
		ArkApi::GetCommands().RemoveChatCommand("checkvoucher");
		ArkApi::GetCommands().RemoveChatCommand("gamble");

		ArkApi::GetCommands().RemoveRconCommand("upgrade.reload");
		ArkApi::GetCommands().RemoveRconCommand("givevoucher");
		ArkApi::GetCommands().RemoveRconCommand("setvoucher");
		ArkApi::GetCommands().RemoveRconCommand("setpoints");
	}

}