#pragma once

#include <container/seadPtrArray.h>

#include "Library/LiveActor/LiveActor.h"

class ElectricWire;

namespace al {
class CameraTicket;
class CameraTicketId;
class CameraPoser;
class PlacementInfo;
}  // namespace al

class ElectricWireRailKeeper : public al::LiveActor {
public:
    ElectricWireRailKeeper(const char* name);
    ElectricWireRailKeeper(const char* name, al::LiveActor* wire);

    void appear() override;
    void appearBySwitch();
    void endCameraIfActive();
    void exeStandby();
    void exeWait();
    al::CameraTicket* findRailPointCameraTicket(s32 pointIdx) const;
    const al::CameraTicket* findRailPointStartCameraHackEndTicket(s32 pointIdx) const;
    al::PlacementInfo* getRailPointInfo(s32 index) const;
    void init(const al::ActorInitInfo& info) override;
    bool isNerveStandby() const;
    bool isRailPointEnableTargetEndCollision(s32 index) const;
    bool isRailPointFaceToCameraDir(s32 index) const;
    bool isRailPointIgnore(s32 index) const;
    bool isRailPointIsDisplayPointModelForce(s32 index) const;
    bool isRailPointIsExpandRailSelectableAngle(s32 index) const;
    bool isRailPointIsNeedCamera(s32 index) const;
    bool isRailPointIsNeedStartCameraHackEnd(s32 index) const;
    bool isRailPointPlacementPole(s32 index) const;
    bool isRailPointSpringFix(s32 index) const;
    void kill() override;
    void killBySwitch();
    bool tryGetPlayerBottomPosOnVerticalMove(sead::Vector3f* playerBottomPos) const;
    bool tryGetPlayerHeadPosOnVerticalMove(sead::Vector3f* playerHeadPos) const;
    bool tryGetRailPointDestinationTrans(sead::Vector3f* out, s32 index) const;
    bool tryGetRailPointFastenerMoveLimitAreaFlag(s32* out, s32 index) const;
    bool tryGetRailPointOutDir(sead::Vector3f* out, s32 index) const;

    enum class PosType : u32 { NONE, HEAD, BOTTOM };

private:
    struct TicketHolder {
        al::CameraTicket* ticket;
        al::CameraTicket* ticketHackEnd;
        s32 pointIdx;
    };

    ElectricWire* mElectricWire = nullptr;
    f32 mUnused = 75;
    bool mIsShowLine = true;
    bool mIsThrowaway = false;
    PosType mPosType = PosType::NONE;
    sead::Vector3f mPlayerPosOnVerticalMove = sead::Vector3f::zero;
    sead::PtrArray<TicketHolder> mCameraTickets{};
};

static_assert(sizeof(ElectricWireRailKeeper) == 0x138);
