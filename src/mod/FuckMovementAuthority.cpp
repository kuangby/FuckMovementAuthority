#include "mod/FuckMovementAuthority.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/service/Bedrock.h"
#include "mc/entity/components/PackedItemUseLegacyInventoryTransaction.h"
#include "mc/entity/components/PlayerBlockActionData.h"
#include "mc/entity/components/PlayerBlockActions.h"
#include "mc/entity/components/player_tick_policy/ThrottledTickPolicy.h"
#include "mc/entity/systems/ServerPlayerInventoryTransactionSystem.h"
#include "mc/network/ServerNetworkHandler.h"
#include "mc/network/ServerPlayerBlockUseHandler.h"
#include "mc/network/packet/InventoryTransactionPacket.h"
#include "mc/network/packet/InventoryTransactionPacketPayload.h"
#include "mc/network/packet/LegacySetSlot.h"
#include "mc/network/packet/PlayerAuthInputPacket.h"
#include "mc/server/ServerPlayer.h"
#include "mc/world/inventory/network/ItemStackRequestData.h"
#include "mc/world/inventory/transaction/ItemUseInventoryTransaction.h"
#include "mc/world/level/Level.h"

#ifdef LL_PLAT_C
#include "ll/api/service/Bedrock.h"
#include "mc/server/ServerInstance.h"
#include <thread>
#endif


namespace fuck_movement_authority {

FuckMovementAuthority& FuckMovementAuthority ::getInstance() {
    static FuckMovementAuthority instance;
    return instance;
}
int allow = 0;

LL_TYPE_INSTANCE_HOOK(
    FuckMovementAuthorityHook,
    ll::memory::HookPriority::Normal,
    PlayerTickPolicy::ThrottledTickPolicy,
    &ThrottledTickPolicy ::$shouldTickPlayer,
    ::IPlayerTickPolicy::TickAction,
    uint64 const /*creditTicks*/,
    uint64 unprocessedTicksSize
) {
    return unprocessedTicksSize == 0 ? TickAction::StopProcessing : TickAction::ProcessTick;
}

// LL_TYPE_INSTANCE_HOOK(
//     StripOneShotActionsHook,
//     ll::memory::HookPriority::Normal,
//     ServerNetworkHandler,
//     &ServerNetworkHandler::$handle,
//     void,
//     ::NetworkIdentifier const&     source,
//     ::PlayerAuthInputPacket const& packet
// ) {
// #ifdef LL_PLAT_C
//     if (auto serverInstance = ll::service::getServerInstance();
//         !serverInstance || std::this_thread::get_id() != serverInstance->mServerInstanceThread->get_id())
//         return origin(source, packet);
// #endif
//     if (auto player = thisFor<NetEventCallback>()->_getServerPlayer(source, packet.mSenderSubId)) {
//         auto& pkt = const_cast<PlayerAuthInputPacket&>(packet);

//         // ---- 1. 剥离(必须在 origin 之前,否则出队时会二次执行)----

//         // 1a. 方块动作(Start/Continue/Stop DestroyBlock 等)
//         std::vector<PlayerBlockActionData> blockActions;
//         if (!pkt.mPlayerBlockActions->mActions->empty()) {
//             blockActions = std::move(*pkt.mPlayerBlockActions->mActions);
//         }
//         // 1b. 物品堆请求 —— 成员本身就是 unique_ptr,直接 move
//         auto itemStackRequest = std::move(pkt.mItemStackRequest);
//         // 1c. 放置/使用方块的 legacy 事务 —— 同上
//         auto itemUseTransaction = std::move(pkt.mItemUseTransaction);

//         if (blockActions.empty() && !itemStackRequest && !itemUseTransaction) {
//             return origin(source, packet); // 纯移动包,快进
//         }

//         pkt.mInputData->reset((size_t)::PlayerAuthInputPacket::InputData::PerformBlockActions);
//         pkt.mInputData->reset((size_t)::PlayerAuthInputPacket::InputData::PerformItemStackRequest);

//         // ---- 2. 移动数据照常入队(队列里只剩移动,服务端权威移动/rewind 不受影响)----
//         origin(source, packet);

//         // ---- 3. 立即执行,与队列出队路径同一批入口 ----

//         // 3a. 挖掘:与 ProcessPlayerActionPacketSystemImpl::doProcessPlayerActionPacket
//         //     出队时调用的是同一个函数; textFilter 同样取自 ServerNetworkHandler 成员
//         if (!blockActions.empty() || itemStackRequest) {
//             PlayerBlockActions actions{};
//             *actions.mActions = std::move(blockActions);
//             ServerPlayerBlockUseHandler::onBeforeMovementSimulation(
//                 *player,
//                 actions,
//                 std::move(itemStackRequest),
//                 *mTextFilteringProcessor
//             );
//         }

//         // 3b. 放置:出队路径是把包内 PackedItemUseLegacyInventoryTransaction 组建成
//         //     InventoryTransactionPacket 再调 transactInventoryPacket,这里照搬
//         if (itemUseTransaction) {
//             ::InventoryTransactionPacket txPacket{
//                 ::InventoryTransactionPacketPayload{
//                                                     std::make_unique<::ItemUseInventoryTransaction>(*itemUseTransaction->mTransaction),
//                                                     false // mIsClientSide
//                 }
//             };
//             // legacy 字段是 TypedStorageImpl 包装,需要 *;mTransaction/mIsClientSide 直通,不要 *
//             *txPacket.mLegacyRequestId    = *itemUseTransaction->mID;
//             *txPacket.mLegacySetItemSlots = *itemUseTransaction->mSlots;
//             ::ServerPlayerInventoryTransactionSystem::transactInventoryPacket(
//                 txPacket,
//                 *player,
//                 player->getLevel().getBlockPalette()
//             );
//         }
//     }
// }

bool FuckMovementAuthority ::load() {
    getSelf().getLogger().debug("Loading...");
    // StripOneShotActionsHook::hook();
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
