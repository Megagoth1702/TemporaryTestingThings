class Iron_HealYourselfAmbulanceUserAction : SCR_BaseDamageHealSupportStationAction //ScriptedUserAction
{
	[Attribute("0", desc: "If this heal action will also heal the blood hitzone")]
	protected bool m_bHealBloodHitzone;
	
	[Attribute(ECharacterHitZoneGroup.VIRTUAL.ToString(), UIWidgets.ComboBox, "Which hitzone group will be healed, VIRTUAL means all hitzones are healed", enums:ParamEnumArray.FromEnum(ECharacterHitZoneGroup) )]
	protected ECharacterHitZoneGroup m_eHitZoneGroup;	
	
	protected HitZone m_BloodHitZone;
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		m_DamageManagerComponent = character.GetDamageManager();
//		m_DamageManagerComponent.GetAllHitZones(m_aHitZonesToHeal);
		m_DamageManagerComponent.GetAllHitZonesInHierarchy(m_aHitZonesToHeal);
		
		if (!m_DamageManagerComponent || m_aHitZonesToHeal.IsEmpty())
		{
			return false;
		}

		foreach(HitZone hitzone : m_aHitZonesToHeal)
		{			
			//~ Has a hitzone that is damaged so show			
			if (hitzone.GetHealthScaled() < 0.70)
			{
				return true;
//				super.CanBeShownScript(user);
			}	
		}
		
		// not damaged
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		if (!m_World)
		{
			Print("CanBePerformedScript: No world for some reason?...");
			m_World = GetGame().GetWorld();
			return false;
		}
		
		ChimeraCharacter character = ChimeraCharacter.Cast(user);
//		if (!character)
//			return false;
//		
		m_DamageManagerComponent = character.GetDamageManager();
//		if (!m_DamageManagerComponent)
//			return false;
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(m_DamageManagerComponent);
		if (!charDamageManager)
			return super.CanBePerformedScript(user);
		
		m_BloodHitZone = charDamageManager.GetBloodHitZone();
		
		//~ Can heal blood hitzone but somewhere the character is bleeding. Note: m_BloodHitZone will be null if m_bHealBloodHitzone is false
		if (m_BloodHitZone && charDamageManager.IsBleeding())
		{
			SetCanPerform(false, ESupportStationReasonInvalid.HEAL_CHARACTER_IS_BLEEDING);
			return false;
		}
		
		//~ Check if the character is bleeding at the hitzones this action heals
		foreach(HitZone hitZone : m_aHitZonesToHeal)
		{
			SCR_RegeneratingHitZone regenHitZone = SCR_RegeneratingHitZone.Cast(hitZone);
			if (!regenHitZone)
				continue;
			
			if (regenHitZone.GetHitZoneDamageOverTime(EDamageType.BLEEDING) > 0)
			{
				SetCanPerform(false, ESupportStationReasonInvalid.HEAL_CHARACTER_IS_BLEEDING);
				return false;
			}
		}
		
		return super.CanBePerformedScript(user);
 	}
	
 	//------------------------------------------------------------------------------------------------
 	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
 	{
		//~ Action can be performed server only
		if (!m_bIsMaster)
			return;
		
		//~ Resets all references so server grabs them all fresh
		ResetReferencesOnServer();
		
		if (!CanBeShownScript(pUserEntity))
			return;
		
		if (!CanBePerformedScript(pUserEntity))
			return;
		
		//Execute Supply station
		m_SupportStationComponent.OnExecutedServer(pOwnerEntity, pUserEntity, this);
		
		//~ Update supplies
		m_iAvailableSupplies = m_SupportStationComponent.GetMaxAvailableSupplies();
		
		foreach(HitZone hitZone : m_aHitZonesToHeal)
		{
			if (hitZone.GetHealthScaled() < 0.70)
				hitZone.SetHealthScaled(0.70);
		}
 	}
	
	//------------------------------------------------------------------------------------------------
	override protected ESupportStationType GetSupportStationType()
	{
		return ESupportStationType.HEAL;
	}
	
	//------------------------------------------------------------------------------------------------
	ECharacterHitZoneGroup GetHitZoneGroup()
	{
		return m_eHitZoneGroup;
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void SetHitZonesToHeal()
	{
//		m_aHitZonesToHeal.Clear();
		
//		int playerId = GetGame().GetPlayerController().GetPlayerId();
//		if (!playerId)
//			return;
//		
//		IEntity user = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
//		if (!user)
//			return;
		
//		ChimeraCharacter character = ChimeraCharacter.Cast(m_ActionUser);
//		if (!character)
//			return;
//		
//		m_DamageManagerComponent = character.GetDamageManager();
		if (!m_DamageManagerComponent)
			return;
		
		//~ If it should heal the blood hitzone then set reference to it
		if (m_bHealBloodHitzone)
		{
			SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(m_DamageManagerComponent);
			
			if (charDamageManager)
				m_BloodHitZone = charDamageManager.GetBloodHitZone();
		}
		
		//~ Virtual gets all hitzones
		if (m_eHitZoneGroup == ECharacterHitZoneGroup.VIRTUAL)
		{
			super.SetHitZonesToHeal();
			return;
		}
		
		//~ Get specific hitzone group
		m_DamageManagerComponent.GetHitZonesOfGroup(m_eHitZoneGroup, m_aHitZonesToHeal);
	}
}