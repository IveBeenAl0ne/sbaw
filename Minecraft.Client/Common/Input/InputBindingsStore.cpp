#include "stdafx.h"
#include "InputBindingsStore.h"
#include "InputBindings.h"

#ifdef _WINDOWS64

#include "nlohmann/json.hpp"

#include <fstream>
#include <string>

namespace
{
	const int kFormatVersion = 1;

	// Same convention as CSaveGame, so this lands beside the worlds.
	std::string GetBindingsPath()
	{
		char dirName[MAX_PATH] = { 0 };
		GetCurrentDirectoryA(sizeof(dirName), dirName);

		char path[MAX_PATH] = { 0 };
		sprintf(path, "%s/Windows64/GameHDD/", dirName);
		CreateDirectoryA(path, 0);

		strcat(path, "keybindings.json");
		return std::string(path);
	}

	const char *SourceToString(EBindSource src)
	{
		switch (src)
		{
		case eBind_Key:         return "key";
		case eBind_MouseButton: return "mouse";
		case eBind_MouseWheel:  return "wheel";
		default:                return nullptr;
		}
	}

	EBindSource SourceFromString(const std::string &s)
	{
		if (s == "key")   return eBind_Key;
		if (s == "mouse") return eBind_MouseButton;
		if (s == "wheel") return eBind_MouseWheel;
		return eBind_None;
	}

	bool IsValidCode(EBindSource src, int code)
	{
		switch (src)
		{
		case eBind_Key:         return code >= 1 && code <= 255;
		case eBind_MouseButton: return code >= 0 && code <= 2;
		case eBind_MouseWheel:  return code == 1 || code == -1;
		default:                return false;
		}
	}
}

void InputBindingsStore::Load()
{
	// Unconditionally first: anything the file omits, or gets wrong, keeps its default.
	InputBindings::ResetToDefaults();

	const std::string path = GetBindingsPath();

	std::ifstream in(path.c_str());
	if (!in.is_open())
		return; // No file yet. Defaults stand.

	try
	{
		nlohmann::json root;
		in >> root;

		if (!root.is_object() || !root.contains("bindings") || !root["bindings"].is_object())
		{
			app.DebugPrintf("keybindings.json has no bindings object - using defaults\n");
			return;
		}

		const nlohmann::json &bindings = root["bindings"];

		for (nlohmann::json::const_iterator it = bindings.begin(); it != bindings.end(); ++it)
		{
			const int action = InputBindings::GetActionByName(it.key().c_str());
			if (action < 0)
				continue; // Unknown action name, likely from a newer or older build.

			if (!it.value().is_array())
				continue;

			// The file supplies the whole slot list for an action, so drop the defaults
			// for it first. Otherwise removing a binding by hand could never take effect.
			for (int s = 0; s < eBindSlot_Count; ++s)
				InputBindings::Clear(action, s);

			int slot = 0;
			for (nlohmann::json::const_iterator e = it.value().begin();
			     e != it.value().end() && slot < eBindSlot_Count; ++e)
			{
				if (!e->is_object() || !e->contains("src") || !e->contains("code"))
					continue;
				if (!(*e)["src"].is_string() || !(*e)["code"].is_number_integer())
					continue;

				const EBindSource src  = SourceFromString((*e)["src"].get<std::string>());
				const int         code = (*e)["code"].get<int>();

				if (src == eBind_None || !IsValidCode(src, code))
					continue;

				InputBindings::Set(action, slot, Binding(src, code));
				++slot;
			}
		}
	}
	catch (const nlohmann::json::exception &e)
	{
		// Covers parse_error, type_error, out_of_range and other_error - they all derive
		// from json::exception. A corrupt file must not stop the game booting.
		app.DebugPrintf("keybindings.json could not be read (%s) - using defaults\n", e.what());
		InputBindings::ResetToDefaults();
	}
}

void InputBindingsStore::Save()
{
	const std::string path = GetBindingsPath();

	try
	{
		nlohmann::json root;
		root["version"] = kFormatVersion;

		nlohmann::json bindings = nlohmann::json::object();

		int count = 0;
		const int *actions = InputBindings::GetBindableActions(count);

		for (int n = 0; n < count; ++n)
		{
			const int   action = actions[n];
			const char *name   = InputBindings::GetActionName(action);
			if (name == nullptr)
				continue;

			nlohmann::json slots = nlohmann::json::array();

			for (int s = 0; s < eBindSlot_Count; ++s)
			{
				const Binding &b = InputBindings::Get(action, s);
				const char *src = SourceToString(b.src);
				if (src == nullptr)
					continue;

				nlohmann::json entry;
				entry["src"]  = src;
				entry["code"] = b.code;
				slots.push_back(entry);
			}

			bindings[name] = slots;
		}

		root["bindings"] = bindings;

		std::ofstream out(path.c_str(), std::ios::trunc);
		if (!out.is_open())
		{
			app.DebugPrintf("keybindings.json could not be opened for writing\n");
			return;
		}

		out << root.dump(2);
	}
	catch (const nlohmann::json::exception &e)
	{
		app.DebugPrintf("keybindings.json could not be written (%s)\n", e.what());
	}
}

#else // !_WINDOWS64

void InputBindingsStore::Load() {}
void InputBindingsStore::Save() {}

#endif
