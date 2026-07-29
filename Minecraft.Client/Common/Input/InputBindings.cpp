#include "stdafx.h"
#include "InputBindings.h"

#include <cstring>

// Windows64 only, so VK_* resolves through windows.h in stdafx.h. Mouse codes are
// literals matching KeyboardMouseInput::MOUSE_LEFT/RIGHT/MIDDLE to keep the table free of
// the raw input layer.

namespace
{
	const int kMouseLeft   = 0;
	const int kMouseRight  = 1;

	Binding s_bindings[PC_ACTION_MAX][eBindSlot_Count];
	Binding s_unbound;

	struct DefaultBind
	{
		int         action;
		int         slot;
		EBindSource src;
		int         code;
	};

	// Seeded from the KeyboardMouseInput key constants. KEY_CONFIRM and KEY_CANCEL had no
	// call sites and are dropped.
	const DefaultBind kDefaults[] =
	{
		// --- Gameplay -------------------------------------------------------------
		{ MINECRAFT_ACTION_JUMP,               eBindSlot_Primary,   eBind_Key,         VK_SPACE   },
		{ MINECRAFT_ACTION_FORWARD,            eBindSlot_Primary,   eBind_Key,         'W'        },
		{ MINECRAFT_ACTION_BACKWARD,           eBindSlot_Primary,   eBind_Key,         'S'        },
		{ MINECRAFT_ACTION_LEFT,               eBindSlot_Primary,   eBind_Key,         'A'        },
		{ MINECRAFT_ACTION_RIGHT,              eBindSlot_Primary,   eBind_Key,         'D'        },
		{ MINECRAFT_ACTION_ACTION,             eBindSlot_Primary,   eBind_MouseButton, kMouseLeft },
		{ MINECRAFT_ACTION_USE,                eBindSlot_Primary,   eBind_MouseButton, kMouseRight},
		{ MINECRAFT_ACTION_LEFT_SCROLL,        eBindSlot_Primary,   eBind_MouseWheel,   1         },
		{ MINECRAFT_ACTION_RIGHT_SCROLL,       eBindSlot_Primary,   eBind_MouseWheel,  -1         },
		{ MINECRAFT_ACTION_INVENTORY,          eBindSlot_Primary,   eBind_Key,         'E'        },
		{ MINECRAFT_ACTION_PAUSEMENU,          eBindSlot_Primary,   eBind_Key,         VK_ESCAPE  },
		{ MINECRAFT_ACTION_DROP,               eBindSlot_Primary,   eBind_Key,         'Q'        },
		{ MINECRAFT_ACTION_SNEAK_TOGGLE,       eBindSlot_Primary,   eBind_Key,         VK_LSHIFT  },
		{ MINECRAFT_ACTION_CRAFTING,           eBindSlot_Primary,   eBind_Key,         'C'        },
		{ MINECRAFT_ACTION_CRAFTING,           eBindSlot_Secondary, eBind_Key,         'R'        },
		{ MINECRAFT_ACTION_RENDER_THIRD_PERSON,eBindSlot_Primary,   eBind_Key,         VK_F5      },
		{ MINECRAFT_ACTION_RENDER_DEBUG,       eBindSlot_Primary,   eBind_Key,         VK_F4      },
		{ MINECRAFT_ACTION_SCREENSHOT,         eBindSlot_Primary,   eBind_Key,         VK_F2      },
		// GAME_INFO, FLY_TOGGLE, CHANGE_SKIN and SPAWN_CREEPER ship unbound.

		// --- Menu -----------------------------------------------------------------
		// Keyboard only. Clicking a UI element and scrolling a list need hit-test state that
		// only UIController has, so it keeps them.
		{ ACTION_MENU_OK,                      eBindSlot_Primary,   eBind_Key,         VK_RETURN  },
		{ ACTION_MENU_A,                       eBindSlot_Primary,   eBind_Key,         VK_RETURN  },
		{ ACTION_MENU_CANCEL,                  eBindSlot_Primary,   eBind_Key,         VK_ESCAPE  },
		{ ACTION_MENU_B,                       eBindSlot_Primary,   eBind_Key,         VK_ESCAPE  },
		{ ACTION_MENU_UP,                      eBindSlot_Primary,   eBind_Key,         VK_UP      },
		{ ACTION_MENU_DOWN,                    eBindSlot_Primary,   eBind_Key,         VK_DOWN    },
		{ ACTION_MENU_LEFT,                    eBindSlot_Primary,   eBind_Key,         VK_LEFT    },
		{ ACTION_MENU_RIGHT,                   eBindSlot_Primary,   eBind_Key,         VK_RIGHT   },
		{ ACTION_MENU_X,                       eBindSlot_Primary,   eBind_Key,         'R'        },
		{ ACTION_MENU_Y,                       eBindSlot_Primary,   eBind_Key,         VK_TAB     },
		{ ACTION_MENU_LEFT_SCROLL,             eBindSlot_Primary,   eBind_Key,         'Q'        },
		{ ACTION_MENU_RIGHT_SCROLL,            eBindSlot_Primary,   eBind_Key,         'E'        },
		{ ACTION_MENU_PAGEUP,                  eBindSlot_Primary,   eBind_Key,         VK_PRIOR   },
		{ ACTION_MENU_PAGEDOWN,                eBindSlot_Primary,   eBind_Key,         VK_NEXT    },
		{ ACTION_MENU_QUICK_MOVE,              eBindSlot_Primary,   eBind_Key,         VK_LSHIFT  },

		// --- PC-only --------------------------------------------------------------
		{ PC_ACTION_HOTBAR_1,                  eBindSlot_Primary,   eBind_Key,         '1'        },
		{ PC_ACTION_HOTBAR_2,                  eBindSlot_Primary,   eBind_Key,         '2'        },
		{ PC_ACTION_HOTBAR_3,                  eBindSlot_Primary,   eBind_Key,         '3'        },
		{ PC_ACTION_HOTBAR_4,                  eBindSlot_Primary,   eBind_Key,         '4'        },
		{ PC_ACTION_HOTBAR_5,                  eBindSlot_Primary,   eBind_Key,         '5'        },
		{ PC_ACTION_HOTBAR_6,                  eBindSlot_Primary,   eBind_Key,         '6'        },
		{ PC_ACTION_HOTBAR_7,                  eBindSlot_Primary,   eBind_Key,         '7'        },
		{ PC_ACTION_HOTBAR_8,                  eBindSlot_Primary,   eBind_Key,         '8'        },
		{ PC_ACTION_HOTBAR_9,                  eBindSlot_Primary,   eBind_Key,         '9'        },
		// VK_CONTROL means either ctrl, matching the old KEY_SPRINT.
		{ PC_ACTION_SPRINT,                    eBindSlot_Primary,   eBind_Key,         VK_CONTROL },
		{ PC_ACTION_CHAT,                      eBindSlot_Primary,   eBind_Key,         'T'        },
		{ PC_ACTION_TOGGLE_HUD,                eBindSlot_Primary,   eBind_Key,         VK_F1      },
		{ PC_ACTION_FULLSCREEN,                eBindSlot_Primary,   eBind_Key,         VK_F11     },
		{ PC_ACTION_DEBUG_INFO,                eBindSlot_Primary,   eBind_Key,         VK_F3      },
		{ PC_ACTION_DEBUG_CONSOLE,             eBindSlot_Primary,   eBind_Key,         VK_F6      },
		{ PC_ACTION_HOST_SETTINGS,             eBindSlot_Primary,   eBind_Key,         VK_TAB     },
	};

