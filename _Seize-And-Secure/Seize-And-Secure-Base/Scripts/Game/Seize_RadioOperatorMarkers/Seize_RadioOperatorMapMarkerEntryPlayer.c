[BaseContainerProps(), SCR_MapMarkerTitle()]
class Seize_RadioOperatorMapMarkerEntryPlayer : SCR_MapMarkerEntryDynamic
{
	Seize_RadioOperatorPlayerComponent m_PlayerMarkerWidgetComponent;
    protected ref map<IEntity, Seize_RadioOperatorMapMarkerPlayer> m_mPlayerMarkers = new map<IEntity, Seize_RadioOperatorMapMarkerPlayer>();
    
	//------------------------------------------------------------------------------------------------
	//! Get an array of all Radio Operator map markers
	ref array<Seize_RadioOperatorMapMarkerPlayer> GetRadioOperatorMapPlayerMarkers()
	{
	    ref array<Seize_RadioOperatorMapMarkerPlayer> markerArray = new array<Seize_RadioOperatorMapMarkerPlayer>();
	    
	    // Iterate through the map and add all markers to the array
	    foreach (Seize_RadioOperatorMapMarkerPlayer marker : m_mPlayerMarkers)
	    {
	        markerArray.Insert(marker);
	    }
	    
	    return markerArray;
	}
	
    //------------------------------------------------------------------------------------------------
    //! Register marker here so it can be fetched from the map
    void CreateMarker(int playerId, IEntity playerEntity)
    {		
        if (!CanCreateMarker(playerEntity))
            return;

        Seize_RadioOperatorMapMarkerPlayer marker = CreateAndSetupMarker(playerId, playerEntity);
        if (!marker)
		{
			Print("Marker Failed To Print!");
			return;
		}
		
		marker.SetInfo(marker.GetType(), playerId, playerEntity);
    }

    //------------------------------------------------------------------------------------------------
    //! Check if a marker can be created for the player
    bool CanCreateMarker(IEntity playerEntity)
    {
        return playerEntity && !m_mPlayerMarkers.Contains(playerEntity);
    }

    //------------------------------------------------------------------------------------------------
    //! Create and set up the markers near radio operator
    Seize_RadioOperatorMapMarkerPlayer CreateAndSetupMarker(int playerId, IEntity playerEntity)
    {
        Seize_RadioOperatorMapMarkerPlayer marker = Seize_RadioOperatorMapMarkerPlayer.Cast(m_MarkerMgr.InsertDynamicMarker(SCR_EMapMarkerType.RADIO_OPERATOR_PLAYER, playerEntity));
        if (!marker || !SetupMarkerComponents(playerEntity, marker))
		{
			return null;
		}
		
        if (marker.IsSquadLeader(playerId) || marker.IsRadioOperator(playerId))
        {
			marker.SetGlobalText(marker.GetPlayerNameWithRank(playerId, playerEntity));
//			marker.SetSquadIcon(true);
		}
//		
//		else
//		{
//			marker.SetSquadIcon(false);
//		}
        
		marker.SetMilitarySymbolIcons(EMilitarySymbolIcon.INFANTRY);
        marker.SetGlobalVisible(true);
        m_mPlayerMarkers.Insert(playerEntity, marker);
		
        return marker;
    }

    //------------------------------------------------------------------------------------------------
    //! Setup the marker components and return success status
    bool SetupMarkerComponents(IEntity playerEntity, Seize_RadioOperatorMapMarkerPlayer marker)
    {
        SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(playerEntity.FindComponent(SCR_CharacterControllerComponent));
        if (charCtrl) 
		{
            charCtrl.m_OnLifeStateChanged.Insert(marker.OnLifeStateChanged);
        }

        SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(playerEntity.FindComponent(SCR_CompartmentAccessComponent));
        if (compartmentAccess) 
		{
            compartmentAccess.GetOnPlayerCompartmentEnter().Insert(OnPlayerEnterCompartment);
            compartmentAccess.GetOnPlayerCompartmentExit().Insert(OnPlayerExitCompartment);
        }

        SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
        if (playerCharacter) 
		{
            Faction faction = playerCharacter.GetFaction();
            if (faction) 
				marker.SetFaction(faction);

            return true;
        }
		
        return false;
    }

