modded class SCR_CampaignBuildingGadgetToolComponent : SCR_GadgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! \return the value how much this tool adds to a building value with one action
	override int GetToolConstructionValue()
	{	
		int constructionValue = m_iConstructionValue;
		
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return constructionValue;
		
		SCR_PlayerLoadoutComponent m_PlyLoadoutComp = SCR_PlayerLoadoutComponent.Cast(pc.FindComponent(SCR_PlayerLoadoutComponent));
		if (!m_PlyLoadoutComp)
			return constructionValue;
		
		SCR_BasePlayerLoadout loadout = m_PlyLoadoutComp.GetLoadout();
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		if (!factionLoadout)
			return constructionValue;
		
		LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);		
		if (loadoutType == LoadoutType.COMBAT_ENGINEER)
			constructionValue *= 3;
		
		return constructionValue;
	}
}

