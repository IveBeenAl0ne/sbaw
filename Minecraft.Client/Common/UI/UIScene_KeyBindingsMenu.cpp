#include "stdafx.h"
#include "UI.h"
#include "UIScene_KeyBindingsMenu.h"
#include "../Input/PCInput.h"
#include "../Input/InputBindingsStore.h"

UIScene_KeyBindingsMenu::UIScene_KeyBindingsMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	m_bNeedsMultiListPopulate = true;
	m_iCapturingAction = -1;
	m_bDirty = false;

	doHorizontalResizeCheck();
}

UIScene_KeyBindingsMenu::~UIScene_KeyBindingsMenu()
{
	// s_capturing is process-global, so a scene torn down with a row still armed would
	// leave the keyboard dead. Idempotent.
	PCInput::CancelCapture();

	if (m_bDirty)
		InputBindingsStore::Save();
}

wstring UIScene_KeyBindingsMenu::getMoviePath()
{
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"MultilistMenuSplit";
	}
	else
	{
		return L"MultilistMenu";
	}
}

wstring UIScene_KeyBindingsMenu::BuildRowLabel(int action)
{
	WCHAR TempString[256];

	if (action == m_iCapturingAction)
	{
		swprintf(TempString, 256, L"%ls: ...", InputBindings::GetActionLabel(action).c_str());
		return wstring(TempString);
	}

	// Show the secondary slot too, or CRAFTING's default 'R' looks like it vanished.
	const Binding &second = InputBindings::Get(action, eBindSlot_Secondary);
	if (second.IsBound())
	{
		swprintf(TempString, 256, L"%ls: %ls / %ls",
			InputBindings::GetActionLabel(action).c_str(),
			InputBindings::GetDisplayName(action, eBindSlot_Primary).c_str(),
			InputBindings::GetDisplayName(action, eBindSlot_Secondary).c_str());
	}
	else
	{
		swprintf(TempString, 256, L"%ls: %ls",
			InputBindings::GetActionLabel(action).c_str(),
			InputBindings::GetDisplayName(action, eBindSlot_Primary).c_str());
	}

	return wstring(TempString);
}

void UIScene_KeyBindingsMenu::tick()
{
	if(m_bNeedsMultiListPopulate)
	{
		m_bNeedsMultiListPopulate = false;
		m_multiList.setupControl(this, m_rootPath, "MultiList");
		m_controls.push_back(&m_multiList);
		m_multiList.clearList();
		m_multiList.init(eControl_MultiList);

		int count = 0;
		const int *actions = InputBindings::GetBindableActions(count);
		for (int n = 0; n < count; n++)
		{
			m_multiList.AddNewButton(BuildRowLabel(actions[n]), eControl_FirstAction + actions[n]);
		}

		m_multiList.AddNewButton(app.GetString(IDS_KEYBINDINGS_RESET), eControl_ResetDefaults);

		IggyName funcDoVert = registerFastName(L"DoVerticalResizeCheck");
		IggyName funcHideDesc = registerFastName(L"HideDescription");
		IggyDataValue result;
		IggyPlayerCallMethodRS(getMovie(), &result, m_rootPath, funcDoVert, 0, nullptr);
		doHorizontalResizeCheck();
		IggyPlayerCallMethodRS(getMovie(), &result, m_rootPath, funcHideDesc, 0, nullptr);
	}

	if (m_iCapturingAction >= 0)
	{
		Binding captured;
		if (PCInput::PollCapture(captured))
		{
			// A hand-edited keybindings.json can put the same key on two actions.
			int prev;
			while ((prev = InputBindings::FindActionBoundTo(captured, m_iCapturingAction)) >= 0)
			{
				for (int s = 0; s < eBindSlot_Count; s++)
				{
					if (InputBindings::Get(prev, s) == captured)
						InputBindings::Clear(prev, s);
				}
				m_multiList.SetItemLabel(eControl_FirstAction + prev, BuildRowLabel(prev));
			}

			InputBindings::Set(m_iCapturingAction, eBindSlot_Primary, captured);

			const int bound = m_iCapturingAction;
			m_iCapturingAction = -1;
			m_multiList.SetItemLabel(eControl_FirstAction + bound, BuildRowLabel(bound));

			m_bDirty = true;
			ui.PlayUISFX(eSFX_Press);
		}
	}

	UIScene::tick();
}

void UIScene_KeyBindingsMenu::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT, IDS_TOOLTIPS_BACK);
}

void UIScene_KeyBindingsMenu::updateComponents()
{
	bool bNotInGame=(Minecraft::GetInstance()->level==nullptr);
	if(bNotInGame)
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,true);
		m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
	}
	else
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,false);

		if( app.GetLocalPlayerCount() == 1 ) m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
		else m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,false);
	}
}

void UIScene_KeyBindingsMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);
	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			if (m_iCapturingAction >= 0)
			{
				PCInput::CancelCapture();
				const int cancelled = m_iCapturingAction;
				m_iCapturingAction = -1;
				m_multiList.SetItemLabel(eControl_FirstAction + cancelled, BuildRowLabel(cancelled));
			}
			else
			{
				if (m_bDirty)
				{
					InputBindingsStore::Save();
					m_bDirty = false;
				}
				navigateBack();
			}
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
	case ACTION_MENU_LEFT:
	case ACTION_MENU_RIGHT:
		// So the arrow keys stay bindable.
		if (m_iCapturingAction < 0)
			sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_KeyBindingsMenu::handlePress(F64 controlId, F64 childId)
{
	ui.PlayUISFX(eSFX_Press);

	const int id = static_cast<int>(childId);

	if (id == eControl_ResetDefaults)
	{
		InputBindings::ResetToDefaults();
		m_bDirty = true;
		m_bNeedsMultiListPopulate = true; // cheapest correct relabel of every row
		return;
	}

	if (id >= eControl_FirstAction)
	{
		m_iCapturingAction = id - eControl_FirstAction;
		m_multiList.SetItemLabel(id, BuildRowLabel(m_iCapturingAction));
		PCInput::BeginCapture();
	}
}

void UIScene_KeyBindingsMenu::handleGainFocus(bool navBack)
{
	if(navBack)
	{
		m_bNeedsMultiListPopulate = true;
	}
}