    //------------------------------------------------------------------------------------------------
    string GetPlayerNameWithRank(int playerId, IEntity playerEntity)
    {
        string name = GetGame().GetPlayerManager().GetPlayerName(playerId);
        string rank = SCR_CharacterRankComponent.GetCharacterRankNameShort(playerEntity);
        
		return string.Format("%1 %2", rank, name);
    }
    
    //------------------------------------------------------------------------------------------------
    void DeleteMarker(IEntity playerEntity)
    {
		Seize_RadioOperatorMapMarkerPlayer marker = m_mPlayerMarkers.Get(playerEntity);
        if (marker) 
		{
            m_MarkerMgr.RemoveDynamicMarker(marker);
            m_mPlayerMarkers.Remove(playerEntity);
        }
    }

    //------------------------------------------------------------------------------------------------
    protected void OnPlayerConnected(int playerId)
    {
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		
        if (playerController) 
		{
            playerController.m_Seize_OnControlledEntityChangedServer.Insert(OnControlledEntityChangedServer);
        }
    }

    //------------------------------------------------------------------------------------------------
    protected void OnControlledEntityChangedServer(int playerId, IEntity from, IEntity to)
    {
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		
        if (playerController) 
		{
            if (!playerController.IsPossessing()) 
			{
                DeleteMarker(from);
                CreateMarker(playerId, to);
            }
        }
    }

    //------------------------------------------------------------------------------------------------
    protected void UpdatePlayerMarkersInVehicle(Vehicle vehicle, IEntity excludedOccupant = null)
    {
        SCR_BaseCompartmentManagerComponent compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
        if (!compartmentManager) return;
        
        array<BaseCompartmentSlot> compartments = {};
        compartmentManager.GetCompartments(compartments);
        array<IEntity> occupants = {};

        CollectOccupants(compartments, occupants, excludedOccupant);
        
        IEntity effectiveCommander = UpdateMarkersForOccupants(occupants, vehicle);
    }

