//! Base class which all deployable spawn points / radios inherit from
modded class SCR_BaseDeployableSpawnPointComponent : SCR_BaseDeployableInventoryItemComponent
{
	
	
	//------------------------------------------------------------------------------------------------
	//! Spawns replacement composition and attaches owner entity to it - called from SCR_DeployItemBaseAction.PerformAction
	//! \param[in] userEntity
	override void Deploy(IEntity userEntity = null, bool reload = false)
	{
		super.Deploy(userEntity, reload);
		m_SpawnPoint.SetRespawnTime(RespawnTimeDistance());		
	}
		
	//------------------------------------------------------------------------------------------------
	float RespawnTimeDistance()
	{
		SCR_CampaignFactionManager factionManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
		SCR_CampaignFaction pFaction = SCR_CampaignFaction.Cast(factionManager.GetLocalPlayerFaction());
		int originalRespawnTime = m_SpawnPoint.GetRespawnTime();
		
		if (!pFaction) return originalRespawnTime;
		
		SCR_CampaignMilitaryBaseComponent HQ = pFaction.GetMainBase();
		
		if (!HQ) return originalRespawnTime;
		
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		
		if (!CampaignInfo) return originalRespawnTime;
		
		float multiplier = CampaignInfo.GetMobileSpawnTimeDistanceMultiplierRP();
		vector HQDistance = HQ.GetOwner().GetOrigin();
		vector spawnpointDistance = m_SpawnPoint.GetOrigin(); //spawnPoint.GetOrigin();
		float distance = vector.DistanceXZ(HQDistance,spawnpointDistance);
		float respawnTime = Math.Clamp(distance * multiplier, 0, 120);
		
		return respawnTime;
	}
	
	//------------------------------------------------------------------------------------------------
	void EvaluateRespawnTime(SCR_SpawnPoint spawnPoint, IEntity owner)
	{
	    if (!spawnPoint)
			return;
		
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();		
		if (!CampaignInfo) 
			return;
		
		SCR_CampaignFactionManager factionManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return;
		
		SCR_CampaignFaction pFaction = SCR_CampaignFaction.Cast(factionManager.GetLocalPlayerFaction());		
		if (!pFaction) 
			return;
		
		SCR_CampaignMilitaryBaseComponent HQ = pFaction.GetMainBase();
		
		if (!HQ) 
			return;
		
		float multiplier = CampaignInfo.GetMobileSpawnTimeDistanceMultiplierRP();
		vector HQDistance = HQ.GetOwner().GetOrigin();
		vector spawnpointDistance = spawnPoint.GetOrigin();
		float distance = vector.DistanceXZ(HQDistance,spawnpointDistance);
		float respawnTime = Math.Clamp(distance * multiplier, 0, 120);
		
		spawnPoint.SetRespawnTime(respawnTime);
		
#ifdef WORKBENCH
		Print("Distance from HQ: " + distance + " Respawn Time: " + respawnTime);
#endif
	}
	
}
