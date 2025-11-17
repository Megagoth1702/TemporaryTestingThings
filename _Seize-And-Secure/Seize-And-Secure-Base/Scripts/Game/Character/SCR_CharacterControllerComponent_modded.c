//------------------------------------------------------------------------------------------------
enum Iron_Ragdoll_EUnconsciousPose
{
    NONE = 0,
    BACK = 4,
    RECOVERY = 5,
	TEST = 1
}

[ComponentEditorProps(category: "GameScripted/Character", description: "Scripted character controller", icon: HYBRID_COMPONENT_ICON)]
modded class SCR_CharacterControllerComponentClass : CharacterControllerComponentClass
{
}

modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	IEntity m_cKillerEntity;
	WorldTimestamp m_wDeathTimestamp;
	SCR_ChimeraCharacter m_cPlayerCharacter;
	CharacterControllerComponent m_cCharacterController;
	Faction m_cKillerFaction;
	Instigator m_iKiller;
	string m_cKillerName;
	bool m_bScoreReduced;
	Faction killerFaction;
	
	[RplProp(onRplName: "UpdateDroppedWeapon")]
	EntityID m_DroppedWeaponId;
	
	protected bool m_bIsMedic;
	protected bool m_bWasArmed = true;
	protected bool m_bIsNearBase = true;
	WorldTimestamp m_UnconTimestamp;
	
	ResourceName m_sMedkitPrefabUS = "{AE578EEA4244D41F}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_US.et";
	ResourceName m_sMedkitPrefabUSSR = "{21EF98BFC1EB3793}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_USSR.et";
	ResourceName m_sMedkitPrefabFIA = "{9132672C4D9B2102}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_FIA.et";

	[RplProp(onRplName: "Action_OnUnconsciousPoseChanged")]
    protected Iron_Ragdoll_EUnconsciousPose m_UnconsciousPosePosition;
	
	//------------------------------------------------------------------------------------------------
	override void OnDeath(IEntity instigatorEntity, notnull Instigator instigator)
	{ 
		super.OnDeath(instigatorEntity, instigator);
		
		SCR_ChimeraCharacter killerCharacter = SCR_ChimeraCharacter.Cast(instigatorEntity);
		
		if (killerCharacter)
			killerFaction = killerCharacter.GetFaction(); // Null error if character is AI?
		
		m_cKillerName = GetGame().GetPlayerManager().GetPlayerName(instigator.GetInstigatorPlayerID());
		m_iKiller = instigator;
		m_cKillerEntity = instigatorEntity;
		m_cKillerFaction = killerFaction;
		IEntity playerCharacter = GetCharacter();
		m_cPlayerCharacter = SCR_ChimeraCharacter.Cast(playerCharacter);
		
		ChimeraWorld world = GetGame().GetWorld();
		SCR_MenuSpawnLogic spawnLogic;
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		if (respawnSystem)
		{
			spawnLogic = SCR_MenuSpawnLogic.Cast(respawnSystem.GetSpawnLogic());
		}
	
		if (spawnLogic)
		{
			m_wDeathTimestamp = world.GetServerTimestamp().PlusSeconds(spawnLogic.GetDeployMenuOpenDelay());
		}
		
		m_bIsNearBase = GetIsNearBase(125);
		
//		PrintFormat("OnDeath: Near Base: %1", m_bIsNearBase);
//		if (CheckIfUnarmed(playerCharacter) && IsMedic(playerCharacter) && killerFaction != m_cPlayerCharacter.GetFaction() && !m_cPlayerCharacter.IsInVehicle())
		if (!m_bIsNearBase && !m_bWasArmed && killerFaction != m_cPlayerCharacter.GetFaction() && !m_cPlayerCharacter.IsInVehicle())
		{
			int killerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(instigatorEntity);
			SCR_XPHandlerComponent compXP = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
			
			if (compXP)
			{
				if (IsMedic(playerCharacter))
				{
					compXP.AwardXP(killerID, SCR_EXPRewards.WARCRIME_MEDIC);
				}
				else
				{
					compXP.AwardXP(killerID, SCR_EXPRewards.WARCRIME_NONCOMBATANT);
				}
			}						
		}
	}
	
	//------------------------------------------------------------------------------------------------
	Instigator GetKillerInstigator()
	{
		return m_iKiller;
	}
	
	//------------------------------------------------------------------------------------------------	
	IEntity GetKillerEntity()
	{
		return m_cKillerEntity;
	}
	
	//------------------------------------------------------------------------------------------------	
	Faction GetKillerFaction()
	{
		return m_cKillerFaction;
	}

	//------------------------------------------------------------------------------------------------
	string GetKillerName()
	{
		return m_cKillerName;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetScoreReduced()
	{
		return m_bScoreReduced;
	}
	
	//------------------------------------------------------------------------------------------------
	WorldTimestamp GetDeathTimestamp()
	{
		return m_wDeathTimestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_ChimeraCharacter GetPlayerCharacter()
	{
		return m_cPlayerCharacter;
	}
	
	//------------------------------------------------------------------------------------------------	
	CharacterControllerComponent GetCharacterController()
	{
		return m_cCharacterController;
	}

	//------------------------------------------------------------------------------------------------
    protected void Action_OnUnconsciousPoseChanged()
    {
        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetOwner());
        if (char && IsRpcAuthority(char))
        {
            CharacterAnimationComponent animComponent = char.GetAnimationComponent();
            if (animComponent)
            {
                animComponent.SetVariableInt(animComponent.BindVariableInt("UnconsciousPose"), m_UnconsciousPosePosition);
            }
        }
    }

    //------------------------------------------------------------------------------------------------
    protected override void OnAnimationEvent(AnimationEventID animEventType, AnimationEventID animUserString, int intParam, float timeFromStart, float timeToEnd)
    {
        super.OnAnimationEvent(animEventType, animUserString, intParam, timeFromStart, timeToEnd);

        if (IsAuthority(GetOwner()) && animEventType == GetAnimationEvent("Action_Event_UnconsciousPoseChange") && intParam != m_UnconsciousPosePosition)
        {
            Do_UnconPoseReposition(intParam);
        }
    }

    //------------------------------------------------------------------------------------------------
    void Do_UnconPoseReposition(Iron_Ragdoll_EUnconsciousPose pose)
    {
        if (m_UnconsciousPosePosition != pose) // Avoid unnecessary updates
        {
            m_UnconsciousPosePosition = pose;
            Replication.BumpMe();
            Action_OnUnconsciousPoseChanged();
        }
    }

    //------------------------------------------------------------------------------------------------
    Iron_Ragdoll_EUnconsciousPose GetUnconsciousPose()
    {
        return m_UnconsciousPosePosition;
    }

    //------------------------------------------------------------------------------------------------
    private bool IsRpcAuthority(SCR_ChimeraCharacter char)
    {
        RplComponent rpl = char.GetRplComponent();
        return rpl && (rpl.IsOwner() || rpl.Role() == RplRole.Authority);
    }

    //------------------------------------------------------------------------------------------------
    private bool IsAuthority(IEntity entity)
    {
        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(entity);
        return char && IsRpcAuthority(char);
    }
    
    //------------------------------------------------------------------------------------------------
    private AnimationEventID GetAnimationEvent(string eventName)
    {
        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetOwner());
        return char.GetAnimationComponent().BindEvent(eventName);
    }

	//------------------------------------------------------------------------------------------------
	//! \If inventory contains medkit, then true
	bool IsMedic(IEntity user)
	{
		/// First check Loadout Type ///
		PlayerController pc = GetGame().GetPlayerController();
		SCR_PlayerLoadoutComponent m_PlyLoadoutComp = SCR_PlayerLoadoutComponent.Cast(pc.FindComponent(SCR_PlayerLoadoutComponent));
		SCR_BasePlayerLoadout loadout = m_PlyLoadoutComp.GetLoadout();
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
		
		if (loadoutType == LoadoutType.MEDIC)
			return true;
		
		///////////////////////////////
		
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
		InventoryStorageManagerComponent InventoryManager = characterController.GetInventoryStorageManager();
		SCR_EntityCatalogManagerComponent entityCatalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		
		ref array<IEntity> inventoryItems = {};
		InventoryManager.GetItems(inventoryItems);
		
		foreach (IEntity item : inventoryItems)
		{

			SCR_ArsenalComponent arsenalComponent = SCR_ArsenalComponent.FindArsenalComponent(item);
			ResourceName resourceNameItem = item.GetPrefabData().GetPrefabName();
				
			if (resourceNameItem == m_sMedkitPrefabUS || resourceNameItem == m_sMedkitPrefabUSSR || resourceNameItem == m_sMedkitPrefabFIA)
			{
				m_bIsMedic = true;
				return true;
			}		
		}
		
		m_bIsMedic = false;
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CheckIfUnarmed(IEntity playerEntity)
	{
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(playerEntity.FindComponent(SCR_CharacterControllerComponent));
		if (!characterController)
		{
			return false;
		}
		
		BaseWeaponManagerComponent weaponComponent = characterController.GetWeaponManagerComponent(); //BaseWeaponManagerComponent.Cast(characterController.GetWeaponManagerComponent());
		if (!weaponComponent)
		{
			return false;
		}

		BaseWeaponComponent currentWeapon = weaponComponent.GetCurrentWeapon(); //IEntity currentWeapon = weaponComponent.GetCurrentWeapon().GetOwner();
		
		if (currentWeapon == null || currentWeapon.GetWeaponType() == EWeaponType.WT_NONE || currentWeapon.GetWeaponType() == EWeaponType.WT_SMOKEGRENADE)
		{		
			return true;		
		}
		else
		{
			return false;
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	void SetUnconTimestamp() //(WorldTimestamp time)
	{
		ChimeraWorld world = GetGame().GetWorld();
		m_UnconTimestamp = world.GetTimestamp(); // GetServerTimestamp().PlusSeconds(10);
	}	
	
	//-----------------------------------------------------------------------------------------------------------
	void SetWasArmed(bool armed)
	{
//		PrintFormat("SetWasArmed: Armed = %1", armed);
		m_bWasArmed = armed;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool GetIsNearBase(float distanceFromBase)
	{
		IEntity character = GetCharacter();
		SCR_GameModeCampaign m_Campaign = SCR_GameModeCampaign.GetInstance();
		if (!m_Campaign || !character)
		{
			return false;
		}
		
		SCR_CampaignMilitaryBaseComponent base = m_Campaign.GetBaseManager().FindClosestBase(character.GetOrigin());	
		vector baseOrigin = base.GetOwner().GetOrigin();
		Faction baseFaction = base.GetFaction();
		Faction playerFaction = m_cPlayerCharacter.GetFaction();
		SCR_CampaignFaction baseCampaignFaction = SCR_CampaignFaction.Cast(baseFaction);
		
		float distance = vector.DistanceXZ(character.GetOrigin(), baseOrigin);
		
		if (distance <= distanceFromBase)
		{
			return true;
		}
		
		else
		{
			return false;
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	void UpdateDroppedWeapon()
	{
		// Used for RPL PROP
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override event protected void OnWeaponDropped(IEntity pWeaponEntity, WeaponSlotComponent pWeaponSlot)
	{
		m_DroppedWeaponId = pWeaponEntity.GetID();
		Replication.BumpMe();
        UpdateDroppedWeapon();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	IEntity GetDroppedWeapon()
	{
		IEntity weapon = GetGame().GetWorld().FindEntityByID(m_DroppedWeaponId);
		return weapon;
	}
}