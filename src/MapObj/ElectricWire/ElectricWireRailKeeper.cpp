#include "MapObj/ElectricWire/ElectricWireRailKeeper.h"

#include "Library/Camera/CameraTicket.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorInitFunction.h"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/LiveActorUtil.h"
#include "Library/Math/MathAngleUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Rail/RailUtil.h"
#include "Library/Stage/StageSwitchKeeper.h"
#include "Library/Thread/FunctorV0M.h"

#include "MapObj/ElectricWire/ElectricWire.h"

namespace {
NERVE_IMPL(ElectricWireRailKeeper, Standby)
NERVE_IMPL(ElectricWireRailKeeper, Wait)

NERVES_MAKE_NOSTRUCT(ElectricWireRailKeeper, Standby, Wait)
}  // namespace

void ElectricWireRailKeeper::appear() {
    LiveActor::appear();
    mElectricWire->tryUpdateDisplayModel();
    al::setNerve(this, &Wait);
}

void ElectricWireRailKeeper::appearBySwitch() {
    if (al::isAlive(this))
        return;

    appear();
    al::invalidateClipping(this);
    al::setNerve(this, &Standby);
}

void ElectricWireRailKeeper::exeStandby() {
    if (al::isGreaterStep(this, 10)) {
        al::validateClipping(this);
        al::setNerve(this, &Wait);
    }
}

void ElectricWireRailKeeper::exeWait() {}

bool ElectricWireRailKeeper::isNerveStandby() const {
    return al::isNerve(this, &Standby);
}

al::PlacementInfo* ElectricWireRailKeeper::getRailPointInfo(s32 index) const {
    return al::getRailPointInfo(this, index);
}

bool ElectricWireRailKeeper::isRailPointEnableTargetEndCollision(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret = true;
    al::tryGetArg(&ret, *info, "IsEnableTargetEndCollision");
    return ret;
}

bool ElectricWireRailKeeper::isRailPointFaceToCameraDir(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret;
    if (al::tryGetArg(&ret, *info, "IsFaceToCamera"))
        return ret;
    return false;
}

bool ElectricWireRailKeeper::isRailPointIgnore(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret;
    if (al::tryGetArg(&ret, *info, "IsIgnore"))
        return ret;
    return false;
}

bool ElectricWireRailKeeper::isRailPointIsDisplayPointModelForce(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret = false;
    al::tryGetArg(&ret, *info, "IsDisplayPointModelForce");
    return ret;
}

bool ElectricWireRailKeeper::isRailPointIsExpandRailSelectableAngle(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret = false;
    al::tryGetArg(&ret, *info, "IsExpandRailSelectableAngle");
    return ret;
}

bool ElectricWireRailKeeper::isRailPointIsNeedCamera(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret = false;
    al::tryGetArg(&ret, *info, "IsNeedCamera");
    return ret;
}

bool ElectricWireRailKeeper::isRailPointIsNeedStartCameraHackEnd(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret = false;
    al::tryGetArg(&ret, *info, "IsNeedStartCameraHackEnd");
    return ret;
}

bool ElectricWireRailKeeper::isRailPointPlacementPole(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret;
    if (al::tryGetArg(&ret, *info, "IsPlacementPole"))
        return ret;
    return false;
}

bool ElectricWireRailKeeper::isRailPointSpringFix(s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    bool ret;
    if (al::tryGetArg(&ret, *info, "IsSpringFix"))
        return ret;
    return false;
}

void ElectricWireRailKeeper::kill() {
    LiveActor::kill();
    mElectricWire->tryUpdateDisplayModel();
}

void ElectricWireRailKeeper::killBySwitch() {
    if (al::isDead(this))
        return;
    kill();
}

bool ElectricWireRailKeeper::tryGetPlayerBottomPosOnVerticalMove(
    sead::Vector3f* playerBottomPos) const {
    if (mPosType == PosType::BOTTOM) {
        playerBottomPos->set(mPlayerPosOnVerticalMove);
        return true;
    }
    return false;
}

bool ElectricWireRailKeeper::tryGetPlayerHeadPosOnVerticalMove(
    sead::Vector3f* playerHeadPos) const {
    if (mPosType == PosType::HEAD) {
        playerHeadPos->set(mPlayerPosOnVerticalMove);
        return true;
    }
    return false;
}

bool ElectricWireRailKeeper::tryGetRailPointDestinationTrans(sead::Vector3f* out, s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    return al::tryGetLinksTrans(out, *info, "DestinationPoint");
}

bool ElectricWireRailKeeper::tryGetRailPointFastenerMoveLimitAreaFlag(s32* out, s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    return al::tryGetArg(out, *info, "FastenerMoveLimitAreaFlag");
}

bool ElectricWireRailKeeper::tryGetRailPointOutDir(sead::Vector3f* out, s32 index) const {
    auto info = al::getRailPointInfo(this, index);
    al::PlacementInfo linksInfo{};
    if (!al::tryGetLinksInfo(&linksInfo, *info, "DestinationPoint")) {
        bool isOutToRailPointDir = false;
        al::tryGetArg(&isOutToRailPointDir, *info, "IsOutToRailPointDir");
        if (isOutToRailPointDir)
            return al::tryGetUp(out, *info);
    } else {
        sead::Vector3f linksTrans;
        if (al::tryGetLinksTrans(&linksTrans, *info, "DestinationPoint")) {
            sead::Vector3f railPointPos{};
            al::calcRailPointPos(&railPointPos, this, index);
            out->set(linksTrans);
            *out -= railPointPos;
            return al::tryNormalizeOrZero(out);
        }
    }
    return false;
}

