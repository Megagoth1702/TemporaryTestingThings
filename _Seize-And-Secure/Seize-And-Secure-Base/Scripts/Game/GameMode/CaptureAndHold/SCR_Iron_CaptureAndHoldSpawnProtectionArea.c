class SCR_Iron_CaptureAndHoldSpawnProtectionAreaClass : SCR_CaptureAndHoldSpawnAreaClass
{
}

//! This area trigger detects enemies not belonging to assigned faction,
//! and raises callbacks which allow penalizing abusive behaviour.
class SCR_Iron_CaptureAndHoldSpawnProtectionArea : SCR_CaptureAndHoldSpawnArea
{
	[Attribute("50.0", UIWidgets.EditBox, "Distance to check for nearest base")]
	protected float m_fDistance;
	
	//------------------------------------------------------------------------------------------------
	/*!
		Check whether provided faction is friendly in relation to this area trigger.
		\return Returns true in case faction is friendly, false otherwise.
	*/
	override bool IsFriendly(notnull SCR_ChimeraCharacter character)
	{
		IEntity owner = m_pRplComponent.GetEntity();
		if (!owner)
			return true;
		
		vector ownerOrigin = owner.GetOrigin();
		if (!ownerOrigin)
			return true;
		
		SCR_GameModeCampaign m_Campaign = SCR_GameModeCampaign.GetInstance();
		if (!m_Campaign)
			return true;
		
		SCR_CampaignMilitaryBaseComponent base = m_Campaign.GetBaseManager().FindClosestBase(ownerOrigin);
		if (!base)
			return true;
		
		vector baseOrigin = base.GetOwner().GetOrigin();
		float distance = vector.DistanceXZ(ownerOrigin, baseOrigin);
		
		if (distance > m_fDistance)
		{
			SCR_CaptureAndHoldManager parentManager = SCR_CaptureAndHoldManager.GetAreaManager();
#ifdef WORKBENCH
			PrintFormat("SCR_Iron_CaptureAndHoldSpawnProtectionArea: No Base Found. Unregistering Area For Base: %1", base);
#endif
			parentManager.UnregisterSpawnArea(this);
			SCR_EntityHelper.DeleteEntityAndChildren(owner);
			return true;
		}
		
		// If Base is not used as HQ and just has FIA in area
		Faction baseFaction = base.GetFaction();
		SCR_CampaignFaction campaignFaction = SCR_CampaignFaction.Cast(baseFaction);
		if (!campaignFaction.IsPlayable())
		{
			return true;
		}
		
		Faction charFaction = character.GetFaction();
		if (!charFaction || charFaction != baseFaction)
			return false;
		
		return true;
	}
}
