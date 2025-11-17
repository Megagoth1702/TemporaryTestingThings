[BaseContainerProps()]
modded class SCR_ConsumableEffectHealthItems : SCR_ConsumableEffectBase
{
	[Attribute("0", UIWidgets.EditBox, "Regeneration speed of related hitZone when consuming this item", category: "Regeneration")]
	protected float m_fItemRegenerationSpeed;
	
	[Attribute("0", UIWidgets.EditBox, "Regeneration duration of related hitZone when consuming this item in seconds", category: "Regeneration")]
	protected float m_fItemRegenerationDuration;	
	
	[Attribute("0", UIWidgets.EditBox, "Total amount of regeneration that will be applied to the related hitZone. Will be ignored if m_fItemRegenerationDuration > 0", category: "Regeneration")]
	protected float m_fItemAbsoluteRegenerationAmount;
	
	[Attribute(desc: "DamageEffects to add when the effect is applied")]
	protected ref array<ref SCR_DamageEffect> m_aDamageEffectsToLoad;
	
	//Cached to remember which bodypart to remove effect from
	protected ECharacterHitZoneGroup m_eTargetHZGroup;
	
	[Attribute(defvalue: "{AE578EEA4244D41F}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_US.et", desc: "Resource name of the Medical Kit for US")]
	ResourceName m_sMedkitPrefabUS;
	
	[Attribute(defvalue: "{21EF98BFC1EB3793}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_USSR.et", desc: "Resource name of the Medical Kit for USSR")]
	ResourceName m_sMedkitPrefabUSSR;
	
	[Attribute(defvalue: "{9132672C4D9B2102}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_FIA.et", desc: "Resource name of the Medical Kit for FIA")]
	ResourceName m_sMedkitPrefabFIA;
	
	protected bool m_bIsMedic;
	
	//------------------------------------------------------------------------------------------------	
	override bool ActivateEffect(IEntity target, IEntity user, IEntity item, ItemUseParameters animParams = null)
	{
		ItemUseParameters localAnimParams = animParams;
		if (!localAnimParams)
		{
			//user-held healthitem needs to get data from target to perform anim
			localAnimParams = GetAnimationParameters(item, target);
		}
		
		if (!localAnimParams)
			return false;
		
		if (!super.ActivateEffect(target, user, item, localAnimParams))
			return false;		

		ChimeraCharacter character = ChimeraCharacter.Cast(target);
		if (!character)
			return false;
		
		CharacterControllerComponent controller = character.GetCharacterController();
		if (!controller)
			return false;
			
		if (localAnimParams.GetIntParam() == EBandagingAnimationBodyParts.LeftLeg || localAnimParams.GetIntParam() == EBandagingAnimationBodyParts.RightLeg)
		{
			if (controller.GetStance() == ECharacterStance.STAND)
				controller.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
		}
		
//		SCR_ConsumableEffectBase medicalItem = SCR_ConsumableEffectBase.Cast(item.FindComponent(SCR_ConsumableEffectBase));
//		if (medicalItem)
//		{
//			if (IsMedic(user))
//			{
//				medicalItem.SetApplyToSelfDuraction(0.5);
////				medicalItem.SetApplyToOtherDuraction(0.5);
//				medicalItem.m_fApplyToOtherDuration = 3;
//			}
//			else
//			{
//				medicalItem.SetApplyToSelfDuraction(1);
//				medicalItem.SetApplyToOtherDuraction(1);
//			}
//		}
//
//		Print("Is Medic: " + IsMedic(user));
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		InventoryItemComponent itemComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
 	 	if (itemComp)
 	 		itemComp.RequestUserLock(user, false);
		
		ChimeraCharacter char = ChimeraCharacter.Cast(target);
		if (!char)
			return;
		
		
		AddConsumableDamageEffects(char, user);
	}
	
	//------------------------------------------------------------------------------------------------
	override void AddConsumableDamageEffects(notnull ChimeraCharacter char, IEntity instigator)
	{
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		SCR_DotDamageEffect dotClone;
		
		array<HitZone> hitZones = {};
		damageMgr.GetHitZonesOfGroup(GetTargetHitZoneGroup(), hitZones);
		if (hitZones.IsEmpty())
			hitZones.Insert(damageMgr.GetDefaultHitZone());
		
		foreach (SCR_DamageEffect effect : m_aDamageEffectsToLoad)
		{
			effect.SetInstigator(Instigator.CreateInstigator(instigator));
			effect.SetDamageType(effect.GetDefaultDamageType());
			effect.SetAffectedHitZone(hitZones[0]);
			
			dotClone = SCR_DotDamageEffect.Cast(effect);
			if (dotClone)
			{
				dotClone.SetDPS(-GetItemRegenSpeed());
				dotClone.SetMaxDuration(m_fItemRegenerationDuration);
			}
			
			damageMgr.AddDamageEffect(effect);
		}	
	};
	
	//------------------------------------------------------------------------------------------------
	//! \If inventory contains medkit, then true
	bool IsMedic(IEntity user)
	{
		/// First check Loadout Type ///
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return false;
		
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
	
	bool GetIsMedic()
	{
		Print("Getting Is Medic: " + m_bIsMedic);
		return m_bIsMedic;
	}
}
