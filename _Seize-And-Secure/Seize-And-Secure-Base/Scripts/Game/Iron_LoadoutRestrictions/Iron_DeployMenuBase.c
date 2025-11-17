//------------------------------------------------------------------------------------------------
//! Main deploy menu with the map present.
modded class SCR_DeployMenuMain : SCR_DeployMenuBase
{	
	SCR_GameModeCampaign m_CampaignInfo;
	
	//------------------------------------------------------------------------------------------------
//	override void OnMenuOpened()
//	{
//		super.OnMenuOpened();
//		
//		Faction plyFaction = m_PlyFactionAffilComp.GetAffiliatedFaction();
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (CampaignInfo)// && m_CampaignInfo.GetIsLoadoutRolesRestrictedActions())
//		{
//			int playerId = GetGame().GetPlayerController().GetPlayerId();
//			array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
//			SCR_LoadoutManager m_LoadoutManager = GetGame().GetLoadoutManager();
//			m_LoadoutManager.GetPlayerLoadoutsByFaction(plyFaction, availableLoadouts);
//			
//			RequestLoadoutAvailability(playerId, availableLoadouts);
//		}
//			
//		m_LoadoutRequestUIHandler.ShowAvailableLoadouts(plyFaction);
//		m_GroupRequestUIHandler.ShowAvailableGroups(plyFaction);
//		if (!m_GroupRequestUIHandler.GetPlayerGroup())
//			m_GroupRequestUIHandler.JoinGroupAutomatically();
//		m_SpawnPointRequestUIHandler.ShowAvailableSpawnPoints(plyFaction);
//	}
	
	//----------------------------------------------------------------------------------------------
//	override protected void OnPlayerFactionSet(Faction assignedFaction)
//	{
//		if (!assignedFaction)
//			return;
//
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (CampaignInfo)// && m_CampaignInfo.GetIsLoadoutRolesRestrictedActions())
//		{
//			int playerId = GetGame().GetPlayerController().GetPlayerId();
//			array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
//			SCR_LoadoutManager m_LoadoutManager = GetGame().GetLoadoutManager();
//			m_LoadoutManager.GetPlayerLoadoutsByFaction(assignedFaction, availableLoadouts);
//			RequestLoadoutAvailability(playerId, availableLoadouts);
//		}
//		
//		if (m_LoadoutRequestUIHandler)
//			m_LoadoutRequestUIHandler.ShowAvailableLoadouts(assignedFaction);
//		
//		if (m_GroupRequestUIHandler)
//			m_GroupRequestUIHandler.ShowAvailableGroups(assignedFaction);
//		
//		if (m_SpawnPointRequestUIHandler)
//			m_SpawnPointRequestUIHandler.ShowAvailableSpawnPoints(assignedFaction);
//
//	}

	//------------------------------------------------------------------------------------------------
	//! Callback when player joins a group
//	override protected void OnLocalPlayerGroupJoined(SCR_AIGroup group)
//	{
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (CampaignInfo)// && m_CampaignInfo.GetIsLoadoutRolesRestrictedActions())
//		{
//			int playerId = GetGame().GetPlayerController().GetPlayerId();
//			array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
//			SCR_LoadoutManager m_LoadoutManager = GetGame().GetLoadoutManager();
//			Faction playerFaction = group.GetFaction();
//			m_LoadoutManager.GetPlayerLoadoutsByFaction(playerFaction, availableLoadouts);
//			RequestLoadoutAvailability(playerId, availableLoadouts);
//		}
//		
//		if (m_SpawnPointRequestUIHandler)
//			m_SpawnPointRequestUIHandler.UpdateRelevantSpawnPoints();
//	}
	
	//------------------------------------------------------------------------------------------------
	protected void RequestLoadoutAvailability(int playerId, array<ref SCR_BasePlayerLoadout> availableLoadouts)
	{
		array<ref int> loadoutIndexes = {};
		
		foreach (SCR_BasePlayerLoadout loadout : availableLoadouts)
		{
			SCR_LoadoutManager loadoutManager = GetGame().GetLoadoutManager();
		    int loadoutIndex = loadoutManager.GetLoadoutIndex(loadout);
		    
		    if (m_PlyLoadoutComp)
		    {
		        loadoutIndexes.Insert(loadoutIndex);
		    }
		}
		
		m_PlyLoadoutComp.CanIHasLoadout(playerId, loadoutIndexes);
	}
}
