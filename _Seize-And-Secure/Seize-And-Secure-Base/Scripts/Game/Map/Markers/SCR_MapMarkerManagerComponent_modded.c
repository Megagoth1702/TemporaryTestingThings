modded class SCR_MapMarkerManagerComponent : SCR_BaseGameModeComponent
{
	SCR_GameModeCampaign m_CampaignInfo;
	protected float m_fRadioOperatorDistance = 600;
	protected SCR_FactionManager m_FactionManager;

	[RplProp()]
	bool m_bIsWithinRangeOfRadioOperators;
	
	[RplProp()]
	bool m_bIsRadioOperator;
	
	ref array<SCR_MapMarkerEntity> m_aRadioOperatorMarkers = new array<SCR_MapMarkerEntity>();
	
	[RplProp()]
	protected ref array<ref int> m_RadioOperators = new array<ref int>();
	SCR_PlayerLoadoutComponent m_PlyLoadoutComp;
	
	//------------------------------------------------------------------------------------------------
    override void SetStreamRulesForPlayer(int playerID)
    {            
        PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerID);
		if (!playerController)
			return;
		
//        foreach (SCR_MapMarkerEntity marker : m_aDynamicMarkers)
//        {
//            if (!marker)
//                continue;
//
//            Faction markerFaction = marker.GetFaction();
//            bool isPlayerFaction = markerFaction && markerFaction == SCR_FactionManager.SGetPlayerFaction(playerID);
//
////			Print("SetStreamRulesForPlayer");
//            HandleStreamOut(marker, playerController, !isPlayerFaction); //false condition shows enemies, !isPlayerFaction is TRUE that it is NOT player faction....
//        }
		
		foreach (SCR_MapMarkerEntity dynamicMarker : m_aDynamicMarkers)
		{
			if (!dynamicMarker)
                continue;
			
			Faction markerFaction = dynamicMarker.GetFaction();
			
			if (!markerFaction || markerFaction == SCR_FactionManager.SGetPlayerFaction(playerID))
				HandleStreamOut(dynamicMarker, GetGame().GetPlayerManager().GetPlayerController(playerID), false);
			else
				HandleStreamOut(dynamicMarker, GetGame().GetPlayerManager().GetPlayerController(playerID), true);
		}
    }
	
	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{
	    m_MapEntity.GetMapVisibleFrame(m_vVisibleFrameMin, m_vVisibleFrameMax);
		
		for (int i = m_aStaticMarkers.Count() - 1; i >= 0; i--)
		{
			if (!m_aStaticMarkers[i].OnUpdate(m_vVisibleFrameMin, m_vVisibleFrameMax))
				SetStaticMarkerDisabled(m_aStaticMarkers[i], true);
		}

		int playerId = GetGame().GetPlayerController().GetPlayerId();
		m_aRadioOperatorMarkers = FindRadioOperators(playerId);

		/*		
#ifdef WORKBENCH
		Print("Radio Operators: " + m_aRadioOperatorMarkers.Count() + " Dynamic Markers: " + m_aDynamicMarkers.Count() + " Disabled: " + m_aDisabledMarkers.Count());
#endif
		*/
	    foreach (SCR_MapMarkerEntity markerEnt : m_aDynamicMarkers)
	    {
			bool isWithinRange;
			bool isLeader;
			bool isInGroup;
			
	        if (markerEnt)
	        {
				
				isLeader = markerEnt.GetIsSquadLeader(); //IsPlayerLeader(markerEnt.GetPlayerId());
				isWithinRange = IsWithinRangeOfRadioOperator(markerEnt, m_aRadioOperatorMarkers, m_fRadioOperatorDistance);
/* #ifdef WORKBENCH				
				Print("Showing Marker For Player ID: " + markerEnt.GetPlayerId() + " : " + (isWithinRange || isLeader));
#endif	*/
				markerEnt.SetGlobalVisible(isWithinRange || isLeader);
				Seize_RadioOperatorMapMarkerPlayer marker = Seize_RadioOperatorMapMarkerPlayer.Cast(markerEnt);
				if (marker)
				{		
					isInGroup = IsInGroup(playerId, marker);
					if (isInGroup && !marker.IsIncap(marker.GetPlayerId()))
					{
						marker.SetGroupColor();
					}
					else if (!isInGroup && !marker.IsIncap(marker.GetPlayerId()))
					{
						Faction faction = SCR_FactionManager.SGetPlayerFaction(playerId);
						marker.SetMarkerColorFaction(faction);
					}					
				}
				
				markerEnt.OnUpdate();
			}	
	    }

	    #ifdef MARKERS_DEBUG
	        UpdateDebug(timeSlice);
	    #endif
	}
	
	//------------------------------------------------------------------------------------------------
	array<SCR_MapMarkerEntity> FindRadioOperators(int playerId)
	{
		array<SCR_MapMarkerEntity> radioOperators = {};
		
		foreach (SCR_MapMarkerEntity markerEnt : m_aDynamicMarkers)
		{
			if (markerEnt && markerEnt.GetType() == SCR_EMapMarkerType.RADIO_OPERATOR_PLAYER)
			{
				int factionPlayerId = markerEnt.GetPlayerId();
				if (IsPlayerInSameFaction(factionPlayerId) && markerEnt.GetIsRadioOperator())
					radioOperators.Insert(markerEnt);
			}	
		}
		
		return radioOperators;
	}
		
	//------------------------------------------------------------------------------------------------
	bool IsPlayerInSameFaction(int playerId)
	{
		
	    SCR_Faction playerFaction = SCR_Faction.Cast(m_FactionManager.GetPlayerFaction(playerId));
		if (!playerFaction)
			return false;
		
	    SCR_Faction localPlayerFaction = SCR_Faction.Cast(m_FactionManager.GetLocalPlayerFaction());
		if (!localPlayerFaction)
			return false;
		
//		Print("Local Player Faction: " + localPlayerFaction + " Marker Entity Faction for Player ID: " + playerId + " : " + playerFaction);
		
	    return playerFaction && localPlayerFaction && playerFaction == localPlayerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsWithinRangeOfRadioOperator(SCR_MapMarkerEntity marker, array<SCR_MapMarkerEntity> radioMarkers, float distance)
	{
		if (!marker)
			return false;

		int markerPlayerId = marker.GetPlayerId();
		
		if (radioMarkers.Count() < 1 || !IsPlayerInSameFaction(markerPlayerId))
			return false;

		vector markerPos = marker.GetWorldPos();
		bool isWithinRange = false;
		
	    foreach (SCR_MapMarkerEntity markerEnt : radioMarkers)
	    {
			int playerId = markerEnt.GetPlayerId();
			
			if (markerEnt.IsIncap(playerId))
				continue;
			
#ifdef WORKBENCH			
//			Print("Checking Marker Player ID: " + markerPlayerId + " Is Radio Operator: " + marker.GetIsRadioOperator() + " Checking Player ID: " + playerId + " Marker Type: " + type + " Is Radio Operator: " + isRadioOperator);
#endif
            vector operatorPos = markerEnt.GetWorldPos();
            float distanceToMarker = vector.DistanceXZ(operatorPos, markerPos);
            if (distanceToMarker <= distance)
            {
				isWithinRange = true;
				break;
            }
	    }
		
	    return isWithinRange;
	}
		
	//------------------------------------------------------------------------------------------------
	bool IsPlayerLeader(int playerId)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId)); //GetPlayerController();
		if (!playerController)
		{
			Print("IsPlayerLeader: No Player Controller for Player ID: " + playerId);
			return false;
		}
			
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupController)
		{
			Print("IsPlayerLeader: No Group Controller for Player ID: " + playerId);
			return false;
		}
			
		
		SCR_AIGroup group = groupController.GetPlayersGroup(); // SCR_PlayerControllerGroupComponent.GetPlayerControllerComponent(playerId).GetPlayersGroup();
		if (!group)
		{
			Print("IsPlayerLeader: No Group for Player ID: " + playerId);
			return false;
		}
		
