//! Bandage effect
[BaseContainerProps()]
modded class SCR_ConsumableBandage : SCR_ConsumableEffectHealthItems
{
	
	
	//------------------------------------------------------------------------------------------------
	//! Optional param for specific hitzone, still will ensure external users don't have to create their own local animParams
	override ItemUseParameters GetAnimationParameters(IEntity item, notnull IEntity target, ECharacterHitZoneGroup group = ECharacterHitZoneGroup.VIRTUAL)
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(target);
		if (!char)
			return null;
		
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageMgr)
			return null;

		EBandagingAnimationBodyParts bodyPartToBandage = EBandagingAnimationBodyParts.Invalid;
		
		if (group != ECharacterHitZoneGroup.VIRTUAL)
		{
			if (!damageMgr.GetGroupIsBeingHealed(group))
				bodyPartToBandage = damageMgr.FindAssociatedBandagingBodyPart(group);
		}
		else
		{
			group = damageMgr.GetCharMostDOTHitzoneGroup(EDamageType.BLEEDING, ignoreIfBeingTreated: true);
			array<HitZone> hitzones = {};
			damageMgr.GetHitZonesOfGroup(group, hitzones);
			if (!hitzones || hitzones.IsEmpty())
				return null;
				
			SCR_CharacterHitZone hitzone = SCR_CharacterHitZone.Cast(hitzones[0]);
			if (!hitzone)
				return null;
	
			bodyPartToBandage = hitzone.GetBodyPartToHeal();
		}
		
		if (bodyPartToBandage == EBandagingAnimationBodyParts.Invalid)
			return null;
		
		bool allowMovement = bodyPartToBandage != EBandagingAnimationBodyParts.RightLeg && bodyPartToBandage != EBandagingAnimationBodyParts.LeftLeg;
		
//		SCR_HealingUserAction action = SCR_HealingUserAction.Cast(item.FindComponent(SCR_HealingUserAction));
//		bool isMedic = action.GetIsMedic();
//		SCR_ConsumableItemComponent consumableItemComp = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
//		SCR_ConsumableEffectBase consumableItem = consumableItemComp.GetConsumableEffect();
		
		float duration;
		
		if (IsMedic())
		{
			duration = m_fApplyToSelfDuration * 0.5;
		}
		
		else
		{
			duration = m_fApplyToSelfDuration;
		}
		
		ItemUseParameters params = ItemUseParameters();
		params.SetEntity(item);
		params.SetAllowMovementDuringAction(allowMovement);
		params.SetKeepInHandAfterSuccess(false);
		params.SetCommandID(GetApplyToSelfAnimCmnd(target));
		params.SetCommandIntArg(1);
		params.SetCommandFloatArg(0.0);
		params.SetMaxAnimLength(duration); // m_fApplyToSelfDuration
		params.SetIntParam(bodyPartToBandage);
			
		return params;
	}
	
	//------------------------------------------------------------------------------------------------
	//! \If inventory contains medkit, then true
	bool IsMedic()
	{
		/// First check Loadout Type ///
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return false;
		
		SCR_PlayerLoadoutComponent m_PlyLoadoutComp = SCR_PlayerLoadoutComponent.Cast(pc.FindComponent(SCR_PlayerLoadoutComponent));
		if (!m_PlyLoadoutComp)
			return false;
		
		SCR_BasePlayerLoadout loadout = m_PlyLoadoutComp.GetLoadout();
		if (!loadout)
			return false;
		
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		if (!factionLoadout)
			return false;
		
		LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
		
		if (loadoutType == LoadoutType.MEDIC)
			return true;
		
		///////////////////////////////
		
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		if (!player)
			return false;
		
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
		if (!characterController)
			return false;
		
		InventoryStorageManagerComponent InventoryManager = characterController.GetInventoryStorageManager();
		if (!InventoryManager)
			return false;
		
		SCR_EntityCatalogManagerComponent entityCatalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		if (!entityCatalogManager)
			return false;
		
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
}