	struct ActionName
	{
		int         action;
		const char *name;
	};

	// EControllerActions values shift between platforms, so bindings persist by name.
	// Do not rename these once shipped.
	const ActionName kActionNames[] =
	{
		{ MINECRAFT_ACTION_JUMP,                "jump"               },
		{ MINECRAFT_ACTION_FORWARD,             "forward"            },
		{ MINECRAFT_ACTION_BACKWARD,            "backward"           },
		{ MINECRAFT_ACTION_LEFT,                "left"               },
		{ MINECRAFT_ACTION_RIGHT,               "right"              },
		{ MINECRAFT_ACTION_USE,                 "use"                },
		{ MINECRAFT_ACTION_ACTION,              "attack"             },
		{ MINECRAFT_ACTION_LEFT_SCROLL,         "scroll_left"        },
		{ MINECRAFT_ACTION_RIGHT_SCROLL,        "scroll_right"       },
		{ MINECRAFT_ACTION_INVENTORY,           "inventory"          },
		{ MINECRAFT_ACTION_PAUSEMENU,           "pause"              },
		{ MINECRAFT_ACTION_DROP,                "drop"               },
		{ MINECRAFT_ACTION_SNEAK_TOGGLE,        "sneak"              },
		{ MINECRAFT_ACTION_CRAFTING,            "crafting"           },
		{ MINECRAFT_ACTION_RENDER_THIRD_PERSON, "third_person"       },
		{ MINECRAFT_ACTION_GAME_INFO,           "game_info"          },
		{ MINECRAFT_ACTION_SPAWN_CREEPER,       "spawn_creeper"      },
		{ MINECRAFT_ACTION_CHANGE_SKIN,         "change_skin"        },
		{ MINECRAFT_ACTION_FLY_TOGGLE,          "fly_toggle"         },
		{ MINECRAFT_ACTION_RENDER_DEBUG,        "debug_menu"         },
		{ MINECRAFT_ACTION_SCREENSHOT,          "screenshot"         },

		{ ACTION_MENU_A,                        "menu_a"             },
		{ ACTION_MENU_B,                        "menu_b"             },
		{ ACTION_MENU_X,                        "menu_x"             },
		{ ACTION_MENU_Y,                        "menu_y"             },
		{ ACTION_MENU_UP,                       "menu_up"            },
		{ ACTION_MENU_DOWN,                     "menu_down"          },
		{ ACTION_MENU_LEFT,                     "menu_left"          },
		{ ACTION_MENU_RIGHT,                    "menu_right"         },
		{ ACTION_MENU_OK,                       "menu_ok"            },
		{ ACTION_MENU_CANCEL,                   "menu_cancel"        },
		{ ACTION_MENU_PAGEUP,                   "menu_pageup"        },
		{ ACTION_MENU_PAGEDOWN,                 "menu_pagedown"      },
		{ ACTION_MENU_LEFT_SCROLL,              "menu_scroll_left"   },
		{ ACTION_MENU_RIGHT_SCROLL,             "menu_scroll_right"  },
		{ ACTION_MENU_OTHER_STICK_UP,           "menu_stick_up"      },
		{ ACTION_MENU_OTHER_STICK_DOWN,         "menu_stick_down"    },
		{ ACTION_MENU_QUICK_MOVE,               "menu_quick_move"    },

		{ PC_ACTION_HOTBAR_1,                   "hotbar_1"           },
		{ PC_ACTION_HOTBAR_2,                   "hotbar_2"           },
		{ PC_ACTION_HOTBAR_3,                   "hotbar_3"           },
		{ PC_ACTION_HOTBAR_4,                   "hotbar_4"           },
		{ PC_ACTION_HOTBAR_5,                   "hotbar_5"           },
		{ PC_ACTION_HOTBAR_6,                   "hotbar_6"           },
		{ PC_ACTION_HOTBAR_7,                   "hotbar_7"           },
		{ PC_ACTION_HOTBAR_8,                   "hotbar_8"           },
		{ PC_ACTION_HOTBAR_9,                   "hotbar_9"           },
		{ PC_ACTION_SPRINT,                     "sprint"             },
		{ PC_ACTION_CHAT,                       "chat"               },
		{ PC_ACTION_TOGGLE_HUD,                 "toggle_hud"         },
		{ PC_ACTION_FULLSCREEN,                 "fullscreen"         },
		{ PC_ACTION_DEBUG_INFO,                 "debug_info"         },
		{ PC_ACTION_DEBUG_CONSOLE,              "debug_console"      },
		{ PC_ACTION_HOST_SETTINGS,              "host_settings"      },
	};