//		Print("IsPlayerLeader: Player Id: " + playerId + " - " + group.IsPlayerLeader(playerId));
		return group.IsPlayerLeader(playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsInGroup(int playerId, Seize_RadioOperatorMapMarkerPlayer markerEnt)
	{
		if (!markerEnt)
		{
			PrintFormat("IsInGroup: No Marker Entity for Player ID: %1", playerId);
			return false;
		}
					
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId)); //GetPlayerController();
		if (!playerController)
		{
			Print("IsInGroup: No Player Controller for Player ID: " + playerId);
			return false;
		}
			
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupController)
		{
			Print("IsInGroup: No Group Controller for Player ID: " + playerId);
			return false;
		}			
		
		SCR_AIGroup group = groupController.GetPlayersGroup(); // SCR_PlayerControllerGroupComponent.GetPlayerControllerComponent(playerId).GetPlayersGroup();
		if (!group)
		{
			Print("IsInGroup: No Group for Player ID: " + playerId);
			return false;
		}
		
		array<int>playerIDsInGroup = group.GetPlayerIDs();	
		int markerId = markerEnt.GetPlayerId();
		if (!markerId)
		{
			return false;
		}
					
		return playerIDsInGroup.Contains(markerId);
	}
	
	//------------------------------------------------------------------------------------------------
    bool IsRadioOperator(int playerId)
    {	
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
		{
			Print("IsRadioOperator: No Player Controller! Player ID: " + playerId);
			return false;
		}

		m_PlyLoadoutComp = SCR_PlayerLoadoutComponent.Cast(playerController.FindComponent(SCR_PlayerLoadoutComponent));
		if (!m_PlyLoadoutComp)
		{
			Print("IsRadioOperator: No SCR_PlayerLoadoutComponent! Player ID: " + playerId);
			return false;
		}

		SCR_BasePlayerLoadout loadout = m_PlyLoadoutComp.GetLoadout();
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		if (!factionLoadout)
			return false;
		
		LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
		
		if (loadoutType == LoadoutType.RADIO_OPERATOR)
		{
//			Print("IsRadioOperator: Player ID: " + playerId + " Is A Radio Operator!");
			return true;
		}
			
//		Print("IsRadioOperator: False! Player ID: " + playerId);

        return false;
    }
	
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{	
		m_FactionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (m_CampaignInfo)
		{
			m_fRadioOperatorDistance = m_CampaignInfo.GetRadioOperatorMarkerDistance();
		}
		
		super.EOnInit(owner);
	}
}