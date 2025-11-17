enum AADBaseType
{
	None,
	Attacker,
	Defender,
}

modded class SCR_CampaignMilitaryBaseComponent : SCR_MilitaryBaseComponent
{
	[Attribute("2000", category: "Campaign"), RplProp(onRplName: "OnRadioRangeChanged")]
	protected float m_fRadioAntennaServiceRange;
	
	[Attribute("90", desc: "Time (in seconds) added to bases that receive enemy radio signal",category: "Frontline Respawn Time")]
	protected int m_iFrontlineRespawnTime;
	
	[Attribute("", UIWidgets.ComboBox, "If AAD game mode, what type of base", enums: ParamEnumArray.FromEnum(AADBaseType))]
	protected AADBaseType m_AADBaseType;
	
	[Attribute("0", UIWidgets.CheckBox, "Is this base on a far away island?")]
	protected bool m_bIsOnIsland;
	
	[Attribute("0", UIWidgets.CheckBox, "Should this base be excluded from randomization?", category: "Randomized Bases")]
	protected bool m_bExcludeFromRandomization;
	
	[Attribute("0", desc: "Is Base part of a group?", category: "Randomized Bases")]
	protected bool m_bHasGroup;
	
	[Attribute("", desc: "The group name this base is associated with.", category: "Randomized Bases")]
	protected string m_sGroupName;
	
	[Attribute("1000", UIWidgets.Slider, "Duration in milliseconds before each point is awarded.", params: "1 60000 1*", category: "Reward")]
	protected int m_fTickRate;

	[Attribute("10", UIWidgets.Slider, "Score awarded per tick.", params: "0 1000 1", category: "Reward")]
	protected int m_iScorePerTick;


	[Attribute("0", UIWidgets.Slider, "Score awarded per tick when contested.", params: "0 1000 1", category: "Reward")]
	protected int m_iScorePerTickContested;
	
	protected float m_ObjectivesReward_fTimeOfLastTick_modded;
	protected float m_ObjectivesReward_fTickTime_modded;
	Faction m_fFactionOwner;
	protected bool initialized;
	
	SCR_GameModeCampaign CampaignInfo;
	SCR_CaptureAndHoldManager m_CHManager;
	SCR_FactionManager m_FactionManager;
	ref array<Faction> m_aFactions = {}; //new array<Faction>();
	
	string m_sAASAttackingFaction;
	string m_sAASDefendingFaction;
	int m_iMaxScore;
	
	protected float m_fPreviousRadioRange;
	protected bool m_bIsFringeBase;
	protected bool m_bIsConnectingBase;
	protected bool m_bIsFrontlineBase;
	protected bool m_bIsRegistered;
	protected bool m_bIsProcessed;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		CampaignInfo = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		
		if (CampaignInfo)
		{
			m_sAASDefendingFaction = CampaignInfo.GetAADDefendingFaction();
			m_sAASAttackingFaction = CampaignInfo.GetAADAttackingFaction();
		}
		
//		m_FactionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
//		if (!m_FactionManager)
//		{
//			return;
//		}
//		
//		m_FactionManager.GetFactionsList(m_aFactions);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnHQSet()
	{
		super.OnHQSet();
		
		if (IsHQ() && !IsProxy())
		{
			GetGame().GetCallqueue().Remove(ObjectivesRewardTimer_Modded);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnHasSignalChanged()
	{
		super.OnHasSignalChanged();
		
		if (!CampaignInfo || CampaignInfo.GetIsMatchOver())
		{
			Print("OnHasSignalChanged: Match is over. Returning...");
			return;
		}		
	
		SignalChangedInternal_modded();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnFactionChanged(FactionAffiliationComponent owner, Faction previousFaction, Faction faction)
	{
		super.OnFactionChanged(owner,  previousFaction,  faction);
		
		if (!CampaignInfo)
			return;
		
		SCR_BaseScoringSystemComponent scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
		if (!scoringSystem)
			return;
		
		SCR_CaptureAndHoldManager chManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
		if (!chManager)
			return;
		
		int baseCaptureScore = chManager.GetBaseCaptureScore();
		int factionStartScore = chManager.GetFactionStartScore();

		ObjectivesRewardTimer_Modded(true);
	
		// If AAD Mode, don't let defenders have tasks since they cannot capture the bases back anyways
		if (CampaignInfo.IsAADEnabled())
		{
//			SCR_BaseTaskManager taskManager = GetTaskManager();
//			ref array<SCR_BaseTask>baseTasks = {};
//			taskManager.GetTasks(baseTasks);
//			
//			if (CampaignInfo)
//			{
//				// OnPostInit may not be working to set proper attacker/defender
//				m_sAASDefendingFaction = CampaignInfo.GetAADDefendingFaction();
//				m_sAASAttackingFaction = CampaignInfo.GetAADAttackingFaction();
//			}
//			
//			foreach (SCR_BaseTask task : baseTasks)
//			{	
//				if (task.GetTargetFaction().GetFactionKey() == m_sAASDefendingFaction)
//				{
//					task.Remove();
//					task.Fail();
//					taskManager.DeleteTask(task);
//				}
//					
//			}

//			SCR_BaseTaskManager taskManager = GetTaskManager();
//	        ref array<SCR_BaseTask> baseTasks = {};
//	        taskManager.GetTasks(baseTasks);
//			
//			foreach (SCR_BaseTask task : baseTasks)
//	        {
//	//			PrintFormat("SCR_CampaignMilitaryBaseComponent: CancelBaseTasks: Task: %1 | Type: %2", task, task.Type());
//				SCR_CampaignBaseTask baseTask = SCR_CampaignBaseTask.Cast(task);
//				if (!baseTask)
//				{
//					continue;
//				}
//							
//	            if (task.GetTargetFaction().GetFactionKey() == CampaignInfo.GetAADDefendingFaction())
//	            {
//	                task.Remove();
//	                task.Fail();
//	                taskManager.DeleteTask(task);
//					task.SetState(SCR_TaskState.CANCELLED);
//	            }
//	        }
		}
		
		if (!owner || !faction || !previousFaction)
		return;
		
		scoringSystem.AddFactionObjective(faction, baseCaptureScore);
		SCR_CampaignFaction campaignFaction = SCR_CampaignFaction.Cast(previousFaction);
		
		
		if (previousFaction && scoringSystem.GetFactionScore(previousFaction) >= baseCaptureScore && campaignFaction.IsPlayable())
		{
			if (!CampaignInfo.IsAADEnabled())	
				scoringSystem.AddFactionObjective(previousFaction, -baseCaptureScore);
		}

		SignalChangedInternal_modded();
	}
	
	//------------------------------------------------------------------------------------------------
	private void SignalChangedInternal_modded()
	{
		if (!IsHQ() && !IsProxy())
		{
			
			Faction owner;
			owner = GetFaction();
			if(!owner)
			{
				return;
			}
			
			if (GetGame().GetCallqueue().GetRemainingTime(ObjectivesRewardTimer_Modded) == -1)
			{
				
				if (IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(owner), SCR_ERadioCoverageStatus.BOTH_WAYS))
				{
					// ObjectivesRewardTimer_Modded(true); we are nice and give many objectives					
					GetGame().GetCallqueue().CallLater(ObjectivesRewardTimer_Modded, m_fTickRate, true, false);
				}
			}
			else
			{
				if (!IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(owner), SCR_ERadioCoverageStatus.BOTH_WAYS))
				{
					
					GetGame().GetCallqueue().Remove(ObjectivesRewardTimer_Modded);
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------ v2
	//! \param[in] faction
	override void SetFaction(SCR_CampaignFaction faction)
	{
		if (IsProxy())
			return;

		if (!m_FactionComponent)
			return;

		CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (CampaignInfo.IsAADEnabled() && IsInitialized() && !IsHQ()) //!IsHQ() && IsInitialized())
		{
			Faction defenderFaction;
			Faction attackerFaction;
			
			FactionKey defendingKey = CampaignInfo.GetAADDefendingFaction();
    		FactionKey attackingKey = CampaignInfo.GetAADAttackingFaction();
			FactionManager factionManager = GetGame().GetFactionManager();
		    if (!factionManager)
		    {
		        Print("SetFaction: FactionManager not found");
		        return;
		    }
			
			if (m_AADBaseType == AADBaseType.Defender && !defendingKey.IsEmpty())
		    {
		        defenderFaction = factionManager.GetFactionByKey(defendingKey);
		    }
			
			if (m_AADBaseType == AADBaseType.Attacker && !attackingKey.IsEmpty())
			{
				attackerFaction = factionManager.GetFactionByKey(attackingKey);
			}
			
			if (defendingKey.IsEmpty() || attackingKey.IsEmpty())
			{
				Print("SetFaction: Key is empty! This error should never happen! Prentending to set it again...");
				
				defendingKey = CampaignInfo.GetAADDefendingFaction();
				attackingKey = CampaignInfo.GetAADAttackingFaction();
				
				defenderFaction = factionManager.GetFactionByKey(defendingKey);
				attackerFaction = factionManager.GetFactionByKey(attackingKey);
				
//				array<SCR_MilitaryBaseComponent> bases = {};
//				SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
//				baseManager.GetBases(bases);
//				
//				foreach (SCR_MilitaryBaseComponent base : bases)
//				{
//					SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
//					if (campaignBase && campaignBase.IsHQ())
//					{
//						if (campaignBase.GetAADBaseType() == AADBaseType.Defender)
//						{
//							defenderFaction = campaignBase.GetFaction();
//							CampaignInfo.SetAADDefendingFaction(defenderFaction.GetFactionKey());
//						}
//		
//						else if (campaignBase.GetAADBaseType() == AADBaseType.Attacker)
//						{
//							attackerFaction = campaignBase.GetFaction();
//							CampaignInfo.SetAADAttackingFaction(attackerFaction.GetFactionKey());
//						}
//					}
//				}
			}
	
			if (m_FactionComponent)
			{			
				if (m_AADBaseType == AADBaseType.Defender)
					m_FactionComponent.SetAffiliatedFaction(defenderFaction);
				
				else if (m_AADBaseType == AADBaseType.Attacker)
					m_FactionComponent.SetAffiliatedFaction(attackerFaction);
			}			
		}
		
		else
		{
			m_FactionComponent.SetAffiliatedFaction(faction);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Retrieves or sets the faction for the current base type
	private Faction GetFactionForBaseType(AADBaseType type)
	{
	    FactionKey key;
	    bool isDefender = (type == AADBaseType.Defender);
	
	    // Get the appropriate key based on base type
	    if (isDefender)
	        key = CampaignInfo.GetAADDefendingFaction();
	    else
	        key = CampaignInfo.GetAADAttackingFaction();
	
	    // If key exists, return the corresponding faction
	    if (!key.IsEmpty())
		{
			FactionManager factionManager = GetGame().GetFactionManager();
			PrintFormat("GetFactionForBaseType: Has Key: Returning %1", factionManager.GetFactionByKey(key));
			return factionManager.GetFactionByKey(key);
		}	        
	
	    // Key is empty: find HQ and update CampaignInfo
	    Faction hqFaction = FindHQFaction(type);
	    if (hqFaction)
	    {
	        if (isDefender)
	            CampaignInfo.SetAADDefendingFaction(hqFaction.GetFactionKey());
	        else
	            CampaignInfo.SetAADAttackingFaction(hqFaction.GetFactionKey());
	    }
		
	    return hqFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	// Finds the HQ faction for a given base type
	private Faction FindHQFaction(AADBaseType baseType)
	{
	    array<SCR_MilitaryBaseComponent> bases = {};
	    SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
	    if (!baseManager || !baseManager.GetBases(bases) || bases.IsEmpty())
	    {
	        Print("FindHQFaction: No bases available");
	        return null;
	    }
	
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		array<Faction>factions = {};
		factionManager.GetFactionsList(factions);
		array<Faction>playableFactions = {};
		
	    foreach (SCR_MilitaryBaseComponent base : bases)
	    {
	        SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
	        if (campaignBase && campaignBase.IsHQ() && campaignBase.GetAADBaseType() == baseType)
	        {		
				Faction faction = campaignBase.GetFaction();
				if (!faction)
				{
					Print("FindHQFaction: Setting Factions...");
									
					foreach(Faction pfaction: factions)
					{
						SCR_CampaignFaction campaignFaction = SCR_CampaignFaction.Cast(pfaction);
						if (campaignFaction && campaignFaction.IsPlayable() && !playableFactions.Contains(pfaction))
						{
							playableFactions.Insert(pfaction);
						}
					}
					
					foreach (SCR_MilitaryBaseComponent cbase : bases)
					{
						SCR_CampaignMilitaryBaseComponent cCampaignBase = SCR_CampaignMilitaryBaseComponent.Cast(cbase);
						if (cCampaignBase && cCampaignBase.IsHQ())
						{
							Faction cCampaignBaseFaction = cCampaignBase.GetFaction();
							if (cCampaignBaseFaction && playableFactions.Contains(cCampaignBaseFaction))
							{
								playableFactions.RemoveItem(cCampaignBaseFaction);
							}
						}
					}
					
					faction = playableFactions[Math.RandomInt(0, playableFactions.Count() -1)];					
				}
				
				PrintFormat("FindHQFaction: Found HQ Faction For Base: %1 | %2", GetBaseName(), campaignBase.GetFaction());
	            return faction;
	        }
			
			else
			{
				Print("FindHQFaction: No Campaign Base or Is Not HQ or is Not Type");
			}
	    }
	
	    PrintFormat("FindHQFaction: No HQ found for base type %1", baseType.ToString());
	    return null;
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	override void OnInitialized()
	{
		super.OnInitialized();
		
		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		if (!campaign)
			return;
		
		if (m_eType == SCR_ECampaignBaseType.BASE && !campaign.IsTutorial())
		{
			// Check if base is receiving radio signal from enemy
			GetGame().GetCallqueue().CallLater(EvaluateFrontline, 3000, true);
		}
		
		if(!IsHQ() && !IsProxy())
		{
			Faction owner;
			owner = GetFaction();
			if(!owner)
			{
				return;
			}
			
			if(IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(owner), SCR_ERadioCoverageStatus.BOTH_WAYS))
			{
				ObjectivesRewardTimer_Modded(true);
				GetGame().GetCallqueue().CallLater(ObjectivesRewardTimer_Modded, m_fTickRate, true, false);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void EvaluateFrontline()
	{
		SCR_CampaignFaction baseFaction = GetCampaignFaction();
		if (!baseFaction)
			return;
		
		if (!baseFaction.IsPlayable())
			return;
		
		SCR_CampaignFactionManager factionManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());

		if (!factionManager)
			return;
		
		if (!m_SpawnPoint)
			return;

		if (!CampaignInfo.IsRunning())
		{
			return;
		}
		
		m_CapturingFaction = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager()).GetCampaignFactionByIndex(m_iCapturingFaction);
		
//		if (m_CapturingFaction && m_CapturingFaction != GetFaction())
//			m_SpawnPoint.SetRespawnTime((CONTESTED_RESPAWN_DELAY - CONTESTED_RESPAWN_DELAY) + m_iFrontlineRespawnTime + m_iAdditionalContestedTime);
		
		SCR_RespawnTimerComponent respawnTimerComponent = SCR_RespawnTimerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_RespawnTimerComponent));
		int respawnTime = respawnTimerComponent.GetRespawnTime();
		
		if (!m_bIsHQ && IsHQRadioTrafficPossible(factionManager.GetEnemyFaction(baseFaction)))
		{	
			
			m_SpawnPoint.SetRespawnTime(m_iFrontlineRespawnTime);
			m_bIsFrontlineBase = true;
		}

		else if (!m_bIsHQ && !IsHQRadioTrafficPossible(factionManager.GetEnemyFaction(baseFaction)))
		{
			m_SpawnPoint.SetRespawnTime(respawnTime);
			m_bIsFrontlineBase = false;
		}
		
		else
		{
			m_SpawnPoint.SetRespawnTime(0);
			m_bIsFrontlineBase = false;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsFrontlineBase()
	{
		return m_bIsFrontlineBase;
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	override void RecalculateRadioRange()
	{
		float range = m_fRadioRangeDefault;
		float thisRange;
		array<SCR_ServicePointComponent> antennas = {};
		GetServicesByType(antennas, SCR_EServicePointType.RADIO_ANTENNA);
		BaseRadioComponent radio;

		// Find antenna services, read max radio range from the radio component on their owners
		foreach (SCR_ServicePointComponent service : antennas)
		{
			SCR_AntennaServicePointComponent antenna = SCR_AntennaServicePointComponent.Cast(service);
			radio = BaseRadioComponent.Cast(antenna.GetOwner().FindComponent(BaseRadioComponent));

			if (!radio)
				continue;

			// Turn off the radio so we don't hit performance too much with every antenna built
			if (radio.IsPowered())
				radio.SetPower(false);

			thisRange = GetCustomRadioAntennaRange(); //m_fRadioAntennaServiceRange; // GetRelayRadioRange(radio);

			if (thisRange > range)
				range = thisRange;
		}

		if (m_fRadioRange == range)
			return;

		// Instead of relying on antenna radio which has been turned off, apply the antenna's signal range to the radio component on the base itself
		if (m_RadioComponent)
		{
			RelayTransceiver transceiver;

			for (int i = 0, count = m_RadioComponent.TransceiversCount(); i < count; i++)
			{
				transceiver = RelayTransceiver.Cast(m_RadioComponent.GetTransceiver(i));

				if (!transceiver)
					continue;

				transceiver.SetRange(range);
			}
		}

		m_fRadioRange = range;
		Replication.BumpMe();
		OnRadioRangeChanged();
	}
	
	//------------------------------------------------------------------------------------------------
	//!
//	override void OnAllBasesInitialized()
//	{
//		super.OnAllBasesInitialized();
//		
//		CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (!CampaignInfo)
//			return;
//		
////		if (CampaignInfo.GetAreBasesRandomized())
////		{
////			ProcessFringeBases();
////			EstablishDynamicRadioRanges();
////		}
//	}
	
	// This is a mixup of SupplyIncomeTimer from SCR_CampaignMilitaryBaseComponent and OnFrame in SCR_CaptureAndHoldArea
	//------------------------------------------------------------------------------------------------
	protected void ObjectivesRewardTimer_Modded(bool reset = false)
	{
		if (IsProxy())
			return;
		
		ChimeraWorld world = GetOwner().GetWorld();
		m_fFactionOwner = GetFaction();
		
		if (!world)
		{
			m_ObjectivesReward_fTickTime_modded = 0;
			return;
		}

		if (reset)
		{
			
			m_ObjectivesReward_fTickTime_modded = 0;
			m_ObjectivesReward_fTimeOfLastTick_modded = world.GetWorldTime();
			return;
		}
		if (IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(m_fFactionOwner), SCR_ERadioCoverageStatus.BOTH_WAYS))
		{
			
			m_ObjectivesReward_fTickTime_modded += (world.GetWorldTime() - m_ObjectivesReward_fTimeOfLastTick_modded);
		
			while (m_ObjectivesReward_fTickTime_modded >= m_fTickRate)
			{
				
				m_ObjectivesReward_fTickTime_modded -= m_fTickRate;
				m_ObjectivesReward_fTimeOfLastTick_modded = world.GetWorldTime();
				OnTick_ObjectivesReward_Modded();
			}
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnTick_ObjectivesReward_Modded()
	{
		// Remove from CallQueue when signal is lost and do not reward points if HQ, if the reward is zero don't run it either
		
		m_fFactionOwner = GetFaction();
		if(!m_fFactionOwner)
		{
			return;
		}
		
		if(!IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(m_fFactionOwner), SCR_ERadioCoverageStatus.BOTH_WAYS))
		{
			return;
		}		
		
		SCR_Faction owner_scr = SCR_Faction.Cast(m_fFactionOwner);
		if (owner_scr)
		{
			
			// Do not reward points to FIA
			if (!owner_scr.IsPlayable())
			{
				return;
			}
		}
		
		if(IsHQ())
		{
			return;
		}
		
		int reward = m_iScorePerTick;
		
		// I don't care that we are scoring slightly wrong by not discriminating between elapsed contested and elapsed uncontested time.
		if (m_CapturingFaction && m_CapturingFaction != m_fFactionOwner)
		{			
			reward = m_iScorePerTickContested;			
		}

		SCR_BaseScoringSystemComponent scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
		if (!scoringSystem)
			return;
		
		SCR_CaptureAndHoldManager chManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
		m_ObjectivesReward_fTickTime_modded = 0; // No dumping of points 
		
		if (!chManager.TicketScoreSystem())
			scoringSystem.AddFactionObjective(m_fFactionOwner, reward);
		
		else if (chManager.TicketScoreSystem() && CampaignInfo.IsAADEnabled())
		{
			m_iMaxScore = chManager.GetFactionStartScore();
			
			if (CampaignInfo)
			{
				// OnPostInit may not be working to set proper attacker/defender
				m_sAASDefendingFaction = CampaignInfo.GetAADDefendingFaction();
				m_sAASAttackingFaction = CampaignInfo.GetAADAttackingFaction();
			}
			
			if (m_fFactionOwner.GetFactionKey() == m_sAASDefendingFaction && scoringSystem.GetFactionScore(m_fFactionOwner) <= m_iMaxScore)
				scoringSystem.AddFactionObjective(m_fFactionOwner, reward);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	void ProcessFringeBases()
	{
		float customRadioRange = m_fRadioAntennaServiceRange;
		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
		GetBasesAtRange(basesAtRange);
		
		// Process all bases and set Fringe status
		foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
		{
			array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
			excludeBases.Insert(base);
			
			vector basePosition = base.GetOwner().GetOrigin();
			SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
			excludeBases.Insert(closestBase);
			
			float distance = GetDistanceToBase(basePosition, closestBase);
			if (distance > customRadioRange)
			{
//				PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 (%3) | Next Closest: %2 (%4)", closestBase.GetBaseName(), nextClosestBase.GetBaseName(), distance, distanceToNextClosestBase);

				base.SetIsFringeBase(true);
				closestBase.SetIsFringeBase(true);
				
				base.SetRadioAntennaServiceRange(distance + 100);
				closestBase.SetRadioAntennaServiceRange(distance + 100);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	void EstablishDynamicRadioRanges()
	{
		float customRadioRange = m_fRadioAntennaServiceRange;
		
		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
		GetBasesAtRange(basesAtRange);
		
		// Check closest bases to me
		vector myPosition = GetOwner().GetOrigin();
		
		array<SCR_CampaignMilitaryBaseComponent> closestBases = {};
		array<SCR_CampaignMilitaryBaseComponent> closestFriendlyBases = {};
		array<SCR_CampaignMilitaryBaseComponent> outOfRangeBases = {};
		array<float> outOfRangeDistances = {};
		array<float> closestFriendlyDistances = {};
//		FindNext5ClosestBases(basesAtRange, this, myPosition, closestBases);
		FindNextNClosestBases(3, basesAtRange, this, myPosition, closestBases);
		int totalClosestBases;
		float totalDistance;
		float closestFriendlyBaseDistance;
		Faction thisBaseFaction = GetFaction();

		foreach (SCR_CampaignMilitaryBaseComponent base : closestBases)
		{
			float distance = GetDistanceToBase(myPosition, base);
			
//			if (distance < customRadioRange || base.GetFaction() == thisBaseFaction)
			if (base.GetFaction() == thisBaseFaction && !base.IsHQ())
			{			
				closestFriendlyDistances.Insert(distance);
				closestFriendlyBases.Insert(base);
				continue;
			}
			
			else
			{
				totalClosestBases++;
				totalDistance += distance;
				if (distance > customRadioRange)
				{
					outOfRangeBases.Insert(base);
					outOfRangeDistances.Insert(distance);
				}					
			}
			
			PrintFormat("GetCustomRadioAntennaRange: Base: %1 | Distance: %2", base.GetBaseName(), distance);
			
//				if (totalClosestBases > 0)
//				{
//					customRadioRange = totalDistance / totalClosestBases;
//					if (base.GetRadioAntennaServiceRange() < customRadioRange)
//					{
//						base.SetRadioAntennaServiceRange(customRadioRange);
//						PrintFormat("GetCustomRadioAntennaRange: Setting Base (%1) to: %2", base.GetBaseName(), customRadioRange);
//						break;
//					}					
//				}				
		}
			
		if (outOfRangeDistances.Count() > 1)
		{
			outOfRangeDistances.Sort(true);
		}
		
//			if (closestFriendlyDistances.Count() > 1)
//			{
//				closestFriendlyDistances.Sort();
//				closestFriendlyBaseDistance = closestFriendlyDistances[0];
//			}
//			
//			if (closestFriendlyBaseDistance > customRadioRange)
//			{
//				customRadioRange = closestFriendlyBaseDistance;
//			}
		
		//3, basesAtRange, this, myPosition, closestBases
		SCR_CampaignMilitaryBaseComponent closestFriendlyBase = FindClosestFriendlyBase(basesAtRange, this, myPosition, thisBaseFaction);
		
		if (closestFriendlyBase)
		{
			closestFriendlyBaseDistance = GetDistanceToBase(myPosition, closestFriendlyBase);
			if (closestFriendlyBaseDistance > customRadioRange)
			{
				customRadioRange = closestFriendlyBaseDistance;
			}
		}
	
//			if (totalClosestBases > 2)
		if (outOfRangeBases.Count() > 0 && (totalDistance / totalClosestBases) > customRadioRange) // > 1
		{
			customRadioRange = totalDistance / totalClosestBases;
		}
					
		SCR_CampaignMilitaryBaseComponent closestBaseToMe = FindNextClosestBase(basesAtRange, this, myPosition);
		float distanceToMe = GetDistanceToBase(myPosition, closestBaseToMe);
		
		if (distanceToMe > customRadioRange)
		{
			SetIsFringeBase(true);
			closestBaseToMe.SetIsFringeBase(true);
			
			float closestBaseRange = closestBaseToMe.GetRadioAntennaServiceRange();
			if (closestBaseRange < distanceToMe)
			{
				closestBaseToMe.SetRadioAntennaServiceRange(distanceToMe);
			}
			
			if (GetRadioAntennaServiceRange() < distanceToMe)
			{
				SetRadioAntennaServiceRange(distanceToMe);
			}			
		}
		
		if (GetIsFringeBase())
		{
#ifdef WORKBENCH
		Print("GetCustomRadioAntennaRange: I'm a FRINGE Base!");
#endif
		SetRadioAntennaServiceRange(GetRadioAntennaServiceRange() + 100);
		}
			
		// Check next closest base to me?
	}
	
	//------------------------------------------------------------------------------------------------
	void GetBasesAtRange(array<SCR_CampaignMilitaryBaseComponent> outBases)
	{
		SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
		if (!baseManager)
			return;
		
		array<SCR_MilitaryBaseComponent>bases = {};
		array<SCR_CampaignMilitaryBaseComponent>basesAtRange = {};
		baseManager.GetBases(bases);
		
		array<float>distances = {};
		SCR_CampaignMilitaryBaseComponent previousBase;
		Faction baseFaction = GetFaction();
		vector thisPosition = GetOwner().GetOrigin();
				
		foreach (SCR_MilitaryBaseComponent base : bases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
			if (campaignBase && campaignBase.IsInitialized() && !campaignBase.IsHQ() && campaignBase.GetType() != SCR_ECampaignBaseType.RELAY)
			{
				basesAtRange.Insert(campaignBase);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------ v1
	//! \return
//	float GetCustomRadioAntennaRange()
//	{
//		float customRadioRange = m_fRadioAntennaServiceRange;
//		
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (CampaignInfo.GetAreBasesRandomized())
//		{
////			float range;
////			vector thisPosition = GetOwner().GetOrigin();
////			SCR_CampaignMilitaryBaseManager baseManager = CampaignInfo.GetBaseManager();
////			SCR_CampaignMilitaryBaseComponent previousBase;
////			SCR_CampaignMilitaryBaseComponent campaignBase = baseManager.FindClosestBase(thisPosition);
////			vector basePosition = campaignBase.GetOwner().GetOrigin();
////			range = vector.DistanceXZ(thisPosition, basePosition);
////			
////			if (range > m_fRadioAntennaServiceRange)
////			{
////				m_fRadioAntennaServiceRange = Math.Ceil(range);
////				PrintFormat("GetCustomRadioAntennaRange: Returning subsidized range: %1", m_fRadioAntennaServiceRange);
////			}
//			
//			array<SCR_MilitaryBaseComponent>bases = {};
//			SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
//			baseManager.GetBases(bases);
//			
////			map<SCR_MilitaryBaseComponent, float> distances = new map<SCR_MilitaryBaseComponent, float>;
//			array<float>distances = {};
//			SCR_MilitaryBaseComponent previousBase;
//			
//			vector thisPosition = GetOwner().GetOrigin();
//			
//			foreach (SCR_MilitaryBaseComponent base : bases)
//			{
//				SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//				if (campaignBase && campaignBase.IsInitialized())
//				{
//					vector basePosition = base.GetOwner().GetOrigin();
//					float distance = vector.DistanceXZ(thisPosition, basePosition);
//					distances.Insert(distance);
////					PrintFormat("GetCustomRadioAntennaRange: Distance Added: %1", distance);
//				}		
//			}
//			
//			distances.Sort();
//			int count;
//			
//			foreach (float distance : distances)
//			{
//				if (distance <= m_fRadioAntennaServiceRange)
//				{
////					count++;
//					continue;
//				}
//				else if (count < 2 && distance > m_fRadioAntennaServiceRange)
//				{
//					count++;
//					customRadioRange = Math.Ceil(distance + 5);
//					PrintFormat("GetCustomRadioAntennaRange: Returning subsidized range: %1", customRadioRange);
//				}
//				else
//				{
//					break;
//				}
//			}
//		}
//		
//		return customRadioRange;
//	}
	
	//------------------------------------------------------------------------------------------------ v2
	//! \return
//	float GetCustomRadioAntennaRange()
//	{
//		float customRadioRange = m_fRadioAntennaServiceRange;
//		
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (!CampaignInfo)
//		{
//			return;
//		}
//		
//		SCR_CampaignMilitaryBaseManager campaignBaseManager = CampaignInfo.GetBaseManager();
//		
//		if (CampaignInfo.GetAreBasesRandomized())
//		{
//			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
//			SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
//			if (!factionManager || baseManager)
//			{
//				Print("GetCustomRadioAntennaRange: No Faction Manager or Base Manager", LogLevel.ERROR);
//				return;
//			}
//			
//			array<SCR_MilitaryBaseComponent>bases = {};
//			array<SCR_MilitaryBaseComponent>connectedBases = {};
//			array<SCR_MilitaryBaseComponent>enemyBases = {};
//			baseManager.GetBases(bases);
//
//			array<float>distances = {};
//			array<float>connectedDistances = {};
//			array<float>differences = {};
//			SCR_MilitaryBaseComponent previousBase;
//			
//			vector thisPosition = GetOwner().GetOrigin();
//			Faction thisFaction = GetFaction();
//			
//			foreach (SCR_MilitaryBaseComponent base : bases)
//			{
//				SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//				if (campaignBase && campaignBase.IsInitialized())
//				{
//					Faction baseFaction = campaignBase.GetFaction();
//					vector basePosition = base.GetOwner().GetOrigin();
//					float distance = vector.DistanceXZ(thisPosition, basePosition);
//					
//					if (distance > 0 && thisFaction != baseFaction)
//					{
//						distances.Insert(distance);
//						enemyBases.Insert(base);
//						PrintFormat("GetCustomRadioAntennaRange: Distance Added: %1", distance);
//					}
//					
//					if (distance > 0 && thisFaction == baseFaction)
//					{
//						connectedBases.Insert(base);
//						connectedDistances.Insert(distance);
//					}
//				}		
//			}
//
//			distances.Sort();
//			connectedDistances.Sort();
//
//			for (int i = 0; i < distances.Count() - 1; i++)
//			{
//				float diff;
//				diff = distances[i++] - distances[i];
//				differences.Insert(diff);
//			}
//			
////			differences.Sort(true);
//			foreach (float difference : differences)
//			{
//				PrintFormat("GetCustomRadioAntennaRange: Difference: %1", difference);
//			}
//			
//			PrintFormat("GetCustomRadioAntennaRange: Largest Difference: %1 | Total Count: %2", differences[0], distances.Count());
//			
////			float sum;
////			float div = count * 1.4;
////			
////			PrintFormat("GetCustomRadioAntennaRange: Count: %1", count);
////			foreach (float distance : distances)
////			{
////				sum += distance;
////			}
////			
////			PrintFormat("GetCustomRadioAntennaRange: Sum: %1", sum);
////			if (sum > m_fRadioAntennaServiceRange)
////			{
////				customRadioRange = Math.Ceil(sum / div);
////			}			
//
//		}
//		
//		PrintFormat("GetCustomRadioAntennaRange: Returning range: %1", customRadioRange);
//		return customRadioRange;
//	}
	
	//------------------------------------------------------------------------------------------------ v3
	//! \return
//	float GetCustomRadioAntennaRange()
//	{
//		float customRadioRange = m_fRadioAntennaServiceRange;
//		
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (!CampaignInfo)
//			return customRadioRange;
//		
//		if (CampaignInfo.GetAreBasesRandomized())
//		{
//			SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
//			if (!baseManager)
//				return customRadioRange;
//			
//			array<SCR_MilitaryBaseComponent>bases = {};
//			array<SCR_CampaignMilitaryBaseComponent>basesAtRange = {};
//			baseManager.GetBases(bases);
//			
//			array<float>distances = {};
//			SCR_CampaignMilitaryBaseComponent previousBase;
//			Faction baseFaction = GetFaction();
//			vector thisPosition = GetOwner().GetOrigin();
//					
//			foreach (SCR_MilitaryBaseComponent base : bases)
//			{
//				SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//				if (campaignBase && campaignBase.IsInitialized() && !campaignBase.IsHQ())
//				{
//					float distance = GetDistanceToBase(thisPosition, base);
//					basesAtRange.Insert(campaignBase);
////					distances.Insert(distance);
////					PrintFormat("GetCustomRadioAntennaRange: Distance Added: %1", distance);
//				}
//			}
//			
//			SCR_CampaignMilitaryBaseComponent closestBaseToMe = FindNextClosestBase(basesAtRange, this, thisPosition);
//			float distanceToMe = GetDistanceToBase(thisPosition, closestBaseToMe);
//			PrintFormat("GetCustomRadioAntennaRange: Nearest Distance To This Base: %1", distanceToMe);
//			float largestDistance;
//			foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
//			{
//				vector basePosition = base.GetOwner().GetOrigin();
//				SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
//				float distance = GetDistanceToBase(basePosition, closestBase);
//				
//				if (!distances.Contains(distance))
//				{
//					distances.Insert(distance);
//				}
//			}
//			
//			distances.Sort(true);
//			
//			foreach (float distance : distances)
//			{
//				PrintFormat("GetCustomRadioAntennaRange: Distance: %1", distance);
//			}
//
//			if (distances[0] > customRadioRange)
//			{
//				customRadioRange = Math.Ceil(distances[0] + 70);
//			}
//			
//			if (distanceToMe > customRadioRange)
//			{
//				customRadioRange = distanceToMe + 70;
//			}
//		}
//			
//		PrintFormat("GetCustomRadioAntennaRange: Returning Distance: %1", customRadioRange);
//		
//		return customRadioRange;
//	}
	
	//------------------------------------------------------------------------------------------------ v4 ***
	//! \return
//	float GetCustomRadioAntennaRange()
//	{
//		float customRadioRange = m_fRadioAntennaServiceRange;
//		
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (!CampaignInfo)
//			return customRadioRange;
//		
//		if (CampaignInfo.GetAreBasesRandomized())
//		{
//			SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
//			if (!baseManager)
//				return customRadioRange;
//			
//			array<SCR_MilitaryBaseComponent>bases = {};
//			array<SCR_CampaignMilitaryBaseComponent>basesAtRange = {};
//			baseManager.GetBases(bases);
//			
//			array<float>distances = {};
//			SCR_CampaignMilitaryBaseComponent previousBase;
//			Faction baseFaction = GetFaction();
//			vector thisPosition = GetOwner().GetOrigin();
//					
//			foreach (SCR_MilitaryBaseComponent base : bases)
//			{
//				SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//				if (campaignBase && campaignBase.IsInitialized() && !campaignBase.IsHQ())
//				{
//					float distance = GetDistanceToBase(thisPosition, base);
//					basesAtRange.Insert(campaignBase);
////					distances.Insert(distance);
////					PrintFormat("GetCustomRadioAntennaRange: Distance Added: %1", distance);
//				}
//			}
//			
//			float largestDistance;
//			foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
//			{
//				vector basePosition = base.GetOwner().GetOrigin();
//				SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
//				float distance = GetDistanceToBase(basePosition, closestBase);
//				
//				if (!distances.Contains(distance))
//				{
//					distances.Insert(distance);
//				}
//			}
//			
//			distances.Sort(true);
//			
//			SCR_CampaignMilitaryBaseComponent closestBaseToMe = FindNextClosestBase(basesAtRange, this, thisPosition);
//			float distanceToMe = GetDistanceToBase(thisPosition, closestBaseToMe);
////			PrintFormat("GetCustomRadioAntennaRange: Nearest Distance To This Base: %1", distanceToMe);
//			
//			foreach (float distance : distances)
//			{
////				PrintFormat("GetCustomRadioAntennaRange: Distance: %1", distance);				
//			}
//			
//			foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
//			{
//				vector basePosition = base.GetOwner().GetOrigin();
//				SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
//				float distance = GetDistanceToBase(basePosition, closestBase);
//				
//				if (distance > customRadioRange)
//				{
//					distance = Math.Ceil(distances[0] + 100);
//					base.SetIsFringeBase(true);
//					closestBase.SetIsFringeBase(true);
//					base.SetRadioAntennaServiceRange(distance);
//					closestBase.SetRadioAntennaServiceRange(distance);
//				}
//			}
//	
//			if (GetIsFringeBase())
//			{
////				Print("GetCustomRadioAntennaRange: I'm a FRINGE Base!");
//				customRadioRange = GetRadioAntennaServiceRange();
//			}
//		}
//			
////		PrintFormat("GetCustomRadioAntennaRange: Returning Distance: %1", customRadioRange);
//		
//		return customRadioRange;
//	}
	
	//------------------------------------------------------------------------------------------------ v5 **** Full Coverage
//	float GetCustomRadioAntennaRange()
//	{		
//		float customRadioRange = m_fRadioAntennaServiceRange;
//		
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (!CampaignInfo)
//			return customRadioRange;
//		
//		if (CampaignInfo.GetAreBasesRandomized())
//		{
//			SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
//			if (!baseManager)
//				return customRadioRange;
//			
//			array<SCR_MilitaryBaseComponent>bases = {};
//			array<SCR_CampaignMilitaryBaseComponent>basesAtRange = {};
//			baseManager.GetBases(bases);
//			
//			array<float>distances = {};
//			SCR_CampaignMilitaryBaseComponent previousBase;
//			Faction baseFaction = GetFaction();
//			vector thisPosition = GetOwner().GetOrigin();
//					
//			foreach (SCR_MilitaryBaseComponent base : bases)
//			{
//				SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//				if (campaignBase && campaignBase.IsInitialized() && !campaignBase.IsHQ() && campaignBase.GetType() != SCR_ECampaignBaseType.RELAY)
//				{
//					float distance = GetDistanceToBase(thisPosition, base);
//					basesAtRange.Insert(campaignBase);
//				}
//			}
//			
//			// Process all bases and set Fringe status
//			foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
//			{
//				array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
//				excludeBases.Insert(base);
//				
//				vector basePosition = base.GetOwner().GetOrigin();
//				SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
//				excludeBases.Insert(closestBase);
//				
//				float distance = GetDistanceToBase(basePosition, closestBase);
//				if (distance > customRadioRange)
//				{
////					PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 (%3) | Next Closest: %2 (%4)", closestBase.GetBaseName(), nextClosestBase.GetBaseName(), distance, distanceToNextClosestBase);
//
//					base.SetIsFringeBase(true);
//					closestBase.SetIsFringeBase(true);
//					
//					base.SetRadioAntennaServiceRange(distance + 100);
//					closestBase.SetRadioAntennaServiceRange(distance + 100);
//				}
//			}
//	
//			// Check closest bases to me
//			vector myPosition = GetOwner().GetOrigin();
//			SCR_CampaignMilitaryBaseComponent closestBaseToMe = FindNextClosestBase(basesAtRange, this, myPosition);
//			float distanceToMe = GetDistanceToBase(myPosition, closestBaseToMe);
//			
//			array<SCR_CampaignMilitaryBaseComponent> closestBases = {};
//			array<SCR_CampaignMilitaryBaseComponent> closestBasesFiltered = {};
//			
//			FindNext5ClosestBases(basesAtRange, this, myPosition, closestBases);
////			FindNext3ClosestBases(basesAtRange, this, myPosition, closestBases);
//			int totalClosestBases;
//			float totalDistance;
//			
//			foreach (SCR_CampaignMilitaryBaseComponent base : closestBases)
//			{
//				float distance = GetDistanceToBase(myPosition, base);
//				if (distance < customRadioRange)
//				{
//					continue;
//				}
//				else
//				{
//					totalClosestBases++;
//					totalDistance += distance;
//					closestBasesFiltered.Insert(base);
//				}
//				
//				PrintFormat("GetCustomRadioAntennaRange: Base: %1 | Distance: %2", base.GetBaseName(), distance);				
//			}
//			
//			if (totalClosestBases > 0)
//			{
//				customRadioRange = totalDistance / totalClosestBases;
//			}
//			
//			foreach (SCR_CampaignMilitaryBaseComponent base : closestBasesFiltered)
//			{
//				if (base.GetRadioAntennaServiceRange() < customRadioRange && !base.GetIsConnectingBase())
//				{
//					base.SetRadioAntennaServiceRange(customRadioRange);
//					base.SetIsConnectingBase(true);
//					PrintFormat("GetCustomRadioAntennaRange: Set Base (%1) to: %2", base.GetBaseName(), customRadioRange);
//				}
//			}
//			
//			if (distanceToMe > customRadioRange)
//			{
//				SetIsFringeBase(true);
//				closestBaseToMe.SetIsFringeBase(true);
//				
//				float closestBaseRange = closestBaseToMe.GetRadioAntennaServiceRange();
//				if (closestBaseRange < distanceToMe)
//				{
//					closestBaseToMe.SetRadioAntennaServiceRange(distanceToMe);
//				}
//				
//				if (GetRadioAntennaServiceRange() < distanceToMe)
//				{
//					SetRadioAntennaServiceRange(distanceToMe);
//				}			
//			}
//			
//			if (GetIsFringeBase())
//			{
//#ifdef WORKBENCH
//				Print("GetCustomRadioAntennaRange: I'm a FRINGE Base!");
//#endif
//				customRadioRange = GetRadioAntennaServiceRange() + 100;
//			}
//			
//			// Check next closest base to me?
//		}
//			
//		
//		PrintFormat("GetCustomRadioAntennaRange: Returning Distance: %1", customRadioRange);
//		
//		return customRadioRange;
//		
//	    // Default range if conditions aren't met
////	    float customRadioRange = m_fRadioAntennaServiceRange;
////	
////	    SCR_GameModeCampaign campaignInfo = SCR_GameModeCampaign.GetInstance();
////	    if (!campaignInfo || !campaignInfo.GetAreBasesRandomized())
////		{
////			return customRadioRange;
////		}
////
////	    SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
////	    if (!baseManager)
////		{
////			return customRadioRange;
////		}
////
////	    array<SCR_CampaignMilitaryBaseComponent> campaignBases = {};
////		array<SCR_CampaignMilitaryBaseComponent> HQBases = {};
////	    array<SCR_MilitaryBaseComponent> bases = {};
////	    baseManager.GetBases(bases);
////	
////	    foreach (SCR_MilitaryBaseComponent base : bases)
////	    {
////	        SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
////	        if (campaignBase && campaignBase.IsInitialized())
////			{
////				if (campaignBase.IsHQ())
////				{
////					HQBases.Insert(campaignBase);
////				}
////				else
////				{
////					campaignBases.Insert(campaignBase);	
////				}					
////			}	            
////	    }
////	
////	    if (campaignBases.IsEmpty())
////		{
////			return customRadioRange;
////		}
////
////	    // Calculate distances to closest bases and find the maximum
////	    map<SCR_CampaignMilitaryBaseComponent, float> closestDistances = new map<SCR_CampaignMilitaryBaseComponent, float>;
////	    float maxClosestDistance = 0;
////	
//////	    for (int i = 0; i < campaignBases.Count(); i++)
//////	    {
//////			array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
//////			
//////	        SCR_CampaignMilitaryBaseComponent base = campaignBases[i];
//////			excludeBases.Insert(base);
//////	        vector basePosition = base.GetOwner().GetOrigin();
//////	        SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(campaignBases, base, basePosition);
//////			excludeBases.Insert(closestBase);
//////			
//////			vector closestBasePosition = closestBase.GetOwner().GetOrigin();
//////			SCR_CampaignMilitaryBaseComponent nextClosestBase = FindNextClosestBases(campaignBases, excludeBases, closestBasePosition);
//////	        if (nextClosestBase)
//////	        {
//////	            float nextClosestDistance = GetDistanceToBase(basePosition, nextClosestBase);
//////	            closestDistances[base] = nextClosestDistance;
//////	            if (nextClosestDistance > maxClosestDistance)
//////				{
//////					maxClosestDistance = nextClosestDistance;				
//////				}    
//////				
//////				PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 | Next Closest: %2", closestBase, nextClosestBase);            
//////	        }
//////	    }
////	
////		array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
////		excludeBases.Insert(this);
////		vector thisBasePosition = GetOwner().GetOrigin();
////		SCR_CampaignMilitaryBaseComponent closestBaseFromHere = FindNextClosestBase(campaignBases, this, thisBasePosition);
////		excludeBases.Insert(closestBaseFromHere);
////		float distanceToClosestBase = GetDistanceToBase(thisBasePosition, closestBaseFromHere);
////		
////		vector closestBasePosition = closestBaseFromHere.GetOwner().GetOrigin();
////		SCR_CampaignMilitaryBaseComponent nextClosestBase = FindNextClosestBases(campaignBases, excludeBases, closestBasePosition);
////		float distanceToNextClosestBase = GetDistanceToBase(thisBasePosition, nextClosestBase);
////		
////		PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 (%3) | Next Closest: %2 (%4)", closestBaseFromHere.GetBaseName(), nextClosestBase.GetBaseName(), distanceToClosestBase, distanceToNextClosestBase);
////		
////		if (distanceToClosestBase > customRadioRange && distanceToNextClosestBase > customRadioRange)
////		{
////			maxClosestDistance = distanceToNextClosestBase;
////		}
////		else
////		{
////			maxClosestDistance = customRadioRange;
////		}
////		
////	    // Set extended range for fringe bases and their closest neighbors
////	    float extendedRange = Math.Ceil(maxClosestDistance + 100);
////		
////		foreach (SCR_CampaignMilitaryBaseComponent campaignBase : HQBases)
////		{
////			vector basePosition = campaignBase.GetOwner().GetOrigin();
////	        SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(campaignBases, campaignBase, basePosition);
////			if (closestBase)
////			{
////				float distance = GetDistanceToBase(basePosition, closestBase);
////				if (distance > extendedRange)
////				{
////					closestBase.SetIsFringeBase(true);
////					closestBase.SetRadioAntennaServiceRange(distance);
////				}
////			}			
////		}
////		
////	    for (int i = 0; i < campaignBases.Count(); i++)
////	    {
////	        SCR_CampaignMilitaryBaseComponent base = campaignBases[i];
////	        float distance = closestDistances[base];
////			
////	        // Only set fringe bases for the pair with the largest distances
////	        if (distance == maxClosestDistance)
////	        {
////	            base.SetIsFringeBase(true);
////	            base.SetRadioAntennaServiceRange(extendedRange);
////	            SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(campaignBases, base, base.GetOwner().GetOrigin());
////	            if (closestBase)// || closestBase.IsHQ() && distance > customRadioRange)
//                                                                                                                                                                                                     //	            {
////	                closestBase.SetIsFringeBase(true);
////	                closestBase.SetRadioAntennaServiceRange(extendedRange);
////	            }
////	        }
////	    }
////	
////	    if (GetIsFringeBase())
////		{
////			PrintFormat("GetCustomRadioAntennaRange: Fringe Base: Returning: %1", GetRadioAntennaServiceRange());
////			return GetRadioAntennaServiceRange();
////		}
////	        
////		PrintFormat("GetCustomRadioAntennaRange: Returning: %1", customRadioRange);
////	    return customRadioRange;
//	}
	
	//------------------------------------------------------------------------------------------------ v6 **** More Linear
//	float GetCustomRadioAntennaRange()
//	{
//		float customRadioRange = m_fRadioAntennaServiceRange;
//		
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (!CampaignInfo)
//			return customRadioRange;
//		
//		if (CampaignInfo.GetAreBasesRandomized())
//		{
//			SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
//			if (!baseManager)
//				return customRadioRange;
//			
//			array<SCR_MilitaryBaseComponent>bases = {};
//			array<SCR_CampaignMilitaryBaseComponent>basesAtRange = {};
//			baseManager.GetBases(bases);
//			
//			array<float>distances = {};
//			SCR_CampaignMilitaryBaseComponent previousBase;
//			Faction baseFaction = GetFaction();
//			vector thisPosition = GetOwner().GetOrigin();
//					
//			foreach (SCR_MilitaryBaseComponent base : bases)
//			{
//				SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//				if (campaignBase && campaignBase.IsInitialized() && !campaignBase.IsHQ() && campaignBase.GetType() != SCR_ECampaignBaseType.RELAY)
//				{
//					float distance = GetDistanceToBase(thisPosition, base);
//					basesAtRange.Insert(campaignBase);
//				}
//			}
//			
//			// Process all bases and set Fringe status
//			foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
//			{
//				array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
//				excludeBases.Insert(base);
//				
//				vector basePosition = base.GetOwner().GetOrigin();
//				SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
//				excludeBases.Insert(closestBase);
//				
//				float distance = GetDistanceToBase(basePosition, closestBase);
//				if (distance > customRadioRange)
//				{
////					PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 (%3) | Next Closest: %2 (%4)", closestBase.GetBaseName(), nextClosestBase.GetBaseName(), distance, distanceToNextClosestBase);
//
//					base.SetIsFringeBase(true);
//					closestBase.SetIsFringeBase(true);
//					
//					base.SetRadioAntennaServiceRange(distance + 100);
//					closestBase.SetRadioAntennaServiceRange(distance + 100);
//				}
//			}
//	
//			// Check closest bases to me
//			vector myPosition = GetOwner().GetOrigin();
//			
//			array<SCR_CampaignMilitaryBaseComponent> closestBases = {};
//			array<SCR_CampaignMilitaryBaseComponent> closestFriendlyBases = {};
//			array<SCR_CampaignMilitaryBaseComponent> outOfRangeBases = {};
//			array<float> outOfRangeDistances = {};
//			array<float> closestFriendlyDistances = {};
////			FindNext5ClosestBases(basesAtRange, this, myPosition, closestBases);
//			FindNextNClosestBases(3, basesAtRange, this, myPosition, closestBases);
//			int totalClosestBases;
//			float totalDistance;
//			float closestFriendlyBaseDistance;
//			Faction thisBaseFaction = GetFaction();
//
//			foreach (SCR_CampaignMilitaryBaseComponent base : closestBases)
//			{
//				float distance = GetDistanceToBase(myPosition, base);
////				if (distance < customRadioRange || base.GetFaction() == thisBaseFaction)
//				if (base.GetFaction() == thisBaseFaction && !base.IsHQ())
//				{			
//					closestFriendlyDistances.Insert(distance);
//					closestFriendlyBases.Insert(base);
//					continue;
//				}
//				else
//				{
//					totalClosestBases++;
//					totalDistance += distance;
//					if (distance > customRadioRange)
//					{
//						outOfRangeBases.Insert(base);
//						outOfRangeDistances.Insert(distance);
//					}					
//				}
//				
//				PrintFormat("GetCustomRadioAntennaRange: Base: %1 | Distance: %2", base.GetBaseName(), distance);
//				
////				if (totalClosestBases > 0)
////				{
////					customRadioRange = totalDistance / totalClosestBases;
////					if (base.GetRadioAntennaServiceRange() < customRadioRange)
////					{
////						base.SetRadioAntennaServiceRange(customRadioRange);
////						PrintFormat("GetCustomRadioAntennaRange: Setting Base (%1) to: %2", base.GetBaseName(), customRadioRange);
////						break;
////					}					
////				}				
//			}
//			
//			if (outOfRangeDistances.Count() > 1)
//			{
//				outOfRangeDistances.Sort(true);
//			}
//			
////			if (closestFriendlyDistances.Count() > 1)
////			{
////				closestFriendlyDistances.Sort();
////				closestFriendlyBaseDistance = closestFriendlyDistances[0];
////			}
////			
////			if (closestFriendlyBaseDistance > customRadioRange)
////			{
////				customRadioRange = closestFriendlyBaseDistance;
////			}
//			
//			//3, basesAtRange, this, myPosition, closestBases
//			SCR_CampaignMilitaryBaseComponent closestFriendlyBase = FindClosestFriendlyBase(basesAtRange, this, myPosition, thisBaseFaction);
//			
//			if (closestFriendlyBase)
//			{
//				closestFriendlyBaseDistance = GetDistanceToBase(myPosition, closestFriendlyBase);
//				if (closestFriendlyBaseDistance > customRadioRange)
//				{
//					customRadioRange = closestFriendlyBaseDistance;
//				}
//			}
//		
////			if (totalClosestBases > 2)
//			if (outOfRangeBases.Count() > 0 && (totalDistance / totalClosestBases) > customRadioRange) // > 1
//			{
//				customRadioRange = totalDistance / totalClosestBases;
//			}
//			
////			else if (outOfRangeDistances.Count() > 0 && outOfRangeDistances[0] > customRadioRange)
////			{
////				customRadioRange = outOfRangeDistances[0];
////			}
//			
////			foreach (SCR_CampaignMilitaryBaseComponent base : outOfRangeBases)
////			{
////				if (base.GetRadioAntennaServiceRange() < customRadioRange)// && !base.GetIsConnectingBase())
////				{
////					base.SetRadioAntennaServiceRange(customRadioRange);
////					base.SetIsConnectingBase(true);
////					PrintFormat("GetCustomRadioAntennaRange: Set Base (%1) to: %2", base.GetBaseName(), customRadioRange);
////				}
////			}
//			
//			SCR_CampaignMilitaryBaseComponent closestBaseToMe = FindNextClosestBase(basesAtRange, this, myPosition);
//			float distanceToMe = GetDistanceToBase(myPosition, closestBaseToMe);
//			
//			if (distanceToMe > customRadioRange)
//			{
//				SetIsFringeBase(true);
//				closestBaseToMe.SetIsFringeBase(true);
//				
//				float closestBaseRange = closestBaseToMe.GetRadioAntennaServiceRange();
//				if (closestBaseRange < distanceToMe)
//				{
//					closestBaseToMe.SetRadioAntennaServiceRange(distanceToMe);
//				}
//				
//				if (GetRadioAntennaServiceRange() < distanceToMe)
//				{
//					SetRadioAntennaServiceRange(distanceToMe);
//				}			
//			}
//			
//			if (GetIsFringeBase())
//			{
//#ifdef WORKBENCH
//				Print("GetCustomRadioAntennaRange: I'm a FRINGE Base!");
//#endif
//				customRadioRange = GetRadioAntennaServiceRange() + 100;
//			}
//			
//			// Check next closest base to me?
//		}
//			
//		
//		PrintFormat("GetCustomRadioAntennaRange: Returning Distance: %1", customRadioRange);
//		
//		return customRadioRange;
//		
//	    // Default range if conditions aren't met
////	    float customRadioRange = m_fRadioAntennaServiceRange;
////	
////	    SCR_GameModeCampaign campaignInfo = SCR_GameModeCampaign.GetInstance();
////	    if (!campaignInfo || !campaignInfo.GetAreBasesRandomized())
////		{
////			return customRadioRange;
////		}
////
////	    SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
////	    if (!baseManager)
////		{
////			return customRadioRange;
////		}
////
////	    array<SCR_CampaignMilitaryBaseComponent> campaignBases = {};
////		array<SCR_CampaignMilitaryBaseComponent> HQBases = {};
////	    array<SCR_MilitaryBaseComponent> bases = {};
////	    baseManager.GetBases(bases);
////	
////	    foreach (SCR_MilitaryBaseComponent base : bases)
////	    {
////	        SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
////	        if (campaignBase && campaignBase.IsInitialized())
////			{
////				if (campaignBase.IsHQ())
////				{
////					HQBases.Insert(campaignBase);
////				}
////				else
////				{
////					campaignBases.Insert(campaignBase);	
////				}					
////			}	            
////	    }
////	
////	    if (campaignBases.IsEmpty())
////		{
////			return customRadioRange;
////		}
////
////	    // Calculate distances to closest bases and find the maximum
////	    map<SCR_CampaignMilitaryBaseComponent, float> closestDistances = new map<SCR_CampaignMilitaryBaseComponent, float>;
////	    float maxClosestDistance = 0;
////	
//////	    for (int i = 0; i < campaignBases.Count(); i++)
//////	    {
//////			array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
//////			
//////	        SCR_CampaignMilitaryBaseComponent base = campaignBases[i];
//////			excludeBases.Insert(base);
//////	        vector basePosition = base.GetOwner().GetOrigin();
//////	        SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(campaignBases, base, basePosition);
//////			excludeBases.Insert(closestBase);
//////			
//////			vector closestBasePosition = closestBase.GetOwner().GetOrigin();
//////			SCR_CampaignMilitaryBaseComponent nextClosestBase = FindNextClosestBases(campaignBases, excludeBases, closestBasePosition);
//////	        if (nextClosestBase)
//////	        {
//////	            float nextClosestDistance = GetDistanceToBase(basePosition, nextClosestBase);
//////	            closestDistances[base] = nextClosestDistance;
//////	            if (nextClosestDistance > maxClosestDistance)
//////				{
//////					maxClosestDistance = nextClosestDistance;				
//////				}    
//////				
//////				PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 | Next Closest: %2", closestBase, nextClosestBase);            
//////	        }
//////	    }
////	
////		array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
////		excludeBases.Insert(this);
////		vector thisBasePosition = GetOwner().GetOrigin();
////		SCR_CampaignMilitaryBaseComponent closestBaseFromHere = FindNextClosestBase(campaignBases, this, thisBasePosition);
////		excludeBases.Insert(closestBaseFromHere);
////		float distanceToClosestBase = GetDistanceToBase(thisBasePosition, closestBaseFromHere);
////		
////		vector closestBasePosition = closestBaseFromHere.GetOwner().GetOrigin();
////		SCR_CampaignMilitaryBaseComponent nextClosestBase = FindNextClosestBases(campaignBases, excludeBases, closestBasePosition);
////		float distanceToNextClosestBase = GetDistanceToBase(thisBasePosition, nextClosestBase);
////		
////		PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 (%3) | Next Closest: %2 (%4)", closestBaseFromHere.GetBaseName(), nextClosestBase.GetBaseName(), distanceToClosestBase, distanceToNextClosestBase);
////		
////		if (distanceToClosestBase > customRadioRange && distanceToNextClosestBase > customRadioRange)
////		{
////			maxClosestDistance = distanceToNextClosestBase;
////		}
////		else
////		{
////			maxClosestDistance = customRadioRange;
////		}
////		
////	    // Set extended range for fringe bases and their closest neighbors
////	    float extendedRange = Math.Ceil(maxClosestDistance + 100);
////		
////		foreach (SCR_CampaignMilitaryBaseComponent campaignBase : HQBases)
////		{
////			vector basePosition = campaignBase.GetOwner().GetOrigin();
////	        SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(campaignBases, campaignBase, basePosition);
////			if (closestBase)
////			{
////				float distance = GetDistanceToBase(basePosition, closestBase);
////				if (distance > extendedRange)
////				{
////					closestBase.SetIsFringeBase(true);
////					closestBase.SetRadioAntennaServiceRange(distance);
////				}
////			}			
////		}
////		
////	    for (int i = 0; i < campaignBases.Count(); i++)
////	    {
////	        SCR_CampaignMilitaryBaseComponent base = campaignBases[i];
////	        float distance = closestDistances[base];
////			
////	        // Only set fringe bases for the pair with the largest distances
////	        if (distance == maxClosestDistance)
////	        {
////	            base.SetIsFringeBase(true);
////	            base.SetRadioAntennaServiceRange(extendedRange);
////	            SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(campaignBases, base, base.GetOwner().GetOrigin());
////	            if (closestBase)// || closestBase.IsHQ() && distance > customRadioRange)
////	            {
////	                closestBase.SetIsFringeBase(true);
////	                closestBase.SetRadioAntennaServiceRange(extendedRange);
////	            }
////	        }
////	    }
////	
////	    if (GetIsFringeBase())
////		{
////			PrintFormat("GetCustomRadioAntennaRange: Fringe Base: Returning: %1", GetRadioAntennaServiceRange());
////			return GetRadioAntennaServiceRange();
////		}
////	        
////		PrintFormat("GetCustomRadioAntennaRange: Returning: %1", customRadioRange);
////	    return customRadioRange;
//	}
	
	//------------------------------------------------------------------------------------------------ v7 Default
	float GetCustomRadioAntennaRange()
	{
		float customRadioRange = m_fRadioAntennaServiceRange;
	
	    SCR_GameModeCampaign campaignInfo = SCR_GameModeCampaign.GetInstance();
	    if (!campaignInfo || !campaignInfo.GetAreBasesRandomized())
		{
			return customRadioRange;
		}
		
		PrintFormat("GetCustomRadioAntennaRange: Distance: %1", customRadioRange);
		return customRadioRange;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDistanceToBase(vector currentPosition, SCR_MilitaryBaseComponent targetBase)
	{
		float distance;
		
		vector basePosition = targetBase.GetOwner().GetOrigin();
		distance = vector.DistanceXZ(currentPosition, basePosition);
		
		return distance;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent FindNextClosestBase(array<SCR_CampaignMilitaryBaseComponent>bases, SCR_CampaignMilitaryBaseComponent excludedBase, vector position)
	{
		SCR_CampaignMilitaryBaseComponent closestBase;
		SCR_CampaignMilitaryBaseComponent nextClosestBase;
		float closestBaseDistance = float.MAX;

		foreach (SCR_CampaignMilitaryBaseComponent base : bases)
		{
			if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY)
				continue;

			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);

			if (distance < closestBaseDistance)
			{
				closestBaseDistance = distance;
				closestBase = base;
			}
		}

		return closestBase;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent FindClosestFriendlyBase(array<SCR_CampaignMilitaryBaseComponent>bases, SCR_CampaignMilitaryBaseComponent excludedBase, vector position, Faction baseFaction)
	{
		SCR_CampaignMilitaryBaseComponent closestBase;
		SCR_CampaignMilitaryBaseComponent nextClosestBase;
		float closestBaseDistance = float.MAX;

		foreach (SCR_CampaignMilitaryBaseComponent base : bases)
		{
			if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY || base.GetFaction() != baseFaction)
				continue;

			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);

			if (distance < closestBaseDistance)
			{
				closestBaseDistance = distance;
				closestBase = base;
			}
		}

		return closestBase;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent FindNextClosestBases(array<SCR_CampaignMilitaryBaseComponent>bases, array<SCR_CampaignMilitaryBaseComponent> excludedBases, vector position)
	{
		SCR_CampaignMilitaryBaseComponent closestBase;
		SCR_CampaignMilitaryBaseComponent nextClosestBase;
		float closestBaseDistance = float.MAX;

		foreach (SCR_CampaignMilitaryBaseComponent base : bases)
		{
			if (!base.IsInitialized() || excludedBases.Contains(base) || base.GetType() == SCR_ECampaignBaseType.RELAY)
				continue;

			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);

			if (distance < closestBaseDistance)
			{
				closestBaseDistance = distance;
				closestBase = base;
			}
		}

		return closestBase;
	}
	
	//------------------------------------------------------------------------------------------------
	void FindNextNClosestBases(int baseCount, array<SCR_CampaignMilitaryBaseComponent>bases, SCR_CampaignMilitaryBaseComponent excludedBase, vector position, out array<SCR_CampaignMilitaryBaseComponent> outBases)
	{
		array<SCR_CampaignMilitaryBaseComponent> closestBases = {};
	    array<float> closestDistances = {};
	
	    foreach (SCR_CampaignMilitaryBaseComponent base : bases)
	    {
	        // Skip invalid, excluded, or relay bases
	        if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY)
	            continue;
	
	        float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);
	
	        if (closestBases.Count() < baseCount)
	        {
	            closestBases.Insert(base);
	            closestDistances.Insert(distance);
	        }
	        else
	        {
	            // Find the index of the farthest base in the current set
	            int maxIndex = 0;
	            for (int i = 1; i < closestDistances.Count(); i++)
	            {
	                if (closestDistances[i] > closestDistances[maxIndex])
	                    maxIndex = i;
	            }
	
	            // If the new base is closer than the farthest, replace it
	            if (distance < closestDistances[maxIndex])
	            {
	                closestBases[maxIndex] = base;
	                closestDistances[maxIndex] = distance;
	            }
	        }
	    }
	
	    outBases = closestBases;
	}
	
	//------------------------------------------------------------------------------------------------
	void FindNext5ClosestBases(array<SCR_CampaignMilitaryBaseComponent>bases, SCR_CampaignMilitaryBaseComponent excludedBase, vector position, out array<SCR_CampaignMilitaryBaseComponent> outBases)
	{
		 array<SCR_CampaignMilitaryBaseComponent> closestBases = {};
	    array<float> closestDistances = {};
	
	    foreach (SCR_CampaignMilitaryBaseComponent base : bases)
	    {
	        // Skip invalid, excluded, or relay bases
	        if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY)
	            continue;
	
	        float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);
	
	        if (closestBases.Count() < 5)
	        {
	            closestBases.Insert(base);
	            closestDistances.Insert(distance);
	        }
	        else
	        {
	            // Find the index of the farthest base in the current set
	            int maxIndex = 0;
	            for (int i = 1; i < closestDistances.Count(); i++)
	            {
	                if (closestDistances[i] > closestDistances[maxIndex])
	                    maxIndex = i;
	            }
	
	            // If the new base is closer than the farthest, replace it
	            if (distance < closestDistances[maxIndex])
	            {
	                closestBases[maxIndex] = base;
	                closestDistances[maxIndex] = distance;
	            }
	        }
	    }
	
	    outBases = closestBases;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetFrontlineRespawnTime()
	{
		return m_iFrontlineRespawnTime;
	}
	
	//------------------------------------------------------------------------------------------------
	AADBaseType GetAADBaseType()
	{
		return m_AADBaseType;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsBaseOnIsland()
	{
		return m_bIsOnIsland;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetExcludedFromRandomization()
	{
		return m_bExcludeFromRandomization;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsFringeBase()
	{
		return m_bIsFringeBase;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetIsFringeBase(bool value)
	{
		m_bIsFringeBase = value;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsConnectingBase()
	{
		return m_bIsConnectingBase;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetIsConnectingBase(bool value)
	{
		m_bIsConnectingBase = value;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetRadioAntennaServiceRange()
	{
		return m_fRadioAntennaServiceRange;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetRadioAntennaServiceRange(float value)
	{	
		m_fRadioAntennaServiceRange = value;
	}
	
	//------------------------------------------------------------------------------------------------	
	string GetBasesGroupName()
	{
		return m_sGroupName;
	}

	//------------------------------------------------------------------------------------------------
	bool GetBaseHasGroup()
	{
		return m_bHasGroup;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetScorePerTick()
	{
		return m_iScorePerTick;
	}
	
		//------------------------------------------------------------------------------------------------
	bool GetIsRegistered()
	{
		return m_bIsRegistered;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetIsRegistered(bool value)
	{
		m_bIsRegistered = value;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsProcessed()
	{
		retur