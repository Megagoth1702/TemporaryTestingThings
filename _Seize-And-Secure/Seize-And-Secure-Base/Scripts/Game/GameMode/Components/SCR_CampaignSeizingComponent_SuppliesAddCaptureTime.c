modded class SCR_CampaignSeizingComponent : SCR_SeizingComponent
{
	[Attribute("0", category: "Seize & Secure")]
	protected bool m_bRequiresCaptureAndHoldAreas;
	
	[Attribute("100", params: "0 100 1", desc: "Percentage of C&H zones owned in order to capture base", category: "Seize & Secure")]
	protected int m_iAreaThreshold;
	
	[Attribute(category: "Seize & Secure")]
	protected ref array<ref string> m_aCaptureAndHoldAreaNames;
	
	[Attribute("60", params: "0 inf 0.1", category: "Campaign")]
	protected float m_fExtraTimePerService;
	
	[Attribute("0.1", desc: "How much influence should supplies have on capture time, per supply (in seconds)", params: "0.01 inf 0.1", category: "Supply Capture Time")]
	protected float m_fSupplyMultiplier;
	
	[Attribute("600", desc: "Maximum amount of time (in seconds) supplies can influence capture time", params: "1 inf 1", category: "Supply Capture Time")]
	protected float m_iMaxCaptureTimeFromSupplies;

	[Attribute(desc: "Group Type required to start capturing", category: "Seize & Secure", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(SCR_EGroupRole))]
	protected ref array<ref SCR_EGroupRole> m_RequiredGroupTypes;
	
	[Attribute("0", category: "Seize & Secure")]
	protected bool m_bCanCaptureFromVehicle;
	
	protected SCR_CampaignMilitaryBaseComponent m_Base;
	protected SCR_GameModeCampaign m_CampaignInfo;
	protected SCR_CaptureAndHoldManager m_CAHManager;
	string m_sAADDefendingFaction;
	
	//------------------------------------------------------------------------------------------------
	ref array<ref string> GetCaptureAndHoldAreaNames()
	{
		return m_aCaptureAndHoldAreaNames;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetRequiresCaptureAndHoldAreas()
	{
		return m_bRequiresCaptureAndHoldAreas;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetRequiresCaptureAndHoldAreas(bool value)
	{
		m_bRequiresCaptureAndHoldAreas = value;
	}
	
	//------------------------------------------------------------------------------------------------
	protected override SCR_Faction EvaluateEntityFaction(IEntity ent)
	{
		if (!m_Base || m_Base.IsHQ() || !m_Base.IsInitialized())
			return null;
		
		SCR_Faction faction = super.EvaluateEntityFaction(ent);
		
		if (!faction)
			return null;
		
		// Players of faction not covering this base with radio signal should not be able to capture or prevent capture
		SCR_CampaignFaction cFaction = SCR_CampaignFaction.Cast(faction);
		
		if (!cFaction)
			return null;

		if (m_bRequiresCaptureAndHoldAreas)
		{
			m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
			if (!m_CampaignInfo)
				return null;
			
			m_CAHManager = SCR_CaptureAndHoldManager.Cast(m_CampaignInfo.FindComponent(SCR_CaptureAndHoldManager));
			if (!m_CAHManager)
				return null;
			
			bool areCHZonesCaptured;
			int areaCount = m_aCaptureAndHoldAreaNames.Count();
			float owningCount;
			array<SCR_CaptureAndHoldArea> cahAreas = {};
			m_CAHManager.GetAreas(cahAreas);
			foreach (SCR_CaptureAndHoldArea area : cahAreas)
			{
				string areaName = area.GetAssignedName(); //GetName();
				Faction ownerFaction = area.GetOwningFaction();
				
				if (m_aCaptureAndHoldAreaNames.Contains(areaName) && ownerFaction == faction)
				{
					owningCount++;
				}
			}
			
			float threshold = areaCount * (m_iAreaThreshold/100);
//			Print("Owning Count: " + owningCount + " Threshold: " + threshold);
			if (owningCount >= threshold)
			{
				areCHZonesCaptured = true;
			}
			
			else
			{
				areCHZonesCaptured = false;
				return null;
			}		
		}
		
		
		if (faction.IsPlayable() && !m_Base.IsHQRadioTrafficPossible(cFaction))
			return null;
		
		return faction;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnQueryFinished(BaseGameTriggerEntity trigger)
	{
		m_bQueryFinished = true;

		array<IEntity> presentEntities = {};
		int presentEntitiesCount = m_Trigger.GetEntitiesInside(presentEntities);
		m_bCharacterPresent = presentEntitiesCount != 0;

		// Nobody is here, no need to evaluate
		if (!m_bCharacterPresent)
		{
			if (m_PrevailingFaction)
			{
				m_PrevailingFactionPrevious = m_PrevailingFaction;
				m_PrevailingFaction = null;
				OnPrevailingFactionChanged();
			}

			return;
		}
	
		map<SCR_Faction, int> factionsPresence = new map<SCR_Faction, int>();
		map<SCR_Faction, bool> factionsPlayerPresence = new map<SCR_Faction, bool>();
		SCR_Faction evaluatedEntityFaction;
		int factionCount;
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		
		bool isGroupTypeRequired = !m_RequiredGroupTypes.IsEmpty();
		bool isCharacterInVehicle;
		
		// Go through all entities and check their factions
		for (int i = 0; i < presentEntitiesCount; i++)
		{
			IEntity entity = presentEntities[i];
				
			if (m_bDeleteDisabledAIs && IsDisabledAI(entity))
			{
				RplComponent.DeleteRplEntity(entity, false);	
				continue;
			}				
			
			evaluatedEntityFaction = EvaluateEntityFaction(presentEntities[i]);

			if (!evaluatedEntityFaction)
				continue;

			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
			if (!m_bCanCaptureFromVehicle && character && character.IsInVehicle())
			{
				continue;
			}				
		
			factionCount = factionsPresence.Get(evaluatedEntityFaction);

			// Go through all entities and check if group type is present							
			if (isGroupTypeRequired)
			{
				bool isRequiredGroupTypePresent = GetIsRequiredGroupTypePresent(presentEntities, evaluatedEntityFaction);	
				
				if (factionCount == 0 && isRequiredGroupTypePresent)
				{
					factionsPresence.Insert(evaluatedEntityFaction, 1);
				}					
				else if (isRequiredGroupTypePresent)
				{
					factionsPresence.Set(evaluatedEntityFaction, factionCount + 1);
				}
				else if (!isRequiredGroupTypePresent)
				{
					factionsPresence.Set(evaluatedEntityFaction, 0);
				}	
			}	
			else
			{
				// If faction is not yet registered, do it now - otherwise just increase its presence counter
				if (factionCount == 0)
				{
					factionsPresence.Insert(evaluatedEntityFaction, 1);
				}
					
				else
				{
					factionsPresence.Set(evaluatedEntityFaction, factionCount + 1);
				}
			}

			// Check if there are some players present in case they are required
			if (m_bCapturingRequiresPlayer && playerManager.GetPlayerIdFromControlledEntity(entity) != 0)
				factionsPlayerPresence.Set(evaluatedEntityFaction, true);			
		}
		
		m_bDeleteDisabledAIs = false;
		SCR_Faction prevailingFaction;
		string prevailingFactionKey;
		int highestAttackingPresence;
		int highestDefendingPresence;
		int curSeizingCharacters;

		// Evaluate the highest attacking presence
		foreach (SCR_Faction faction, int presence : factionsPresence)
		{
			// FIA is not allowed
			if (faction == SCR_GameModeCampaign.GetInstance().GetFactionByEnum(SCR_ECampaignFaction.INDFOR))
				continue;
			
			// Non-playable attackers are not allowed
			if (m_bIgnoreNonPlayableAttackers && !faction.IsPlayable())
				continue;

			// In case players are required but are not present, ignore this faction for attacking
			if (m_bCapturingRequiresPlayer && !factionsPlayerPresence.Get(faction))
				continue;

			if (presence > highestAttackingPresence)
			{
				highestAttackingPresence = presence;
				prevailingFaction = faction;
			}
			else if (presence == highestAttackingPresence)	// When 2 or more factions have the same presence, none should prevail
			{
				prevailingFaction = null;
			}
		}

		// Evaluate the highest defending presence
		if (prevailingFaction)
		{
			foreach (SCR_Faction faction, int presence : factionsPresence)
			{
				// Non-playable defenders are not allowed
				if (m_bIgnoreNonPlayableDefenders && !faction.IsPlayable())
					continue;

				// This faction is already considered attacking
				if (faction == prevailingFaction)
					continue;

				highestDefendingPresence = Math.Max(presence, highestDefendingPresence);
			}

			// Get net amount of players effectively seizing (clamp for max attackers attribute)
			if (prevailingFaction && highestAttackingPresence > highestDefendingPresence)
			{
				prevailingFactionKey = prevailingFaction.GetFactionKey();
				curSeizingCharacters = Math.Min(highestAttackingPresence - highestDefendingPresence, m_iMaximumSeizingCharacters);
			}
			else
			{
				prevailingFaction = null;
				curSeizingCharacters = 0;
			}
		}

		if (prevailingFaction != m_PrevailingFaction)
		{
			m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
			m_sAADDefendingFaction = m_CampaignInfo.GetAADDefendingFaction();

			if (!m_CampaignInfo.IsAADEnabled() || m_CampaignInfo.IsAADEnabled() && prevailingFactionKey != m_sAADDefendingFaction)
			{
				m_iSeizingCharacters = curSeizingCharacters;
				m_PrevailingFactionPrevious = m_PrevailingFaction;
				m_PrevailingFaction = prevailingFaction;
				OnPrevailingFactionChanged();
			}		
		}
		
		else if (prevailingFaction && curSeizingCharacters != m_iSeizingCharacters)
		{
			m_iSeizingCharacters = curSeizingCharacters;
			RefreshSeizingTimer();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsRequiredGroupTypePresent(array<IEntity> entities, SCR_Faction faction)
	{
		bool isPresent;
		int count = entities.Count();
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		
		for (int i = 0; i < count; i++)
		{
			SCR_Faction groupTypeEntityFaction;					
			IEntity groupTypeEntity = entities[i];
			int playerId = playerManager.GetPlayerIdFromControlledEntity(groupTypeEntity);
			
			if (!groupTypeEntity)
			{
				continue;
			}
			
			if (!groupsManager)
			{
				//PrintFormat("GetIsRequiredGroupTypePresent: No Group Manager");
				continue;
			}

			SCR_PlayerController pc = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerId));
			if (!pc)
			{
				//PrintFormat("GetIsRequiredGroupTypePresent: No Player Controller");
				continue;
			}
			
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(groupTypeEntity);
			if (!character)
			{
				//PrintFormat("GetIsRequiredGroupTypePresent: No Character");
				continue;
			}
			
			groupTypeEntityFaction = SCR_Faction.Cast(character.GetFaction());
			if (faction != groupTypeEntityFaction)
			{
				continue;
			}
			
			// Let anyone defend
			if (m_Base.GetFaction() == groupTypeEntityFaction)
			{
				return true;
			}
			
			if (!m_bCanCaptureFromVehicle && character.IsInVehicle())
			{
				//PrintFormat("GetIsRequiredGroupTypePresent: Skipping character in vehicle");
				continue;
			}
			
			SCR_AIGroup group = groupsManager.GetPlayerGroup(pc.GetPlayerId());
			if (group)
			{
				SCR_EGroupRole groupRole = group.GetGroupRole();				
				if (m_RequiredGroupTypes.Contains(groupRole))
				{
					isPresent = true;
					break;
				}
			}				
		}
		
		return isPresent;
	}
	
	//------------------------------------------------------------------------------------------------
	override void RefreshSeizingTimer()
	{
		if (!m_fSeizingStartTimestamp)
			return;

		if (!m_Base)
			return;
		
		int servicesCount;
	
		if (m_Base)
		{
			array<SCR_EServicePointType> checkedTypes = {
				SCR_EServicePointType.ARMORY,
				SCR_EServicePointType.HELIPAD,
				SCR_EServicePointType.BARRACKS,
				SCR_EServicePointType.RADIO_ANTENNA,
				SCR_EServicePointType.FIELD_HOSPITAL,
				SCR_EServicePointType.LIGHT_VEHICLE_DEPOT,
				SCR_EServicePointType.HEAVY_VEHICLE_DEPOT
			};

			foreach (SCR_EServicePointType type : checkedTypes)
			{
				if (m_Base.GetServiceDelegateByType(type))
					servicesCount++;
			}
		}
		
		int radioConnectionsCount;
		SCR_CoverageRadioComponent comp = SCR_CoverageRadioComponent.Cast(m_Base.GetOwner().FindComponent(SCR_CoverageRadioComponent));
		
		if (comp)
		{
			SCR_CampaignFaction faction = m_Base.GetCampaignFaction();
			
			if (faction && faction.IsPlayable())
				radioConnectionsCount = comp.GetRadiosInRangeOfCount(faction.GetFactionRadioEncryptionKey());
		}

		float seizingTimeVar = m_fMaximumSeizingTime - m_fMinimumSeizingTime;
		float deduct;

		if (m_iMaximumSeizingCharacters > 1)	// Avoid division by 0
		{
			float deductPerPlayer = seizingTimeVar / (m_iMaximumSeizingCharacters - 1);
			deduct = deductPerPlayer * (m_iSeizingCharacters - 1);
		}

		float multiplier = 1;
		
		if ((m_fMaximumSeizingTime - m_fMinimumSeizingTime) > 0)
		{
			multiplier += (servicesCount * (m_fExtraTimePerService / (m_fMaximumSeizingTime - m_fMinimumSeizingTime)));
			multiplier += (radioConnectionsCount * (m_fExtraTimePerRadioConnection / (m_fMaximumSeizingTime - m_fMinimumSeizingTime)));
		}
		
		int supplyAmount = m_Base.GetSupplies();
		float supplyAdditionalTime = Math.Clamp(supplyAmount * m_fSupplyMultiplier, 0, m_iMaxCaptureTimeFromSupplies);

		m_fSeizingEndTimestamp = m_fSeizingStartTimestamp.PlusSeconds(multiplier * (m_fMaximumSeizingTime - deduct) + supplyAdditionalTime);
		
		ChimeraWorld world = GetGame().GetWorld();
		WorldTimestamp currentTime = world.GetServerTimestamp();

		// Add a tiny delay if removing a service would cause immediate capture
		if (m_fSeizingEndTimestamp.LessEqual(currentTime))
			m_fSeizingEndTimestamp = currentTime.PlusMilliseconds(SCR_GameModeCampaign.DEFAULT_DELAY);

		if (m_bGradualTimerReset && m_fInterruptedCaptureDuration != 0)
			HandleGradualReset();
		
		Replication.BumpMe();
		OnSeizingTimestampChanged();
	}
}
