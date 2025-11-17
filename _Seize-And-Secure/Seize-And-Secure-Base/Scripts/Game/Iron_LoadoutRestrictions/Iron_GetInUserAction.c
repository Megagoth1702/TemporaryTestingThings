modded class SCR_GetInUserAction : SCR_CompartmentUserAction
{	
	[Attribute("0", desc: "Must have Pilot Role to perform.", category: "Loadout Restrictions")]
	protected bool m_bMustBePilot;
	
	[Attribute("0", desc: "Must have Crewman Role to perform.", category: "Loadout Restrictions")]
	protected bool m_bMustBeCrewman;

	SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (m_DamageManager && m_DamageManager.GetState() == EDamageState.DESTROYED)
			return false;

		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return false;
		
		//~ TODO: Hotfix until proper solution, only blocks player does not block AI or Editor actions
		float storedResources;
		if (m_ResourceComp && m_CompartmentManager && m_CompartmentManager.BlockSuppliesIfOccupied())
		{
			if (SCR_ResourceSystemHelper.GetStoredResources(m_ResourceComp, storedResources) && storedResources > 0)
			{
				SetCannotPerformReason(OCCUPIED_BY_SUPPLIES);
				return false;
			}
		}

		if (CampaignInfo && CampaignInfo.GetIsLoadoutRolesRestrictedActions())
		{
			/// First check Loadout Type ///
		    LoadoutType loadoutType = GetLoadoutType(user);
		
		    // Check for pilot and crewman requirements
		    if (m_bMustBePilot && m_bMustBeCrewman)
		    {
		        if (loadoutType == LoadoutType.PILOT || loadoutType == LoadoutType.CREWMAN)
		        {
		            return true;
		        }
				
		        SetCannotPerformReason("Pilot/Crewman roles only");
		        return false;
		    }
		
		    if (m_bMustBePilot && loadoutType != LoadoutType.PILOT)
		    {
		        SetCannotPerformReason("Pilot role only");
		        return false;
		    }
		
		    if (m_bMustBeCrewman && loadoutType != LoadoutType.CREWMAN)
		    {
		        SetCannotPerformReason("Crewman role only");
		        return false;
		    }
		}
		
		///////////////////////////////////////
		IEntity owner = compartment.GetOwner();
		Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(owner, true));
		if (vehicle)
		{
			Faction characterFaction = character.GetFaction();
			Faction vehicleFaction = vehicle.GetFaction();
			if (characterFaction && vehicleFaction && characterFaction.IsFactionEnemy(vehicleFaction))
			{
				SetCannotPerformReason("#AR-UserAction_SeatHostile");
				return false;
			}
		}
		
		if (compartment.GetOccupant())
		{
			SetCannotPerformReason("#AR-UserAction_SeatOccupied");
			return false;
		}
		
		// Check if the position isn't lock.
		if (m_pLockComp && m_pLockComp.IsLocked(user, compartment))
		{
			SetCannotPerformReason(m_pLockComp.GetCannotPerformReason(user));
			return false;
		}
		
		// Make sure vehicle can be enter via provided door, if not, set reason.
		if (!compartmentAccess.CanGetInVehicleViaDoor(owner, m_CompartmentManager, GetRelevantDoorIndex(user)))
		{
			SetCannotPerformReason("#AR-UserAction_SeatObstructed");
			return false;
		}
		
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	LoadoutType GetLoadoutType(IEntity owner)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(owner);
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		SCR_PlayerLoadoutComponent m_PlyLoadoutComp = SCR_PlayerLoadoutComponent.Cast(pc.FindComponent(SCR_PlayerLoadoutComponent));
		SCR_BasePlayerLoadout loadout = m_PlyLoadoutComp.GetLoadout();
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
		return loadoutType;
	}
};
