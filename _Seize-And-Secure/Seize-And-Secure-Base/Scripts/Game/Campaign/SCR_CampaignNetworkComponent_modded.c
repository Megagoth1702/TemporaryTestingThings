//! Takes care of Campaign-specific server <> client communication and requests
modded class SCR_CampaignNetworkComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void OnSuppliesLoaded(vector position, float amount, notnull SCR_ResourceComponent resourceComponentTo)
	{
		super.OnSuppliesLoaded(position, amount, resourceComponentTo);

		int playerId = m_PlayerController.GetPlayerId();
		if (!playerId)
			return;
		
		SCR_XPHandlerComponent compXPReward = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
		if (compXPReward && compXPReward.IsNearFactionMember(8, playerId))
		{
			//AwardXP(playerId, SCR_EXPRewards.CUSTOM_2);
			//GetGame().GetCallqueue().CallLater(compXPReward.SeizeXPAward, 10000, false, playerId, SCR_EXPRewards.NEAR_SQUAD_MEMBER, 0.25);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnSuppliesUnloaded(vector position, float amount, int playerId, notnull SCR_ResourceComponent resourceComponentFrom, int assistantId = 0)
	{
		super.OnSuppliesUnloaded(position, amount, playerId, resourceComponentFrom, assistantId);
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
		IEntity playerEntity = playerManager.GetPlayerControlledEntity(playerId);
		if (!playerEntity)
			return;
		
		SCR_CampaignMilitaryBaseManager baseManager = SCR_GameModeCampaign.GetInstance().GetBaseManager();
		if (!baseManager)
			return;
			
		bool isFrontline;
		SCR_CampaignMilitaryBaseComponent campaignBase = baseManager.FindClosestBase(position);			
		if (campaignBase && campaignBase.GetIsFrontlineBase())
		{
			isFrontline = true;
		}
		else
		{
			isFrontline = false;
		}
	
		SCR_XPHandlerComponent compXP = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
		if (compXP)
		{
			if (compXP.IsNearFactionMember(8, playerId))
			{
				GetGame().GetCallqueue().CallLater(compXP.SeizeXPAward, 10000, false, playerId, SCR_EXPRewards.NEAR_SQUAD_MEMBER, 0.25);
			}
			
			if (isFrontline)
			{
				GetGame().GetCallqueue().CallLater(compXP.SeizeXPAward, 5000, false, playerId, SCR_EXPRewards.FRONTLINE_BONUS, 1);
			}
		}
	}
}
