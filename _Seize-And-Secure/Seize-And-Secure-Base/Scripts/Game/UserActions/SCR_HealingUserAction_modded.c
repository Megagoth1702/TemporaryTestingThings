modded class SCR_HealingUserAction : ScriptedUserAction
{
	[Attribute("0", UIWidgets.ComboBox, "Which hitzone group will be checked for conditions", enums:ParamEnumArray.FromEnum(ECharacterHitZoneGroup) )]
	protected ECharacterHitZoneGroup m_eHitZoneGroup;	
	
	[Attribute("0", UIWidgets.ComboBox, "Which consumabletype will be checked for conditions", enums:ParamEnumArray.FromEnum(SCR_EConsumableType) )]
	protected SCR_EConsumableType m_eConsumableType;
	
    [Attribute("#AR-FailReason_AlreadyApplied", UIWidgets.EditBox, "String for already applied healing gadgets")]
	protected LocalizedString m_sAlreadyApplied;    
	
	[Attribute("#AR-FailReason_NotBleeding", UIWidgets.EditBox, "String for when target hitzone isn't bleeding")]
	protected LocalizedString m_sNotBleeding;	
	
	[Attribute("#AR-FailReason_NoBloodLoss", UIWidgets.EditBox, "String for when blood hitzone isn't damaged")]
	protected LocalizedString m_sNoBloodLoss;	
	
	[Attribute("#AR-FailReason_NotDamaged", UIWidgets.EditBox, "String for when target hitzone isn't damaged")]
	protected LocalizedString m_sNotDamaged;
	
	[Attribute(defvalue: "{AE578EEA4244D41F}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_US.et", desc: "Resource name to determine if player is considered a medic for US")]
	ResourceName m_sMedkitPrefabUS;
	
	[Attribute(defvalue: "{21EF98BFC1EB3793}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_USSR.et", desc: "Resource name to determine if player is considered a medic for USSR")]
	ResourceName m_sMedkitPrefabUSSR;
	
	[Attribute(defvalue: "{9132672C4D9B2102}Prefabs/Items/Equipment/Kits/MedicalKit_01/MedicalKit_01_FIA.et", desc: "Resource name to determine if player is considered a medic for FIA")]
	ResourceName m_sMedkitPrefabFIA;
	
	protected bool m_bIsMedic;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Target character
		ChimeraCharacter targetCharacter = ChimeraCharacter.Cast(GetOwner());
		if (!targetCharacter)
			return;
		
		// Medic character
		ChimeraCharacter userCharacter = ChimeraCharacter.Cast(pUserEntity);
		if (!userCharacter)
			return;
		
		SCR_CharacterControllerComponent userController = SCR_CharacterControllerComponent.Cast(userCharacter.GetCharacterController());
		if (!userController)
			return;
		
		IEntity item = userController.GetAttachedGadgetAtLeftHandSlot();
		if (!item)
			return;
		
		SCR_ConsumableItemComponent consumableComponent = GetConsumableComponent(userCharacter);
		if (!consumableComponent)
			return;
		
		consumableComponent.SetTargetCharacter(targetCharacter);
		SCR_ConsumableEffectHealthItems consumableEffect = SCR_ConsumableEffectHealthItems.Cast(consumableComponent.GetConsumableEffect());
		if (!consumableEffect)
			return;
			
		TAnimGraphCommand desiredCmd;
		if (pOwnerEntity == pUserEntity)
			desiredCmd = consumableEffect.GetApplyToSelfAnimCmnd(pOwnerEntity);
		else
			desiredCmd = consumableEffect.GetApplyToOtherAnimCmnd(pOwnerEntity);

		SCR_CharacterControllerComponent targetController = SCR_CharacterControllerComponent.Cast(targetCharacter.GetCharacterController());
		if (targetController && targetController.IsUnconscious())
			desiredCmd = consumableEffect.GetReviveAnimCmnd(pOwnerEntity);
	
		SCR_CharacterDamageManagerComponent targetDamageMan = SCR_CharacterDamageManagerComponent.Cast(targetCharacter.GetDamageManager());
		if (!targetDamageMan)
			return;
		
		// Make Medics Great Again!
		float duration;
		
		if (IsMedic(pUserEntity))
		{
			duration = consumableEffect.GetApplyToOtherDuraction() * 0.5;
		}
		
		else
		{
			duration = consumableEffect.GetApplyToOtherDuraction();
		}
		
		ItemUseParameters params = ItemUseParameters();
		params.SetEntity(item);
		params.SetAllowMovementDuringAction(false);
		params.SetKeepInHandAfterSuccess(true);
		params.SetCommandID(desiredCmd);
		params.SetCommandIntArg(1);
		params.SetCommandFloatArg(0.0);
		params.SetMaxAnimLength(duration);
		params.SetIntParam(targetDamageMan.FindAssociatedBandagingBodyPart(m_eHitZoneGroup));
		consumableEffect.ActivateEffect(pOwnerEntity, pUserEntity, item, params);
		
		// Give XP for doing medic stuff on others or something
		if (pOwnerEntity != pUserEntity)
		{
			GiveXP(pUserEntity);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	//! \If inventory contains medkit, then true
	bool IsMedic(IEntity user)
	{
		/// First check Loadout Type ///
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		if (!playerId)
			return false;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (playerController)
		{
			SCR_PlayerLoadoutComponent m_PlyLoadoutComp = SCR_PlayerLoadoutComponent.Cast(playerController.FindComponent(SCR_PlayerLoadoutComponent));
			SCR_BasePlayerLoadout loadout = m_PlyLoadoutComp.GetLoadout();
			SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
			LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
			
			if (loadoutType == LoadoutType.MEDIC)
				return true;
		
		}
		///////////////////////////////
		
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
		InventoryStorageManagerComponent InventoryManager = characterController.GetInventoryStorageManager();
		
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
	bool GetIsMedic()
	{
		return m_bIsMedic;
	}
	
	//------------------------------------------------------------------------------------------------
	void GiveXP(IEntity player, float multiplier = 0.5)
	{
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(characterController.GetOwner());
		SCR_XPHandlerComponent compXP = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
		
		if (compXP)
			compXP.AwardXP(playerId, SCR_EXPRewards.SUPPORT_EVAC, multiplier);	
	}
};
