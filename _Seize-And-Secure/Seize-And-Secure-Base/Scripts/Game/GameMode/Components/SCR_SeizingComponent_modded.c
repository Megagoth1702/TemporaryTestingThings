//modded class SCR_SeizingComponent : SCR_MilitaryBaseLogicComponent
//{
//	SCR_GameModeCampaign CampaignInfo;
//	string m_sAADDefendingFaction;
//	
//	//------------------------------------------------------------------------------------------------
//	override protected void OnQueryFinished(BaseGameTriggerEntity trigger)
//	{
//		m_bQueryFinished = true;
//
//		array<IEntity> presentEntities = {};
//		int presentEntitiesCnt = m_Trigger.GetEntitiesInside(presentEntities);
//		m_bCharacterPresent = presentEntitiesCnt != 0;
//
//		// Nobody is here, no need to evaluate
//		if (!m_bCharacterPresent)
//		{
//			if (m_PrevailingFaction)
//			{
//				m_PrevailingFactionPrevious = m_PrevailingFaction;
//				m_PrevailingFaction = null;
//				OnPrevailingFactionChanged();
//			}
//
//			return;
//		}
//
//		map<SCR_Faction, int> factionsPresence = new map<SCR_Faction, int>();
//		SCR_Faction evaluatedEntityFaction;
//
//		int factionCnt;
//
//		// Go through all entities and check their factions
//		for (int i = 0; i < presentEntitiesCnt; i++)
//		{
//			IEntity entity = presentEntities[i];
//			
//			if (m_bDeleteDisabledAIs && IsDisabledAI(entity))
//			{
//				RplComponent.DeleteRplEntity(entity, false);	
//				continue;
//			}				
//			
//			evaluatedEntityFaction = EvaluateEntityFaction(presentEntities[i]);
//
//			if (!evaluatedEntityFaction)
//				continue;
//
//			factionCnt = factionsPresence.Get(evaluatedEntityFaction);
//
//			// If faction is not yet registered, do it now - otherwise just increase its presence counter
//			if (factionCnt == 0)
//				factionsPresence.Insert(evaluatedEntityFaction, 1);
//			else
//				factionsPresence.Set(evaluatedEntityFaction, factionCnt + 1);
//		}
//		m_bDeleteDisabledAIs = false;
//		SCR_Faction prevailingFaction;
//		int highestAttackingPresence;
//		int highestDefendingPresence;
//		int curSeizingCharacters;
//		int presence;
//		string prevailingFactionKey;
//		// Evaluate the highest attacking presence
//		for (int i = 0, cnt = factionsPresence.Count(); i < cnt; i++)
//		{
//			// Non-playable attackers are not allowed
//			if (m_bIgnoreNonPlayableAttackers && !factionsPresence.GetKey(i).IsPlayable())
//				continue;
//
//			presence = factionsPresence.GetElement(i);
//
//			if (presence > highestAttackingPresence)
//			{
//				highestAttackingPresence = presence;
//				prevailingFaction = factionsPresence.GetKey(i);
//			}
//			else if (presence == highestAttackingPresence)	// When 2 or more factions have the same presence, none should prevail
//			{
//				prevailingFaction = null;
//			}
//		}
//
//		// Evaluate the highest defending presence
//		if (prevailingFaction)
//		{
//			for (int i = 0, cnt = factionsPresence.Count(); i < cnt; i++)
//			{
//				// Non-playable defenders are not allowed
//				if (m_bIgnoreNonPlayableDefenders && !factionsPresence.GetKey(i).IsPlayable())
//					continue;
//
//				// This faction is already considered attacking
//				if (factionsPresence.GetKey(i) == prevailingFaction)
//					continue;
//
//				highestDefendingPresence = Math.Max(factionsPresence.GetElement(i), highestDefendingPresence);
//			}
//
//			// Get net amount of players effectively seizing (clamp for max attackers attribute)
//			if (prevailingFaction && highestAttackingPresence > highestDefendingPresence)
//			{
//				prevailingFactionKey = prevailingFaction.GetFactionKey();
//				curSeizingCharacters = Math.Min(highestAttackingPresence - highestDefendingPresence, m_iMaximumSeizingCharacters);
//			}
//			else
//			{
//				prevailingFaction = null;
//				curSeizingCharacters = 0;
//			}
//		}
//
//		
////		if (prevailingFaction != m_PrevailingFaction)
////		if (prevailingFaction != m_PrevailingFaction && prevailingFactionKey != m_sAADDefendingFaction)
//			
//		if (prevailingFaction != m_PrevailingFaction)
//		{	
//			CampaignInfo = SCR_GameModeCampaign.GetInstance();
//			
//			// OnPostInit may not be working to set proper attacker/defender
//			m_sAADDefendingFaction = CampaignInfo.GetAADDefendingFaction();
//			
//			if (!CampaignInfo.IsAADEnabled() || CampaignInfo.IsAADEnabled() && prevailingFactionKey != m_sAADDefendingFaction)
//			{
//				m_iSeizingCharacters = curSeizingCharacters;
//				m_PrevailingFactionPrevious = m_PrevailingFaction;
//				m_PrevailingFaction = prevailingFaction;
//				OnPrevailingFactionChanged();
//			}
//		}
//		else if (prevailingFaction && curSeizingCharacters != m_iSeizingCharacters)
//		{
//			m_iSeizingCharacters = curSeizingCharacters;
//			RefreshSeizingTimer();
//		}
//	}
//}
