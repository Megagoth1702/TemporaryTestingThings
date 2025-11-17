class SCR_Iron_FactionAffiliationComponent_AutoClass : SCR_FactionAffiliationComponentClass
{
}

class SCR_Iron_FactionAffiliationComponent_Auto : SCR_FactionAffiliationComponent
{	
	//------------------------------------------------------------------------------------------------
	//! Set faction.
	//! \param[in] owner
	//! \param[in] faction the desired faction
	override static void SetFaction(IEntity owner, Faction faction)
	{
		if (!owner) // || !faction )
		{
			Print("SCR_Iron_FactionAffiliationComponent_Auto: No Owner");
			return;
		} 

		SCR_GameModeCampaign m_Campaign = SCR_GameModeCampaign.GetInstance();
		if (!m_Campaign)
			return;
		
		SCR_Iron_FactionAffiliationComponent_Auto factionComponent = SCR_Iron_FactionAffiliationComponent_Auto.Cast(owner.FindComponent(SCR_Iron_FactionAffiliationComponent_Auto));
		if (factionComponent)
		{
			vector ownerOrigin = owner.GetOrigin();
			
			SCR_CampaignMilitaryBaseComponent base = m_Campaign.GetBaseManager().FindClosestBase(ownerOrigin);	
			Faction baseFaction = base.GetFaction();
			
			factionComponent.SetAffiliatedFaction(baseFaction);
			PrintFormat("SCR_Iron_FactionAffiliationComponent_Auto: Set Faction To: %1", baseFaction);
		}
		else
		{
			PrintFormat("SCR_Iron_FactionAffiliationComponent_Auto: Could Not Find Faction");
		}			
	}
}
