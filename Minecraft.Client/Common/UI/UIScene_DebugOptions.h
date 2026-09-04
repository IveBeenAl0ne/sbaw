#pragma once

#include "UIControl_MultiList.h"
#include "UIScene.h"

class UIScene_DebugOptionsMenu : public UIScene
{
  private:
    enum EControls
    {
        eControl_MultiList = 0,
        eControl_ArtTools = 1,
        eControl_FreezePlayers = 2,
        eControl_Safearea = 3,
        eControl_MobsDontAttack = 4,
        eControl_FreezeTime = 5,
        eControl_DisableWeather = 6,
        eControl_CraftAnything = 7,
        eControl_UseDpadForDebug = 8,
        eControl_MobsDontTick = 9,
        eControl_ShowUIMarketingGuide = 10,
        eControl_ShowUIConsole = 11,
        eControl_DistributableSave = 12,
        eControl_DebugLeaderboards = 13,
        eControl_EnableHeightWaterOverride = 14,
        eControl_SuperflatNether = 15,
        eControl_RegularLightning = 16,
        eControl_EnableBiomeOverride = 17,
        eControl_GoToOverworld = 18,
        eControl_GoToNether = 19,
        eControl_GoToEnd = 20,
        //eControl_UnlockAllDLC = 21, //unused
        eControl_LoadSavesFromDisk = 21,
        eControl_WriteSavesToDisk = 22,
    };

    UIControl_MultiList m_multiList;
    bool m_bNeedsMultiListPopulate;
    bool m_bInitialPopulateDone;
    bool m_bPendingSliderUpdate;
    int m_iPendingSliderId;
    int m_iPendingSliderValue;

    bool m_bGoToOverworld;
    bool m_bGoToNether;
    bool m_bGoToEnd;
    bool m_bTeleportBusy;

    bool m_bNotInGame;

    UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
    UI_END_MAP_ELEMENTS_AND_NAMES()

  public:
    UIScene_DebugOptionsMenu(int iPad, void *initData, UILayer *parentLayer);
    virtual ~UIScene_DebugOptionsMenu();

    virtual EUIScene getSceneType()
    {
        return eUIScene_DebugOptions;
    }

    virtual void tick();
    virtual void updateTooltips();
    virtual void updateComponents();

  protected:
    // TODO: This should be pure virtual in this class
    virtual wstring getMoviePath();

  public:
    // INPUT
    virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

    virtual void handleSliderMove(F64 sliderId, F64 currentValue);
    virtual void handleCheckboxToggled(F64 controlId, bool selected);
    virtual void handlePress(F64 controlId, F64 childId);
    virtual void handleGainFocus(bool navBack);

    void setGameSettings();

    static int LevelToDistance(int dist);

    static int DistanceToLevel(int dist);
    virtual void render(S32 width, S32 height, C4JRender::eViewportType viewport);
};
