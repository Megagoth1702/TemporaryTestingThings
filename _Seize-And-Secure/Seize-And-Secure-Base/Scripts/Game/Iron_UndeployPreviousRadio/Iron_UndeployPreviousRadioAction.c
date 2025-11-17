class Iron_UndeployPreviousRadioAction : ScriptedUserAction
{
	protected SCR_BaseDeployableInventoryItemComponent m_DeployableItemComp;
	protected SCR_BaseDeployableSpawnPointComponent m_PreviousDeployableItemComp;
	
	protected ref array<SCR_BaseDeployableSpawnPointComponent> s_aActiveDeployedSpawnPoints = {};

	SCR_RestrictedDeployableSpawnPointComponent m_RestrictedSpawnPoint;
	protected SCR_BaseDeployableSpawnPointComponent m_DeployableSpawnPointComponent;
	protected RplComponent m_RplComp;
	
	[Attribute("0", desc: "Must have Squad Leader Role to perform.", category: "Loadout Restrictions")]
	protected bool m_bMustBeSquadLeader;
	
	[Attribute("0", desc: "Must have Radio Operator Role to perform.", category: "Loadout Restrictions")]
	protected bool m_bMustBeRadioOperator;
	
	SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		string reason;
		
		if (CampaignInfo)
		{
			SCR_PlayerSpawnPointManagerComponent playerSpawnManager = SCR_PlayerSpawnPointManagerComponent.Cast(CampaignInfo.FindComponent(SCR_PlayerSpawnPointManagerComponent));
			if (playerSpawnManager && !playerSpawnManager.IsDeployingSpawnPointsEnabled())
			{
				return false;
			}
		}
		
		if (!m_DeployableItemComp.IsDeployed() && !m_RestrictedSpawnPoint.CanDeployBePerformed(reason))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		if (CampaignInfo && CampaignInfo.GetIsLoadoutRolesRestrictedActions())
		{
			/// First check Loadout Type ///
		    LoadoutType loadoutType = GetLoadoutType(user);
		
		    // Check for pilot and crewman requirements
		    if (m_bMustBeSquadLeader && m_bMustBeRadioOperator)
		    {
		        if (loadoutType == LoadoutType.SQUAD_LEADER || loadoutType == LoadoutType.RADIO_OPERATOR)
		        {
		            return true;
		        }
				
		        SetCannotPerformReason("Squad Leader/Radio Operators only");
		        return false;
		    }
		
		    if (m_bMustBeSquadLeader && loadoutType != LoadoutType.SQUAD_LEADER)
		    {
		        SetCannotPerformReason("Squad Leader only");
		        return false;
		    }
		
		    if (m_bMustBeRadioOperator && loadoutType != LoadoutType.RADIO_OPERATOR)
		    {
		        SetCannotPerformReason("Radio Operator only");
		        return false;
		    }
		}
			
		return true;
 	}
	
 	//------------------------------------------------------------------------------------------------
 	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
 	{
		s_aActiveDeployedSpawnPoints = SCR_BaseDeployableSpawnPointComponent.GetActiveDeployedSpawnPoints();
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		foreach (SCR_BaseDeployableSpawnPointComponent activeSpawnPoint : s_aActiveDeployedSpawnPoints)
		{
			SCR_DeployableSpawnPoint spawnPoint = SCR_DeployableSpawnPoint.Cast(activeSpawnPoint.GetSpawnPoint());
			SCR_RestrictedDeployableSpawnPoint restrictedSpawnPoint = SCR_RestrictedDeployableSpawnPoint.Cast(activeSpawnPoint.GetSpawnPoint());
	       
			if (spawnPoint.IsSpawnPointVisibleForPlayer(playerId)) 
			{ 	//
				m_RplComp = RplComponent.Cast(activeSpawnPoint.FindComponent(RplComponent));
				activeSpawnPoint.Dismantle(pUserEntity, true);
				return;
	        }
		}
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
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		IEntity owner = GetOwner();
		if (!owner)
			return;
			
		m_RestrictedSpawnPoint = SCR_RestrictedDeployableSpawnPointComponent.Cast(owner.FindComponent(SCR_RestrictedDeployableSpawnPointComponent));
		m_DeployableItemComp = SCR_BaseDeployableInventoryItemComponent.Cast(owner.FindComponent(SCR_BaseDeployableInventoryItemComponent));

	}
}