	struct ActionLabel
	{
		int            action;
		const wchar_t *label;
	};

	// English only. Localising would mean ~50 new IDS_ entries for a Windows-only screen.
	const ActionLabel kActionLabels[] =
	{
		{ MINECRAFT_ACTION_FORWARD,             L"Walk Forwards"     },
		{ MINECRAFT_ACTION_BACKWARD,            L"Walk Backwards"    },
		{ MINECRAFT_ACTION_LEFT,                L"Strafe Left"       },
		{ MINECRAFT_ACTION_RIGHT,               L"Strafe Right"      },
		{ MINECRAFT_ACTION_JUMP,                L"Jump"              },
		{ MINECRAFT_ACTION_SNEAK_TOGGLE,        L"Sneak"             },
		{ PC_ACTION_SPRINT,                     L"Sprint"            },

		{ MINECRAFT_ACTION_ACTION,              L"Attack / Destroy"  },
		{ MINECRAFT_ACTION_USE,                 L"Use / Place"       },
		{ MINECRAFT_ACTION_INVENTORY,           L"Open Inventory"    },
		{ MINECRAFT_ACTION_CRAFTING,            L"Open Crafting"     },
		{ MINECRAFT_ACTION_DROP,                L"Drop Item"         },
		{ MINECRAFT_ACTION_LEFT_SCROLL,         L"Previous Item"     },
		{ MINECRAFT_ACTION_RIGHT_SCROLL,        L"Next Item"         },

		{ PC_ACTION_HOTBAR_1,                   L"Hotbar Slot 1"     },
		{ PC_ACTION_HOTBAR_2,                   L"Hotbar Slot 2"     },
		{ PC_ACTION_HOTBAR_3,                   L"Hotbar Slot 3"     },
		{ PC_ACTION_HOTBAR_4,                   L"Hotbar Slot 4"     },
		{ PC_ACTION_HOTBAR_5,                   L"Hotbar Slot 5"     },
		{ PC_ACTION_HOTBAR_6,                   L"Hotbar Slot 6"     },
		{ PC_ACTION_HOTBAR_7,                   L"Hotbar Slot 7"     },
		{ PC_ACTION_HOTBAR_8,                   L"Hotbar Slot 8"     },
		{ PC_ACTION_HOTBAR_9,                   L"Hotbar Slot 9"     },

		{ MINECRAFT_ACTION_PAUSEMENU,           L"Pause Menu"        },
		{ PC_ACTION_CHAT,                       L"Open Chat"         },
		{ PC_ACTION_HOST_SETTINGS,              L"Host Settings"     },
		{ MINECRAFT_ACTION_RENDER_THIRD_PERSON, L"Change Camera"     },
		{ PC_ACTION_TOGGLE_HUD,                 L"Toggle HUD"        },
		{ PC_ACTION_FULLSCREEN,                 L"Toggle Fullscreen" },
		{ MINECRAFT_ACTION_SCREENSHOT,          L"Screenshot"        },
		{ PC_ACTION_DEBUG_INFO,                 L"Debug Info"        },
		{ MINECRAFT_ACTION_RENDER_DEBUG,        L"Debug Menu"        },
		{ PC_ACTION_DEBUG_CONSOLE,              L"Debug Console"     },
		{ MINECRAFT_ACTION_GAME_INFO,           L"Game Info"         },

		{ ACTION_MENU_OK,                       L"Menu Select"       },
		{ ACTION_MENU_CANCEL,                   L"Menu Back"         },
		{ ACTION_MENU_UP,                       L"Menu Up"           },
		{ ACTION_MENU_DOWN,                     L"Menu Down"         },
		{ ACTION_MENU_LEFT,                     L"Menu Left"         },
		{ ACTION_MENU_RIGHT,                    L"Menu Right"        },
		{ ACTION_MENU_X,                        L"Menu Action"       },
		{ ACTION_MENU_Y,                        L"Menu Alt Action"   },
		{ ACTION_MENU_PAGEUP,                   L"Menu Page Up"      },
		{ ACTION_MENU_PAGEDOWN,                 L"Menu Page Down"    },
		{ ACTION_MENU_QUICK_MOVE,               L"Quick Move Item"   },
	};

