class Iron_HealYourselfUserAction : SCR_ScriptedUserAction //SCR_BaseDamageHealSupportStationAction //ScriptedUserAction
{
	SCR_DamageManagerComponent m_DamageManager;	
	protected HitZone m_BloodHitZone;
	protected ref array<HitZone> m_aHitZonesToHeal = {};
	protected BaseWorld m_World;
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		if (!character)
		{
			Print("Iron_HealYourselfUserAction: CanBeShownScript: No Character. Returning...");
			return false;
		}
		
		m_DamageManager = character.GetDamageManager();
		if (!m_DamageManager)
		{
			Print("Iron_HealYourselfUserAction: CanBeShownScript: Damage Manager Component. Returning...");
			return false;
		}
		
//		m_DamageManagerComponent.GetAllHitZones(m_aHitZonesToHeal);
		m_DamageManager.GetAllHitZonesInHierarchy(m_aHitZonesToHeal);
		
		if (!m_DamageManager || m_aHitZonesToHeal.IsEmpty())
		{
			return false;
		}

		foreach(HitZone hitzone : m_aHitZonesToHeal)
		{			
			//~ Has a hitzone that is damaged so show			
			if (hitzone.GetHealthScaled() != 1)
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
		m_World = GetGame().GetWorld();
		if (!m_World)
		{
			Print("Iron_HealYourselfUserAction: CanBePerformedScript: No world for some reason?...");
			m_World = GetGame().GetWorld();
			return false;
		}
		
		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		if (!character)
			return false;
	
		m_DamageManager = character.GetDamageManager();
		if (!m_DamageManager)
			return false;
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(m_DamageManager);
		if (!charDamageManager)
			return super.CanBePerformedScript(user);
		
		m_BloodHitZone = charDamageManager.GetBloodHitZone();
		
		//~ Can heal blood hitzone but somewhere the character is bleeding. Note: m_BloodHitZone will be null if m_bHealBloodHitzone is false
		if (m_BloodHitZone && charDamageManager.IsBleeding())
		{
			SetCannotPerformReason("###You're Bleeding, Bro!");
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
				SetCannotPerformReason("###You're Bleeding, Bro!");
				return false;
			}
		}
		
		return super.CanBePerformedScript(user);
 	}
	
 	//------------------------------------------------------------------------------------------------
 	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
 	{

		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		if (playerId)
		{
			HealPlayer(playerId, pUserEntity);
		}	
 	}
	
	//------------------------------------------------------------------------------------------------
	void HealPlayer(int playerId, IEntity user)
	{	
		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		if (!character)
		{
			Print("Iron_HealYourselfUserAction: HealPlayer: No Character. Returning...");
			return;
		}
		
		SCR_DamageManagerComponent damageManager = character.GetDamageManager();
		if (!damageManager)
		{
			Print("Iron_HealYourselfUserAction: HealPlayer: No Damage Manager. Returning...");
			return;
		}
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(damageManager);
		if (!charDamageManager)
		{
			Print("Iron_HealYourselfUserAction: HealPlayer: No Character Damage Manager. Returning...");
			return;
		}
		
		HitZone bloodHitZone = charDamageManager.GetBloodHitZone();
		if (bloodHitZone && bloodHitZone.GetHealthScaled() < 1)
			bloodHitZone.SetHealthScaled(1);
		
		array<HitZone> hitZonesToHeal = {};
		charDamageManager.GetAllHitZonesInHierarchy(hitZonesToHeal);
		
		foreach(HitZone hitzone : hitZonesToHeal)
		{
			hitzone.SetHealthScaled(1);
		}
	}
}