class SEIZE_DeathInfoDisplay : SCR_InfoDisplayExtended
{
	Widget m_wDeathOverlay;
	Widget m_wRestrictionZoneOverlay;
	Widget m_wRestrictionZoneOverlay2;
	TextWidget m_wTimeRemaining;
	TextWidget m_wDeathMessage;
	TextWidget m_wWarning;
	TextWidget m_wInjuries;
	TextWidget m_wFlavor;
	TextWidget m_wFormula;
	TextWidget m_wTips;
	
	SCR_GameModeCampaign m_gCampaign;
	SCR_GameModeCampaign CampaignInfo;
	SCR_CaptureAndHoldManager chManager;

	// OnPlayerKilled info
	SCR_BaseScoringSystemComponent scoringSystem;
	int factionScore;
	bool m_bIsScoreReduced;
	SCR_ChimeraCharacter m_cKillerEntity;
	WorldTimestamp m_wDeathTimestamp;
	SCR_ChimeraCharacter m_cPlayerCharacter;
	CharacterControllerComponent m_cCharacterController;
	Instigator m_iKillerInstigator;
	float countdown;
	bool m_bTipInitialized;
	string m_sTip;
	int m_iCount;
	
	IEntity playerControlledEntity;
	SCR_CharacterControllerComponent characterController;
	
	bool isDead;
	int m_iPlayerID;
	PlayerManager playerManager;
	SCR_ChimeraCharacter player;
	ref array<ref string> m_PlayerInjuryList = {};
	
	ref array<string> m_aTips = {
	"Suppression effects scale by rank. The higher your rank, the faster you recover from suppression.",
	"Suppression is a two-way street. If you're being suppressed, suppress them back by directing intense fire towards the target.",
	"The Command Truck (MHQ) can be turned into a forward outpost with an arsenal, living quarters, and vehicle service point.",
	"Resupply packs rely on supply sources which can be found in military vehicles and offer portable ammo.",
	"In Seize & Secure, spawning on objectives may be disabled. Plan MHQ and squad radio placements carefully.",
	"Each role in Seize & Secure has a unique loadout - some even have unique perks.",
	"Objectives may be outside friendly base radio coverage. Deploy the MHQ to extend signal range.",
	"Deployable squad radios (RPs) are crucial in Seize & Secure. A well-placed RP keeps your squad together.",
	"Hold Alt (on PC) to show interaction points, useful for healing allies.",
	"Win in Seize & Secure by capturing key objectives (purple names) or outlasting enemies (ticket bars on top).",
	"Complete objectives near team mates to rank up faster in Seize & Secure.",
	"Rank penalties when dying are restored if you are killed near a team member.",
	"Friendly objectives may not generate supplies but provide 500 supplies on first capture.",
	"In Seize & Secure, the Ticket System may scale by number of players."
	};


	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		CampaignInfo = SCR_GameModeCampaign.GetInstance();
		m_gCampaign = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		
		
		if (!isDead)
			Show(false);
		
		
		m_wDeathOverlay = m_wRoot.FindAnyWidget("OnDeathInfoDisplay");
		m_wRestrictionZoneOverlay = m_wRoot.FindAnyWidget("WarningZoneDistanceVisuals");
		m_wRestrictionZoneOverlay2 = m_wRoot.FindAnyWidget("WarningVisualsHolder");
		m_wTimeRemaining = TextWidget.Cast(m_wRoot.FindAnyWidget("TimeRemaining"));
		m_wDeathMessage = TextWidget.Cast(m_wRoot.FindAnyWidget("DeathMessage"));
		m_wWarning = TextWidget.Cast(m_wRoot.FindAnyWidget("Warning"));
		m_wInjuries = TextWidget.Cast(m_wRoot.FindAnyWidget("Injuries"));
		m_wFlavor = TextWidget.Cast(m_wRoot.FindAnyWidget("Flavor"));
		m_wFormula = TextWidget.Cast(m_wRoot.FindAnyWidget("MultiplierFormula1"));
		m_wTips = TextWidget.Cast(m_wRoot.FindAnyWidget("Tips"));
		
//			if (m_gCampaign) // This is handled elsewhere
//				FindConditions(owner);
//				UpdateHUD();
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayUpdate(IEntity owner, float timeSlice)
	{

		if (!m_wRoot || !CampaignInfo)
			return;
		
		playerControlledEntity = GetGame().GetPlayerController().GetControlledEntity();
		characterController = SCR_CharacterControllerComponent.Cast(playerControlledEntity.FindComponent(SCR_CharacterControllerComponent));
		m_PlayerController = SCR_PlayerController.Cast(PlayerController.Cast(owner));
		chManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
	
		if (characterController.IsDead())
		{
			FindConditions(owner);
		}
		
		else
		{
			m_iCount = 0;
			Show(false);
		}
			
	}	
	
