//------------------------------------------------------------------------------------------------
/*!
	Capture & Hold manager that allows registration and management of areas.
	This component must be attached to a SCR_BaseGameMode entity!
	There should only be a single manager at any given time.
*/
modded class SCR_CaptureAndHoldManager : SCR_BaseGameModeComponent
{
	//GetStartingScore();
	//------------------------------------------------------------------------------------------------
	/*!
		Called when provided player dies.
	*/
	[Attribute(defvalue: "1", desc: "...", category: "CaptureAndHold: Technical")]
	protected bool m_bIsCampaignGameMode;
	
	[Attribute(defvalue: "0", desc: "If set, adds extra points to faction score for capturing a base", category: "CaptureAndHold: Scoring")]
	protected int m_bBaseCaptureScore;
	
	[Attribute(defvalue: "0", desc: "If true, enables extra points to faction score at beginning of match", category: "CaptureAndHold: Scoring")]
	protected bool m_bTicketScoreSystem;
	
	[Attribute(defvalue: "0", desc: "If true, shows score/tickets of enemy team. Should use Scoring Player Controller", category: "CaptureAndHold: Scoring")]
	protected bool m_bShowEnemyScore;
	
	[Attribute(defvalue: "500", desc: "If set, adds extra points to faction score at beginning of match", category: "CaptureAndHold: Scoring")]
	protected int m_iFactionStartScore;
	
	[Attribute(defvalue: "1", desc: "If set, scales base capture score based on player count.", category: "CaptureAndHold: Scoring")]
	protected bool m_bScaleScore;
	
	[Attribute(defvalue: "1", desc: "If set, scales the cost of respawn death based on player count.", category: "CaptureAndHold: Scoring")]
	protected bool m_bScaleDeathScore;
	
	[Attribute(defvalue: "1", desc: "If set, CAH Zones must have a radio signal in order to capture.", category: "CaptureAndHold: Area")]
	protected bool m_bRequiresRadioSignal;
	
	////////////////////////////////////////////////////////////////////////////
	
	[Attribute(defvalue: "10", desc: "", category: "Scaling: Base Capture Score")]
	protected int m_iPlayerCount1;
	
	[Attribute(defvalue: "0.3", desc: "", category: "Scaling: Base Capture Score")]
	protected float m_iMultiplier1;
	
	[Attribute(defvalue: "20", desc: "", category: "Scaling: Base Capture Score")]
	protected int m_iPlayerCount2;
	
	[Attribute(defvalue: "0.5", desc: "", category: "Scaling: Base Capture Score")]
	protected float m_iMultiplier2;
	
	[Attribute(defvalue: "32", desc: "", category: "Scaling: Base Capture Score")]
	protected int m_iPlayerCount3;
	
	[Attribute(defvalue: "0.7", desc: "", category: "Scaling: Base Capture Score")]
	protected float m_iMultiplier3;
	
	[Attribute(defvalue: "48", desc: "", category: "Scaling: Base Capture Score")]
	protected int m_iPlayerCount4;
	
	[Attribute(defvalue: "1.0", desc: "", category: "Scaling: Base Capture Score")]
	protected float m_iMultiplier4;
	
	[Attribute(defvalue: "64", desc: "", category: "Scaling: Base Capture Score")]
	protected int m_iPlayerCount5;
	
	[Attribute(defvalue: "1.3", desc: "", category: "Scaling: Base Capture Score")]
	protected float m_iMultiplier5;
	
	[Attribute(defvalue: "80", desc: "", category: "Scaling: Base Capture Score")]
	protected int m_iPlayerCount6;
	
	[Attribute(defvalue: "1.6", desc: "", category: "Scaling: Base Capture Score")]
	protected float m_iMultiplier6;
	
	[Attribute(defvalue: "96", desc: "", category: "Scaling: Base Capture Score")]
	protected int m_iPlayerCount7;
	
	[Attribute(defvalue: "2.0", desc: "", category: "Scaling: Base Capture Score")]
	protected float m_iMultiplier7;
	
	////////////////////////////////////////////////////////////////////////////
	
	[Attribute(defvalue: "10", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iPlayerCountDeath1;
	
	[Attribute(defvalue: "8", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iTicketLossDeath1;
	
	[Attribute(defvalue: "20", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iPlayerCountDeath2;
	
	[Attribute(defvalue: "5", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iTicketLossDeath2;
	
	[Attribute(defvalue: "32", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iPlayerCountDeath3;
	
	[Attribute(defvalue: "3", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iTicketLossDeath3;
	
	[Attribute(defvalue: "48", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iPlayerCountDeath4;
	
	[Attribute(defvalue: "2", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iTicketLossDeath4;
	
	[Attribute(defvalue: "64", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iPlayerCountDeath5;
	
	[Attribute(defvalue: "1", desc: "", category: "Scaling: Death/Ticket Loss")]
	protected int m_iTicketLossDeath5;
	
	// Death overlay widget info
	bool m_bScoreReduced;
//	SCR_ChimeraCharacter m_cKillerEntity;
	IEntity m_cKillerEntity;
	WorldTimestamp m_wDeathTimestamp;
	SCR_ChimeraCharacter m_cPlayerCharacter;
	CharacterControllerComponent m_cCharacterController;
	Faction m_cKillerFaction;
	Instigator m_iKiller;
	string m_cKillerName;
	SCR_HUDManagerComponent m_HudManager;
	
	//------------------------------------------------------------------------------------------------	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void PushMessage(string message, int playerId)
	{	
		int playerCheck = SCR_PlayerController.GetLocalPlayerId();
		if (playerId != playerCheck)
		return;
		SCR_PopUpNotification.GetInstance().PopupMsg(message, prio: SCR_ECampaignPopupPriority.RESPAWN, duration: 5);
	}
	
	//------------------------------------------------------------------------------------------------	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_SetDeathTimestamp(int playerId)
	{
		int playerCheck = SCR_PlayerController.GetLocalPlayerId();
		if (playerId != playerCheck)
		return;
		
		m_HudManager = GetGame().GetHUDManager();

	}
	
	//------------------------------------------------------------------------------------------------	
	protected override void OnPlayerKilled(notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnPlayerKilled(instigatorContextData);
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(instigatorContextData.GetVictimEntity());
		m_cPlayerCharacter = character;
		m_cCharacterController = character.GetCharacterController();
		m_iKiller = instigatorContextData.GetInstigator();
		
		if (!character)
			return;
		
		m_cKillerEntity = instigatorContextData.GetKillerEntity();
		m_wDeathTimestamp = SCR_GameModeCampaign.GetInstance().GetWorld().GetTimestamp().PlusSeconds(10); //.PlusMilliseconds(5000);
		int playerId = instigatorContextData.GetVictimPlayerID();
//		Rpc(RpcDo_SetDeathTimestamp,playerId);
		
		m_HudManager = GetGame().GetHUDManager();
		
		// Player faction
		Faction faction = character.GetFaction();
		if (!faction)
			return;
		
		// Enemy/killer faction	
		SCR_ChimeraCharacter enemyCharacter = SCR_ChimeraCharacter.Cast(instigatorContextData.GetKillerEntity());
		if (!enemyCharacter)
			return;
		
		Faction enemyFaction = enemyCharacter.GetFaction();
		SCR_CampaignFaction enemyCampaignFaction = SCR_CampaignFaction.Cast(enemyFaction);
		
		m_cKillerFaction = enemyFaction;
		m_cKillerName = GetGame().GetPlayerManager().GetPlayerName(instigatorContextData.GetKillerPlayerID()); //killer.GetInstigatorPlayerID());
	
		if (!enemyFaction || enemyFaction == faction)
			return; 
		
		// Enemy Faction Key 	
		FactionKey enemyFactionKey = enemyFaction.GetFactionKey();
		
		// Subtract 100 points from your factions score for dying

		SCR_BaseScoringSystemComponent scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
		int factionScore = scoringSystem.GetFactionScore(faction);
		
		// Perhaps don't use server broadcasting for death messages -- have to since local overlay wont work on clients (only server)
/*		if (factionScore >= 100)
		{
		scoringSystem.AddFactionObjective(faction, -100); // seems to do this twice
		Rpc(PushMessage,"YOU DIED: -100 POINTS FOR YOUR TEAM",playerId);
		}
		
		// Give 50 points to enemy faction if killer is not on same team or FIA
		if (enemyFaction != faction && enemyFactionKey != "FIA")
		{
		scoringSystem.AddFactionObjective(enemyFaction, 50);
		Rpc(PushMessage,"KILLED: +50 POINTS TO ENEMY",playerId);
		} */
	
		if (!TicketScoreSystem())
		{
			if (factionScore >= 100)
			{
				m_bScoreReduced = true;
				scoringSystem.AddFactionObjective(faction, -100);
			}
			else
			{
				m_bScoreReduced = false;
			}
			
			//Instead, lets make sure the faction is playable
//			if (enemyFaction != faction && enemyFactionKey != "FIA")
			if (enemyFaction != faction && enemyCampaignFaction.IsPlayable())
			{
				scoringSystem.AddFactionObjective(enemyFaction, 50);
			}
		}

		foreach (SCR_SpawnArea area : m_aSpawnAreas)
		{
			Faction areaFaction = area.GetAffiliatedFaction();
			if (!areaFaction)
				continue;

			if (!areaFaction.IsFactionFriendly(faction))
				continue;

			// We have to query ourselves, the character is dead and filtered out
			if (!area.QueryEntityInside(character))
				continue;

			// Character died in a friendly spawn zone, therefore
			// let's clear their respawn timer to reduce frustration
			// of spawn / team kills
			SCR_BaseGameMode gameMode = GetGameMode();
			if (!gameMode)
				return;

			// Unless SCR_RespawnTimerComponent handles individual times on top of faction times,
			// this will have no impact unfortunately
			SCR_RespawnTimerComponent respawnTimer = SCR_RespawnTimerComponent.Cast(gameMode.FindComponent(SCR_RespawnTimerComponent));
			if (!respawnTimer)
				return;

			// Reset to 0
			respawnTimer.SetRespawnTime(playerId, 0.0);
		}
		
	
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DoPanZoomMap(float x, float z, float zoom)
	{
		// If you're not going to work, then don't do anything
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Finds the average center of all objectives and pans and zooms the map to it.
	*/
	override protected void OnMapOpen(MapConfiguration config)
	{
 		// Leave my map alone!
	}
	
	//------------------------------------------------------------------------------------------------	
	int GetBaseCaptureScore()
	{
		int ticketScore = m_bBaseCaptureScore;

	    if (m_bScaleScore)
	    {
	        PlayerManager playerManager = GetGame().GetPlayerManager();
	        int players = playerManager.GetPlayerCount();
	
	        if (players >= m_iPlayerCount7)
	            ticketScore = m_bBaseCaptureScore * m_iMultiplier7;
	        else if (players >= m_iPlayerCount6)
	            ticketScore = m_bBaseCaptureScore * m_iMultiplier6;
	        else if (players >= m_iPlayerCount5)
	            ticketScore = m_bBaseCaptureScore * m_iMultiplier5;
	        else if (players >= m_iPlayerCount4)
	            ticketScore = m_bBaseCaptureScore * m_iMultiplier4;
	        else if (players >= m_iPlayerCount3)
	            ticketScore = m_bBaseCaptureScore * m_iMultiplier3;
	        else if (players >= m_iPlayerCount2)
	            ticketScore = m_bBaseCaptureScore * m_iMultiplier2;
	        else if (players >= m_iPlayerCount1)
	            ticketScore = m_bBaseCaptureScore * m_iMultiplier1;
	        else
	            ticketScore = m_bBaseCaptureScore * 0.2; // For players < m_iPlayerCount1
	    }
	
	    return ticketScore;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetDeathTicketScore()
	{
		int deathScore = 1;
		
	    if (m_bScaleDeathScore)
	    {
	        PlayerManager playerManager = GetGame().GetPlayerManager();
	        int players = playerManager.GetPlayerCount();
	
	        if (players >= m_iPlayerCountDeath5)
	            deathScore = m_iTicketLossDeath5;
	        else if (players >= m_iPlayerCountDeath4)
	            deathScore = m_iTicketLossDeath4;
	        else if (players >= m_iPlayerCountDeath3)
	            deathScore = m_iTicketLossDeath3;
	        else if (players >= m_iPlayerCountDeath2)
	            deathScore = m_iTicketLossDeath2;
	        else if (players >= m_iPlayerCountDeath1)
	            deathScore = m_iTicketLossDeath1;
	        else
	            deathScore = 10; // When players < m_iPlayerCountDeath1
	    }
		
		return -deathScore;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetScoreMultiplier()
	{
		int deathScore = 1;
		
		if (m_bScaleDeathScore)
		{
/*			PlayerManager playerManager = GetGame().GetPlayerManager();
			int players = playerManager.GetPlayerCount();
			
			if (players >= 48)
			deathScore = 1;
			
			if (players >= 32)
			deathScore = 2;
			
			if (players >= 20)
			deathScore = 5;
			
			if (players >= 10)
			deathScore = 8;
			
			if (players < 10)
			deathScore = 10; */
			
			deathScore = GetDeathTicketScore() * -1;
		}
		
		return deathScore;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetPlayerNumbers()
	{
			PlayerManager playerManager = GetGame().GetPlayerManager();
			int playerNumbers = playerManager.GetPlayerCount();

		return playerNumbers;
	}
	
	//------------------------------------------------------------------------------------------------	
	int GetFactionStartScore()
	{
		return m_iFactionStartScore;
	}
	
	//------------------------------------------------------------------------------------------------	
	bool TicketScoreSystem()
	{
		return m_bTicketScoreSystem;
	}
	
	//------------------------------------------------------------------------------------------------	
	bool ShowEnemyScore()
	{
		return m_bShowEnemyScore;
	}
	
	//------------------------------------------------------------------------------------------------
	Instigator GetKillerInstigator()
	{
		return m_iKiller;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetKillerEntity()
	{
		return m_cKillerEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	Faction GetKillerFaction()
	{
		return m_cKillerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetKillerName()
	{
		return m_cKillerName;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetScoreReduced()
	{
		return m_bScoreReduced;
	}
	
	//------------------------------------------------------------------------------------------------
	WorldTimestamp GetDeathTimestamp()
	{
		return m_wDeathTimestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_ChimeraCharacter GetPlayerCharacter()
	{
		return m_cPlayerCharacter;
	}
	
	//------------------------------------------------------------------------------------------------
	CharacterControllerComponent GetCharacterController()
	{
		return m_cCharacterController;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetRequiresRadioSignal()
	{
		return m_bRequiresRadioSignal;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Finds area manager on current game mode and returns it or null if none.
//	override static SCR_CaptureAndHoldManager GetAreaManager()
//	{
////		BaseGameMode gameMode = GetGame().GetGameMode(); // <--- Unable to rename CAH zones in Seize & Secure if BaseGameMode class
//		SCR_GameModeCampaign gameMode = SCR_GameModeCampaign.GetInstance();
//		if (!gameMode)
//			return null;
//
//		if (!s_Instance)
//			s_Instance = SCR_CaptureAndHoldManager.Cast(gameMode.FindComponent(SCR_CaptureAndHoldManager));
//
//		return s_Instance;
//	}
}
