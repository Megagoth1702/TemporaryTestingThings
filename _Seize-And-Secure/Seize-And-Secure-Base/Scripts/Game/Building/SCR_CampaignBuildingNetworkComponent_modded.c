modded class SCR_CampaignBuildingNetworkComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override protected void RpcAsk_AddXPReward(int playerId)
	{
		super.RpcAsk_AddXPReward(playerId);
		
		// Add supplies to personal wallet based on rank
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
		IEntity playerEntity = playerManager.GetPlayerControlledEntity(playerId);
		if (!playerEntity)
			return;
		
		SCR_XPHandlerComponent compXPReward = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
		if (!compXPReward)
			return;
		
		// Award extra XP for being near team member
		if (compXPReward.IsNearFactionMember(5, playerId))
		{
			GetGame().GetCallqueue().CallLater(compXPReward.SeizeXPAward, 5000, false, playerId, SCR_EXPRewards.NEAR_SQUAD_MEMBER, 0.5);
		}
		
		vector playerPosition = playerEntity.GetOrigin();
		if (playerPosition && compXPReward.GetIsOnFrontline(playerPosition))
		{
			GetGame().GetCallqueue().CallLater(compXPReward.SeizeXPAward, 7000, false, playerId, SCR_EXPRewards.FRONTLINE_BONUS, 1);
		}
	}
	
}