	//------------------------------------------------------------------------------------------------
	protected void HideHUD()
	{
		Show(false, UIConstants.FADE_RATE_SLOW);
	}
	
	//------------------------------------------------------------------------------------------------ v1
//	protected void UpdateHUDValues()
//	{
//		ChimeraWorld world = GetGame().GetWorld();
//		WorldTimestamp serverTime = world.GetServerTimestamp();
//			
//		m_iCount++;
//		if (m_iCount <= 75)
//		{
//			m_sTip = m_aTips[Math.RandomInt(0, m_aTips.Count())];
//		}
//		
//		if (!isDead) 
//		{	
//			m_wDeathMessage.SetVisible(false);
//			m_wTimeRemaining.SetVisible(false);
//			m_wWarning.SetVisible(false);
//			m_wInjuries.SetVisible(false);
//			m_wFlavor.SetVisible(false);
//			m_wFormula.SetVisible(false);
//			m_wTips.SetVisible(false);
//			m_wDeathOverlay.SetVisible(false);
//		}
//		
//		else
//		{
//			countdown = m_wDeathTimestamp.DiffMilliseconds(serverTime);
//			m_wTimeRemaining.SetColor(Color.FromInt(Color.WHITE));
//			countdown = Math.Max(0, Math.Ceil(countdown / 1000)); // 1000
//			string shownTime = SCR_FormatHelper.GetTimeFormatting(countdown, ETimeFormatParam.DAYS | ETimeFormatParam.HOURS, ETimeFormatParam.DAYS | ETimeFormatParam.HOURS | ETimeFormatParam.MINUTES);
//			Faction playerFaction = m_cPlayerCharacter.GetFaction();
//			Faction killerFaction = m_cKillerEntity.GetFaction();
//			string killerName = GetGame().GetPlayerManager().GetPlayerName(m_iKillerInstigator.GetInstigatorPlayerID());
//
//			if (killerName == "")
//			{
//				killerName = "AI Character";
//			}
//
//			SCR_PopUpNotification.GetInstance().Offset(true);	
//			m_wTimeRemaining.SetText(shownTime);
//			m_wDeathMessage.SetTextFormat("YOU ARE DEAD.");
//			m_wTips.SetTextFormat(m_sTip);
//			
//			if (GetGame().GetGameMode().FindComponent(SCR_PersistentInjuryManagerComponent))
//			{
//				int playerId = SCR_PlayerController.GetLocalPlayerId();
//				SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
//				SCR_Iron_InjuryManagerComponent injuryComponent = SCR_Iron_InjuryManagerComponent.Cast(playerController.FindComponent(SCR_Iron_InjuryManagerComponent));
//
//				m_PlayerInjuryList = injuryComponent.GetPlayerInjuryList();
//				int injuryCount = m_PlayerInjuryList.Count();
//				string injuries;
//				for (int i = 0; i < injuryCount; i++)
//				{
//					string injury = m_PlayerInjuryList[i];
//					injuries = injuries + " | " + injury;
//				}
//				
//				if (!injuries.IsEmpty())
//				{
//					int length = injuries.Length() - 2;
//					string injuriesSubStr = injuries.Substring(2, length);
//					m_wInjuries.SetTextFormat("Injuries (%1): %2", injuryCount, injuriesSubStr);
//				}		
//			}
//			else
//			{
//				m_wInjuries.SetText("");
//			}
//			
//			if (chManager.TicketScoreSystem())
//			{
//				m_wFormula.SetTextFormat("Players: " + chManager.GetPlayerNumbers() + " | Multiplier: " + chManager.GetScoreMultiplier());
//			
//				if (playerFaction == killerFaction)
//				{
//					m_wWarning.SetTextFormat("KILLED BY FRIENDLY: " + (chManager.GetDeathTicketScore() - 2) + " SPAWN POINTS"); // -2 because points are coming from somewhere unknown
////						m_wWarning.SetTextFormat("KILLED BY FRIENDLY: " + (chManager.GetDeathTicketScore()) + " SPAWN POINTS");
//					m_wFlavor.SetText("Instigator: " + killerName);
//				}
//			
//				else
//				{
//					m_wWarning.SetTextFormat("" + chManager.GetDeathTicketScore() + " SPAWN POINTS FOR YOUR TEAM");
//					m_wFlavor.SetText("Killed by: " + killerName);
//				}	
//			}
//		
//			else
//			{
//					// Probably should use a switch instead
//				if (m_bIsScoreReduced && playerFaction != killerFaction && killerFaction.GetFactionKey() != "FIA")
//				{
//					m_wWarning.SetTextFormat("-100 POINTS FOR YOUR TEAM. +50 POINTS FOR ENEMY.");
//					m_wFlavor.SetText("Killed by: " + killerName);
//				}
//			
//				if (m_bIsScoreReduced && killerFaction.GetFactionKey() == "FIA")
//				{
//					m_wWarning.SetTextFormat("-100 POINTS FOR YOUR TEAM");
//					m_wFlavor.SetText("Killed by: " + killerName);
//				}
//			
//				if (playerFaction == killerFaction)
//				{
//					m_wWarning.SetTextFormat("YOU WERE KILLED BY A FRIENDLY");
//					m_wFlavor.SetText("Killed by: " + killerName);
//				}
//			
//				if (!m_bIsScoreReduced && playerFaction != killerFaction)
//				{
//					m_wWarning.SetTextFormat("BETTER LUCK NEXT TIME...");
//					m_wFlavor.SetText("Killed by: " + killerName);
//				}
//			}
//			
//
//			m_wDeathOverlay.SetVisible(true);
//			m_wDeathMessage.SetVisible(true);
//			m_wTimeRemaining.SetVisible(true);
//			m_wWarning.SetVisible(true);
//			m_wInjuries.SetVisible(true);
//			m_wFlavor.SetVisible(true);
//			m_wFormula.SetVisible(true);
//			m_wTips.SetVisible(true);
//		} 
//	}
	
