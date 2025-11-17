[EntityEditorProps(category: "GameScripted/Campaign", description: "Allow respawning at this vehicle in Campaign", color: "0 0 255 255")]
modded class SCR_CampaignMobileAssemblyComponentClass : ScriptComponentClass
{
	[Attribute("{6D282026AB95FC81}Prefabs/MP/Campaign/CampaignMobileAssemblySpawnpoint.et", UIWidgets.ResourceNamePicker, "", "et")]
	protected ResourceName m_sSpawnpointPrefab;
	
	SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
	
	//------------------------------------------------------------------------------------------------	
	override ResourceName GetSpawnpointPrefab()
	{
		if (CampaignInfo)
		m_sSpawnpointPrefab = CampaignInfo.GetMHQPrefab();
		
		return m_sSpawnpointPrefab;
	}
	
}

modded class SCR_CampaignMobileAssemblyComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//!
	override void CreateSpawnpoint()
	{
		SCR_CampaignMobileAssemblyComponentClass componentData = SCR_CampaignMobileAssemblyComponentClass.Cast(GetComponentData(GetOwner()));

		if (!componentData)
			return;
		
		Resource spawnpointResource = Resource.Load(componentData.GetSpawnpointPrefab());
		
		if (!spawnpointResource || !spawnpointResource.IsValid())
			return;
		
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		GetOwner().GetTransform(params.Transform);
		m_SpawnPoint = SCR_SpawnPoint.Cast(GetGame().SpawnEntityPrefab(spawnpointResource, null, params));
		
		if (!m_SpawnPoint)
			return;
		
		m_StandaloneComponent = SCR_CampaignMobileAssemblyStandaloneComponent.Cast(m_SpawnPoint.FindComponent(SCR_CampaignMobileAssemblyStandaloneComponent));

		if (m_StandaloneComponent)
		{
			m_StandaloneComponent.SetRadioRange(GetRadioRange());
			m_StandaloneComponent.SetVehicle(SCR_EntityHelper.GetMainParent(GetOwner(), true));
			
			// Delay so map item can initialize
			GetGame().GetCallqueue().CallLater(m_StandaloneComponent.SetParentFactionID, SCR_GameModeCampaign.MINIMUM_DELAY, false, m_iParentFaction);
		}
				/////////Make MHQ have supplies/////////
//		SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.Cast(m_SpawnPoint.FindComponent(SCR_ResourceComponent));
//		if (resourceComponent)
//		{
//			SCR_ResourceContainer container = resourceComponent.GetContainer(EResourceType.SUPPLIES);
////			if (!container)
////				return;
//			
//			if (container)
//			{
//				float m_fSuppliesValue = container.GetResourceValue();
//				container.SetResourceValue(m_fSuppliesValue);
//			}
//		}
		
		m_iSpawnpointId = Replication.FindId(m_SpawnPoint);
		OnSpawnpointCreated();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	//! \param[in] status
	//! \param[in] playerId
	//! \return
	override bool Deploy(SCR_EMobileAssemblyStatus status, int playerId = 0)
	{
		if (!m_bIsInRadioRange && status == SCR_EMobileAssemblyStatus.DEPLOYED)
			return false;
		
		if (!m_ParentFaction)
			return false;
		
		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		
		if (!campaign)
			return false;
		
		if (m_ParentFaction.GetMobileAssembly() && status == SCR_EMobileAssemblyStatus.DEPLOYED)
			return false;
		
		if (status == SCR_EMobileAssemblyStatus.DEPLOYED)
		{
			CreateSpawnpoint();
			IEntity radioEntity = m_SpawnPoint.GetRootParent();
			EvaluateRespawnTime(m_SpawnPoint, radioEntity);
			m_bIsDeployed = true;
		}
		else
		{
			m_bIsDeployed = false;
			
			if (m_SpawnPoint)
				RplComponent.DeleteRplEntity(m_SpawnPoint, false);
		}
		
		OnDeployChanged();
		Replication.BumpMe();
		campaign.BroadcastMHQFeedback(status, playerId, GetGame().GetFactionManager().GetFactionIndex(m_ParentFaction));
		
		/////////Make MHQ have supplies/////////
//		SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.Cast(m_SpawnPoint.FindComponent(SCR_ResourceComponent));
//		if (resourceComponent)
//		{
//			SCR_ResourceContainer container = resourceComponent.GetContainer(EResourceType.SUPPLIES);
////			if (!container)
////				return;
//			
//			if (container)
//			{
//				float m_fSuppliesValue = container.GetResourceValue();
//				container.SetResourceValue(m_fSuppliesValue);
//			}
//		}
		
		return true;
	}
	
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
		
		float multiplier = CampaignInfo.GetMobileSpawnTimeDistanceMultiplierRP();
		vector HQDistance = HQ.GetOwner().GetOrigin();
		vector spawnpointDistance = spawnPoint.GetOrigin();
		float distance = vector.DistanceXZ(HQDistance,spawnpointDistance);
		int respawnTime = Math.Clamp(distance * multiplier, 0, 120);
		
	    spawnPoint.SetRespawnTime(respawnTime);
		
#ifdef WORKBENCH
		Print("Distance from HQ: " + distance + " Respawn Time: " + respawnTime);
#endif
	}
}
