//------------------------------------------------------------------------------------------------
modded class SCR_GameModeCampaign : SCR_BaseGameMode
{
	
	[Attribute("0", desc: "Show warmup messages if there is a timer (5 Min)", category: "Seize & Secure")]
	protected bool m_bShowBattlePrepTimer;
	
	[Attribute("0", desc: "Play classic counter-strike sound when battleprep is finished", category: "Seize & Secure")]
	protected bool m_bPlayCounterStrikeSound;
	
	[Attribute("600", UIWidgets.Slider, "Teams can not leave their main base area for this duration (seconds)", "1 7200 1", category: "Seize & Secure")]
	protected float BATTLE_PREP_TIME_S;
	
	[Attribute("600", UIWidgets.Slider, "Distance players must be within of Radio Operator class for their marker to show on the map", category: "Seize & Secure")]
	protected float m_fRadioOperatorMarkerDistance;
	
//	[Attribute("{8369FC680E4D7BB2}Prefabs/MP/Campaign/MHQ_Spawn_Prefab/CampaignMobileAssemblySpawnpoint_2K.et", UIWidgets.ResourceNamePicker, "", "et", category: "Seize & Secure")]
//	protected ResourceName MHQ_SPAWN_PREFAB;

	[Attribute("0", params: "0 inf 1", desc: "Additional antenna range to be added to fringe base connections", category: "Seize & Secure")]
	protected int m_iMHQRange;
	
	[Attribute("1", desc: "Are certain actions locked to Loadout Roles?", category: "Seize & Secure")]
	protected bool m_bLoadoutRolesRestrictedActions;
	
	[Attribute("0", desc: "Is Rank Allowance enabled for saving loadout?", category: "Seize & Secure")]
	protected bool m_bSavingLoadoutRankRequired;
	
	[Attribute("0", desc: "Allowance for Renegade", category: "Seize & Secure")]
	protected int m_iRenegadeAllowance;
	
	[Attribute("50", desc: "Allowance for Private", category: "Seize & Secure")]
	protected int m_iPrivateAllowance;
	
	[Attribute("75", desc: "Allowance for Corporal", category: "Seize & Secure")]
	protected int m_iCorporalAllowance;
	
	[Attribute("100", desc: "Allowance for Sergeant", category: "Seize & Secure")]
	protected int m_iSergeantAllowance;
	
	[Attribute("150", desc: "Allowance for Lieutenant", category: "Seize & Secure")]
	protected int m_iLieutenantAllowance;
	
	[Attribute("250", desc: "Allowance for Captain", category: "Seize & Secure")]
	protected int m_iCaptainAllowance;
	
	[Attribute("400", desc: "Allowance for Major", category: "Seize & Secure")]
	protected int m_iMajorAllowance;
	
	[Attribute("1", UIWidgets.CheckBox, "If enabled, use scaling respawn timer (does not work with Wave respawns)", category: "Scaling Respawn Penalty")]
	protected bool m_bEnableScaledRespawnTimer;

	[Attribute("1200", UIWidgets.EditBox, "Cooldown (in seconds) since last death before resetting respawn penalties", params: "0 inf 1", category: "Scaling Respawn Penalty")]
	protected int m_iRespawnPenaltyCooldown;
	
	[Attribute("5", UIWidgets.EditBox, "Time (in seconds) added to respawn timer for each death ", params: "0 inf 1", category: "Scaling Respawn Penalty")]
	protected int m_iRespawnPenalty;
	
	[Attribute("90", UIWidgets.EditBox, "Maximum Time (in seconds) respawn timer can be", params: "0 inf 1", category: "Scaling Respawn Penalty")]
	protected int m_iMaxRespawnPenalty;
	
	[Attribute("1", UIWidgets.CheckBox, "Scale respawn timer based on rank?", category: "Scaling Respawn Penalty By Rank")]
	protected bool m_bScaleRespawnTimer;
	
	[Attribute("20", UIWidgets.EditBox, "", params: "0 inf 1", category: "Scaling Respawn Penalty By Rank")]
	protected int m_iRespawnPenaltyRenegade;
	
	[Attribute("5", UIWidgets.EditBox, "", params: "0 inf 1", category: "Scaling Respawn Penalty By Rank")]
	protected int m_iRespawnPenaltyPrivate;
	
	[Attribute("5", UIWidgets.EditBox, "", params: "0 inf 1", category: "Scaling Respawn Penalty By Rank")]
	protected int m_iRespawnPenaltyCorporal;
	
	[Attribute("4", UIWidgets.EditBox, "", params: "0 inf 1", category: "Scaling Respawn Penalty By Rank")]
	protected int m_iRespawnPenaltySergeant;
	
	[Attribute("4", UIWidgets.EditBox, "", params: "0 inf 1", category: "Scaling Respawn Penalty By Rank")]
	protected int m_iRespawnPenaltyLieutenant;
	
	[Attribute("3", UIWidgets.EditBox, "", params: "0 inf 1", category: "Scaling Respawn Penalty By Rank")]
	protected int m_iRespawnPenaltyCaptain;
	
	[Attribute("3", UIWidgets.EditBox, "", params: "0 inf 1", category: "Scaling Respawn Penalty By Rank")]
	protected int m_iRespawnPenaltyMajor;
	
	[Attribute("2", UIWidgets.EditBox, "", params: "0 inf 1", category: "Scaling Respawn Penalty By Rank")]
	protected int m_iRespawnPenaltyColonel;
	
	[Attribute("0.02", UIWidgets.EditBox, "", params: "0 1 0.01", category: "Mobile Spawnpoint Timer Distance Multiplier")]
	protected float m_fMobileSpawnTimeMultiplierRP;
	
	[Attribute("0.035", UIWidgets.EditBox, "", params: "0 1 0.01", category: "Mobile Spawnpoint Timer Distance Multiplier")]
	protected float m_fMobileSpawnTimeMultiplierMHQ;
	
	[Attribute("0", UIWidgets.EditBox, "Determines if AAD style game mode is enabled", category: "AAS")]
	bool m_bAADEnabled;
	
	[Attribute("FIA", UIWidgets.EditBox, "Determines which faction is Defending for AAS style game mode", category: "AAS"), RplProp(onRplName: "OnSetFactionKey")] //, RplProp(onRplName: "OnSetFactionKey")]
	string m_sAADDefendingFaction;

	[Attribute("US", UIWidgets.EditBox, "Determines which faction is Attacking for AAS style game mode",category: "AAS"), RplProp(onRplName: "OnSetFactionKey")] //, RplProp(onRplName: "OnSetFactionKey")]
	string m_sAADAttackingFaction;

	[Attribute("0", desc: "Should Bases (not control points) be randomized?", category: "Random Conflict Bases")]
	protected bool m_bRandomizeBases;
	
	[Attribute("5", params: "0 10 1", desc: "From 0-10 (Higher = more bases removed)", category: "Random Conflict Bases")]
	protected int m_iRandomizeTreshold;
	
//	[Attribute("3", params: "0 inf 1", desc: "How many bases should the sytem check for when determining fringe base range", category: "Random Conflict Bases")]
	protected int m_iClosestBasesCheck = 3;
	
	[Attribute("10", params: "0 inf 1", desc: "Additional antenna range to be added to fringe base connections", category: "Random Conflict Bases")]
	protected int m_iAdditionalRange;
	
	[Attribute("0", desc: "", category: "Base Map Markers")]
	protected bool m_bHideBasesOutsideRadioRange;
	
	[Attribute("0", desc: "", category: "Base Map Markers")]
	protected bool m_bHideCAHZonesOutsideRadioRange;
	
	[Attribute("1", desc: "", category: "Frontline Map Module")]
	protected bool m_bEnableFrontlineMapModule;
	
	[Attribute("0", desc: "", category: "Frontline Map Module")]
	protected bool m_bAllowEnemyHQTerritory;
	
	[Attribute("0", desc: "", category: "Frontline Map Module")]
	protected bool m_bHideEnemyTerritoryOutsideRadioRange;
	
	[Attribute("1", desc: "", category: "Frontline Map Module")]
	protected bool m_bObfuscateTerritoryOutsideRadioRange;
	
	[Attribute("0", desc: "Must not be in range of enemy radio signal", category: "Rally Point")]
	protected bool m_bMustNotBeOnFrontline;
	
	[Attribute("1", desc: "Must be within friendly HQ radio range", category: "Rally Point")]
	protected bool m_bMustBeWithinHQRadioRange;
	
	[Attribute("1", desc: "Enable strafe spam penalty", category: "Seize & Secure: Experimental")]
	protected bool m_bStrafeSpamPenaltyEnabled;
	
	[Attribute("1", desc: "Enable bushes slowing you down", category: "Seize & Secure: Experimental")]
	protected bool m_bBushSlowingPlayerEnabled;
	
//	[RplProp(onRplName: "OnTimerStarted")]
	protected bool m_bHasTimerStarted;
	protected WorldTimestamp m_StartedTimestamp;
	protected ref ScriptInvoker m_BattlePrepEndedInvoker;
	protected float m_fTimeElapsed;
	protected bool m_bHasBattlePrepEnded;
	protected bool m_bRandomizedBasesInitialized;
	
	//------------------------------------------------------------------------------------------------
	protected override void Start()
	{
		// Compose custom bases array from header
		SCR_MilitaryBaseSystem baseManagerRandomize = SCR_MilitaryBaseSystem.GetInstance();
		SCR_CampaignMilitaryBaseManager baseManager = GetBaseManager();
		if (baseManager && baseManagerRandomize && GetAreBasesRandomized())
		{
			baseManagerRandomize.FinalizeBaseRegistration();
			baseManager.ProcessRandomizedBaseState();				
		}		
		
		super.Start();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnStarted()
	{
		super.OnStarted();
		
		if (!m_BattlePrepEndedInvoker)
			m_BattlePrepEndedInvoker = GetBattlePrepEndedInvoker();
		
		SetStartedTimestamp(GetGame().GetWorld().GetTimestamp());
		SetFactionScore();
		
		if (GetAreBasesRandomized() && IsAADEnabled())
		{
			SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
			if (baseManager)
			{
				array<SCR_MilitaryBaseComponent> bases = {};
				baseManager.GetBases(bases);
				
				foreach (SCR_MilitaryBaseComponent base : bases)
				{
					SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
					if (campaignBase)
					{
						campaignBase.RecalculateRadioRange();
					}
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
//	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
//	{
//
////		RequestLoadoutAvailability(playerId);
//		
//		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
//		
//		if (m_bEnableScaledRespawnTimer)
//		{
//			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
//			
//			if (!factionManager)
//				return;
//			
//			int killerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(killerEntity);
//			Faction playerFaction = factionManager.GetPlayerFaction(playerId);
//			Faction killerFaction = factionManager.GetPlayerFaction(killerId);
//			
//			if (playerFaction != killerFaction || playerId == killerId)
//			{
//				OnPlayerDeath(playerId, playerEntity);
//			}				
//		}
//		
//		if (IsProxy())
//			return;
//
//		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerId);
//
//		if (!pc)
//			return;
//
//		SCR_CampaignNetworkComponent campaignNetworkComponent = SCR_CampaignNetworkComponent.Cast(pc.FindComponent(SCR_CampaignNetworkComponent));
//		
//		if (campaignNetworkComponent)
//		{
//			campaignNetworkComponent.OnPlayerAliveStateChanged(false);
//			campaignNetworkComponent.ResetSavedSupplies();
//		}
//
//		UpdateRespawnPenalty(playerId);
//		
//		if (playerId == killer.GetInstigatorPlayerID())
//		{
//			OnSuicide(playerId);
//		}
//
//		SCR_CampaignClientData clientData = GetClientData(playerId);
//
//		if (clientData)
//		{
//			clientData.SetLatestKillerID(killer.GetInstigatorPlayerID());
//		}
//	}

	//------------------------------------------------------------------------------------------------
	protected override void OnPlayerKilledEx(notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnPlayerKilledEx(instigatorContextData);
		
		//RequestLoadoutAvailability(instigatorContextData.GetVictimPlayerID());
		
		if (m_bEnableScaledRespawnTimer)
		{
			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			
			if (!factionManager)
				return;
			
			int killerId = instigatorContextData.GetKillerPlayerID();
			int playerId = instigatorContextData.GetVictimPlayerID();
			IEntity playerEntity = instigatorContextData.GetVictimEntity();
			Faction playerFaction = factionManager.GetPlayerFaction(playerId);
			Faction killerFaction = factionManager.GetPlayerFaction(killerId);
			
			if (playerFaction != killerFaction || playerId == killerId)
			{
				OnPlayerDeath(playerId, playerEntity);
			}				
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPlayerDeath(int playerId, IEntity playerEntity)
	{
		if (m_iRespawnPenalty == 0)
			return;
		
		SCR_CampaignClientData clientData = GetClientData(playerId);
		if (!clientData)
		{
			PrintFormat("SCR_GameModeCampaign: OnPlayerDeath: No client data for Player ID: %1. Returning...", playerId);
			return;		
		}
	
		float addedPenalty = m_iRespawnPenalty;
		
		if (m_bScaleRespawnTimer)
		{
			SCR_ECharacterRank userRank = SCR_CharacterRankComponent.GetCharacterRank(playerEntity);
			
			if (userRank == SCR_ECharacterRank.RENEGADE)
				addedPenalty = m_iRespawnPenaltyRenegade;
			
			else if (userRank == SCR_ECharacterRank.PRIVATE)
				addedPenalty = m_iRespawnPenaltyPrivate;
			
			else if (userRank == SCR_ECharacterRank.CORPORAL)
				addedPenalty = m_iRespawnPenaltyCorporal;
			
			else if (userRank == SCR_ECharacterRank.SERGEANT)
				addedPenalty = m_iRespawnPenaltySergeant;
			
			else if (userRank == SCR_ECharacterRank.LIEUTENANT)
				addedPenalty = m_iRespawnPenaltyLieutenant;
			
			else if (userRank == SCR_ECharacterRank.CAPTAIN)
				addedPenalty = m_iRespawnPenaltyCaptain;
			
			else if (userRank >= SCR_ECharacterRank.MAJOR)
				addedPenalty = m_iRespawnPenaltyMajor;
			
			else if (userRank >= SCR_ECharacterRank.COLONEL)
				addedPenalty = m_iRespawnPenaltyColonel;
		}
		
		float respawnPenalty = clientData.GetDeathRespawnPenalty();
		float lastDeathTimestamp = clientData.GetLastDeathTimestamp();
		float curTime = GetGame().GetWorld().GetWorldTime();
		clientData.SetLastDeathTimestamp(curTime);
		
		// No penalty if this is the first time player is dying
		if (lastDeathTimestamp == 0)
		{
			respawnPenalty = 0;
			addedPenalty = 0;
		}

		float timeSinceLastDeath = curTime - lastDeathTimestamp;
		float penaltyCooldownMs = (float)m_iRespawnPenaltyCooldown * 1000;

		// Reset penalty/timer if player is a good boy and stays alive for a long enough period of time
		if (timeSinceLastDeath > penaltyCooldownMs)
		{
			respawnPenalty = 0;
			addedPenalty = 0;
		}
	
		clientData.SetLastOnDeathPenaltyDeductionTimestamp(curTime);
		clientData.SetDeathRespawnPenalty(respawnPenalty + addedPenalty);
		float totalPenalty = Math.Clamp(respawnPenalty + addedPenalty, 0, m_iMaxRespawnPenalty);
		
		string playerName = GetGame().GetPlayerManager().GetPlayerName(playerId);
		PrintFormat("SCR_GameModeCampaign: OnPlayerDeath: PlayerID: %1 (%2) | Respawn Penalty: %3 | Added Penalty: %4 | Capped Penalty: %5", playerId, playerName, respawnPenalty, addedPenalty, totalPenalty, LogLevel.ERROR);
		array<Managed> timers = {};
		FindComponents(SCR_RespawnTimerComponent, timers);

		foreach (Managed timer : timers)
		{
			// Skip this specific type as it's handled separately for radio spawns
			if (timer.Type() == SCR_TimedSpawnPointComponent)
				continue;

			SCR_RespawnTimerComponent timerCast = SCR_RespawnTimerComponent.Cast(timer);

			if (!timerCast)
				continue;
			
			timerCast.SetRespawnTime(playerId, timerCast.GetRespawnTime() + totalPenalty); //respawnPenalty + addedPenalty);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RequestLoadoutAvailability(int playerId)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;
		
		SCR_PlayerLoadoutComponent playerLoadoutComponent = SCR_PlayerLoadoutComponent.Cast(playerController.FindComponent(SCR_PlayerLoadoutComponent));
		if (!playerLoadoutComponent)
			return;
		
		array<ref SCR_BasePlayerLoadout> availableLoadouts = {};
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		Faction playerFaction = factionManager.GetPlayerFaction(playerId);
		SCR_LoadoutManager loadoutManager = GetGame().GetLoadoutManager();
		loadoutManager.GetPlayerLoadoutsByFaction(playerFaction, availableLoadouts);
		array<ref int> loadoutIndexes = {};
		
		foreach (SCR_BasePlayerLoadout loadout : availableLoadouts)
		{
		    int loadoutIndex = loadoutManager.GetLoadoutIndex(loadout);
		    if (playerLoadoutComponent)
		    {
		        loadoutIndexes.Insert(loadoutIndex);
		    }
		}
		
		playerLoadoutComponent.CanIHasLoadout(playerId, loadoutIndexes);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void DisableExtraSpawnpoint(SCR_SpawnPoint spawnpoint)
	{
		// Null error in 1.5 experimental because of no spawnpoint
		if (!spawnpoint)
			return;
		
		if (spawnpoint.Type() != SCR_SpawnPoint)
			return;

		spawnpoint.SetFaction(null);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void SetPrioritySpawnpoint(SCR_SpawnPoint spawnpoint)
	{
		// Null error in 1.5 experimental because of no spawnpoint
		if (!spawnpoint)
			return;
		
		IEntity spawnpointParent = spawnpoint.GetRootParent();
		if (!spawnpointParent)
			return;

		SCR_CampaignMilitaryBaseComponent baseComponent = SCR_CampaignMilitaryBaseComponent.Cast(spawnpointParent.FindComponent(SCR_CampaignMilitaryBaseComponent));
		if (!baseComponent || !baseComponent.IsHQ())
			return;

		spawnpoint.SetPriority(m_iHQSpawnPointPriority);
	}
	
	//------------------------------------------------------------------------------------------------
	int GetMHQRange()
	{
		return m_iMHQRange;
	}

	//------------------------------------------------------------------------------------------------
	bool ShowBattlePrepTimer()
	{
		return m_bShowBattlePrepTimer;
	}	
	
	//------------------------------------------------------------------------------------------------
	bool PlayClassicCS()
		{
			return m_bPlayCounterStrikeSound;
		}
	
	//------------------------------------------------------------------------------------------------
	float GetBattlePrepTime()
	{
		return BATTLE_PREP_TIME_S;
	}	
	
	//------------------------------------------------------------------------------------------------
//	ResourceName GetMHQPrefab()
//	{
//		return MHQ_SPAWN_PREFAB;
//	}
	
	//------------------------------------------------------------------------------------------------
	bool IsRankAllowanceEnabled()
	{
		return m_bSavingLoadoutRankRequired;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsLoadoutRolesRestrictedActions()
	{
		return m_bLoadoutRolesRestrictedActions;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetRankAllowance(SCR_ECharacterRank userRank)
	{
		int allowance;
	
		if (userRank == SCR_ECharacterRank.RENEGADE)
		allowance = m_iRenegadeAllowance;
	
		if (userRank == SCR_ECharacterRank.PRIVATE)
		allowance = m_iPrivateAllowance;
	
		if (userRank == SCR_ECharacterRank.CORPORAL)
		allowance = m_iCorporalAllowance;
	
		if (userRank == SCR_ECharacterRank.SERGEANT)
		allowance = m_iSergeantAllowance;
	
		if (userRank == SCR_ECharacterRank.LIEUTENANT)
		allowance = m_iLieutenantAllowance;
	
		if (userRank == SCR_ECharacterRank.CAPTAIN)
		allowance = m_iCaptainAllowance;
		
		if (userRank == SCR_ECharacterRank.MAJOR)
		allowance = m_iMajorAllowance;
	
		return allowance;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetMobileSpawnTimeDistanceMultiplierRP()
	{
		return m_fMobileSpawnTimeMultiplierRP;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetMobileSpawnTimeDistanceMultiplierMHQ()
	{
		return m_fMobileSpawnTimeMultiplierMHQ;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetAADDefendingFaction()
	{
		return m_sAADDefendingFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetAADAttackingFaction()
	{
		return m_sAADAttackingFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAADDefendingFaction(string factionKey)
	{		
		m_sAADDefendingFaction = factionKey;
		OnSetFactionKey();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAADAttackingFaction(string factionKey)
	{
		m_sAADAttackingFaction = factionKey;
		OnSetFactionKey();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSetFactionKey()
	{
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	bool IsAADEnabled()
	{
		return m_bAADEnabled;
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetFactionScore()
	{
		SCR_CaptureAndHoldManager chManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
		if (!chManager || !chManager.TicketScoreSystem()) return;
		
		int factionStartScore = chManager.GetFactionStartScore();
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager) return;

		SCR_BaseScoringSystemComponent scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
		if (!scoringSystem) return;
		
		ref array<Faction>FactionList = {};
		
		factionManager.GetFactionsList(FactionList);
		
		foreach (Faction faction : FactionList)
		{
			scoringSystem.AddFactionObjective(faction, factionStartScore);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetControlPointsHeld(SCR_CampaignFaction faction, int newCount)
	{
		if (faction.GetControlPointsHeld() == newCount || IsAADEnabled() && faction.GetFactionKey() == m_sAADDefendingFaction)
		{
//			PrintFormat("SetControlPointsHeld: Faction Key: %1 | Defending Faction Key: %2", faction.GetFactionKey(), m_sAADDefendingFaction);
			return;
		}			

		int index = GetGame().GetFactionManager().GetFactionIndex(faction);

		PrintFormat("SetControlPointsHeld: Setting Control Points Threshold: %1 | Faction: %2", newCount, faction);
		Rpc(RPC_DoSetControlPointsHeld, index, newCount);
		RPC_DoSetControlPointsHeld(index, newCount)
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetHasTimerStarted()
	{
		return m_bHasTimerStarted;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnTimerStarted()
	{
		// Maybe invoke something here
	}
	
	//------------------------------------------------------------------------------------------------
	float GetRadioOperatorMarkerDistance()
	{
		return m_fRadioOperatorMarkerDistance;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		super.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);

		if (IsAADEnabled())
		{
			int playerId = requestComponent.GetPlayerId();
			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			Faction playerFaction = factionManager.GetPlayerFaction(playerId);
			string DefendingFactionHeader = "YOU ARE DEFENDING";
			string DefendingFactionMessage = "Hold your ground! Bases that are lost cannot be recaptured!";
			string AttackingFactionHeader = "YOU ARE ATTACKING";
			string AttackingFactionMessage = "Your team has a fixed amount of tickets available. Use them wisely!";
			
			if (playerFaction.GetFactionKey() == GetAADDefendingFaction())
			{
				Rpc(PushMessage, DefendingFactionHeader, DefendingFactionMessage, playerId, 10);
			}
			if (playerFaction.GetFactionKey() == GetAADAttackingFaction())
			{
				Rpc(PushMessage, AttackingFactionHeader, AttackingFactionMessage, playerId, 10);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void PushMessage(string messageTitle, string messageText, int playerId, int duration)
	{	
		int playerCheck = SCR_PlayerController.GetLocalPlayerId();
		if (playerId != playerCheck)
			return;
		
		SCR_PopUpNotification.GetInstance().PopupMsg(messageTitle, duration, messageText, prio: 1000);
	}
	
	//------------------------------------------------------------------------------------------------	
	WorldTimestamp GetStartedTimestamp()
	{
		return m_StartedTimestamp;
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetStartedTimestamp(WorldTimestamp time)
	{
		m_StartedTimestamp = time;
	}
	
	//------------------------------------------------------------------------------------------------	
	ScriptInvoker GetBattlePrepEndedInvoker()
	{
		if (!m_BattlePrepEndedInvoker)
			m_BattlePrepEndedInvoker = new ScriptInvoker();
		
		return m_BattlePrepEndedInvoker;
	}
	
	//------------------------------------------------------------------------------------------------	
	bool HasBattlePrepEnded()
	{
		return m_bHasBattlePrepEnded;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetAreBasesRandomized()
	{
		return m_bRandomizeBases;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetRandomizeThreshold()
	{
		return m_iRandomizeTreshold;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetClosestBasesCheck()
	{
		return m_iClosestBasesCheck;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetAdditionalAntennaRange()
	{
		return m_iAdditionalRange;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetRandomizedBasesInitialized()
	{
		return m_bRandomizedBasesInitialized;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetRandomizedBasesInitialized(bool value)
	{
		m_bRandomizedBasesInitialized = value;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetControlPointTreshold(int value)
	{
		m_iControlPointsThreshold = value;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Find out if any faction has won and it's time to end the match
	override protected void CheckForWinner()
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		array<Faction> factions = {};
		factionManager.GetFactionsList(factions);
		ChimeraWorld world = GetWorld();
		WorldTimestamp curTime = world.GetServerTimestamp();
		WorldTimestamp lowestVictoryTimestamp;
		WorldTimestamp blockPauseTimestamp;
		WorldTimestamp actualVictoryTimestamp;
		SCR_CampaignFaction winner;

		foreach (Faction faction : factions)
		{
			SCR_CampaignFaction fCast = SCR_CampaignFaction.Cast(faction);

			if (!fCast || !fCast.IsPlayable())
				continue;

			blockPauseTimestamp = fCast.GetPauseByBlockTimestamp();

			if (blockPauseTimestamp == 0)
				actualVictoryTimestamp = fCast.GetVictoryTimestamp();
			else
				actualVictoryTimestamp = curTime.PlusMilliseconds(
					fCast.GetVictoryTimestamp().DiffMilliseconds(fCast.GetPauseByBlockTimestamp())
				);

			if (actualVictoryTimestamp != 0)
			{
				if (!winner || actualVictoryTimestamp.Less(lowestVictoryTimestamp))
				{
					lowestVictoryTimestamp = actualVictoryTimestamp;
					winner = fCast;
				}
			}
		}

		if (winner)
		{
			if (IsAADEnabled() && winner.GetFactionKey() == m_sAADDefendingFaction)
			{
				return;
			}
			
			EndGameAction.SetWinningFaction(winner);
			
			if (lowestVictoryTimestamp.LessEqual(curTime))
			{
				GetGame().GetCallqueue().Remove(CheckForWinner);
				int winnerId = factionManager.GetFactionIndex(winner);
				RPC_DoEndMatch(winnerId);
				Rpc(RPC_DoEndMatch, winnerId);
				OnMatchSituationChanged();
			}
			else if (factionManager.GetFactionIndex(winner) != m_iWinningFactionId || winner.GetVictoryTimestamp() != m_fVictoryTimestamp || winner.GetPauseByBlockTimestamp() != m_fVictoryPauseTimestamp)
			{
				m_iWinningFactionId = factionManager.GetFactionIndex(winner);
				m_fVictoryTimestamp = winner.GetVictoryTimestamp();
				m_fVictoryPauseTimestamp = winner.GetPauseByBlockTimestamp();
				OnMatchSituationChanged();
				Replication.BumpMe();
			}
		}
		else if (m_iWinningFactionId != -1 || m_fVictoryTimestamp != 0)
		{
			m_iWinningFactionId = -1;
			m_fVictoryTimestamp = null;
			m_fVictoryPauseTimestamp = null;
			OnMatchSituationChanged();
			Replication.BumpMe();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetHideBasesOutsideRadioRange()
	{
		return m_bHideBasesOutsideRadioRange; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetEnableFrontlineMapModule()
	{
		return m_bEnableFrontlineMapModule; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetHideCAHZonesOutsideRadioRange()
	{
		return m_bHideCAHZonesOutsideRadioRange; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetAllowEnemyHQTerritory()
	{
		return m_bAllowEnemyHQTerritory;
	}

	//------------------------------------------------------------------------------------------------
	bool GetHideEnemyTerritoryOutsideRadioRange()
	{
		return m_bHideEnemyTerritoryOutsideRadioRange; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetObfuscateTerritoryOutsideRadioRange()
	{
		return m_bObfuscateTerritoryOutsideRadioRange; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetRPMustNotBeOnFrontline()
	{
		return m_bMustNotBeOnFrontline; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetDoesRPRequireHQSignal()
	{
		return m_bMustBeWithinHQRadioRange; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetStrafeSpamPenaltyEnabled()
	{
		return m_bStrafeSpamPenaltyEnabled; 
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetBushSlowingPlayerEnabled()
	{
		return m_bBushSlowingPlayerEnabled; 
	}
	
	//------------------------------------------------------------------------------------------------	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		if (!HasBattlePrepEnded())
		{
			float timeNow = m_StartedTimestamp.PlusSeconds(BATTLE_PREP_TIME_S).DiffSeconds(GetGame().GetWorld().GetTimestamp());		
			if (timeNow <= 0)
			{
				m_bHasBattlePrepEnded = true;
				m_BattlePrepEndedInvoker.Invoke();
			}
		}		
	}
}
