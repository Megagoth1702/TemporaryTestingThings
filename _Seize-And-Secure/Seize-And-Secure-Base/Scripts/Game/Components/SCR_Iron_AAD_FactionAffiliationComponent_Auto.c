class SCR_Iron_AAD_FactionAffiliationComponent_AutoClass : SCR_FactionAffiliationComponentClass
{
}

class SCR_Iron_AAD_FactionAffiliationComponent_Auto : SCR_FactionAffiliationComponent
{
	
	protected Faction m_DefendingFaction;
	protected Faction m_AttackingFaction;
	
	//------------------------------------------------------------------------------------------------
	//! Set faction.
	//! \param[in] owner
	//! \param[in] faction the desired faction
	override static void SetFaction(IEntity owner, Faction faction)
	{
		Print("SCR_Iron_AAD_FactionAffiliationComponent_Auto");
		
		if (!owner) // || !faction )
		{
			Print("SCR_Iron_AAD_FactionAffiliationComponent_Auto: No Owner");
			return;
		} 
		
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (!CampaignInfo)
			return;
		
		Faction defenderFaction;
		Faction attackerFaction;
	
		array<SCR_MilitaryBaseComponent> bases = {};
		SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
		baseManager.GetBases(bases);
		
		foreach (SCR_MilitaryBaseComponent base : bases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
			if (campaignBase.IsHQ())
			{
				if (campaignBase.GetAADBaseType() == AADBaseType.Defender)
				{
					defenderFaction = campaignBase.GetFaction();
					CampaignInfo.SetAADDefendingFaction(defenderFaction.GetFactionKey());
				}

				else if (campaignBase.GetAADBaseType() == AADBaseType.Attacker)
				{
					attackerFaction = campaignBase.GetFaction();
					CampaignInfo.SetAADAttackingFaction(attackerFaction.GetFactionKey());
				}
			}
		}

		SCR_Iron_AAD_FactionAffiliationComponent_Auto factionComponent = SCR_Iron_AAD_FactionAffiliationComponent_Auto.Cast(owner.FindComponent(SCR_Iron_AAD_FactionAffiliationComponent_Auto));
		if (factionComponent)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(owner.FindComponent(SCR_CampaignMilitaryBaseComponent));
			if (campaignBase.GetAADBaseType() == AADBaseType.Defender)
				factionComponent.SetAffiliatedFaction(defenderFaction);
			
			else if (campaignBase.GetAADBaseType() == AADBaseType.Attacker)
				factionComponent.SetAffiliatedFaction(attackerFaction);
		}			
	}
	
	//------------------------------------------------------------------------------------------------
	Faction GetDefendingFaction()
	{
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (!CampaignInfo)
		{
			return null;
		}
		FactionManager factionMan = GetGame().GetFactionManager();
		if (!factionMan)
		{
			return null;
		}
		
		return factionMan.GetFactionByKey(CampaignInfo.GetAADDefendingFaction());
	}
	
	//------------------------------------------------------------------------------------------------
	Faction GetAttackingFaction()
	{
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (!CampaignInfo)
		{
			return null;
		}
		FactionManager factionMan = GetGame().GetFactionManager();
		if (!factionMan)
		{
			return null;
		}
		
		return factionMan.GetFactionByKey(CampaignInfo.GetAADAttackingFaction());
	}
}
