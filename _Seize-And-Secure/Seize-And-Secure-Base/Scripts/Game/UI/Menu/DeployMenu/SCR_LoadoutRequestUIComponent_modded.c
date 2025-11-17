modded class SCR_LoadoutRequestUIComponent : SCR_DeployRequestUIBaseComponent
{
	protected ref array<ref SCR_BasePlayerLoadout> m_aFilteredLoadouts = {}; //new array<ref SCR_BasePlayerLoadout>();	
	SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
	
	//------------------------------------------------------------------------------------------------
//	override protected void ResetPlayerLoadoutPreview()
//	{
//
//		int playerId = GetGame().GetPlayerController().GetPlayerId();
//		array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
//		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
//		Faction playerFaction = factionManager.GetPlayerFaction(playerId);
//		
//		m_LoadoutManager.GetPlayerLoadoutsByFaction(playerFaction, availableLoadouts);
//		RequestLoadoutAvailability(playerId, availableLoadouts);
//
//		if (m_wLoadoutPreview)
//			m_wLoadoutPreview.SetVisible(false);
//
//		if (m_wLoadoutName)
//			m_wLoadoutName.SetText("#AR-DeployMenu_SelectLoadout");
//	}
	
	//------------------------------------------------------------------------------------------------
	//! Show available loadouts in the loadout selector.
//	override void ShowAvailableLoadouts(Faction faction)
//	{
//		if (!m_LoadoutManager)
//			return;
//
//		ResetPlayerLoadoutPreview();
//
//		if (!m_LoadoutSelector)
//			return;
//
//		m_LoadoutSelector.ClearAll();
//
//		m_aFilteredLoadouts.Clear();
//		
//		int playerId = GetGame().GetPlayerController().GetPlayerId();
//		array<ref SCR_BasePlayerLoadout> filteredLoadouts = {};
//		array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
//		m_LoadoutManager.GetPlayerLoadoutsByFaction(faction, availableLoadouts);
//
//		RequestLoadoutAvailability(playerId, availableLoadouts);
//		array<ref int> availableFilteredLoadoutIndexes = m_PlyLoadoutComp.GetFilteredLoadoutIndexes();		
//				
//		foreach (int index : availableFilteredLoadoutIndexes)
//		{
//			SCR_LoadoutManager loadoutManager = GetGame().GetLoadoutManager();
//			SCR_BasePlayerLoadout loadout = loadoutManager.GetLoadoutByIndex(index);
//			
//			filteredLoadouts.Insert(loadout);
//		}
//
//		SCR_PlayerArsenalLoadout arsenalLoadout = null;
//		foreach (SCR_BasePlayerLoadout loadout : filteredLoadouts) //availableLoadouts)
//		{
//			if (!loadout.IsLoadoutAvailableClient())
//				continue;
//
//			if (loadout.IsInherited(SCR_PlayerArsenalLoadout))
//				arsenalLoadout = SCR_PlayerArsenalLoadout.Cast(loadout);
//
//			m_LoadoutSelector.AddItem(loadout, loadout.IsLoadoutAvailableClient());
//		}
//		
//		if (!availableLoadouts.IsEmpty())
//		{
//			if (arsenalLoadout && GetPlayerLoadout() != arsenalLoadout)
//				m_PlyLoadoutComp.RequestLoadout(arsenalLoadout);
//			else if (!GetPlayerLoadout())
//				m_PlyLoadoutComp.RequestLoadout(availableLoadouts[0]);
//		}
//
//		GetGame().GetCallqueue().CallLater(RefreshLoadoutPreview, 500, false); // 0, delayed refresh call helps with performance
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

};