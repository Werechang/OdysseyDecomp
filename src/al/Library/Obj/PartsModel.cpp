#include "al/Library/Obj/PartsModel.h"

#include "al/Library/Base/String.h"
#include "al/Library/LiveActor/ActorClippingFunction.h"
#include "al/Library/LiveActor/ActorFlagFunction.h"
#include "al/Library/LiveActor/ActorInitInfo.h"
#include "al/Library/LiveActor/ActorModelFunction.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/LiveActor/ActorResourceFunction.h"
#include "al/Library/LiveActor/SubActorFunction.h"
#include "al/Library/LiveActor/SubActorKeeper.h"
#include "al/Library/MapObj/MtxUtil.h"
#include "al/Library/Math/MathUtil.h"
#include "al/Library/Yaml/ByamlUtil.h"

namespace al {
PartsModel::PartsModel(const char* name) : LiveActor(name) {}

void PartsModel::endClipped() {
    LiveActor::endClipped();
    updatePose();
}
void PartsModel::calcAnim() {
    updatePose();
    LiveActor::calcAnim();
}
void PartsModel::attackSensor(HitSensor* target, HitSensor* source) {
    mParentModel->attackSensor(target, source);
}
bool PartsModel::receiveMsg(const SensorMsg* message, HitSensor* source, HitSensor* target) {
    return mParentModel->receiveMsg(message, source, target);
}
void PartsModel::initPartsDirect(LiveActor* parent, const ActorInitInfo& initInfo,
                                 const char* arcName, const sead::Matrix34f* jointMtx,
                                 const sead::Vector3f& localTrans,
                                 const sead::Vector3f& localRotate,
                                 const sead::Vector3f& localScale, bool useFollowMtxScale) {
    mParentModel = parent;
    mJointMtx = jointMtx;
    mIsUseFollowMtxScale = useFollowMtxScale;
    initChildActorWithArchiveNameNoPlacementInfo(this, initInfo, arcName, nullptr);
    invalidateClipping(this);
    registerSubActor(parent, this);
    makeActorAlive();
    mIsUseLocalPos = true;
    mLocalTrans = localTrans;
    mLocalRotate = localRotate;
    mLocalScale = localScale;
}
void PartsModel::initPartsSuffix(LiveActor* parent, const ActorInitInfo& initInfo,
                                 const char* arcName, const char* suffix,
                                 const sead::Matrix34f* jointMtx, bool useFollowMtxScale) {
    mParentModel = parent;
    mJointMtx = jointMtx;
    mIsUseFollowMtxScale = useFollowMtxScale;
    initChildActorWithArchiveNameNoPlacementInfo(this, initInfo, arcName, suffix);
    invalidateClipping(this);
    registerSubActor(parent, this);
    makeActorAlive();
}
void PartsModel::initPartsMtx(LiveActor* parent, const ActorInitInfo& initInfo, const char* arcName,
                              const sead::Matrix34f* jointMtx, bool useFollowMtxScale) {
    mParentModel = parent;
    mJointMtx = jointMtx;
    mIsUseFollowMtxScale = useFollowMtxScale;
    initChildActorWithArchiveNameNoPlacementInfo(this, initInfo, arcName, nullptr);
    invalidateClipping(this);
    registerSubActor(parent, this);
    makeActorAlive();
}
void PartsModel::initPartsFixFile(LiveActor* parent, const ActorInitInfo& initInfo,
                                  const char* arcName, const char* arcSuffix, const char* suffix) {
    initPartsFixFileNoRegister(parent, initInfo, arcName, arcSuffix, suffix);
    registerSubActor(parent, this);
}
void PartsModel::initPartsFixFileNoRegister(LiveActor* parent, const ActorInitInfo& initInfo,
                                            const char* arcName, const char* arcSuffix,
                                            const char* suffix) {
    mParentModel = parent;
    mJointMtx = parent->getBaseMtx();

    initChildActorWithArchiveNameNoPlacementInfo(this, initInfo, arcName, arcSuffix);
    invalidateClipping(this);

    sead::FixedSafeString<0x80> initArcName;
    createFileNameBySuffix(&initArcName, "InitPartsFixInfo", suffix);

    if (!isExistModelResourceYaml(mParentModel, initArcName.cstr(), nullptr))
        return makeActorAlive();
    u8* modelResByml = getModelResourceYaml(mParentModel, initArcName.cstr(), nullptr);
    ByamlIter modelResIter(modelResByml);

    const char* jointName = nullptr;
    modelResIter.tryGetStringByKey(&jointName, "JointName");

    if (jointName)
        mJointMtx = getJointMtxPtr(mParentModel, jointName);

    tryGetByamlV3f(&mLocalTrans, modelResIter, "LocalTrans");
    tryGetByamlV3f(&mLocalRotate, modelResIter, "LocalRotate");
    tryGetByamlV3f(&mLocalScale, modelResIter, "LocalScale");

    mIsUseLocalScale = tryGetByamlKeyBoolOrFalse(modelResIter, "UseLocalScale");

    if (!isNearZero(mLocalTrans, 0.001f) || !isNearZero(mLocalRotate, 0.001f) || mIsUseLocalScale)
        mIsUseLocalPos = true;

    mIsUseFollowMtxScale = tryGetByamlKeyBoolOrFalse(modelResIter, "UseFollowMtxScale");

    makeActorAlive();
}
// NON-MATCHING: needs to have proper matrix math implemented still
void PartsModel::updatePose() {
    sead::Matrix34f poseMtx;
    sead::Matrix34f jointMtx;

    if (!mUpdate)
        return;
    if (mIsUseLocalPos) {
        jointMtx = *mJointMtx;
        if (mIsUseFollowMtxScale) {
            sead::Vector3f mtxScale;
            calcMtxScale(&mtxScale, jointMtx);
            mtxScale *= 1.0f;
            setScale(this, mtxScale);
        }
        normalize(&jointMtx);
        updatePoseMtx(this, &jointMtx);
        return;
    }

    mLocalRotate *= 0.017453f;

    poseMtx.makeRT(mLocalRotate, mLocalTrans);

    if (mIsUseFollowMtxScale || mIsUseLocalScale) {
    }
}
void PartsModel::offSyncAppearAndHide() {
    offSyncAppearSubActor(mParentModel, this);
    offSyncHideSubActor(mParentModel, this);
}
void PartsModel::onSyncAppearAndHide() {
    onSyncHideSubActor(mParentModel, this);

    if (isHideModel(mParentModel))
        hideModelIfShow(this);
    else
        showModelIfHide(this);

    onSyncAppearSubActor(mParentModel, this);

    if (isDead(mParentModel) && isAlive(this))
        makeActorDead();
    else if (isAlive(mParentModel) && isDead(this))
        makeActorAlive();
}
}  // namespace al