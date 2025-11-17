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
		
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
			return constructionValue;

		SCR_AIGroup group = groupsManager.GetPlayerGroup(pc.GetPlayerId());
		if (!group)
			return constructionValue;
		
		if (group.GetGroupRole() == SCR_EGroupRole.ENGINEER)
			constructionValue *= 3;
		
		return constructionValue;
	}
}