    //------------------------------------------------------------------------------------------------
    //! Collect occupants from compartments
    void CollectOccupants(array<BaseCompartmentSlot> compartments, array<IEntity> occupants, IEntity excludedOccupant)
    {
        foreach (BaseCompartmentSlot compartment : compartments) 
		{
            IEntity occupant = compartment.GetOccupant();
            if (occupant && occupant != excludedOccupant && !occupants.Contains(occupant))
                occupants.Insert(occupant);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Update markers for vehicle occupants
    IEntity UpdateMarkersForOccupants(array<IEntity> occupants, Vehicle vehicle)
    {
        IEntity effectiveCommander;
        int passengerCount = occupants.Count() - 1;

        foreach (IEntity occupant : occupants) 
		{
            int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(occupant);
            if (!playerId) continue;

            if (!effectiveCommander) effectiveCommander = occupant;

			Seize_RadioOperatorMapMarkerPlayer marker = m_mPlayerMarkers[occupant];
            if (marker) 
			{
                bool isEffectiveCommander = (occupant == effectiveCommander);
                marker.SetGlobalVisible(isEffectiveCommander);

                if (isEffectiveCommander) 
				{
                    UpdateEffectiveCommanderMarker(marker, marker.GetPlayerNameWithRank(playerId, occupant), vehicle, passengerCount);
//					UpdateEffectiveCommanderMarker(marker, "", vehicle, passengerCount);
                }
            }
        }
		
        return effectiveCommander;
    }
    
    //------------------------------------------------------------------------------------------------
    protected void UpdateEffectiveCommanderMarker(Seize_RadioOperatorMapMarkerPlayer marker, string commanderName, Vehicle vehicle, int passengerCount)
    {
        SCR_EditableVehicleComponent edit = SCR_EditableVehicleComponent.Cast(vehicle.FindComponent(SCR_EditableVehicleComponent));
        if (!edit) return;

		string text;
        string displayName = edit.GetDisplayName();
		
		if (passengerCount > 0)
		{
//			text = string.Format("%1 (%2) +%3", displayName, commanderName, passengerCount);
			text = string.Format("%1 (%2 +%3)", displayName, commanderName, passengerCount);
		}
		
		else
		{
			text = string.Format("%1 (%2)", displayName, commanderName);
		}
 
        marker.SetGlobalText(text);
        marker.SetMilitarySymbolIcons(GetVehicleSymbolIcons(edit));
    }

    //------------------------------------------------------------------------------------------------
    //! Get symbol icons based on vehicle type
    EMilitarySymbolIcon GetVehicleSymbolIcons(SCR_EditableVehicleComponent edit)
    {
        EMilitarySymbolIcon icons = 0;
        SCR_EditableEntityUIInfo entityUIInfo = SCR_EditableEntityUIInfo.Cast(edit.GetInfo());
        if (!entityUIInfo) return icons;

        if (entityUIInfo.HasEntityLabel(EEditableEntityLabel.VEHICLE_CAR) || entityUIInfo.HasEntityLabel(EEditableEntityLabel.VEHICLE_TRUCK)) 
		{
            icons = EMilitarySymbolIcon.INFANTRY | EMilitarySymbolIcon.MOTORIZED;
        } 
		else if (entityUIInfo.HasEntityLabel(EEditableEntityLabel.VEHICLE_APC)) 
		{
            icons = EMilitarySymbolIcon.INFANTRY | EMilitarySymbolIcon.ARMOR;        
        } 
		else if (entityUIInfo.HasEntityLabel(EEditableEntityLabel.VEHICLE_HELICOPTER)) 
		{
            icons = EMilitarySymbolIcon.ROTARY_WING;
        } 
		else if (entityUIInfo.HasEntityLabel(EEditableEntityLabel.VEHICLE_AIRPLANE)) 
		{
            icons = EMilitarySymbolIcon.FIXED_WING;
        }
        
        return icons;
    }

    //------------------------------------------------------------------------------------------------
    protected void OnPlayerEnterCompartment(ChimeraCharacter playerCharacter, IEntity compartmentEntity)
    {
        Vehicle vehicle = Vehicle.Cast(compartmentEntity.GetRootParent());
        if (vehicle) UpdatePlayerMarkersInVehicle(vehicle);
    }

    //------------------------------------------------------------------------------------------------
    void OnPlayerExitCompartment(ChimeraCharacter playerCharacter, IEntity compartmentEntity)
    {
        Vehicle vehicle = Vehicle.Cast(compartmentEntity.GetRootParent());
        if (!vehicle) return;
        
        UpdatePlayerMarkersInVehicle(vehicle, playerCharacter);
        
        // Reset the marker for the player that exits
        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(playerCharacter);
		Seize_RadioOperatorMapMarkerPlayer marker = m_mPlayerMarkers[playerCharacter];
        
		if (marker) 
		{
			if (marker.IsSquadLeader(playerId))
            	marker.SetGlobalText(marker.GetPlayerNameWithRank(playerId, playerCharacter)); 
			else
				marker.SetGlobalText("");
			
            marker.SetMilitarySymbolIcons(EMilitarySymbolIcon.INFANTRY);
            marker.SetGlobalVisible(true);
        }
    }

    //------------------------------------------------------------------------------------------------
    void OnRankChanged(SCR_ECharacterRank oldRank, SCR_ECharacterRank newRank, notnull IEntity owner, bool silent)
    {
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(owner.FindComponent(SCR_CompartmentAccessComponent));
        if (compartmentAccess) 
		{
            Vehicle vehicle = Vehicle.Cast(compartmentAccess.GetVehicle());
            if (vehicle) 
			{
                UpdatePlayerMarkersInVehicle(vehicle);
                return;
            }
        }

		Seize_RadioOperatorMapMarkerPlayer marker = m_mPlayerMarkers[owner];
        if (marker) 
		{
//            int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(owner);
//            marker.SetGlobalText(GetPlayerNameWithRank(playerId, owner));
			int playerId = marker.GetPlayerId();
			if (marker.IsSquadLeader(playerId) || marker.IsRadioOperator(playerId))
				marker.SetGlobalText(marker.GetPlayerNameWithRank(playerId, owner));
        }
    }

    //------------------------------------------------------------------------------------------------
    protected void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
    {
        IEntity playerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
        DeleteMarker(playerEntity);
    }

    //------------------------------------------------------------------------------------------------
    protected void OnPlayerDeleted(int playerId, IEntity playerEntity)
    {        
        DeleteMarker(playerEntity);
    }

    //------------------------------------------------------------------------------------------------
    override SCR_EMapMarkerType GetMarkerType()
    {
        return SCR_EMapMarkerType.RADIO_OPERATOR_PLAYER;
    }

    //------------------------------------------------------------------------------------------------
    override void InitServerLogic()
    {
        super.InitServerLogic();

		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
        if (gameMode) 
		{
            gameMode.GetOnPlayerConnected().Insert(OnPlayerConnected);
            gameMode.GetOnPlayerDisconnected().Insert(OnPlayerDisconnected);
            gameMode.GetOnPlayerDeleted().Insert(OnPlayerDeleted);
            SCR_CharacterRankComponent.s_OnRankChanged.Insert(OnRankChanged);
        }
    }

    //------------------------------------------------------------------------------------------------
    override void OnMapLayerChangedDynamic(notnull SCR_MapMarkerDynamicWComponent widgetComp, int layerID)
    {
        super.OnMapLayerChangedDynamic(widgetComp, layerID);
		m_PlayerMarkerWidgetComponent = Seize_RadioOperatorPlayerComponent.Cast(widgetComp);
        Seize_RadioOperatorPlayerComponent operatorWidgetComp = Seize_RadioOperatorPlayerComponent.Cast(widgetComp);
        if (operatorWidgetComp)
            operatorWidgetComp.UpdateTextVisibility();
    }
	
	//------------------------------------------------------------------------------------------------
	bool PlayerIsSquadLeaderOrRadioOperator(int playerId)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId)); //GetPlayerController();
		if (!playerController)
		{
			Print("PlayerIsSquadLeaderOrRadioOperator: No Player Controller for Player ID: " + playerId);
			return false;
		}
			
