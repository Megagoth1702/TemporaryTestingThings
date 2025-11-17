modded class SCR_InspectCasualtyWidget : SCR_InfoDisplayExtended
{
	protected ref array<ref string> m_aBleedingAreas = {};
	protected bool m_bBloodLoss;
	protected string m_sBloodLossName = "#InspectCasualty_Blood";
	
	//------------------------------------------------------------------------------------------------
	//! Gather and update data of target character into widget
	override void UpdateWidgetData()
	{
		if (!m_Target || !m_wCasualtyInspectWidget)
			return;
		
		string sName;
		GetCasualtyName(sName, m_Target);
	
		if (GetGame().GetPlatformService().GetLocalPlatformKind() == PlatformKind.PSN)
		{
			PlayerManager playerMgr = GetGame().GetPlayerManager();
			
			if (playerMgr && playerMgr.GetPlatformKind(playerMgr.GetPlayerIdFromControlledEntity(m_Target)) == PlatformKind.PSN)
				sName = string.Format("<color rgba=%1><image set='%2' name='%3' scale='%4'/></color>", UIColors.FormatColor(GUIColors.ENABLED), UIConstants.ICONS_IMAGE_SET, UIConstants.PLATFROM_PLAYSTATION_ICON_NAME, 2) + sName;
			else
				sName = string.Format("<color rgba=%1><image set='%2' name='%3' scale='%4'/></color>", UIColors.FormatColor(GUIColors.ENABLED), UIConstants.ICONS_IMAGE_SET, UIConstants.PLATFROM_GENERIC_ICON_NAME, 2) + sName;
		}

		float bleedingRate;
		int groupDamageIntensity;
		bool regenerating, isTourniquetted, isSalineBagged, isMorphined;
		string damageIntensity, damageIntensityText, bleedingIntensityText;
		array <bool> hZGroupsBleeding = {};
		
		SCR_InventoryHitZonePointUI hitZonePointUI = new SCR_InventoryHitZonePointUI();
		
		m_aBleedingAreas.Clear();
		GetDamageInfo(hitZonePointUI, m_Target, bleedingRate, hZGroupsBleeding, groupDamageIntensity, regenerating, isTourniquetted, isSalineBagged, isMorphined);
		hitZonePointUI.GetDamageInfoTexts(EHitZoneGroup.VIRTUAL, groupDamageIntensity, bleedingRate, damageIntensity, damageIntensityText, bleedingIntensityText);
		
		if (m_aBleedingAreas.Count() > 0)
		{
			foreach (string name : m_aBleedingAreas)
			{
				bleedingIntensityText = bleedingIntensityText + " | " + name;
			}
		}
		
		if (m_bBloodLoss)
		{
			damageIntensityText = damageIntensityText + " (" + m_sBloodLossName + ")";
		}
		
		SCR_InventoryDamageInfoUI damageInfoUI = SCR_InventoryDamageInfoUI.Cast(m_wCasualtyInspectWidget.FindHandler(SCR_InventoryDamageInfoUI));
		if (damageInfoUI)
		{
			damageInfoUI.SetName(sName);
			damageInfoUI.SetDamageStateVisible(groupDamageIntensity, regenerating, damageIntensity, damageIntensityText);
			damageInfoUI.SetBleedingStateVisible(bleedingRate > 0, bleedingIntensityText);
			damageInfoUI.SetTourniquetStateVisible(isTourniquetted);
			damageInfoUI.SetSalineBagStateVisible(isSalineBagged);
			damageInfoUI.SetMorphineStateVisible(isMorphined);
			damageInfoUI.SetFractureStateVisible(0, 0);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void GetDamageInfo(SCR_InventoryHitZonePointUI hitZonePointUI, IEntity targetEntity, inout float bleedingRate, inout array <bool> hZGroupsBleeding, inout int damageIntensity, inout bool regenerating, inout bool isTourniquetted, inout bool isSalineBagged, inout bool isMorphined)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(targetEntity);
		if (!character)
			return;
		
		SCR_CharacterDamageManagerComponent damageMan = SCR_CharacterDamageManagerComponent.Cast( character.GetDamageManager() );
		if (!damageMan)
			return;		

		float defaultHZHealth = damageMan.GetHealthScaled();
		bleedingRate = damageMan.GetBloodHitZone().GetTotalBleedingAmount();
		
		SCR_CharacterBloodHitZone bloodHitzone = damageMan.GetBloodHitZone();
		float bloodHealth = bloodHitzone.GetHealthScaled();
		if (bloodHealth <= 0.35)
		{
			m_bBloodLoss = true;
			
			if (!m_aBleedingAreas.Contains(m_sBloodLossName))
			{
				m_aBleedingAreas.Insert(m_sBloodLossName);
			}		
		}
		else
		{
			m_bBloodLoss = false;
		}
		
		if (bleedingRate)
		{
			hZGroupsBleeding = {};
			hZGroupsBleeding.Resize(damageMan.LIMB_GROUPS.Count());

//			m_aBleedingAreas.Clear();
			
			foreach (ECharacterHitZoneGroup group : damageMan.LIMB_GROUPS)
			{
				hZGroupsBleeding[damageMan.LIMB_GROUPS.Find(group)] = damageMan.GetGroupDamageOverTime(group, EDamageType.BLEEDING) != 0;	
				
				if (damageMan.GetGroupDamageOverTime(group, EDamageType.BLEEDING) != 0)
				{
					string name = GetBleedingAreaName(group);
					if (name != "Virtual" && !m_aBleedingAreas.Contains(name))
					{
						m_aBleedingAreas.Insert(name);
					}
				}
			}
		}
		
		array<EHitZoneGroup> limbGroups = {};
		damageMan.GetAllLimbs(limbGroups);
		foreach (EHitZoneGroup group : limbGroups)
		{
			if (!isTourniquetted)
				isTourniquetted = damageMan.GetGroupTourniquetted(group);

			if (!isSalineBagged)
				isSalineBagged = damageMan.GetGroupSalineBagged(group);

			if (!isMorphined)
			{
				array<ref SCR_PersistentDamageEffect> effects = damageMan.GetAllPersistentEffectsOfType(SCR_MorphineDamageEffect);
				isMorphined = !effects.IsEmpty();
			}

			if (!regenerating)
				regenerating = damageMan.GetGroupDamageOverTime(group, EDamageType.HEALING) != 0 || damageMan.GetGroupDamageOverTime(group, EDamageType.REGENERATION) != 0;
		}

		if (defaultHZHealth == 1)
			damageIntensity = 0;
		else if (defaultHZHealth < hitZonePointUI.m_fLowDamageThreshold)
			damageIntensity = 4;
		else if (defaultHZHealth < hitZonePointUI.m_fMediumDamageThreshold)
			damageIntensity = 3;
		else if (defaultHZHealth < hitZonePointUI.m_fHighDamageThreshold)
			damageIntensity = 2;
		else
			damageIntensity = 1;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetBleedingAreaName(ECharacterHitZoneGroup group)
	{
		if (group == ECharacterHitZoneGroup.HEAD)
			return "#InspectCasualty_Head";
		if (group == ECharacterHitZoneGroup.LEFTLEG)
			return "#InspectCasualty_LeftLeg";
		if (group == ECharacterHitZoneGroup.LEFTARM)
			return "#InspectCasualty_LeftArm";
		if (group == ECharacterHitZoneGroup.RIGHTARM)
			return "#InspectCasualty_RightArm";
		if (group == ECharacterHitZoneGroup.RIGHTLEG)
			return "#InspectCasualty_RightLeg";
		if (group == ECharacterHitZoneGroup.LOWERTORSO)
			return "#InspectCasualty_Stomach";
		if (group == ECharacterHitZoneGroup.UPPERTORSO)
			return "#InspectCasualty_Chest";
		
		return "Virtual";
	}
};
