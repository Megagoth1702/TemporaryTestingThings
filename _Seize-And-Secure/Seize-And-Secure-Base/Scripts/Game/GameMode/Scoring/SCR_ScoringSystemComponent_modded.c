//! Terminates the game once target score is met.
[BaseContainerProps()]
modded class EndGameAction : IScoringAction
{
	ref array<Faction>m_aFactions = {};
	SCR_CaptureAndHoldManager chManager;
	protected static Faction m_WinningFaction;
	
	//------------------------------------------------------------------------------------------------
	override void OnFactionScoreChanged(Faction faction, SCR_BaseScoringSystemComponent scoring)
	{
		if (m_eScoringMode != EScoringActionMode.SLM_Faction && m_eScoringMode != EScoringActionMode.SLM_Any)
			return;

		int score = scoring.GetFactionScore(faction);

//		if (score < m_iScoreLimit)
//			return;
		
//		int factionIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
//		SCR_GameModeEndData endData = SCR_GameModeEndData.CreateSimple(EGameOverTypes.ENDREASON_SCORELIMIT, winnerFactionId: factionIndex);
//		Execute(endData);
		
/*		if (SCR_GameModeCampaign.GetInstance().GetHasTimerStarted())
		{
			return;
		} */
		
		chManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
		if (chManager && chManager.TicketScoreSystem())
		{
			if (score < m_iScoreLimit)
			{			
				FactionKey loserFactionKey = faction.GetFactionKey();
				GetGame().GetFactionManager().GetFactionsList(m_aFactions);
				int factionIndex;
				Faction losingFaction;
				foreach (Faction index : m_aFactions)
				{
					if (index.GetFactionKey() != loserFactionKey && index.GetFactionKey() != "FIA" && index.GetFactionKey() != "CIV")
					{
						factionIndex = GetGame().GetFactionManager().GetFactionIndex(index);
						losingFaction = index;
					}
				}

				m_WinningFaction = GetGame().GetFactionManager().GetFactionByIndex(factionIndex);
				SCR_GameModeEndData endData = SCR_GameModeEndData.CreateSimple(EGameOverTypes.ENDREASON_SCORELIMIT, winnerFactionId: factionIndex);
				Execute(endData);	
			}
		}
		
		else
		{		
			if (score < m_iScoreLimit)
				return;
			
			int factionIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
			m_WinningFaction = GetGame().GetFactionManager().GetFactionByIndex(factionIndex);
			SCR_GameModeEndData endData = SCR_GameModeEndData.CreateSimple(EGameOverTypes.ENDREASON_SCORELIMIT, winnerFactionId: factionIndex);
			Execute(endData);			
		}
		
	}
	
	//------------------------------------------------------------------------------------------------
	static Faction GetWinningFaction()
	{
		return m_WinningFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetWinningFaction(Faction faction)
	{
		m_WinningFaction = faction;
	}
}


//! This component implements logic of handling certain score limits.
modded class SCR_ScoringSystemComponent : SCR_BaseScoringSystemComponent
{
	SCR_CaptureAndHoldManager chManager;
	//------------------------------------------------------------------------------------------------
	//! Handle and dispatch scoring logic for this event.
	protected override void OnPlayerKilled(notnull SCR_InstigatorContextData instigatorContextData)
	{
		// Remove score/ticket from faction 
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(instigatorContextData.GetVictimEntity());
		if (character)
		{	
			int playerId = instigatorContextData.GetVictimPlayerID();
			bool isNearMainBase = IsNearMainBase(playerId);
			
			Faction faction = character.GetFaction();
			SCR_BaseScoringSystemComponent scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
			chManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
			if (chManager.TicketScoreSystem() && !isNearMainBase)
				scoringSystem.AddFactionObjective(faction, chManager.GetDeathTicketScore());
			
			if (chManager.TicketScoreSystem() && isNearMainBase && instigatorContextData.GetKillerPlayerID() == instigatorContextData.GetVictimPlayerID())
				scoringSystem.AddFactionObjective(faction, 1); // 1 point is coming from some unknown location
		}
		
		super.OnPlayerKilled(instigatorContextData);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsNearMainBase(int playerId)
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return false;
		
		SCR_CampaignFaction playerFaction = SCR_CampaignFaction.Cast(factionManager.GetPlayerFaction(playerId));
		
		if (!playerFaction)
			return false;
		
		SCR_CampaignMilitaryBaseComponent baseHQ = playerFaction.GetMainBase();
		if (!baseHQ)
			return false;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return false;
		
		IEntity player = playerController.GetControlledEntity();
		if (!player)
			return false;
		
		vector playerLocation = player.GetOrigin();
		vector HQDistance = baseHQ.GetOwner().GetOrigin();
		float distance = vector.DistanceXZ(HQDistance,playerLocation);
		
#ifdef WORKBENCH
		Print("Player ID: " + playerId + " Distance From Main Base: " + distance);
#endif
		return distance <= 150;
	}
}
