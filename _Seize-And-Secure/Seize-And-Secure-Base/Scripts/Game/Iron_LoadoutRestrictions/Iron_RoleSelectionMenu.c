modded class SCR_RoleSelectionMenu : SCR_DeployMenuBase
{
	
	SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
	
	//------------------------------------------------------------------------------------------------
//	override void OnMenuOpen()
//	{
//		super.OnMenuOpen();
//
//		m_MapEntity = SCR_MapEntity.GetMapInstance();
//		m_FactionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
//		m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
//		m_MenuHandler = SCR_DeployMenuHandler.Cast(GetRootWidget().FindHandler(SCR_DeployMenuHandler));
//
//		m_wPersistentFaction = GetRootWidget().FindAnyWidget("PermanentFaction");
//		if (m_wPersistentFaction)
//			m_wPersistentFaction.SetVisible(!SCR_RespawnSystemComponent.GetInstance().IsFactionChangeAllowed());
//
//		m_wScenarioTimeElapsed = TextWidget.Cast(GetRootWidget().FindAnyWidget("TimeElapsed"));
//		m_wServerName = TextWidget.Cast(GetRootWidget().FindAnyWidget("ServerName"));
//		SCR_MissionHeader header = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());
//		if (header)
//		{
//			if (m_wServerName)
//				m_wServerName.SetText(header.m_sName);
//
//			m_iMaxPlayerCount = header.m_iPlayerCount;
//		}
//
//		if (!m_MenuHandler)
//		{
//			Print("SCR_DeployMenuHandler is missing on root widget of " + this, LogLevel.ERROR);
//			return;
//		}
//
//		PlayerController pc = GetGame().GetPlayerController();
//		m_PlyFactionAffilComp = SCR_PlayerFactionAffiliationComponent.Cast(pc.FindComponent(SCR_PlayerFactionAffiliationComponent));
//		if (!m_PlyFactionAffilComp)
//		{
//			Print("Cannot find player faction affiliation component!", LogLevel.ERROR);
//		}
//
//		m_PlyLoadoutComp = SCR_PlayerLoadoutComponent.Cast(pc.FindComponent(SCR_PlayerLoadoutComponent));
//		if (!m_PlyLoadoutComp)
//		{
//			Print("Cannot find player loadout component!", LogLevel.ERROR);
//		}
//
//		m_wPlayerCount = TextWidget.Cast(GetRootWidget().FindAnyWidget("PlayerCount"));
//		m_FactionPlayerList = m_MenuHandler.GetFactionPlayerList();
//
//		if (m_PlyFactionAffilComp && m_PlyLoadoutComp)
//		{
//			int playerId = GetGame().GetPlayerController().GetPlayerId();
//			array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
//			SCR_LoadoutManager m_LoadoutManager = GetGame().GetLoadoutManager();
//			Faction playerFaction = m_PlyFactionAffilComp.GetAffiliatedFaction(); // group.GetFaction();
//			m_LoadoutManager.GetPlayerLoadoutsByFaction(playerFaction, availableLoadouts);
//			RequestLoadoutAvailability(playerId, availableLoadouts);
//		}
//		
//		UpdateElapsedTime();
//		UpdatePlayerCount();
//
//		FindRequestHandlers();
//		HookEvents();
//
//		m_ContinueButton = SCR_InputButtonComponent.GetInputButtonComponent("CloseButton", GetRootWidget());
//		if (m_ContinueButton)
//		{
//			m_ContinueButton.m_OnActivated.Insert(Close);
//			m_ContinueButton.SetEnabled(CanContinue());
//		}
//
//		m_GroupOpenButton = SCR_InputButtonComponent.GetInputButtonComponent("GroupMenu", GetRootWidget());
//		if (m_GroupOpenButton)
//		{
//			m_GroupOpenButton.m_OnActivated.Insert(OpenGroupMenu);
//			m_GroupOpenButton.SetEnabled(CanOpenGroupMenu());
//		}		
//
//		m_ViewProfileButton = SCR_InputButtonComponent.GetInputButtonComponent("ViewProfile", GetRootWidget());
//		if (m_ViewProfileButton)
//		{
//			UpdateViewProfileButton(true);
//			m_ViewProfileButton.m_OnActivated.Insert(OnViewProfile);
//		}
//		
//		Widget chat = GetRootWidget().FindAnyWidget("ChatPanel");
//		if (chat)
//			m_ChatPanel = SCR_ChatPanel.Cast(chat.FindHandler(SCR_ChatPanel));
//
//		m_ChatButton = SCR_InputButtonComponent.GetInputButtonComponent("ChatButton", GetRootWidget());
//		if (m_ChatButton)
//			m_ChatButton.m_OnActivated.Insert(OnChatToggle);
//
//		m_PauseButton = SCR_InputButtonComponent.GetInputButtonComponent("PauseButton", GetRootWidget());
//		if (m_PauseButton)
//			m_PauseButton.m_OnActivated.Insert(OnPauseMenu);
//		InitMapPlain();
//	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback when player receives a response to afaction request.
//	override protected void OnPlayerFactionResponse(SCR_PlayerFactionAffiliationComponent component, int factionIndex, bool response)
//	{
//		if (response)
//		{		
//			int playerId = GetGame().GetPlayerController().GetPlayerId();
//			array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
//			SCR_LoadoutManager m_LoadoutManager = GetGame().GetLoadoutManager();
//			Faction playerFaction = GetGame().GetFactionManager().GetFactionByIndex(factionIndex);
//			m_LoadoutManager.GetPlayerLoadoutsByFaction(playerFaction, availableLoadouts);
//			RequestLoadoutAvailability(playerId, availableLoadouts);
//				
//			m_FactionRequestUIHandler.OnPlayerFactionAssigned(component.GetAffiliatedFaction());
//			if (factionIndex == -1)
//			{
//				m_GroupRequestUIHandler.SetExpanded(false);
//				m_FactionRequestUIHandler.SetExpanded(false);
//			}
//			else
//			{
//				// If groups are enabled, update them first, otherwise just show players assigned to the faction
//				if (m_GroupRequestUIHandler && m_GroupRequestUIHandler.IsEnabled())
//				{
//					Widget list = m_FactionRequestUIHandler.GetFactionButton(factionIndex).GetList();
//					m_GroupRequestUIHandler.SetListWidget(list);
//					m_GroupRequestUIHandler.ShowAvailableGroups(component.GetAffiliatedFaction());
//					m_LoadoutRequestUIHandler.ShowAvailableLoadouts(component.GetAffiliatedFaction());
//				}
//				else
//				{
//					Widget list = m_FactionRequestUIHandler.GetFactionButton(factionIndex).GetGridList();
//					m_LoadoutRequestUIHandler.SetListWidget(list);
//					m_LoadoutRequestUIHandler.ShowAvailableLoadouts(component.GetAffiliatedFaction());
//
//					array<int> players = {};
//					GetGame().GetPlayerManager().GetPlayers(players);
//					foreach (int pid : players)
//					{
//						if (SCR_Faction.Cast(SCR_FactionManager.SGetPlayerFaction(pid)) != component.GetAffiliatedFaction())
//							players.RemoveItem(pid);
//					}
//
//					m_LoadoutRequestUIHandler.ShowPlayerLoadouts(players);
//				}	
//			}
//	
//			m_GroupOpenButton.SetEnabled(CanOpenGroupMenu());
//			m_ContinueButton.SetEnabled(CanContinue());
//			if (m_wPersistentFaction)
//				m_wPersistentFaction.SetVisible(false);
//		}
//
//		m_FactionRequestUIHandler.Unlock();
//	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback when a player joins a group.
//	override protected void OnPlayerGroupJoined(SCR_AIGroup group, int pid = -1)
//	{
//		if (!group || group.GetGroupID() != m_GroupRequestUIHandler.GetShownGroupId())
//			return;
//
//		SCR_GroupButton groupBtn = m_GroupRequestUIHandler.GetGroupButton(group);
//		if (!groupBtn)
//			return;
//
//		int playerId = GetGame().GetPlayerController().GetPlayerId();
//		array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
//		SCR_LoadoutManager m_LoadoutManager = GetGame().GetLoadoutManager();
//		Faction playerFaction = group.GetFaction();
//		m_LoadoutManager.GetPlayerLoadoutsByFaction(playerFaction, availableLoadouts);
//		RequestLoadoutAvailability(playerId, availableLoadouts);
//
//		Widget list = m_GroupRequestUIHandler.GetGroupButton(group).GetList();
//		array<int> players = group.GetPlayerIDs();
//
//		m_LoadoutRequestUIHandler.SetListWidget(list);
//		m_LoadoutRequestUIHandler.ShowPlayerLoadouts(players, group.GetMaxMembers());
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