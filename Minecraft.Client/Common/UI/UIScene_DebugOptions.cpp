#include "stdafx.h"
#include "../../Minecraft.h"
#include "../../Options.h"
#include "UI.h"
#include "UIScene_DebugOptions.h"

UIScene_DebugOptionsMenu::UIScene_DebugOptionsMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
    // Setup all the Iggy references we need for this scene
    initialiseMovie();

    m_bNotInGame = (Minecraft::GetInstance()->level == nullptr);
    m_bNeedsMultiListPopulate = true;
    m_bInitialPopulateDone = false;
    m_bPendingSliderUpdate = false;
    m_iPendingSliderId = 0;
    m_iPendingSliderValue = 0;

    doHorizontalResizeCheck();

    if (app.GetLocalPlayerCount() > 1)
    {
#if TO_BE_IMPLEMENTED
        app.AdjustSplitscreenScene(m_hObj, &m_OriginalPosition, m_iPad);
#endif
    }
}

UIScene_DebugOptionsMenu::~UIScene_DebugOptionsMenu()
{
}

wstring UIScene_DebugOptionsMenu::getMoviePath()
{
    if (app.GetLocalPlayerCount() > 1)
    {
        return L"MultilistMenuSplit";
    }
    else
    {
        return L"MultilistMenu";
    }
}

void UIScene_DebugOptionsMenu::tick()
{
    if (m_bNeedsMultiListPopulate)
    {
        m_bNeedsMultiListPopulate = false;
        m_multiList.setupControl(this, m_rootPath, "MultiList");
        m_controls.push_back(&m_multiList);
        m_multiList.clearList();
        m_multiList.init(eControl_MultiList);

        WCHAR TempString[256];

        //L"Load Saves From Local Folder Mode",
        //L"Write Saves To Local Folder Mode",
        //L"Freeze Players", // L"Not Used",
        //L"Display Safe Area",
        //L"Mobs don't attack",
        //L"Freeze Time",
        //L"Disable Weather",
        //L"Craft Anything",
        //L"Use DPad for debug",
        //L"Mobs don't tick",
        //L"Art tools", // L"Instant Mine",
        //L"Show UI Console",
        //L"Distributable Save",
        //L"Debug Leaderboards",
        //L"Height-Water Maps",
        //L"Superflat Nether",
        //// L"Light/Dark background",
        //L"More lightning when thundering",
        //L"Biome override",
        //// L"Go To End",
        //L"Go To Overworld",
        //L"Unlock All DLC", // L"Toggle Font",
        //L"Show Marketing Guide",

        m_multiList.AddNewCheckbox(app.GetString(L"Art Tools"), eControl_ArtTools, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_ArtTools)) != 0));

        m_multiList.AddNewCheckbox(app.GetString(L"Load Saves From Local Folder Mode"), eControl_LoadSavesFromDisk, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_LoadSavesFromDisk)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Write Saves To Local Folder Mode"), eControl_WriteSavesToDisk, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_WriteSavesToDisk)) != 0));

        m_multiList.AddNewCheckbox(app.GetString(L"Freeze Players"), eControl_FreezePlayers, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_FreezePlayers)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Display Safe Area"), eControl_Safearea, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_Safearea)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Mobs don't attack"), eControl_MobsDontAttack, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_MobsDontAttack)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Freeze Time"), eControl_FreezeTime, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_FreezeTime)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Disable Weather"), eControl_DisableWeather, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_DisableWeather)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Craft Anything"), eControl_CraftAnything, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_CraftAnything)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Use DPad for debug"), eControl_UseDpadForDebug, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_UseDpadForDebug)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Mobs don't tick"), eControl_MobsDontTick, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_MobsDontTick)) != 0));

        m_multiList.AddNewCheckbox(app.GetString(L"Show Marketing Guide"), eControl_ShowUIMarketingGuide, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_ShowUIMarketingGuide)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Show UI Console"), eControl_ShowUIConsole, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_ShowUIConsole)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Distributable Save"), eControl_DistributableSave, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_DistributableSave)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Debug Leaderboards"), eControl_DebugLeaderboards, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_DebugLeaderboards)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Height-Water Maps"), eControl_EnableHeightWaterOverride, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_EnableHeightWaterOverride)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Superflat Nether"), eControl_SuperflatNether, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_SuperflatNether)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"More lightning when thundering"), eControl_RegularLightning, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_RegularLightning)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(L"Biome override"), eControl_EnableBiomeOverride, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_EnableBiomeOverride)) != 0));

        m_multiList.AddNewCheckbox(app.GetString(L"Go To Overworld"), eControl_GoToOverworld, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_GoToOverworld)) != 0));
        //add these later
        m_multiList.AddNewCheckbox(app.GetString(L"Go To Nether"), eControl_GoToNether, false);
        m_multiList.AddNewCheckbox(app.GetString(L"Go To End"), eControl_GoToEnd, false);

        m_multiList.AddNewCheckbox(app.GetString(L"Unlock All DLC"), eControl_UnlockAllDLC, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_UnlockAllDLC)) != 0));

        IggyName funcDoVert = registerFastName(L"DoVerticalResizeCheck");
        IggyName funcHideDesc = registerFastName(L"HideDescription");
        IggyDataValue result;
        IggyPlayerCallMethodRS(getMovie(), &result, m_rootPath, funcDoVert, 0, nullptr);
        doHorizontalResizeCheck();
        IggyPlayerCallMethodRS(getMovie(), &result, m_rootPath, funcHideDesc, 0, nullptr);
        m_multiList.HighlightItem(eControl_ArtTools);
    }

    if (m_bPendingSliderUpdate)
    {
        m_bPendingSliderUpdate = false;
        m_multiList.SetSliderValue(m_iPendingSliderId, m_iPendingSliderValue);

        WCHAR TempString[256];
    }

    UIScene::tick();
    m_bInitialPopulateDone = true;
}

