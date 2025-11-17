modded class SCR_CampaignDeployMobileAssemblyUserAction : ScriptedUserAction
{
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_AssemblyComponent)
			return false;
		
		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		
		if (!campaign)
			return false;
		
		SCR_CampaignFaction f = m_AssemblyComponent.GetParentFaction();
		
		if (!f)
			return false;
		
		if (f.GetMobileAssembly())
		{
			SetCannotPerformReason("#AR-Campaign_Action_AnotherAssemblyActive-UC");
			return false;
		}
		
		SCR_CampaignFeedbackComponent feedbackComponent = SCR_CampaignFeedbackComponent.GetInstance();
		
		if (!feedbackComponent)
			return false;
		
		SCR_CampaignMilitaryBaseComponent base = feedbackComponent.GetBaseWithPlayer();
		if (base)
		{
			Faction baseFaction = base.GetFaction();
			SCR_Faction playerFaction = SCR_Faction.Cast(SCR_Faction.GetEntityFaction(user));
			
			// Let MHQ be deployed in friendly base
			if (baseFaction.IsFactionEnemy(playerFaction))
			{
				SetCannotPerformReason("#AR-Campaign_Action_AnotherHQNearby-UC");
				return false;
			}
		}

//		if (feedbackComponent.GetBaseWithPlayer())
//		{
//			SetCannotPerformReason("#AR-Campaign_Action_AnotherHQNearby-UC");
//			return false;
//		}
		
		bool isInRange = m_AssemblyComponent.IsInRadioRange();
		
		if (!isInRange)
			SetCannotPerformReason("#AR-Campaign_Action_NoSignal-UC");
		
		IEntity truck = m_TruckBed.GetParent();
		
		if (truck)
		{
			vector pos = truck.GetOrigin();
			
			if (SCR_TerrainHelper.GetHeightAboveTerrain(pos) > MAX_TERRAIN_HEIGHT)
			{
				SetCannotPerformReason("#AR-Campaign_Action_UnevenTerrain-UC");
				return false;
			}
			
			/*if (SCR_TerrainHelper.GetTerrainNormal(pos)[1] < TERRAIN_SLOPE_THRESHOLD)
			{
				SetCannotPerformReason("#AR-Campaign_Action_UnevenTerrain-UC");
				return false;
			}*/
		}
		
		return isInRange;
	}
	
};