	// Display order for the rebinding screen. LOOK_* and DPAD_* are pad analog, so excluded.
	const int kBindableActions[] =
	{
		MINECRAFT_ACTION_FORWARD,
		MINECRAFT_ACTION_BACKWARD,
		MINECRAFT_ACTION_LEFT,
		MINECRAFT_ACTION_RIGHT,
		MINECRAFT_ACTION_JUMP,
		MINECRAFT_ACTION_SNEAK_TOGGLE,
		PC_ACTION_SPRINT,

		MINECRAFT_ACTION_ACTION,
		MINECRAFT_ACTION_USE,
		MINECRAFT_ACTION_INVENTORY,
		MINECRAFT_ACTION_CRAFTING,
		MINECRAFT_ACTION_DROP,
		MINECRAFT_ACTION_LEFT_SCROLL,
		MINECRAFT_ACTION_RIGHT_SCROLL,

		PC_ACTION_HOTBAR_1,
		PC_ACTION_HOTBAR_2,
		PC_ACTION_HOTBAR_3,
		PC_ACTION_HOTBAR_4,
		PC_ACTION_HOTBAR_5,
		PC_ACTION_HOTBAR_6,
		PC_ACTION_HOTBAR_7,
		PC_ACTION_HOTBAR_8,
		PC_ACTION_HOTBAR_9,

		MINECRAFT_ACTION_PAUSEMENU,
		PC_ACTION_CHAT,
		PC_ACTION_HOST_SETTINGS,
		MINECRAFT_ACTION_RENDER_THIRD_PERSON,
		PC_ACTION_TOGGLE_HUD,
		PC_ACTION_FULLSCREEN,
		MINECRAFT_ACTION_SCREENSHOT,
		PC_ACTION_DEBUG_INFO,
		MINECRAFT_ACTION_RENDER_DEBUG,
		PC_ACTION_DEBUG_CONSOLE,

		ACTION_MENU_OK,
		ACTION_MENU_CANCEL,
		ACTION_MENU_UP,
		ACTION_MENU_DOWN,
		ACTION_MENU_LEFT,
		ACTION_MENU_RIGHT,
		ACTION_MENU_X,
		ACTION_MENU_Y,
		ACTION_MENU_PAGEUP,
		ACTION_MENU_PAGEDOWN,
		ACTION_MENU_QUICK_MOVE,
	};