		SCR_PlayerLoadoutComponent loadoutComp = SCR_PlayerLoadoutComponent.Cast(playerController.FindComponent(SCR_PlayerLoadoutComponent));
		SCR_BasePlayerLoadout loadout = loadoutComp.GetLoadout();
		
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		if (!factionLoadout)
		{
			Print("PlayerIsSquadLeaderOrRadioOperator: No Faction Loadout for Player ID: " + playerId);
		}

		LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
		if (!loadoutType)
		{
			Print("PlayerIsSquadLeaderOrRadioOperator: No Loadout Type for Player ID: " + playerId);
		}	
		
		if (loadoutType && loadoutType == LoadoutType.RADIO_OPERATOR)
			return true;
		
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupController)
		{
			Print("IsPlayerLeader: No Group Controller for Player ID: " + playerId);
			return false;
		}
			
		SCR_AIGroup group = groupController.GetPlayersGroup(); // SCR_PlayerControllerGroupComponent.GetPlayerControllerComponent(playerId).GetPlayersGroup();
		if (!group)
		{
			Print("PlayerIsSquadLeaderOrRadioOperator: No Group for Player ID: " + playerId);
			return false;
		}
		
		Print("PlayerIsSquadLeaderOrRadioOperator: Player Id: " + playerId + " - " + group.IsPlayerLeader(playerId));
		return group.IsPlayerLeader(playerId);
	}
}