	//------------------------------------------------------------------------------------------------ v2
	protected void UpdateHUDValues()
	{
	    ChimeraWorld world = GetGame().GetWorld();
	    WorldTimestamp serverTime = world.GetServerTimestamp();
	        
	    // Update tip counter and select a random tip
	    m_iCount++;
	    if (m_iCount <= 75)
	    {
	        m_sTip = m_aTips[Math.RandomInt(0, m_aTips.Count())];
	    }
	    
	    // If player is not dead, hide all death-related HUD elements
	    if (!isDead) 
	    {    
	        m_wDeathMessage.SetVisible(false);
	        m_wTimeRemaining.SetVisible(false);
	        m_wWarning.SetVisible(false);
	        m_wInjuries.SetVisible(false);
	        m_wFlavor.SetVisible(false);
	        m_wFormula.SetVisible(false);
	        m_wTips.SetVisible(false);
	        m_wDeathOverlay.SetVisible(false);
	    }
		
	    else
	    {
	        // Calculate countdown time
	        countdown = m_wDeathTimestamp.DiffMilliseconds(serverTime);
	        m_wTimeRemaining.SetColor(Color.FromInt(Color.WHITE));
	        countdown = Math.Max(0, Math.Ceil(countdown / 1000));
	        string shownTime = SCR_FormatHelper.GetTimeFormatting(
	            countdown, 
	            ETimeFormatParam.DAYS | ETimeFormatParam.HOURS, 
	            ETimeFormatParam.DAYS | ETimeFormatParam.HOURS | ETimeFormatParam.MINUTES
	        );
	        
	        // Get factions and killer name
	        Faction playerFaction = m_cPlayerCharacter.GetFaction();
	        Faction killerFaction = GetEntityFaction(m_cKillerEntity);
	        string killerName = GetGame().GetPlayerManager().GetPlayerName(m_iKillerInstigator.GetInstigatorPlayerID());
	        if (killerName == "")
	        {
	            killerName = "AI Character";
	        }
	        
	        // Basic HUD updates
	        SCR_PopUpNotification.GetInstance().Offset(true);    
	        m_wTimeRemaining.SetText(shownTime);
	        m_wDeathMessage.SetTextFormat("YOU ARE DEAD.");
	        m_wTips.SetTextFormat(m_sTip);
	        
	        // Update injuries
	        if (GetGame().GetGameMode().FindComponent(SCR_PersistentInjuryManagerComponent))
			{
			    int playerId = SCR_PlayerController.GetLocalPlayerId();
			    SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
			    SCR_Iron_InjuryManagerComponent injuryComponent = SCR_Iron_InjuryManagerComponent.Cast(playerController.FindComponent(SCR_Iron_InjuryManagerComponent));
			    
			    m_PlayerInjuryList = injuryComponent.GetPlayerInjuryList();
			    int injuryCount = m_PlayerInjuryList.Count();
			    if (injuryCount > 0)
			    {
			        string injuries = "";
			        for (int i = 0; i < injuryCount; i++)
			        {
			            if (i > 0)
			                injuries += " | ";
			            injuries += m_PlayerInjuryList[i];
			        }
			        m_wInjuries.SetTextFormat("Injuries (%1): %2", injuryCount, injuries);
			    }
			    else
			    {
			        m_wInjuries.SetText("");
			    }
			}
			
			else
			{
			    m_wInjuries.SetText("");
			}
	        
	        // Score message logic
	        bool isFriendlyKill = (playerFaction == killerFaction);
	        bool isFIAKill = (killerFaction && killerFaction.GetFactionKey() == "FIA");
	        
			m_wWarning.SetTextFormat("BETTER LUCK NEXT TIME...");
	        m_wFlavor.SetText("Killed by: " + killerName);
			
	        if (chManager.TicketScoreSystem())
	        {
	            m_wFormula.SetTextFormat("Players: %1 | Multiplier: %2", chManager.GetPlayerNumbers(), chManager.GetScoreMultiplier());
	            
	            if (isFriendlyKill)
	            {
	                m_wWarning.SetTextFormat("KILLED BY FRIENDLY: %1 SPAWN POINTS", chManager.GetDeathTicketScore() - 2); // -2 due to unknown point source
	                m_wFlavor.SetText("Instigator: " + killerName);
	            }
	            else
	            {
	                m_wWarning.SetTextFormat("YOUR DEATH JUST COST YOUR TEAM: %1 TICKETS", chManager.GetDeathTicketScore());
	                m_wFlavor.SetText("Killed by: " + killerName);
	            }
	        }
	        else
	        {
	            if (m_bIsScoreReduced && !isFriendlyKill && !isFIAKill)
	            {
	                m_wWarning.SetTextFormat("-100 POINTS FOR YOUR TEAM. +50 POINTS FOR ENEMY.");
	                m_wFlavor.SetText("Killed by: " + killerName);
	            }
	            else if (m_bIsScoreReduced && isFIAKill)
	            {
	                m_wWarning.SetTextFormat("-100 POINTS FOR YOUR TEAM");
	                m_wFlavor.SetText("Killed by: " + killerName);
	            }
	            else if (isFriendlyKill)
	            {
	                m_wWarning.SetTextFormat("YOU WERE KILLED BY A FRIENDLY");
	                m_wFlavor.SetText("Killed by: " + killerName);
	            }
	            else
	            {
	                m_wWarning.SetTextFormat("BETTER LUCK NEXT TIME...");
	                m_wFlavor.SetText("Killed by: " + killerName);
	            }
	        }
	        
	        // Show all death-related HUD elements
	        m_wDeathOverlay.SetVisible(true);
	        m_wDeathMessage.SetVisible(true);
	        m_wTimeRemaining.SetVisible(true);
	        m_wWarning.SetVisible(true);
	        m_wInjuries.SetVisible(true);
	        m_wFlavor.SetVisible(true);
	        m_wFormula.SetVisible(true);
	        m_wTips.SetVisible(true);
	    } 
	}

