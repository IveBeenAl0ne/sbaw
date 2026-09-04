#include "stdafx.h"
#include "../../Minecraft.h"
#include "../../Options.h"
#include "UI.h"
#include "UIScene_DebugOptions.h"
#include "../../Lighting.h"
#ifdef _WINDOWS64
#include "../../Windows64/4JLibs/inc/4J_Render.h"
#include "../../Windows64/Iggy/gdraw/gdraw_d3d11.h"
#endif

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

    m_bGoToOverworld = false;
    m_bGoToNether = false;
    m_bGoToEnd = false;
    m_bTeleportBusy = false; //fix loading screen softlock

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

        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_ART_TOOLS), eControl_ArtTools, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_ArtTools)) != 0));
        
        // tp
        m_multiList.AddNewButton(app.GetString(IDS_DEBUG_GO_TO_OVERWORLD), eControl_GoToOverworld);
        m_multiList.AddNewButton(app.GetString(IDS_DEBUG_GO_TO_NETHER), eControl_GoToNether);
        m_multiList.AddNewButton(app.GetString(IDS_DEBUG_GO_TO_END), eControl_GoToEnd);

        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_MOBS_DONT_TICK), eControl_MobsDontTick, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_MobsDontTick)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_FREEZE_PLAYERS), eControl_FreezePlayers, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_FreezePlayers)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_FREEZE_TIME), eControl_FreezeTime, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_FreezeTime)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_MOBS_DONT_ATTACK), eControl_MobsDontAttack, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_MobsDontAttack)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_DISABLE_WEATHER), eControl_DisableWeather, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_DisableWeather)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_CRAFT_ANYTHING), eControl_CraftAnything, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_CraftAnything)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_USE_DPAD_FOR_DEBUG), eControl_UseDpadForDebug, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_UseDpadForDebug)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_SUPERFLAT_NETHER), eControl_SuperflatNether, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_SuperflatNether)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_MORE_LIGHTNING), eControl_RegularLightning, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_RegularLightning)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_BIOME_OVERRIDE), eControl_EnableBiomeOverride, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_EnableBiomeOverride)) != 0));

        // UI
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_SAFE_AREA), eControl_Safearea, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_Safearea)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_SHOW_MARKETING_GUIDE), eControl_ShowUIMarketingGuide, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_ShowUIMarketingGuide)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_SHOW_UI_CONSOLE), eControl_ShowUIConsole, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_ShowUIConsole)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_LEADERBOARDS), eControl_DebugLeaderboards, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_DebugLeaderboards)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_HEIGHT_WATER_MAPS), eControl_EnableHeightWaterOverride, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_EnableHeightWaterOverride)) != 0));

        // save files
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_LOAD_SAVES), eControl_LoadSavesFromDisk, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_LoadSavesFromDisk)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_WRITE_SAVES), eControl_WriteSavesToDisk, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_WriteSavesToDisk)) != 0));
        m_multiList.AddNewCheckbox(app.GetString(IDS_DEBUG_DISTRIBUTABLE_SAVE), eControl_DistributableSave, ((app.GetGameSettingsDebugMask(m_iPad) & (1 << eDebugSetting_DistributableSave)) != 0));

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

    if (!m_bTeleportBusy) 
    {
        switch (static_cast<int>(childId))
        {
        case eControl_GoToOverworld:
            m_bGoToOverworld = true;
            m_bTeleportBusy = true;
            break;
        case eControl_GoToNether:
            m_bGoToNether = true;
            m_bTeleportBusy = true;
            break;
        case eControl_GoToEnd:
            m_bGoToEnd = true;
            m_bTeleportBusy = true;
            break;
        }
    }

    //switch (static_cast<int>(childId)) //for buttons that are not teleports

}

