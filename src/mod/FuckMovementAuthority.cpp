#include "mod/FuckMovementAuthority.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/mod/RegisterHelper.h"
#include "mc/deps/ecs/gamerefs_entity/GameRefsEntity.h"
#include "mc/entity/components/MovementCorrection.h"
#include "mc/entity/components/ServerCorrectionPolicy.h"
#include "mc/network/packet/PlayerAuthInputPacket.h"
#include "mc/world/actor/player/Player.h"


namespace fuck_movement_authority {

FuckMovementAuthority& FuckMovementAuthority ::getInstance() {
    static FuckMovementAuthority instance;
    return instance;
}

LL_TYPE_INSTANCE_HOOK(
    FuckMovementAuthorityHook,
    ll::memory::HookPriority::Normal,
    ServerCorrectionPolicy,
    &ServerCorrectionPolicy ::$shouldCorrectMovement,
    MovementCorrection,
    ::EntityContext&               entity,
    ::PlayerAuthInputPacket const& packet,
    uint64                         frame,
    uchar const                    currentCounter,
    bool                           isStrictMovement
) {
    if (Player::tryGetFromEntity(entity, false)->mLastHurtByMobTime > 50)
        return origin(entity, packet, frame, currentCounter, isStrictMovement);
    MovementCorrection result;
    result.mMethod               = CorrectionMethod::AcceptClient;
    result.mAcceptPosition       = packet.mPos;
    result.mNewDivergenceCounter = 0;
    return result;
}

bool FuckMovementAuthority ::load() {
    getSelf().getLogger().debug("Loading...");
    // Code for loading the mod goes here.
    return true;
}

bool FuckMovementAuthority ::enable() {
    getSelf().getLogger().debug("Enabling...");
    FuckMovementAuthorityHook::hook();
    // Code for enabling the mod goes here.
    return true;
}

bool FuckMovementAuthority ::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    return true;
}

} // namespace fuck_movement_authority

LL_REGISTER_MOD(
    fuck_movement_authority ::FuckMovementAuthority,
    fuck_movement_authority ::FuckMovementAuthority ::getInstance()
);