	struct KeyName
	{
		int            vk;
		const wchar_t *name;
	};

	// Named keys only. Numpad, VK_OEM_*, lock keys and VK_LWIN/RWIN fall through to the
	// numeric fallback below.
	const KeyName kKeyNames[] =
	{
		{ VK_SPACE,    L"SPACE"       },
		{ VK_RETURN,   L"ENTER"       },
		{ VK_ESCAPE,   L"ESC"         },
		{ VK_TAB,      L"TAB"         },
		{ VK_BACK,     L"BACKSPACE"   },
		{ VK_SHIFT,    L"SHIFT"       },
		{ VK_LSHIFT,   L"LEFT SHIFT"  },
		{ VK_RSHIFT,   L"RIGHT SHIFT" },
		{ VK_CONTROL,  L"CTRL"        },
		{ VK_LCONTROL, L"LEFT CTRL"   },
		{ VK_RCONTROL, L"RIGHT CTRL"  },
		{ VK_MENU,     L"ALT"         },
		{ VK_LMENU,    L"LEFT ALT"    },
		{ VK_RMENU,    L"RIGHT ALT"   },
		{ VK_UP,       L"UP"          },
		{ VK_DOWN,     L"DOWN"        },
		{ VK_LEFT,     L"LEFT"        },
		{ VK_RIGHT,    L"RIGHT"       },
		{ VK_PRIOR,    L"PAGE UP"     },
		{ VK_NEXT,     L"PAGE DOWN"   },
		{ VK_HOME,     L"HOME"        },
		{ VK_END,      L"END"         },
		{ VK_INSERT,   L"INSERT"      },
		{ VK_DELETE,   L"DELETE"      },
		{ VK_F1,       L"F1"          },
		{ VK_F2,       L"F2"          },
		{ VK_F3,       L"F3"          },
		{ VK_F4,       L"F4"          },
		{ VK_F5,       L"F5"          },
		{ VK_F6,       L"F6"          },
		{ VK_F7,       L"F7"          },
		{ VK_F8,       L"F8"          },
		{ VK_F9,       L"F9"          },
		{ VK_F10,      L"F10"         },
		{ VK_F11,      L"F11"         },
		{ VK_F12,      L"F12"         },
	};

	bool IsValidAction(int action)
	{
		return action >= 0 && action < PC_ACTION_MAX;
	}