	//------------------------------------------------------------------------------------------------
	// Helper function to get faction from an entity (character or vehicle)
	Faction GetEntityFaction(IEntity entity)
	{
	    if (!entity)
	        return null;
	
	    // Check if it's a character
	    CharacterControllerComponent charController = CharacterControllerComponent.Cast(entity.FindComponent(CharacterControllerComponent));
	    if (charController)
	    {
	        FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
	        if (factionComp)
	            return factionComp.GetAffiliatedFaction();
	    }
		
	    else
	    {
	        // Check if it's a vehicle
	        Vehicle vehicle = Vehicle.Cast(entity);
	        if (vehicle && vehicle.IsOccupied())
	        {
	            IEntity commander = vehicle.GetPilot();
	            if (commander)
	            {
	                FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(commander.FindComponent(FactionAffiliationComponent));
	                if (factionComp)
	                    return factionComp.GetAffiliatedFaction();
	            }
	        }
	    }
		
	    return null;
	}
	
	//------------------------------------------------------------------------------------------------
	void FindConditions(IEntity owner)
	{

		if (!CampaignInfo || !owner || !characterController)
			return;
		
		m_cKillerEntity = SCR_ChimeraCharacter.Cast(characterController.GetKillerEntity());
		WorldTimestamp serverTime = SCR_GameModeCampaign.GetInstance().GetWorld().GetTimestamp();
		m_wDeathTimestamp = characterController.GetDeathTimestamp();
		m_cPlayerCharacter = characterController.GetPlayerCharacter();
		m_wDeathOverlay = m_wRoot.FindAnyWidget("OnDeathInfoDisplay");
		m_iKillerInstigator = characterController.GetKillerInstigator();
		m_iPlayerID = m_PlayerController.GetPlayerId();
		playerManager = GetGame().GetPlayerManager();
		player = SCR_ChimeraCharacter.Cast(playerManager.GetPlayerControlledEntity(m_iPlayerID));
		scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
		factionScore = scoringSystem.GetFactionScore(player.GetFaction());
		
		if (factionScore >= 100)
		{
			m_bIsScoreReduced = true;
		}


		if (characterController.IsDead())
		{	
			isDead = true;	
		}
		
		else
		{
			isDead = false;
		}
		
		if (m_wDeathTimestamp.DiffMilliseconds(serverTime) <= 0)
		{
			isDead = false;
			m_wWarning.SetTextFormat("");
			m_wInjuries.SetText("");
			m_wFlavor.SetText("");
			m_wFormula.SetText("");
			m_wTips.SetText("");
			Show(false);
			GetGame().GetCallqueue().CallLater(HideHUD);
		}
		
		UpdateHUD();
			
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateHUD()
	{

//		if (!m_wRoot || !world || !CampaignInfo || !CampaignInfo.ShowBattlePrepTimer())
//			return;
	
		if (isDead)
		{
			Show(true);
			GetGame().GetCallqueue().Remove(HideHUD);
			UpdateHUDValues();
		}
		
		else
		{
			m_bIsScoreReduced = false;
			Show(false);
			GetGame().GetCallqueue().CallLater(HideHUD);
			return;
		}

	}

}
