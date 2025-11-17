//------------------------------------------------------------------------------------------------
//! Dynamic map marker entity base class 
//! Spawned by marker manager when creating a dynamic marker -> see marker config for customization
modded class SCR_MapMarkerEntity : GenericEntity
{	
	[RplProp(onRplName: "OnUpdateType")]
	protected SCR_EMapMarkerType m_eType;
	
	[RplProp(onRplName: "OnUpdateType")]
	protected bool m_bIsRadioOperator;
	
	[RplProp(onRplName: "OnUpdateType")]
	protected bool m_bIsSquadLeader;
	
	[RplProp(onRplName: "OnUpdate")]
	protected int m_iPlayerId;
	
	//------------------------------------------------------------------------------------------------
	bool GetIsRadioOperator()
	{
		return m_bIsRadioOperator;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetIsRadioOperator(bool value)
	{
		m_bIsRadioOperator = value;
		OnUpdateType();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsSquadLeader()
	{
		return m_bIsSquadLeader;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetIsSquadLeader(bool value)
	{
		m_bIsSquadLeader = value;
		OnUpdateType();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetPlayerId()
	{
		return m_iPlayerId;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPlayerId(int value)
	{
		m_iPlayerId = value;
		OnUpdate();
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	void SetInfo(SCR_EMapMarkerType type, int playerId, IEntity playerEntity)
	{
		Print("SetType: Setting type for Player ID: " + playerId + " Type: " + type);
		SetPlayerId(playerId);
		
		if (IsRadioOperator(playerId))
			SetIsRadioOperator(true);
		
		if (IsSquadLeader(playerId))
		{
			SetText(GetPlayerNameWithRank(playerId, playerEntity));
			SetIsSquadLeader(true);
		}	
		
		OnUpdateType();
		Replication.BumpMe();	
	}
	
	//------------------------------------------------------------------------------------------------
    string GetPlayerNameWithRank(int playerId, IEntity playerEntity)
    {
        string name = GetGame().GetPlayerManager().GetPlayerName(playerId);
        string rank = SCR_CharacterRankComponent.GetCharacterRankNameShort(playerEntity);
        string fullName = string.Format("%1 %2", rank, name);
		
		if (IsSquadLeader(playerId))
		{
//			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
//			SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
//			SCR_AIGroup group = groupController.GetPlayersGroup();
//			if (group)
				fullName = "(S.L.) " + fullName; // group.GetCustomNameWithOriginal(); // <-- Null error #return
		}
		
		if (IsRadioOperator(playerId))
		{
			fullName = "(R.O.) " + fullName;
		}
#ifdef WORKBENCH 
		PrintFormat("GetPlayerNameWithRank: %1", fullName);
#endif
		return fullName;
    }
	
	//------------------------------------------------------------------------------------------------
	bool IsSquadLeader(int playerId)
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
		
//		Print("IsSquadLeader: Player Id: " + playerId + " - " + group.IsPlayerLeader(playerId));
		return group.IsPlayerLeader(playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsRadioOperator(int playerId)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return false;
		
		SCR_PlayerLoadoutComponent loadoutComp = SCR_PlayerLoadoutComponent.Cast(playerController.FindComponent(SCR_PlayerLoadoutComponent));
		SCR_BasePlayerLoadout loadout = loadoutComp.GetLoadout();
		
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		if (!factionLoadout)
			return false;
		
		LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
		if (!loadoutType)
			return false;
		
		return loadoutType == LoadoutType.RADIO_OPERATOR;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsIncap(int playerId)
	{
		SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
		if (!playerChar)
			return true;

		CharacterControllerComponent charControl = playerChar.GetCharacterController();
		if (!charControl)
			return true;
		
		return charControl.IsDead() || charControl.IsUnconscious();
	}
}