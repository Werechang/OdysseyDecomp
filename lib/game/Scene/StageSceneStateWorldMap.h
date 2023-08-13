#pragma once

#include <container/seadPtrArray.h>

#include "al/Library/Layout/LayoutActor.h"
#include "al/Library/LiveActor/ActorInitInfo.h"
#include "al/Library/Nerve/NerveStateBase.h"
#include "al/Library/Scene/Scene.h"

#include "game/MapObj/WorldMapParts.h"
#include "game/MapObj/WorldMapRoute.h"

class ShineTowerRocket;
class StageSceneStateCollectionList;
class TalkMessage;

namespace al {
struct SceneInitInfo;
class SimpleLayoutAppearWaitEnd;
class WipeSimple;
class KeyRepeatCtrl;
}  // namespace al

class WorldMapPlayerIcon;
class WorldMapCamera;

class StageSceneStateWorldMap : public al::HostStateBase<al::Scene> {
public:
    StageSceneStateWorldMap(const char*, al::Scene*, ShineTowerRocket*, const al::SceneInitInfo&,
                            const al::ActorInitInfo&, const al::LayoutInitInfo&,
                            StageSceneStateCollectionList*);
    ~StageSceneStateWorldMap() override;

    void control() override;
    void appear() override;
    void kill() override;

    void exeInit();
    void exeDemoPrep();
    void exeDemoStart();
    void appearWorldSelectLayout();
    void exeDemoWorldOpen();
    void exeDemoWorldUnlock();
    void exeDemoWorldSelect();
    void exeDemoWorldComment();
    void exeWait();
    void exeIgnoreMessage();
    void exeCollectionListMain();
    void exeCollectionListExit();
    void exeExit();
    void exeEnd();

    void tryOpenWorldMap();
    void isCloseEndWipe();
    void isOpenEndWipe();
    void isOpenCollectionList();

    static StageSceneStateWorldMap* tryCreate(al::Scene*, const al::SceneInitInfo&,
                                              const al::ActorInitInfo &, const al::LayoutInitInfo &,
                                              StageSceneStateCollectionList*);
                                              
private:
    al::SimpleLayoutAppearWaitEnd* mWorldSelectLyt;
    al::SimpleLayoutAppearWaitEnd* mWorldSelectMovieLyt;
    al::LayoutActor* mWorldSelectFooter;
    void* field_38;
    al::LayoutActor** mParWorldList;
    al::LayoutActor** mParIconList;
    void* field_50;
    al::WipeSimple* mWorldWipeSimple;
    al::WipeSimple* mWorldWipeCapture;
    TalkMessage* mWorldMsg;
    al::KeyRepeatCtrl* mKeyRepeatCtrl;
    al::Scene* mWorldMapStage;
    al::LiveActor* mWorldMapEarth;
    al::LiveActorGroup* mWorldMapGroup;
    WorldMapParts** mWorldMapPartsList;
    WorldMapRoute** mWorldMapRouteList;
    WorldMapRoute** mMapRouteGroup;
    WorldMapPlayerIcon* mPlayerIcon;
    al::LiveActor* mWorldMapKoopaShip;
    ShineTowerRocket* mShineTowerRocket;
    void* field_C0;
    char field_C8;
    char field_C9[47];
    void* field_F8;
    s32 mWorldIndexBoss;
    void* field_100;
    s32 mWorldNum;
    int field_108;
    s32* field_110;
    void* field_118;
    void* field_11C;
    void* field_120;
    void* field_128;
    void* field_130;
    WorldMapCamera* mWorldMapCamera;
    void* field_140;
    StageSceneStateCollectionList* mStateCollectionList;
    bool field_150;
    bool mIsGotAllShine;
    sead::PtrArrayImpl* mPtrArray;
};