modded class SCR_DeployInventoryItemRestrictedAction : SCR_DeployInventoryItemBaseAction
{
	[Attribute("0", desc: "Must have Squad Leader Role to perform.", category: "Loadout Restrictions")]
	protected bool m_bMustBeSquadLeader;
	
	[Attribute("0", desc: "Must have Squad nearby to perform.", category: "Loadout Restrictions")]
	protected bool m_bMustBeNearSquadMember;
	
	[Attribute("10", desc: "Radius to check for if Near Squad Member", category: "Loadout Restrictions")]
	protected int m_iSquadMemberRadius;
	
	[Attribute("0", desc: "Must have Radio Operator Role to perform.", category: "Loadout Restrictions")]
	protected bool m_bMustBeRadioOperator;
	
	SCR_GameModeCampaign m_CampaignInfo;
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		SCR_RestrictedDeployableSpawnPointComponent restrictedDeployableSpawnPointComp = SCR_RestrictedDeployableSpawnPointComponent.Cast(m_DeployableItemComp);
		if (!restrictedDeployableSpawnPointComp)
			return false;

		string reason;
		bool canBePerformed = restrictedDeployableSpawnPointComp.CanDeployBePerformed(reason);
		if (!reason.IsEmpty())
			SetCannotPerformReason(reason);
		
		if (!canBePerformed)
			return false;

		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (m_CampaignInfo && character)
		{
			SCR_CampaignFactionManager factionManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
			if (factionManager)
			{
				SCR_Faction characterFaction = SCR_Faction.Cast(character.GetFaction());			
				SCR_Faction enemyFaction = SCR_Faction.Cast(factionManager.GetEnemyFaction(SCR_CampaignFaction.Cast(characterFaction)));
				bool friendlySignal = SCR_GameModeCampaign.GetInstance().GetBaseManager().IsEntityInFactionRadioSignal(user, characterFaction);
				bool enemySignal = SCR_GameModeCampaign.GetInstance().GetBaseManager().IsEntityInFactionRadioSignal(user, enemyFaction) && enemyFaction.IsPlayable();
				
				if (m_CampaignInfo.GetDoesRPRequireHQSignal() && !friendlySignal)
				{
					SetCannotPerformReason("Must be within HQ Range");
					return false;
				}
				
				if (m_CampaignInfo.GetRPMustNotBeOnFrontline() && enemySignal)
				{
					SetCannotPerformReason("Must not be within enemy radio range");
					return false;
				}
			}			
		}
				
		if (m_CampaignInfo.GetIsLoadoutRolesRestrictedActions())
		{
			// First check if near team member...may change this to squad member
			if (m_bMustBeNearSquadMember)
			{
				int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
				int playerCount = GetGame().GetPlayerManager().GetPlayerCount();
				
				bool isNearTeamMember = IsNearSquadMember(m_iSquadMemberRadius, playerId);
				if (!isNearTeamMember && playerCount > 9)
				{
					SetCannotPerformReason("Must be near team member");
					return false;
				}
			}
			
			/// Next check Loadout Type ///
		    LoadoutType loadoutType = GetLoadoutType(user);
		
		    // Check for pilot and crewman requirements
		    if (m_bMustBeSquadLeader && m_bMustBeRadioOperator)
		    {
		        if (loadoutType == LoadoutType.SQUAD_LEADER || loadoutType == LoadoutType.RADIO_OPERATOR || IsSquadLeader(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user)))
		        {
		            return super.CanBePerformedScript(user); //true;
		        }
				
		        SetCannotPerformReason("Squad Leader/Radio Operators only");
		        return false;
		    }
		
		    else if (m_bMustBeSquadLeader && loadoutType != LoadoutType.SQUAD_LEADER && !m_bMustBeRadioOperator)
		    {
		        SetCannotPerformReason("Squad Leader only");
		        return false;
		    }
		
		    else if (m_bMustBeRadioOperator && loadoutType != LoadoutType.RADIO_OPERATOR && !m_bMustBeSquadLeader)
		    {
		        SetCannotPerformReason("Radio Operator only");
		        return false;
		    }
		}
				
		return super.CanBePerformedScript(user);
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
	bool IsNearSquadMember(int range, int playerId)
	{
		SCR_XPHandlerComponent XPHandler = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
		if (!XPHandler)
			return false;
		
		return XPHandler.IsNearFactionMember(range, playerId);
	}
}