void ElectricWireRailKeeper::endCameraIfActive() {
    for (s32 i = 0; i < mCameraTickets.size(); ++i) {
        auto holder = mCameraTickets[i];
        if (al::isActiveCamera(holder->ticket))
            al::endCamera(mElectricWire, holder->ticket, -1, false);
    }
}

al::CameraTicket* ElectricWireRailKeeper::findRailPointCameraTicket(s32 pointIdx) const {
    for (s32 i = 0; i < mCameraTickets.size(); ++i) {
        auto holder = mCameraTickets[i];
        if (holder->pointIdx == pointIdx)
            return holder->ticket;
    }
    return nullptr;
}

const al::CameraTicket*
ElectricWireRailKeeper::findRailPointStartCameraHackEndTicket(s32 pointIdx) const {
    for (s32 i = 0; i < mCameraTickets.size(); ++i) {
        auto ticket = mCameraTickets[i];
        if (ticket->pointIdx == pointIdx)
            return ticket->ticketHackEnd;
    }
    return nullptr;
}

ElectricWireRailKeeper::ElectricWireRailKeeper(const char* name, al::LiveActor* wire)
    : LiveActor(name), mElectricWire(reinterpret_cast<ElectricWire*>(wire)) {}

ElectricWireRailKeeper::ElectricWireRailKeeper(const char* name) : LiveActor(name) {}

void ElectricWireRailKeeper::init(const al::ActorInitInfo& info) {
    using ElectricWireRailKeeperFunctor =
        al::FunctorV0M<ElectricWireRailKeeper*, void (ElectricWireRailKeeper::*)()>;

    al::initActorSceneInfo(this, info);
    al::initActorPoseTRSV(this);
    al::initActorSRT(this, info);
    al::initActorClipping(this, info);
    al::initStageSwitch(this, info);

    auto wire = mElectricWire;
    al::tryGetArg(&mIsShowLine, info, "IsShowLine");
    if (wire->mIsElectricWireRadio)
        mIsShowLine = false;
    al::tryGetArg(&mIsThrowaway, info, "IsThrowaway");
    if (al::tryGetLinksTrans(&mPlayerPosOnVerticalMove, info, "PlayerHeadPosOnVerticalMove"))
        mPosType = PosType::HEAD;
    else if (al::tryGetLinksTrans(&mPlayerPosOnVerticalMove, info, "PlayerBottomPosOnVerticalMove"))
        mPosType = PosType::BOTTOM;
    if (!al::isExistRail(info, "Rail"))
        return makeActorDead();
    initRailKeeper(info, "Rail");
    s32 pointNum = al::getRailPointNum(this);
    s32 needCameraPointNum = 0;
    for (s32 i = 0; i < pointNum; ++i) {
        auto railPointInfo = al::getRailPointInfo(this, i);
        bool isNeedCamera = false;
        al::tryGetArg(&isNeedCamera, *railPointInfo, "IsNeedCamera");
        if (!isNeedCamera) {
            railPointInfo = al::getRailPointInfo(this, i);
            isNeedCamera = false;
            al::tryGetArg(&isNeedCamera, *railPointInfo, "IsNeedStartCameraHackEnd");
            if (!isNeedCamera)
                continue;
        }
        ++needCameraPointNum;
    }
    if (needCameraPointNum > 0)
        mCameraTickets.allocBuffer(needCameraPointNum, nullptr);
    for (s32 i = 0; i < pointNum; ++i) {
        auto railPointInfo = al::getRailPointInfo(this, i);
        bool isNeedCamera = false;
        al::tryGetArg(&isNeedCamera, *railPointInfo, "IsNeedCamera");
        al::CameraTicket* ticket = nullptr;
        if (isNeedCamera) {
            auto* id = new sead::FixedSafeString<0x20>();
            id->format("%d", i);
            ticket = al::initObjectCamera(mElectricWire, info, id->cstr(), nullptr);
        }
        railPointInfo = al::getRailPointInfo(this, i);
        isNeedCamera = false;
        al::tryGetArg(&isNeedCamera, *railPointInfo, "IsNeedStartCameraHackEnd");
        al::CameraTicket* ticketHack = nullptr;
        if (isNeedCamera) {
            auto* id = new sead::FixedSafeString<0x20>();
            id->format("%d(Entrance)", i);
            ticketHack = al::initEntranceCamera(mElectricWire, info.getPlacementInfo(), id->cstr());
        }
        if (ticket != nullptr || ticketHack != nullptr)
            mCameraTickets.pushBack(new TicketHolder{ticket, ticketHack, i});
    }
    al::initExecutorUpdate(this, info, "地形オブジェ[Movement]");
    al::initNerve(this, &Wait, 0);
    makeActorAlive();
    if (al::listenStageSwitchOnAppear(
            this, ElectricWireRailKeeperFunctor(this, &ElectricWireRailKeeper::appearBySwitch)))
        kill();
    al::listenStageSwitchOnKill(
        this, ElectricWireRailKeeperFunctor(this, &ElectricWireRailKeeper::killBySwitch));
}
