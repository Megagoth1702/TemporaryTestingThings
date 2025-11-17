modded class SCR_CharacterFactionAffiliationComponent : FactionAffiliationComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPerceivedFactionChanged()
	{
		//~ Faction is unknown
		if (m_iOutfitFactionIndex < 0)
		{
			if (m_PerceivedManager)
			{
				//~ Set faction to default depending on setting
				if (m_PerceivedManager.DoesNoOutfitSetFactionToDefault() && m_PerceivedManager.GetCharacterPerceivedFactionOutfitType() != SCR_EPerceivedFactionOutfitType.FULL_OUTFIT)
					m_PerceivedFaction = GetDefaultAffiliatedFaction();
				else 
					m_PerceivedFaction = null;
			}
			else 
			{
				m_PerceivedFaction = null;
			}
		}
		//~ Set Faction
		else 
		{
			m_PerceivedFaction = GetGame().GetFactionManager().GetFactionByIndex(m_iOutfitFactionIndex);
		}

		if (m_OnPerceivedFactionChanged)
				m_OnPerceivedFactionChanged.Invoke(m_PerceivedFaction);
				
		
		Faction affiliatedFaction = GetAffiliatedFaction();
		
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		FactionKey AADDefendingFactionKey = CampaignInfo.GetAADDefendingFaction();
		Faction defendingFaction = GetGame().GetFactionManager().GetFactionByKey(AADDefendingFactionKey);
		Faction civFaction = GetGame().GetFactionManager().GetFactionByKey("CIV");
		
		//~ Local player changed perceived faction
		if (m_PerceivedManager && SCR_PlayerController.GetLocalMainEntity() == GetOwner() && m_PerceivedFaction != affiliatedFaction)
			m_PerceivedManager.ShowPerceivedFactionChangedHint(m_PerceivedFaction);
		
		//~ Set Disguise type which is the relationship between the affiliated faction and perceived faction
		if (m_PerceivedFaction)
		{
			if (!affiliatedFaction || affiliatedFaction == m_PerceivedFaction || CampaignInfo.IsAADEnabled() && m_PerceivedFaction == civFaction && affiliatedFaction == defendingFaction)
				m_eDisguiseType = SCR_ECharacterDisguiseType.DEFAULT_FACTION;
			else 
			{
				SCR_Faction scrPerceivedFaction = SCR_Faction.Cast(m_PerceivedFaction);
				
				//~ Is SCR_Faction
				if (scrPerceivedFaction)
				{
					//~ Check if friendly
					if (scrPerceivedFaction.DoCheckIfFactionFriendly(affiliatedFaction))
						m_eDisguiseType = SCR_ECharacterDisguiseType.FRIENDLY_FACTION;
					else 
						m_eDisguiseType = SCR_ECharacterDisguiseType.HOSTILE_FACTION;
				}
				//~ Not an SCR_Faction
				else 
				{
					//~ Check if friendly
					if (m_PerceivedFaction.IsFactionFriendly(affiliatedFaction))
						m_eDisguiseType = SCR_ECharacterDisguiseType.FRIENDLY_FACTION;
					else 
						m_eDisguiseType = SCR_ECharacterDisguiseType.HOSTILE_FACTION;
				}
			}
		}
		//~ Has no perceived faction so disguise is UNKNOWN type
		else 
		{
			m_eDisguiseType = SCR_ECharacterDisguiseType.UNKNOWN_FACTION;
		}
	}
}