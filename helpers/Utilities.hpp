#pragma once
#define NOMINMAX

#include <Windows.h>
#include <string>
#include <initializer_list>
#include "../valve_sdk/sdk.hpp"

namespace Utilities
{
	std::vector<char> HexToBytes(const std::string& hex);
	std::string BytesToString(unsigned char* data, int len);
	std::uint8_t* PatternScan(void* module, const char* signature);
	std::vector<std::string> Split(const std::string& str, const char* delim);
	unsigned int FindInDataMap(datamap_t * pMap, const char * name);
	void AttachConsole();
    void DetachConsole();
	bool IsBattleRoyale();
    bool ConsolePrint(const char* fmt, ...);
    char ConsoleReadKey();
    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules);
    void SetClantag(const char* tag);
    void SetName(const char* name);
    void RankRevealAll();
	float GetLerpTime();
	void autoBhop(CUserCmd* cmd);
	bool IsTickValid(float SimulationTime, float MaxTime);
}