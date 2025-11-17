//------------------------------------------------------------------------------------------------
modded enum SCR_EXPRewards
{
	
	PLAYER_DIED,
	NEAR_SQUAD_MEMBER,
	LONE_WOLF,
	LEFT_WARMUP_ZONE,
	LOGISTICS_SUPPORT,
	TEAM_CONTRIBUTION,
	WARCRIME_MEDIC,
	WARCRIME_NONCOMBATANT,
	FRONTLINE_BONUS,
}

modded class SCR_XPHandlerComponent : SCR_BaseGameModeComponent
{
//	PlayerManager m_PlayerManager = GetGame().GetPlayerManager();
    protected ref array<int> m_iAllPlayers = {};
    protected ref array<int> m_iFactionMembers = {};
	protected bool m_bHasFactionMembers;
	protected bool m_bIsMedic;
	
	ResourceName m_sMedkitPrefabUS = "{AE578EEA4244D41F}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_US.et";
	ResourceName m_sMedkitPrefabUSSR = "{21EF98BFC1EB3793}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_USSR.et";
	ResourceName m_sMedkitPrefabFIA = "{9132672C4D9B2102}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_FIA.et";

    // -------------------------------------------------------------------------------------------
    void SeizeGetAllPlayers()
    {
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
        m_iAllPlayers.Clear();
        playerManager.GetPlayers(m_iAllPlayers);
    }

    // -------------------------------------------------------------------------------------------
    bool FilterFactionMembers(int playerId)
    {
        m_iFactionMembers.Clear();
        SeizeGetAllPlayers();

        SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
        if (!factionManager) return false;

        Faction playerFaction = factionManager.GetPlayerFaction(playerId);
        if (!playerFaction) return false;

        foreach (int player : m_iAllPlayers)
        {
            if (player == playerId) continue;
			
            if (playerFaction == factionManager.GetPlayerFaction(player))
            {
				m_bHasFactionMembers = true;
                m_iFactionMembers.Insert(player);
            }
        }

        return m_iFactionMembers.Count() > 0;
    }

    // -------------------------------------------------------------------------------------------
    bool IsNearFactionMember(int searchRadius, int playerId)
    {
		m_bHasFactionMembers = FilterFactionMembers(playerId);
		
        if (!FilterFactionMembers(playerId)) return false;

		PlayerManager playerManager = GetGame().GetPlayerManager();
        SCR_PlayerController playerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerId));
		if (!playerController) return false;
		
        SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(playerController.GetControlledEntity());
        if (!player) return false;

        vector playerPos = player.GetOrigin();

        foreach (int memberId : m_iFactionMembers)
        {
            if (memberId == playerId) continue;

            SCR_ChimeraCharacter factionMember = SCR_ChimeraCharacter.Cast(playerManager.GetPlayerControlledEntity(memberId));
            if (!factionMember || factionMember.GetCharacterController().IsDead()) continue;

            if (IsWithinDistance(factionMember.GetOrigin(), playerPos, searchRadius))
            {
                m_iFactionMembers.Clear();  // Clear the list after use
                return true;
            }
        }

        m_iFactionMembers.Clear();
        return false;
    }

	// -------------------------------------------------------------------------------------------
	bool GetIsOnFrontline(vector playerPosition)
	{
		SCR_CampaignMilitaryBaseManager baseManager = SCR_GameModeCampaign.GetInstance().GetBaseManager();
		if (!baseManager)
			return false;
		
		SCR_CampaignMilitaryBaseComponent campaignBase = baseManager.FindClosestBase(playerPosition);
		if (!campaignBase)
			return false;
		
		return campaignBase.GetIsFrontlineBase();
	}
	
    // -------------------------------------------------------------------------------------------
    bool IsWithinDistance(vector pos1, vector pos2, float radius)
    {
        return vector.DistanceSq(pos1, pos2) <= radius * radius;
    }
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnPlayerKilled(instigatorContextData);
		
		if (IsProxy())
			return;
		
		int playerID = instigatorContextData.GetVictimPlayerID();
		int killerID = instigatorContextData.GetKillerPlayerID();
		
		//~ If player killed self (Admins and GM are not punished)
		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.SUICIDE))
		{
			IEntity player = instigatorContextData.GetVictimEntity();
			SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
			if (characterController)
			{
				array<IEntity> weaponsList = {};
				SCR_ChimeraCharacter character = characterController.GetCharacter();
				if (character)
				{
					CharacterWeaponManagerComponent weaponManager = CharacterWeaponManagerComponent.Cast(character.GetWeaponManager());

					if (weaponManager)
					{	
						IEntity droppedWeapon = characterController.GetDroppedWeapon();
						SCR_EntityHelper.DeleteEntityAndChildren(droppedWeapon);
						
						weaponManager.GetWeaponsList(weaponsList);
						foreach (IEntity weapon : weaponsList)
						{
							if (weapon)
							{
								SCR_EntityHelper.DeleteEntityAndChildren(weapon);
							}			
						}
					}		
				}
			}
			
			ProcessSuicide(playerID);
		}

		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerID);		
		if (pc)
		{
			SCR_PlayerXPHandlerComponent compXP = SCR_PlayerXPHandlerComponent.Cast(pc.FindComponent(SCR_PlayerXPHandlerComponent));

			if (compXP)
			{
				compXP.OnPlayerKilled();
				
				SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
				SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(instigatorContextData.GetVictimEntity());
				SCR_ChimeraCharacter killerCharacter = SCR_ChimeraCharacter.Cast(instigatorContextData.GetKillerEntity());
				Faction playerFaction = playerCharacter.GetFaction();
				
				// For some reason the game doesn't track faction of AI...???
				Faction killerFaction = null;
			
				if (killerCharacter)
					killerFaction = killerCharacter.GetFaction();
				
				int playerXP = compXP.GetPlayerXP();
				int rankXP = factionManager.GetRequiredRankXP(compXP.GetPlayerRankByXP());
				int rankXPDelta = playerXP - rankXP;
				
				// Null errors for killer faction if killed by AI
				if (killerFaction)
				{
					if (!killerFaction.IsFactionFriendly(playerFaction) && playerXP > 50)
					{
					#ifdef WORKBENCH
						Print("Your XP is " + playerXP);
					#endif
						AwardXP(playerID, SCR_EXPRewards.PLAYER_DIED);
						
						//Undo penalty if near teammate
						if (IsNearFactionMember(35, playerID))
						{
						#ifdef WORKBENCH
							Print("IsNearFactionMember is true. Restoring XP...");
						#endif
							//AwardXP(playerID, SCR_EXPRewards.CUSTOM_2);
							GetGame().GetCallqueue().CallLater(SeizeXPAward, 5000, false, playerID, SCR_EXPRewards.CUSTOM_2, rankXPDelta);
						}
					}
				}
				
			}				
		}

		AwardTransportXP(playerID);
		
		SCR_CompartmentAccessComponent compartmentAccessComponent = SCR_CompartmentAccessComponent.Cast(instigatorContextData.GetVictimEntity().FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccessComponent)
			return;

		compartmentAccessComponent.GetOnCompartmentLeft().Remove(OnCompartmentLeft);
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	//! \param[in] playerId
	override void ProcessSuicide(int playerId)
	{
		// Delete Entity from suicide so people can't cheese-loot their bodies
		IEntity playerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (playerEntity)
		{
			GetGame().GetCallqueue().CallLater(StripEntityItems, 1000, false, playerEntity);
		}
		
		super.ProcessSuicide(playerId);	
	}
	
	//------------------------------------------------------------------------------------------------
	void StripEntityItems(IEntity entity)
	{
		if (!entity)
		{
			return;
		}
		
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(entity.FindComponent(SCR_CharacterControllerComponent));
		if (!characterController)
		{
			return;
		}

		InventoryStorageManagerComponent InventoryManager = characterController.GetInventoryStorageManager();
		if (InventoryManager)
		{
			ref array<IEntity> inventoryItems = {};
			InventoryManager.GetItems(inventoryItems);
			
			foreach (IEntity item : inventoryItems)
			{
				if (item)
				{
					BaseLoadoutClothComponent cloth = BaseLoadoutClothComponent.Cast(item.FindComponent(BaseLoadoutClothComponent));
					if (!cloth)
					{
						SCR_EntityHelper.DeleteEntityAndChildren(item);	
					}		
				}			
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void DeleteWeapon(IEntity weapon)
	{
		if (!weapon)
		{
			return;
		}
		
		SCR_EntityHelper.DeleteEntityAndChildren(weapon);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnControllableDestroyed(notnull SCR_InstigatorContextData instigatorContextData)
	{
		/////////////////////////////////////////////////////
		
		SCR_ChimeraCharacter instigatorCharacter = SCR_ChimeraCharacter.Cast(instigatorContextData.GetKillerEntity());
		int killerID = instigatorContextData.GetKillerPlayerID();
		if ((!instigatorCharacter || !instigatorCharacter.IsInVehicle()) && IsNearFactionMember(35, killerID))
		{
		#ifdef WORKBENCH
			Print("IsNearFactionMember is true. Award Near Squad Member XP Bonus here...");
		#endif
			//AwardXP(playerId, SCR_EXPRewards.CUSTOM_2);
			GetGame().GetCallqueue().CallLater(SeizeXPAward, 10000, false, killerID, SCR_EXPRewards.NEAR_SQUAD_MEMBER, 1.0);
		}
		
		super.OnControllableDestroyed(instigatorContextData);
	}
	
	//------------------------------------------------------------------------------------------------
	void SeizeXPAward(int playerId, SCR_EXPRewards reward, float multiplier = 1.0)
	{
		AwardXP(playerId, reward, multiplier);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void AwardTransportXP(int playerId)
	{
		super.AwardTransportXP(playerId);
		
		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerId);
		if (!pc)
			return;
		
		SCR_CampaignMilitaryBaseManager baseManager = SCR_GameModeCampaign.GetInstance().GetBaseManager();
		if (!baseManager)
			return;
			
		bool isFrontline;
		float mult = 1.0;
		vector position = pc.GetOrigin();
		SCR_CampaignMilitaryBaseComponent campaignBase = baseManager.FindClosestBase(position);			
		if (campaignBase && campaignBase.GetIsFrontlineBase())
		{
			isFrontline = true;
			mult = 2.0;
		}
		else
		{
			isFrontline = false;
		}
		
		SCR_XPHandlerComponent compXP = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
		if (compXP && isFrontline)
		{
			GetGame().GetCallqueue().CallLater(compXP.SeizeXPAward, 5000, false, playerId, SCR_EXPRewards.FRONTLINE_BONUS, 1);
		}
	}
}