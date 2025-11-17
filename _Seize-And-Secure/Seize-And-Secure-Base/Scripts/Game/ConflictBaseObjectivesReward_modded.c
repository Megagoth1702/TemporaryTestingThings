//modded class SCR_CampaignMilitaryBaseComponent
//{
//	[Attribute("1000", UIWidgets.Slider, "Duration in milliseconds before each point is awarded.", params: "1 60000 1*", category: "Reward")]
//	protected int m_fTickRate;
//
//	[Attribute("10", UIWidgets.Slider, "Score awarded per tick.", params: "0 1000 1", category: "Reward")]
//	protected int m_iScorePerTick;
//
//
//	[Attribute("0", UIWidgets.Slider, "Score awarded per tick when contested.", params: "0 1000 1", category: "Reward")]
//	protected int m_iScorePerTickContested;
//	
//	protected float m_ObjectivesReward_fTimeOfLastTick_modded;
//	protected float m_ObjectivesReward_fTickTime_modded;
//	Faction m_fFactionOwner;
//	protected bool initialized;
//	
//	SCR_GameModeCampaign CampaignInfo;
//	string m_sAASAttackingFaction;
//	string m_sAASDefendingFaction;
//	int m_iMaxScore;
	
	//------------------------------------------------------------------------------------------------
//	override void OnPostInit(IEntity owner)
//	{
//		super.OnPostInit(owner);
//		
//		CampaignInfo = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
//		
//		if (CampaignInfo)
//		{
//			m_sAASDefendingFaction = CampaignInfo.GetAADDefendingFaction();
//			m_sAASAttackingFaction = CampaignInfo.GetAADAttackingFaction();
//		}
//	}
//	
	// This is a mixup of SupplyIncomeTimer from SCR_CampaignMilitaryBaseComponent and OnFrame in SCR_CaptureAndHoldArea
	//------------------------------------------------------------------------------------------------
//	protected void ObjectivesRewardTimer_Modded(bool reset = false)
//	{
//		if (IsProxy())
//			return;
//		
//		ChimeraWorld world = GetOwner().GetWorld();
//		m_fFactionOwner = GetFaction();
//		
//		if (!world)
//		{
//			m_ObjectivesReward_fTickTime_modded = 0;
//			return;
//		}
//
//		if (reset)
//		{
//			
//			m_ObjectivesReward_fTickTime_modded = 0;
//			m_ObjectivesReward_fTimeOfLastTick_modded = world.GetWorldTime();
//			return;
//		}
//		if (IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(m_fFactionOwner), SCR_ERadioCoverageStatus.BOTH_WAYS))
//		{
//			
//			m_ObjectivesReward_fTickTime_modded += (world.GetWorldTime() - m_ObjectivesReward_fTimeOfLastTick_modded);
//		
//			while (m_ObjectivesReward_fTickTime_modded >= m_fTickRate)
//			{
//				
//				m_ObjectivesReward_fTickTime_modded -= m_fTickRate;
//				m_ObjectivesReward_fTimeOfLastTick_modded = world.GetWorldTime();
//				OnTick_ObjectivesReward_Modded();
//			}
//		}		
//	}
	
	//------------------------------------------------------------------------------------------------
//	private void OnTick_ObjectivesReward_Modded()
//	{
//		// Remove from CallQueue when signal is lost and do not reward points if HQ, if the reward is zero don't run it either
//		
//		m_fFactionOwner = GetFaction();
//		if(!m_fFactionOwner)
//		{
//			return;
//		}
//		
//		if(!IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(m_fFactionOwner), SCR_ERadioCoverageStatus.BOTH_WAYS))
//		{
//			return;
//		}		
//		
//		SCR_Faction owner_scr = SCR_Faction.Cast(m_fFactionOwner);
//		if (owner_scr)
//		{
//			
//			// Do not reward points to FIA
//			if (!owner_scr.IsPlayable())
//			{
//				return;
//			}
//		}
//		
//		if(IsHQ())
//		{
//			return;
//		}
//		
//		int reward = m_iScorePerTick;
//		
//		// I don't care that we are scoring slightly wrong by not discriminating between elapsed contested and elapsed uncontested time.
//		if (m_CapturingFaction && m_CapturingFaction != m_fFactionOwner)
//		{
//			
//			reward = m_iScorePerTickContested;
//		}
//		
//		SCR_BaseScoringSystemComponent scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
//		if (!scoringSystem)
//			return;
//		
//		SCR_CaptureAndHoldManager chManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
//		m_ObjectivesReward_fTickTime_modded = 0; // No dumping of points 
//		
//		if (!chManager.TicketScoreSystem())
//			scoringSystem.AddFactionObjective(m_fFactionOwner, reward);
//		
//		else if (chManager.TicketScoreSystem() && CampaignInfo.IsAADEnabled())
//		{
//			m_iMaxScore = chManager.GetFactionStartScore();
//			
//			if (CampaignInfo)
//			{
//				// OnPostInit may not be working to set proper attacker/defender
//				m_sAASDefendingFaction = CampaignInfo.GetAADDefendingFaction();
//				m_sAASAttackingFaction = CampaignInfo.GetAADAttackingFaction();
//			}
//			
//			if (m_fFactionOwner.GetFactionKey() == m_sAASDefendingFaction && scoringSystem.GetFactionScore(m_fFactionOwner) <= m_iMaxScore)
//				scoringSystem.AddFactionObjective(m_fFactionOwner, reward);
//		}
//	}
	
	//------------------------------------------------------------------------------------------------