	bool IsValidSlot(int slot)
	{
		return slot >= 0 && slot < eBindSlot_Count;
	}
}

void InputBindings::ResetToDefaults()
{
	for (int a = 0; a < PC_ACTION_MAX; ++a)
		for (int s = 0; s < eBindSlot_Count; ++s)
			s_bindings[a][s] = Binding();

	for (size_t i = 0; i < sizeof(kDefaults) / sizeof(kDefaults[0]); ++i)
	{
		const DefaultBind &d = kDefaults[i];
		if (IsValidAction(d.action) && IsValidSlot(d.slot))
			s_bindings[d.action][d.slot] = Binding(d.src, d.code);
	}
}

const Binding &InputBindings::Get(int action, int slot)
{
	if (!IsValidAction(action) || !IsValidSlot(slot))
		return s_unbound;

	return s_bindings[action][slot];
}

void InputBindings::Set(int action, int slot, const Binding &b)
{
	if (!IsValidAction(action) || !IsValidSlot(slot))
		return;

	s_bindings[action][slot] = b;
}

void InputBindings::Clear(int action, int slot)
{
	Set(action, slot, Binding());
}

bool InputBindings::IsMenuAction(int action)
{
	return action <= ACTION_MAX_MENU;
}

int InputBindings::FindActionBoundTo(const Binding &b, int forAction)
{
	if (!b.IsBound())
		return -1;

	const bool wantMenu = IsMenuAction(forAction);

	for (int a = 0; a < PC_ACTION_MAX; ++a)
	{
		if (a == forAction)
			continue;
		if (IsMenuAction(a) != wantMenu)
			continue;

		for (int s = 0; s < eBindSlot_Count; ++s)
		{
			if (s_bindings[a][s] == b)
				return a;
		}
	}

	return -1;
}

std::wstring InputBindings::GetDisplayName(int action, int slot)
{
	const Binding &b = Get(action, slot);

	switch (b.src)
	{
	case eBind_MouseWheel:
		return b.code > 0 ? L"WHEEL UP" : L"WHEEL DOWN";

	case eBind_MouseButton:
		switch (b.code)
		{
		case 0:  return L"MOUSE 1";
		case 1:  return L"MOUSE 2";
		case 2:  return L"MOUSE 3";
		default: break;
		}
		break;

	case eBind_Key:
	{
		for (size_t i = 0; i < sizeof(kKeyNames) / sizeof(kKeyNames[0]); ++i)
		{
			if (kKeyNames[i].vk == b.code)
				return kKeyNames[i].name;
		}

		if ((b.code >= '0' && b.code <= '9') || (b.code >= 'A' && b.code <= 'Z'))
		{
			wchar_t single[2] = { static_cast<wchar_t>(b.code), L'\0' };
			return single;
		}

		wchar_t temp[32];
		swprintf(temp, 32, L"KEY %d", b.code);
		return temp;
	}

	case eBind_None:
	default:
		break;
	}

	return L"---";
}

std::wstring InputBindings::GetActionLabel(int action)
{
	for (size_t i = 0; i < sizeof(kActionLabels) / sizeof(kActionLabels[0]); ++i)
	{
		if (kActionLabels[i].action == action)
			return kActionLabels[i].label;
	}

	const char *name = GetActionName(action);
	if (name)
	{
		wchar_t temp[64];
		swprintf(temp, 64, L"%hs", name);
		return temp;
	}

	return L"?";
}

const char *InputBindings::GetActionName(int action)
{
	for (size_t i = 0; i < sizeof(kActionNames) / sizeof(kActionNames[0]); ++i)
	{
		if (kActionNames[i].action == action)
			return kActionNames[i].name;
	}

	return nullptr;
}

int InputBindings::GetActionByName(const char *name)
{
	if (name == nullptr)
		return -1;

	for (size_t i = 0; i < sizeof(kActionNames) / sizeof(kActionNames[0]); ++i)
	{
		if (strcmp(kActionNames[i].name, name) == 0)
			return kActionNames[i].action;
	}

	return -1;
}

const int *InputBindings::GetBindableActions(int &countOut)
{
	countOut = static_cast<int>(sizeof(kBindableActions) / sizeof(kBindableActions[0]));
	return kBindableActions;
}
