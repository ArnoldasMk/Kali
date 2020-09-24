#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "../ImGUI/imgui.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/color.h"

namespace Util
{
	extern const char *logFileName;

	void Log(char const * const format, ...);
	std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace);
	void StdReplaceStr(std::string&, const std::string&, const std::string&);
	const char* PadStringRight(std::string text, size_t value);
	bool Contains(const std::string &word, const std::string &sentence);
	std::string ToLower(std::string str);
	std::string ToUpper(std::string str);
	std::string WstringToString(std::wstring wstr);
	std::wstring StringToWstring(std::string str);
	ImColor GetRainbowColor(float speed);
	Color GetHealthColor(int hp);
	Color GetHealthColor(C_BasePlayer* player);
	int RandomInt(int min, int max);
	long GetEpochTime();

	template <typename T>
	T GetValueByKey(std::vector<std::pair<int, T>> vec, int key)
	{
		for (auto i : vec)
			if (i.first == key)
				return i.second;

		return "";
	}

	template <typename K, typename V>
	K MapReverseSearchOrDefault(std::map<K, V> const* _map, V value, K fallback)
	{
		auto _pair = std::find_if(_map->begin(), _map->end(),
				[value](const std::pair<K, V>& pair) {
					return pair.second == value;
				});

		if (_pair == _map->end())
			return fallback;

		return _pair->first;
	}

	template <typename K, typename V>
	V MapSearchOrDefault(std::map<K, V> const* _map, K key, V fallback)
	{
		auto result = _map->find(key);
		if (result == _map->end())
			return fallback;

		return result->second;
	}

	template<typename T>
	struct IntHash
	{
		size_t operator()(const T& t) const noexcept
		{
			return std::hash<int>()((int) t);
		}
	};

	template<typename N_t = size_t, N_t N>
	constexpr N_t StrLen(const char (&string_literal)[N])
	{
		return N - 1;
	}

	template<typename N_t = size_t, N_t N>
	constexpr N_t StrLen(const wchar_t (&string_literal)[N])
	{
		return N - 1;
	}

	template<typename N_t = size_t, typename T, N_t N>
	constexpr N_t LengthOf(const T (&arr)[N])
	{
		return N;
	}
}
