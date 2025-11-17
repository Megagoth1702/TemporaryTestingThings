
//------------------------------------------------------------------------------------------------
modded class SCR_FactionButton : SCR_DeployButtonBase
{
	
	//------------------------------------------------------------------------------------------------
	//! Set this button's faction.
	override void SetFaction(SCR_Faction faction)
	{
		m_Faction = faction;

		string name = faction.GetFactionName();
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		FactionManager factionMan = GetGame().GetFactionManager();
		if (factionMan && CampaignInfo && CampaignInfo.IsAADEnabled())
		{		
			string attackingKey = CampaignInfo.GetAADAttackingFaction();
			string defendingKey = CampaignInfo.GetAADDefendingFaction();
			SCR_Faction attackingFaction = SCR_Faction.Cast(factionMan.GetFactionByKey(attackingKey));
			SCR_Faction defendingFaction = SCR_Faction.Cast(factionMan.GetFactionByKey(defendingKey));
			
			if (m_Faction == attackingFaction)
			{
				name = name + " (Attacking)";
			}

			else if (m_Faction == defendingFaction)
			{
				name = name + " (Defending)";
			}
		}
		
		SetFactionName(name);
		SetImage(faction.GetFactionFlag());

		UpdatePlayerCount();
	}
		
};
