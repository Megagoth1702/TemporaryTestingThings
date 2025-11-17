modded class SCR_CampaignMobileAssemblyComponent : ScriptComponent
{

	//------------------------------------------------------------------------------------------------
	//!
	//! \param[in] status
	//! \param[in] playerId
	//! \return
//	override bool Deploy(SCR_EMobileAssemblyStatus status, int playerId = 0)
//	{
//		if (!m_bIsInRadioRange && status == SCR_EMobileAssemblyStatus.DEPLOYED)
//			return false;
//		
//		if (!m_ParentFaction)
//			return false;
//		
//		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
//		
//		if (!campaign)
//			return false;
//		
//		if (m_ParentFaction.GetMobileAssembly() && status == SCR_EMobileAssemblyStatus.DEPLOYED)
//			return false;
//		
//		if (status == SCR_EMobileAssemblyStatus.DEPLOYED)
//		{
//			CreateSpawnpoint();
//			IEntity radioEntity = m_SpawnPoint.GetRootParent();
//			EvaluateRespawnTime(m_SpawnPoint, radioEntity);
//			m_bIsDeployed = true;
//		}
//		else
//		{
//			m_bIsDeployed = false;
//			
//			if (m_SpawnPoint)
//				RplComponent.DeleteRplEntity(m_SpawnPoint, false);
//		}
//		
//		OnDeployChanged();
//		Replication.BumpMe();
//		campaign.BroadcastMHQFeedback(status, playerId, GetGame().GetFactionManager().GetFactionIndex(m_ParentFaction));
//
//		return true;
//	}
	
	//------------------------------------------------------------------------------------------------
	void EvaluateRespawnTime(SCR_SpawnPoint spawnPoint, IEntity owner)
	{
	    if (!spawnPoint) return;
		
		SCR_CampaignFactionManager factionManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
		SCR_CampaignFaction pFaction = SCR_CampaignFaction.Cast(factionManager.GetLocalPlayerFaction());
		
		if (!pFaction) return;
		
		SCR_CampaignMilitaryBaseComponent HQ = pFaction.GetMainBase();
		
		if (!HQ) return;
		
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		
		if (!CampaignInfo) return;
		
		float multiplier = CampaignInfo.GetMobileSpawnTimeDistanceMultiplierMHQ();
		
		vector HQDistance = HQ.GetOwner().GetOrigin();
		vector spawnpointDistance = spawnPoint.GetOrigin();
		float distance = vector.DistanceXZ(HQDistance,spawnpointDistance);
		float respawnTime = Math.Clamp(distance * multiplier, 0, 120);
		
	    spawnPoint.SetRespawnTime(respawnTime);
		
#ifdef WORKBENCH
		Print("Distance from HQ: " + distance + " Respawn Time: " + respawnTime);
#endif
	}
	
	//------------------------------------------------------------------------------------------------
	void EvaluateFrontlineRespawnTime(SCR_SpawnPoint spawnPoint, IEntity owner)
	{
	    if (!spawnPoint)
	        return;
			
		if (!owner) return;
		
		SCR_CampaignFaction ownerFaction = SCR_CampaignFaction.Cast(SCR_CampaignFaction.GetEntityFaction(owner));
		
		if (!ownerFaction) return;
		
		SCR_CampaignMilitaryBaseComponent enemyHQ = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager()).GetEnemyFaction(ownerFaction).GetMainBase();
		
		if (!enemyHQ) return;
		
		float respawnTime = 0;
		SCR_RespawnTimerComponent respawnTimerComponent = SCR_RespawnTimerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_RespawnTimerComponent));
		if (respawnTimerComponent)
		{
			respawnTime = respawnTimerComponent.GetRespawnTime();
		}

	    if (enemyHQ.GetIsEntityInMyRange(owner))
	    {
			
	        spawnPoint.SetRespawnTime(enemyHQ.GetFrontlineRespawnTime());
	    }
		else
		{
			spawnPoint.SetRespawnTime(respawnTime);
		}
	}

		//------------------------------------------------------------------------------------------------
	//! \return
	override int GetRadioRange()
	{
		IEntity truck = GetOwner().GetParent();
		
		if (!truck)
			return 0;
		
		BaseRadioComponent comp = BaseRadioComponent.Cast(truck.FindComponent(BaseRadioComponent));
		
		if (!comp)
			return 0;
		
		BaseTransceiver tsv = comp.GetTransceiver(0);
		
		if (!tsv)
			return 0;
		
		float range = tsv.GetRange();
		SCR_GameModeCampaign campaignInfo = SCR_GameModeCampaign.GetInstance();
		if (campaignInfo)
		{
			range = campaignInfo.GetMHQRange();
			tsv.SetRange(range);
		}
		
		return range;
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	override void CreateSpawnpoint()
	{
		SCR_CampaignMobileAssemblyComponentClass componentData = SCR_CampaignMobileAssemblyComponentClass.Cast(GetComponentData(GetOwner()));

		if (!componentData)
		{
			PrintFormat("SCR_CampaignMobileAssemblyComponent: CreateSpawnpoint : No Component Data. Returning...");
			return;
		}
	
		Resource spawnpointResource = Resource.Load(componentData.GetSpawnpointPrefab());
		
		if (!spawnpointResource || !spawnpointResource.IsValid())
		{
			PrintFormat("SCR_CampaignMobileAssemblyComponent: CreateSpawnpoint : No Spawnpoint Resource or is not valid. Returning...");
			return;
		}

		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		GetOwner().GetTransform(params.Transform);
		m_SpawnPoint = SCR_SpawnPoint.Cast(GetGame().SpawnEntityPrefab(spawnpointResource, null, params));
		
		if (!m_SpawnPoint)
		{
			PrintFormat("SCR_CampaignMobileAssemblyComponent: CreateSpawnpoint : No Spawnpoint. Returning...");
			return;
		}

//		if (m_iParentFaction == -1)
//		{
//			PrintFormat("SCR_CampaignMobileAssemblyComponent: CreateSpawnpoint : Parent Faction Index is invalid. Attempting to reset");
//		
//			SCR_CampaignFactionManager fManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
//			int index = fManager.GetFactionIndex(m_ParentFaction);
//			SetParentFactionID(index);
//		}
		
		m_StandaloneComponent = SCR_CampaignMobileAssemblyStandaloneComponent.Cast(m_SpawnPoint.FindComponent(SCR_CampaignMobileAssemblyStandaloneComponent));

		if (m_StandaloneComponent)
		{
			int range = GetRadioRange();
			SCR_GameModeCampaign campaignInfo = SCR_GameModeCampaign.GetInstance();
			if (campaignInfo)
			{
				range = campaignInfo.GetMHQRange();
			}
			
			//m_StandaloneComponent.SetRadioRange(GetRadioRange());
			m_StandaloneComponent.SetRadioRange(range);
			m_StandaloneComponent.SetVehicle(SCR_EntityHelper.GetMainParent(GetOwner(), true));
			m_StandaloneComponent.SetParentFactionID(m_iParentFaction);
		}

		m_iSpawnpointId = Replication.FindId(m_SpawnPoint);
		OnSpawnpointCreated();
		Replication.BumpMe();
	}
}