//	override void Disable()
//	{
//		if (IsProxy())
//			return;
//		
//		GetGame().GetCallqueue().Remove(ObjectivesRewardTimer_Modded);
//		
//		super.Disable();		
//	}
	
	//------------------------------------------------------------------------------------------------
//	override void OnInitialized()
//	{	
//		super.OnInitialized();
//		
//		if(!IsHQ() && !IsProxy())
//		{
//			Faction owner;
//			owner = GetFaction();
//			if(!owner)
//			{
//				return;
//			}
//			
//			if(IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(owner), SCR_ERadioCoverageStatus.BOTH_WAYS))
//			{
//				ObjectivesRewardTimer_Modded(true);
//				GetGame().GetCallqueue().CallLater(ObjectivesRewardTimer_Modded, m_fTickRate, true, false);
//			}
//		}
//	}
	
	//------------------------------------------------------------------------------------------------
//	override protected void OnHQSet()
//	{
//		super.OnHQSet();
//		if(IsHQ() && !IsProxy())
//		{
//			GetGame().GetCallqueue().Remove(ObjectivesRewardTimer_Modded);
//		}
//	}
	
	//------------------------------------------------------------------------------------------------
//	override void OnHasSignalChanged()
//	{
//		if (!CampaignInfo || CampaignInfo.GetIsMatchOver())
//		{
//			Print("OnHasSignalChanged: Match is over. Returning...");
//			return;
//		}		
//		
//		super.OnHasSignalChanged();
//		
//		SignalChangedInternal_modded();
//	}
	
	//------------------------------------------------------------------------------------------------
//	private void SignalChangedInternal_modded()
//	{
//		if(!IsHQ() && !IsProxy())
//		{
//			
//			Faction owner;
//			owner = GetFaction();
//			if(!owner)
//			{
//				return;
//			}
//			
//			if (GetGame().GetCallqueue().GetRemainingTime(ObjectivesRewardTimer_Modded) == -1)
//			{
//				
//				if (IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(owner), SCR_ERadioCoverageStatus.BOTH_WAYS))
//				{
//					// ObjectivesRewardTimer_Modded(true); we are nice and give many objectives
//					
//					GetGame().GetCallqueue().CallLater(ObjectivesRewardTimer_Modded, m_fTickRate, true, false);
//				}
//			}
//			else
//			{
//				if (!IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(owner), SCR_ERadioCoverageStatus.BOTH_WAYS))
//				{
//					
//					GetGame().GetCallqueue().Remove(ObjectivesRewardTimer_Modded);
//				}
//			}
//		}
//	}
	
//	bool initialized;
//	int count = 0;
	
//	//------------------------------------------------------------------------------------------------
//	override protected void OnFactionChanged(FactionAffiliationComponent owner, Faction previousFaction, Faction faction)
//	{
//		
//		SCR_BaseScoringSystemComponent scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
//		SCR_CaptureAndHoldManager chManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
//		int baseCaptureScore = chManager.GetBaseCaptureScore();
//		int factionStartScore = chManager.GetFactionStartScore();
//
//		super.OnFactionChanged(owner,  previousFaction,  faction);
//		ObjectivesRewardTimer_Modded(true);
//	
//		// If AAD Mode, don't let defenders have tasks since they cannot capture the bases back anyways
//		if (CampaignInfo.IsAADEnabled())
//		{
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
//		}
//		
//		if (!owner || !faction || !previousFaction)
//		return;
//		
//		scoringSystem.AddFactionObjective(faction, baseCaptureScore);
//		SCR_CampaignFaction campaignFaction = SCR_CampaignFaction.Cast(previousFaction);
//		
//		
//		if (previousFaction && scoringSystem.GetFactionScore(previousFaction) >= baseCaptureScore && campaignFaction.IsPlayable())
//		{
//			if (!CampaignInfo.IsAADEnabled())	
//				scoringSystem.AddFactionObjective(previousFaction, -baseCaptureScore);
//		}
//
//		SignalChangedInternal_modded();
//	}
	
//}