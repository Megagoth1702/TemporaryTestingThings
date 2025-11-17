//------------------------------------------------------------------------------------------------
modded class SCR_CampaignBuildingStartUserAction : ScriptedUserAction
{
	ref array<CharacterControllerComponent> m_aEnemyEntities = {};
	
	[Attribute("0", UIWidgets.CheckBox, "Must be Squad Leader before mode is accessible.")]
	protected bool m_bMustBeSquadLeader;
	
	[Attribute("0", UIWidgets.CheckBox, "Must be in special group type before mode is accessible.")]
	protected bool m_bRestrictedToGroupType;
	
	[Attribute("", UIWidgets.ComboBox, "Group Type", enums: ParamEnumArray.FromEnum(SCR_EGroupRole))]
    protected ref array<ref SCR_EGroupRole> m_GroupRole;
	
	//------------------------------------------------------------------------------------------------
	//! Set temporary blocked access.
//	override void SetTemporaryBlockedAccess()
//	{
//		IEntity masterProvider = m_ProviderComponent.GetMasterProviderEntity();
//		if (!masterProvider)
//			return;
//		
//		SCR_CampaignBuildingProviderComponent masterProviderComponent = SCR_CampaignBuildingProviderComponent.Cast(masterProvider.FindComponent(SCR_CampaignBuildingProviderComponent));
//		if (!masterProviderComponent)
//			return;
//		
//		GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), masterProviderComponent.GetBuildingRadius(), EvaluateEntity, null, EQueryEntitiesFlags.DYNAMIC);
//		GetGame().GetCallqueue().CallLater(ResetTemporaryBlockedAccess, TEMPORARY_BLOCKED_ACCESS_RESET_TIME, false);
////		GetGame().GetCallqueue().CallLater(ClearEnemyList, 5000, false);
//	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if this entity can block player to enter a building mode. If such anentity is found, return false to stop evaluating next enttiy found by query.
	//\param[in] ent Entity to evaluate by this filter.
//	override bool EvaluateEntity(IEntity ent)
//	{	
//		if (!ent)
//			return true;
//		
//		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(ent);
//		if (!char)
//			return true;
//		
//		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(char.FindComponent(SCR_CharacterDamageManagerComponent));
//		if (!charDamageManager || charDamageManager.GetState() == EDamageState.DESTROYED)
//			return true;
//		
//		if (!m_ProviderComponent.IsEnemyFaction(char))
//		{
//			m_aEnemyEntities.Clear();
//			return true;
//		}
//
//		CharacterControllerComponent charControl = char.GetCharacterController();
//		if (!charControl)
//			return true;
//		
//		m_aEnemyEntities.Insert(charControl);
//		
//		// At least 2 enemies before blocking building access
//		if (m_aEnemyEntities.Count() < 2)
//		{
//			return true;	
//		}
//	
//		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(ent);
//		if (playerId == 0)
//		{
//			AIControlComponent ctrComp = charControl.GetAIControlComponent();
//			if (!ctrComp)
//				return true;
//			
//			if (ctrComp.IsAIActivated())
//			{
//				m_bTemporarilyBlockedAccess = true; // true
//				return false; // false
//			}
//		}
//		else
//		{
//			m_bTemporarilyBlockedAccess = true; // true
//			return false; // false
//		}
//		
//		return true;
//	}
	
	//------------------------------------------------------------------------------------------------
	void ClearEnemyList()
	{
		m_aEnemyEntities.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	// Building with MHQ
	override bool CanBePerformedScript(IEntity user)
	{	
		//FactionAffiliationComponent userAffiliateFaction = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
		//Faction userFaction = SCR_Faction.Cast(userAffiliateFaction.GetAffiliatedFaction());
		//bool isInRadioRange = SCR_GameModeCampaign.GetInstance().GetBaseManager().IsEntityInFactionRadioSignal(user, userFaction);

		if (!m_ProviderComponent || m_bTemporarilyBlockedAccess)
			return false;
		
		if (SCR_XPHandlerComponent.IsXpSystemEnabled() && m_ProviderComponent.GetAccessRank() > GetUserRank(user))
		{
			FactionAffiliationComponent factionAffiliationComp = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
			if (!factionAffiliationComp)
				return false;
			
			string rankName;
			SCR_Faction faction = SCR_Faction.Cast(factionAffiliationComp.GetAffiliatedFaction());
			if (faction)
				rankName = faction.GetRankName(m_ProviderComponent.GetAccessRank());
				
			SetCannotPerformReason(rankName);
			return false;
		}
		
		bool hasComponent = SCR_CampaignMobileAssemblyComponent.Cast(m_ProviderComponent.GetOwner().FindComponent(SCR_CampaignMobileAssemblyComponent));
		SCR_CampaignMobileAssemblyComponent MHQIsDeployed = SCR_CampaignMobileAssemblyComponent.Cast(m_ProviderComponent.GetOwner().FindComponent(SCR_CampaignMobileAssemblyComponent));
		
		if (m_bMustBeSquadLeader && !GetIsSquadLeader(user))
		{
			SetCannotPerformReason("#Iron_MustBeSquadLeader");
			return false;
		}
		
		SCR_EGroupRole playerGroupRole = GetPlayerGroupRole(user);
		if (m_bRestrictedToGroupType && !m_GroupRole.Contains(playerGroupRole))
		{
			SetCannotPerformReason("#Iron_MustBeInSpecializedGroup");
			return false;
		}
		
		if (m_ProviderComponent.RequiresDeployment() && hasComponent)
		{	
			// Check if MHQ Deployed
			if (MHQIsDeployed.IsDeployed())
			{
				return true;	
			}
		
			else
			{
				SetCannotPerformReason("MHQ IS NOT DEPLOYED");
				return false;
			}	
		}
		
		if (m_bAccessCanBeBlocked)
		{
			SetTemporaryBlockedAccess();
		
			if (m_bTemporarilyBlockedAccess)
			{
				SetCannotPerformReason("#AR-Campaign_Action_ShowBuildPreviewEnemyPresence");
				return false;
			}
		}

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsSquadLeader(IEntity user)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		if (!playerId)
		{
			return false;
		}
		
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
		{
			//PrintFormat("GetIsSquadLeader: No Group Manager Found!");
			return false;
		}
		
		bool isLeader;
		SCR_AIGroup group = groupsManager.GetPlayerGroup(playerId);		
		if (group)
		{
			isLeader = group.IsPlayerLeader(playerId);
		}
		
//		PrintFormat("GetIsSquadLeader: Player ID: %1 | %2", playerId, isLeader);
		return isLeader;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_EGroupRole GetPlayerGroupRole(IEntity user)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		if (!playerId)
		{
			return null;
		}
		
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
		{
			//PrintFormat("GetIsSquadLeader: No Group Manager Found!");
			return null;
		}

		SCR_AIGroup group = groupsManager.GetPlayerGroup(playerId);
		if (!group)
		{
			return null;
		}
		
		return group.GetGroupRole();
	}
}
