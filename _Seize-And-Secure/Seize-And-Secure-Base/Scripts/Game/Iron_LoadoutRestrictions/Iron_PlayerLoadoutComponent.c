//! This component should be attached to a PlayerController.
//! It manages player-specific loadout and the communication between player and authority regarding so.
modded class SCR_PlayerLoadoutComponent : ScriptComponent
{
	protected ref OnPlayerLoadoutRequestInvoker m_OnLoadoutAvailabilityRequestInvoker_Ov2 = new OnPlayerLoadoutRequestInvoker();
	protected ref OnPlayerLoadoutRequestInvoker m_OnLoadoutAvailabilityRequestInvoker_Sv2 = new OnPlayerLoadoutRequestInvoker();

	protected ref OnPlayerLoadoutResponseInvoker m_OnLoadoutAvailabilityResponseInvoker_Ov2 = new OnPlayerLoadoutResponseInvoker();
	protected ref OnPlayerLoadoutResponseInvoker m_OnLoadoutAvailabilityResponseInvoker_Sv2 = new OnPlayerLoadoutResponseInvoker();
	
	[RplProp()]
	protected ref array<ref int> m_AvailableLoadoutIndexes = new array<ref int>();
	
	[RplProp()]
	protected ref array<ref float> m_RadioOperatorDistances = new array<ref float>();
	
	[RplProp()]
	protected bool m_bWithinRadioOperatorDistance;
	
	//------------------------------------------------------------------------------------------------
	OnPlayerLoadoutRequestInvoker GetOnLoadoutAvailabilityRequestInvoker_O()
	{
	    return m_OnLoadoutAvailabilityRequestInvoker_Ov2;
	}
	
	//------------------------------------------------------------------------------------------------
	OnPlayerLoadoutResponseInvoker GetOnLoadoutAvailabilityResponseInvoker_O()
	{
	    return m_OnLoadoutAvailabilityResponseInvoker_Ov2;
	}
	
	//------------------------------------------------------------------------------------------------
	OnPlayerLoadoutRequestInvoker GetOnLoadoutAvailabilityRequestInvoker_S()
	{
	    return m_OnLoadoutAvailabilityRequestInvoker_Sv2;
	}
	
	//------------------------------------------------------------------------------------------------
	OnPlayerLoadoutResponseInvoker GetOnLoadoutAvailabilityResponseInvoker_S()
	{
	    return m_OnLoadoutAvailabilityResponseInvoker_Sv2;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetWithinRadioOperatorDistance()
	{
		return m_bWithinRadioOperatorDistance;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref float> GetRadioOperatorDistances()
	{
		return m_RadioOperatorDistances;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetRadioOperatorDistances(array<ref float> distances)
	{
		m_RadioOperatorDistances.Clear();
		foreach(float distance : distances)
		{
			m_RadioOperatorDistances.Insert(distance);
			Replication.BumpMe();
		}	
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref int> GetFilteredLoadoutIndexes()
	{
		return m_AvailableLoadoutIndexes;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetFilteredLoadoutIndexes(array<ref int> loadouts) //array<ref SCR_BasePlayerLoadout> loadouts)
	{
		m_AvailableLoadoutIndexes.Clear();
		foreach(int index : loadouts)
		{
			m_AvailableLoadoutIndexes.Insert(index);
			Replication.BumpMe();
		}	
	}
	
	//------------------------------------------------------------------------------------------------
	void FindRadioOperatorDistances(int playerId, array<ref int> factionPlayers)
	{		
		Rpc(Rpc_FindRadioOperatorDistances_S, playerId, factionPlayers);	
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void Rpc_FindRadioOperatorDistances_S(int playerId, array<ref int> factionPlayers)
    {
		IEntity playerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (!playerEntity)
			return;
		
		vector playerDistance = playerEntity.GetOrigin();
		array<ref float> radioOperatorDistances = {};

		foreach (int player : factionPlayers)
		{
			IEntity factionPlayerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(player);
			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(player));
			SCR_PlayerLoadoutComponent loadoutComp = SCR_PlayerLoadoutComponent.Cast(playerController.FindComponent(SCR_PlayerLoadoutComponent));
			SCR_BasePlayerLoadout loadout = loadoutComp.GetLoadout();
			
			SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
			if (!factionLoadout)
				continue;
			
			LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
			if (!loadoutType)
				continue;
			
			if (loadoutType != LoadoutType.RADIO_OPERATOR)
				continue;
			
			if (playerId == player && loadoutType != LoadoutType.RADIO_OPERATOR)
				continue;
			
			vector factionPlayerDistance = factionPlayerEntity.GetOrigin();
			float distance = vector.DistanceXZ(playerDistance, factionPlayerDistance);
			PrintFormat("Radio Operator Player ID: %1 - Distance: %2", player, distance);
			radioOperatorDistances.Insert(player);
			
			if (distance <= 600)
				m_bWithinRadioOperatorDistance = true;
			else
				m_bWithinRadioOperatorDistance = false;
		}
		
		SetRadioOperatorDistances(radioOperatorDistances);
		Replication.BumpMe();
    }
	
	//------------------------------------------------------------------------------------------------
	void CanIHasLoadout(int playerId, array<ref int> loadoutIndexes)
	{		
		Rpc(Rpc_RequestLoadoutAvailability_S, playerId, loadoutIndexes);	
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void Rpc_RequestLoadoutAvailability_S(int playerId, array<ref int> loadoutIndexes)
    {
		array<ref int> availableLoadouts = {};
		
		SCR_LoadoutManager loadoutManager = GetGame().GetLoadoutManager();
		foreach (int index : loadoutIndexes)
		{
			SCR_BasePlayerLoadout loadout = loadoutManager.GetLoadoutByIndex(index);
			bool isAvailable = GetAllowedLoadouts(playerId, loadout) > 0;
			
			if (isAvailable || GetAllowedLoadouts(playerId, loadout) == -1)
			{
				availableLoadouts.Insert(index);
			}
		}
		
		SetFilteredLoadoutIndexes(availableLoadouts);
		Replication.BumpMe();
    }
	
	//------------------------------------------------------------------------------------------------
    protected void SendLoadoutAvailabilityResponse_S(int playerId, int loadoutIndexes, bool result)
	{

		// Notify server
		GetOnLoadoutAvailabilityResponseInvoker_S().Invoke(this, loadoutIndexes, result);
		
		// Notify user
		Rpc(CanIHasLoadoutResponse_O, loadoutIndexes, result);
		Rpc(CanIHasLoadoutResponse_S, loadoutIndexes, result);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void CanIHasLoadoutResponse_O(int loadoutIndexes, bool response)
	{
		// Unlock this
		SCR_SpawnLockComponent lock = GetLock();
		if (lock)
		{
			lock.Unlock(this, false);
		}

		// Notify owner
		if (IsOwner())
			GetOnLoadoutAvailabilityResponseInvoker_O().Invoke(this, loadoutIndexes, response);
		
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void CanIHasLoadoutResponse_S(int loadoutIndexes, bool response)
	{
		// Unlock this
		SCR_SpawnLockComponent lock = GetLock();
		if (lock)
		{
			lock.Unlock(this, false);
		}

		// Notify owner
		if (!IsProxy())
			GetOnLoadoutAvailabilityResponseInvoker_S().Invoke(this, loadoutIndexes, response);
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected int GetAllowedLoadouts(int playerId, SCR_BasePlayerLoadout loadout)
	{
	    int allowedLoadouts = 0;
		int currentLoadoutsClaimed = 0;
		int squadSize;
		
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		if (!factionLoadout)
			return -1;
		
		ref array<ref LoadoutStep> loadoutSteps = factionLoadout.GetLoadoutSteps();
		LoadoutType loadoutType = GetLoadoutType(loadout);
		
		if (loadoutSteps.Count() == 0)
			return -1;
	
		PlayerController pc = GetPlayerController(); //SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
//		if (!pc)
//			return -1;
		
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(pc.FindComponent(SCR_PlayerControllerGroupComponent));
//		if (!groupController)
//			return -1;
		
		SCR_AIGroup group = groupController.GetPlayersGroup(); // SCR_PlayerControllerGroupComponent.GetPlayerControllerComponent(playerId).GetPlayersGroup();
		if (!group)
		{
			squadSize = 0;
		}
		
		else
		{
			squadSize = group.GetPlayerCount();
			currentLoadoutsClaimed = GetGroupLoadoutsTaken(group, squadSize, playerId, loadoutType);
		}
		
	    // Iterate through loadout steps to determine allowed loadouts based on the current squad size
	    foreach (LoadoutStep step : loadoutSteps) 
		{
			if (step.MaximumLoadouts == -1 || squadSize == 0)
			{
				return -1;
			}

	        if (squadSize >= step.MinimumSquadSize) 
			{
	            allowedLoadouts = step.MaximumLoadouts;
				continue;	
	        }
			 
			else 
			{
	            break; // Exit with the highest applicable step
	        }
#ifdef WORKBENCH			
			Print("Claimed Loadouts for current Step: " + currentLoadoutsClaimed + "/" + allowedLoadouts);
#endif
	    }

	    // Calculate additional loadouts allowed
	    int additionalLoadouts = allowedLoadouts - currentLoadoutsClaimed;

		
	    return Math.Clamp(additionalLoadouts, 0, allowedLoadouts);
	}
	
	//------------------------------------------------------------------------------------------------
	protected int GetGroupLoadoutsTaken(SCR_AIGroup group, int squadSize, int playerId, LoadoutType loadoutType)
	{
		int currentLoadoutsClaimed = 0;
		
		array<int> playerIDs = group.GetPlayerIDs();
		foreach (int player : playerIDs)
		{
#ifdef WORKBENCH
			Print("Group Members..." + playerIDs.Count());
#endif
			// Skip checking loadout of player in question so they can claim it again
			if (playerId == player)
				continue;
			
			IEntity owner = GetGame().GetPlayerManager().GetPlayerControlledEntity(player);
			SCR_PlayerController mpc = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(player));
			
			if (!mpc)
			{
				Print("Unable To Get Player Controller for PlayerID: " + player);
				continue;
			}
			
			SCR_PlayerLoadoutComponent memberLoadoutComponent = SCR_PlayerLoadoutComponent.Cast(mpc.FindComponent(SCR_PlayerLoadoutComponent));
    		SCR_BasePlayerLoadout memberLoadout = memberLoadoutComponent.GetAssignedLoadout();
			LoadoutType memberLoadoutType = GetLoadoutType(memberLoadout);
			
#ifdef WORKBENCH			
			Print("Member ID: " + player + " Has Loadout Type: " + memberLoadoutType);
#endif			
			if (loadoutType == memberLoadoutType)
			{
				currentLoadoutsClaimed++;
			}		
		}
		
		return currentLoadoutsClaimed;
	}
	
	//------------------------------------------------------------------------------------------------
    LoadoutType GetLoadoutType(SCR_BasePlayerLoadout loadout) 
	{
	    // Attempt to cast the loadout to SCR_FactionPlayerLoadout
	    SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);

	    if (factionLoadout) 
		{
	        return factionLoadout.GetLoadoutType(loadout);
	    }
	
	    Print("Warning: Provided loadout is not of type SCR_FactionPlayerLoadout. Returning Default.");
	    return LoadoutType.DEFAULT; // Return a default value if handle error
    }
}