void UIScene_DebugOptionsMenu::updateTooltips()
{
    ui.SetTooltips(m_iPad, IDS_TOOLTIPS_SELECT, IDS_TOOLTIPS_BACK);
}

void UIScene_DebugOptionsMenu::updateComponents()
{
    bool bNotInGame = (Minecraft::GetInstance()->level == nullptr);
    if (bNotInGame)
    {
        m_parentLayer->showComponent(m_iPad, eUIComponent_Panorama, true);
        m_parentLayer->showComponent(m_iPad, eUIComponent_Logo, true);
    }
    else
    {
        m_parentLayer->showComponent(m_iPad, eUIComponent_Panorama, false);

        if (app.GetLocalPlayerCount() == 1)
        {
            m_parentLayer->showComponent(m_iPad, eUIComponent_Logo, true);
        }
        else
        {
            m_parentLayer->showComponent(m_iPad, eUIComponent_Logo, false);
        }
    }
}

void UIScene_DebugOptionsMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
    ui.AnimateKeyPress(iPad, key, repeat, pressed, released);
    switch (key)
    {
    case ACTION_MENU_CANCEL:
        if (pressed)
        {
            setGameSettings();
            navigateBack();
        }
        break;
    case ACTION_MENU_OK:
#ifdef __ORBIS__
    case ACTION_MENU_TOUCHPAD_PRESS:
#endif
        sendInputToMovie(key, repeat, pressed, released);
        break;
    case ACTION_MENU_UP:
    case ACTION_MENU_DOWN:
    case ACTION_MENU_LEFT:
    case ACTION_MENU_RIGHT:
        sendInputToMovie(key, repeat, pressed, released);
        break;
    }
}

void UIScene_DebugOptionsMenu::handleSliderMove(F64 sliderId, F64 currentValue)
{
    int sliderIdInt = static_cast<int>(sliderId);
    int value = static_cast<int>(currentValue);

    m_multiList.handleSliderMove(sliderIdInt, value);

    // switch (sliderIdInt)
}

void UIScene_DebugOptionsMenu::handleCheckboxToggled(F64 controlId, bool selected)
{
    if (m_bInitialPopulateDone)
    {
        ui.PlayUISFX(eSFX_Press);
    }
}

void UIScene_DebugOptionsMenu::handlePress(F64 controlId, F64 childId)
{
    ui.PlayUISFX(eSFX_Press);
}

void UIScene_DebugOptionsMenu::setGameSettings()
{
    //app.SetGameSettings(m_iPad,eGameSetting_ViewBob,m_multiList.GetCheckboxValue(eControl_ViewBob)?1:0);
}

void UIScene_DebugOptionsMenu::handleGainFocus(bool navBack)
{
    if (navBack)
    {
        m_bNeedsMultiListPopulate = true;
        m_bInitialPopulateDone = false;
    }
}