void UIScene_DebugOptionsMenu::setGameSettings()
{
    unsigned int uiMask = 0;

    if (m_multiList.GetCheckboxValue(eControl_ArtTools)) uiMask |= (1 << eDebugSetting_ArtTools);
    if (m_multiList.GetCheckboxValue(eControl_FreezePlayers)) uiMask |= (1 << eDebugSetting_FreezePlayers);
    if (m_multiList.GetCheckboxValue(eControl_FreezeTime)) uiMask |= (1 << eDebugSetting_FreezeTime);
    if (m_multiList.GetCheckboxValue(eControl_MobsDontAttack)) uiMask |= (1 << eDebugSetting_MobsDontAttack);
    if (m_multiList.GetCheckboxValue(eControl_DisableWeather)) uiMask |= (1 << eDebugSetting_DisableWeather);
    if (m_multiList.GetCheckboxValue(eControl_CraftAnything)) uiMask |= (1 << eDebugSetting_CraftAnything);
    if (m_multiList.GetCheckboxValue(eControl_UseDpadForDebug)) uiMask |= (1 << eDebugSetting_UseDpadForDebug);
    if (m_multiList.GetCheckboxValue(eControl_MobsDontTick)) uiMask |= (1 << eDebugSetting_MobsDontTick);
    if (m_multiList.GetCheckboxValue(eControl_SuperflatNether)) uiMask |= (1 << eDebugSetting_SuperflatNether);
    if (m_multiList.GetCheckboxValue(eControl_RegularLightning)) uiMask |= (1 << eDebugSetting_RegularLightning);
    if (m_multiList.GetCheckboxValue(eControl_EnableBiomeOverride)) uiMask |= (1 << eDebugSetting_EnableBiomeOverride);

    if (m_multiList.GetCheckboxValue(eControl_Safearea)) uiMask |= (1 << eDebugSetting_Safearea);
    if (m_multiList.GetCheckboxValue(eControl_ShowUIMarketingGuide)) uiMask |= (1 << eDebugSetting_ShowUIMarketingGuide);
    if (m_multiList.GetCheckboxValue(eControl_ShowUIConsole)) uiMask |= (1 << eDebugSetting_ShowUIConsole);
    if (m_multiList.GetCheckboxValue(eControl_DebugLeaderboards)) uiMask |= (1 << eDebugSetting_DebugLeaderboards);
    if (m_multiList.GetCheckboxValue(eControl_EnableHeightWaterOverride)) uiMask |= (1 << eDebugSetting_EnableHeightWaterOverride);

    if (m_bGoToOverworld) uiMask |= (1 << eDebugSetting_GoToOverworld);
    if (m_bGoToNether) uiMask |= (1 << eDebugSetting_GoToNether);
    if (m_bGoToEnd) uiMask |= (1 << eDebugSetting_GoToEnd);

    if (m_multiList.GetCheckboxValue(eControl_LoadSavesFromDisk)) uiMask |= (1 << eDebugSetting_LoadSavesFromDisk);
    if (m_multiList.GetCheckboxValue(eControl_WriteSavesToDisk)) uiMask |= (1 << eDebugSetting_WriteSavesToDisk);
    if (m_multiList.GetCheckboxValue(eControl_DistributableSave)) uiMask |= (1 << eDebugSetting_DistributableSave);

    unsigned int uiOldMask = app.GetGameSettingsDebugMask(m_iPad);

    //from old file
    if (uiMask != uiOldMask)
    {
        app.SetGameSettingsDebugMask(m_iPad, uiMask);

        if (app.DebugSettingsOn())
        {
            app.ActionDebugMask(m_iPad);
        }
        else
        {
            app.ActionDebugMask(m_iPad, true);
        }

        app.CheckGameSettingsChanged(true, m_iPad);
    }

    m_bGoToOverworld = false;
    m_bGoToNether = false;
    m_bGoToEnd = false;
    m_bTeleportBusy = false;
}

void UIScene_DebugOptionsMenu::handleGainFocus(bool navBack)
{
    if (navBack)
    {
        m_bNeedsMultiListPopulate = true;
        m_bInitialPopulateDone = false;
    }
}

void UIScene_DebugOptionsMenu::render(S32 width, S32 height, C4JRender::eViewportType viewport)
{
    UIScene::render(width, height, viewport);

    Minecraft *pMinecraft = Minecraft::GetInstance();
    if (!pMinecraft || !pMinecraft->font)
    {
        return;
    }

    Font *font = pMinecraft->font;
    const wstring text = app.GetString(IDS_DEBUG_WARNING);
    const float scale = 0.6f;
    const int pad = 4;

    ScreenSizeCalculator ssc(pMinecraft->options, pMinecraft->width_phys, pMinecraft->height_phys);
    S32 sw = ssc.getWidth();
    S32 sh = ssc.getHeight();

#ifdef _WINDOWS64
    RenderManager.StartFrame();
    gdraw_D3D11_setViewport_4J();
#endif
    RenderManager.Set_matrixDirty();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, sw, sh, 0, 1000, 3000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2000.0f);

    Lighting::turnOff();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    int totalW = static_cast<int>(font->width(text) * scale);
    int tx = sw - totalW - pad;
    int ty = sh - static_cast<int>(8 * scale) - pad;

    glPushMatrix();
    glTranslatef(static_cast<float>(tx), static_cast<float>(ty), 0.0f);
    glScalef(scale, scale, scale);
    font->drawShadow(text, 0, 0, 0xFFFF00);
    glPopMatrix();

    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);
}