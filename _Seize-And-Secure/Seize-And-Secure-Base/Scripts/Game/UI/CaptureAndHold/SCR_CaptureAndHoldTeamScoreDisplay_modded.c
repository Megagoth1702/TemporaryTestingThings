//------------------------------------------------------------------------------------------------
/*!
	This InfoDisplay allows drawing of individual HUD scoring elements for individual Capture & Hold factions.
	In addition it draws all available objectives in the screen space.

	Note:
		This HUD element only supports two factions at a single given time.
		In addition only the US, USSR and FIA factions are supported.
		Using more than two factions will yield unexpected results.
		
		Similar approach can be taken, but a list of elements or any other
		dynamically filled element would be a much more preferable choice.
*/
modded class SCR_CaptureAndHoldTeamScoreDisplay : SCR_InfoDisplayExtended
{
	[Attribute("{A458D11FCA957D4E}UI/layouts/HUD/CaptureAndHold/CaptureAndHoldScoringLayout_FactionOnly.layout", params: "layout")] //{56B174A06C7ECFAB}UI/Layouts/HUD/CaptureAndHold/CaptureandHoldObjectiveLayout.layout
	protected ResourceName m_rObjectiveHUDLayout;
	
	protected TextWidget m_wSignalText;
	protected bool m_bMapOpen;
	
	protected ref array<Faction> m_Factions = {};
	
	protected Widget m_wScoreLeft;
	protected Widget m_wScoreRight;
	
	protected ImageWidget m_wIcon_Left;
	protected ImageWidget m_wIcon_Right;
	
	protected ImageWidget m_wBackdropIcon_Left;
	protected ImageWidget m_wBackdropIcon_Right;
	
	protected TextWidget m_wScoreText_Left;
	protected TextWidget m_wScoreText_Right;
	
	SCR_CaptureAndHoldManager m_CHManager;
	
	//------------------------------------------------------------------------------------------------
	//! Creates individual hud elements.
	override void DisplayStartDraw(IEntity owner)
	{
		SCR_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		SCR_MapEntity.GetOnMapClose().Insert(OnMapClose);
		
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		if (!mapEntity.IsOpen())
			Show(false);
		
//		super.DisplayStartDraw(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clears all hud elements.
	override void DisplayStopDraw(IEntity owner)
	{
		super.DisplayStopDraw(owner);
		
		SCR_MapEntity.GetOnMapOpen().Remove(OnMapOpen);
		SCR_MapEntity.GetOnMapClose().Remove(OnMapClose);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates the progress and state of all available elements.
	override void DisplayUpdate(IEntity owner, float timeSlice)
	{
		SCR_BaseScoringSystemComponent scoringSystem = m_pGameMode.GetScoringSystemComponent();

		// Reposition scoring UI based on whether it is in a map or not
		if (m_wScoringFrame)
		{
			SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
			if (editorManager && editorManager.IsOpened())
			{
				FrameSlot.SetPos(m_wScoringFrame, 0.0, 72.0);
			}
			else
			{
				FrameSlot.SetPos(m_wScoringFrame, 0.0, 32.0);
			}
		}

		// Fade out points when a hint is shown to prevent clipping <--- moved this element to map, so not needed
//		if (m_wAreaLayoutWidget)
//		{
//			SCR_PopUpNotification notifications = SCR_PopUpNotification.GetInstance();
//			float targetOpacity = 1.0;
//			if (notifications && notifications.GetCurrentMsg())
//				targetOpacity = 0.0;
//
//			if (m_wAreaLayoutWidget.GetOpacity() != targetOpacity)
//				m_wAreaLayoutWidget.SetOpacity(Math.Lerp(m_wAreaLayoutWidget.GetOpacity(), targetOpacity, timeSlice * POINTS_LAYOUT_FADE_SPEED));
//		}

		// Update scoring
		SCR_CaptureAndHoldTeamScoreDisplayObject scoringObject;
		for (int i = 0, count = m_aScoringElements.Count(); i < count; i++)
		{
			scoringObject = m_aScoringElements[i];
			if (!scoringObject)
				continue;

			int score = 0;
			int maxScore = 0;
			if (scoringSystem)
			{
				score = scoringSystem.GetFactionScore(scoringObject.GetFaction());
				maxScore = scoringSystem.GetScoreLimit();
			}

			scoringObject.Update(score, maxScore);
		}

		// Filter out objective HUD items that are no longer linked to an area
		ClearObjectiveDisplays(true);

		// Make sure that there is enough elements
		array<SCR_CaptureAndHoldArea> areas = {};
		m_pAreaManager.GetAreas(areas);

		// If the count has changed, redraw elements (adding or removing widgets)
		if (m_aObjectiveElements.Count() != areas.Count())
		{
			ClearObjectiveDisplays(false);
			CreateObjectiveDisplays(areas);
		}

		// Update objective markers
		if (m_aObjectiveElements && m_aObjectiveElements.Count() > 0)
		{
			foreach (SCR_CaptureAndHoldObjectiveDisplayObject objective : m_aObjectiveElements)
			{
				if (!objective)
					continue;

				SCR_CaptureAndHoldArea area = objective.GetArea();
				objective.UpdateStatic(area, timeSlice);
//				objective.UpdateStatic(timeSlice);
			}
		}

		// Update remaining time widget if any
		if (m_wRemainingTimeWidget)
		{
			if (m_pGameMode.GetTimeLimit() <= 0.0) // Disable showing if no time limit is set
			{
				m_wRemainingTimeWidget.SetVisible(false);
			}
			else
			{
				// Enable if time limit is set, clamp to 0
				float remainingTime = m_pGameMode.GetRemainingTime();
				if (remainingTime < 0.0)
					remainingTime = 0.0;

				m_wRemainingTimeWidget.SetVisible(true);
				m_wRemainingTimeWidget.SetText(SCR_FormatHelper.FormatTime(remainingTime));
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateRadioSignalStatus(IEntity areaEntity)
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		array<Faction> factions = {};
		factionManager.GetFactionsList(factions);
		foreach (Faction faction : factions)
		{
//			m_wSignalText.SetVisible(HasRadioSignal(areaEntity, faction));
			m_wSignalText.SetVisible(BaseIsReceivingRadioSignal(SCR_GameModeCampaign.GetInstance().GetBaseManager().FindClosestBase(areaEntity.GetOrigin()), faction));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasRadioSignal(IEntity areaEntity, Faction faction)
	{
		return SCR_GameModeCampaign.GetInstance().GetBaseManager().IsEntityInFactionRadioSignal(areaEntity, faction);
	}
	
	//------------------------------------------------------------------------------------------------
	bool BaseIsReceivingRadioSignal(SCR_CampaignMilitaryBaseComponent base, Faction characterFaction)
	{
		if (!characterFaction || !base)
		{
			PrintFormat("SCR_CaptureAndHoldTeamScoreDisplay: BaseIsReceivingRadioSignal: Base: %1 | Character Faction: %2", base, characterFaction);
			return false;
		}
		
		return base.IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(characterFaction));
	}
	
	//------------------------------------------------------------------------------------------------
	void OnMapOpen(MapConfiguration config)
	{
		int playerId = SCR_PlayerController.GetLocalPlayerId();						
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		Faction pFaction = factionManager.GetPlayerFaction(playerId);
		if (!pFaction)
		{
			PrintFormat("OnMapOpen: No Faction for Player ID: %1", playerId);
			pFaction = GetFactionFromGroup(playerId);			
			if (!pFaction)
			{
				return;
			}
		}
		
		m_bMapOpen = true;	
		Show(true);
		
//		if (SCR_DeployMenuMain.GetDeployMenu() == null)
//		{
//			Show(true);
//		}
//		else
//		{
//			Show(false);
//		}
		
		if (m_wRoot)
		{
			// Root frame
			m_wScoringFrame = m_wRoot.FindAnyWidget("Score_Root");

			// Draw elements initially
			array<SCR_CaptureAndHoldArea> areas = {};
			m_pAreaManager.GetAreas(areas);
			CreateObjectiveDisplays(areas);
			
			array<Faction> factions = {};
			int count = GetGame().GetFactionManager().GetFactionsList(factions);
			
			SCR_Faction faction;
			for (int i = count - 1; i >= 0; i--)
			{
				faction = SCR_Faction.Cast(factions[i]);
				if (!faction.IsPlayable())
					factions.RemoveOrdered(i);
			}
			
			if (factions.Count() != 2)
			{
				Print("SCR_CaptureAndHoldTeamScoreDisplay: faction count for C&H must be 2 and is: '" +  factions.Count() + "' instead", LogLevel.ERROR);
				return;
			}

			// Left "side"
			Widget leftParent = m_wRoot.FindAnyWidget("Score_Left");
			if (leftParent)
			{
				ref SCR_CaptureAndHoldTeamScoreDisplayObject usObj = new SCR_CaptureAndHoldTeamScoreDisplayObject(leftParent, factions[0]);
				m_aScoringElements.Insert(usObj);
			}

			// Right "side"
			Widget rightParent = m_wRoot.FindAnyWidget("Score_Right");
			if (rightParent)
			{
				ref SCR_CaptureAndHoldTeamScoreDisplayObject ussrObj = new SCR_CaptureAndHoldTeamScoreDisplayObject(rightParent,  factions[1]);
				m_aScoringElements.Insert(ussrObj);
			}

			// Remaining time text in middle
			m_wRemainingTimeWidget = TextWidget.Cast(m_wRoot.FindAnyWidget("RemainingTime"));
			
			/////////////////////////////////////////
			m_wScoreLeft = m_wRoot.FindAnyWidget("Score_Left");
			m_wScoreRight = m_wRoot.FindAnyWidget("Score_Right");
			GetGame().GetFactionManager().GetFactionsList(m_Factions);
			IEntity playerEntity = SCR_PlayerController.GetLocalMainEntity();
			
			m_CHManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
			if (m_CHManager && playerEntity && !m_CHManager.ShowEnemyScore())
			{
				FactionAffiliationComponent playerFactionComp = FactionAffiliationComponent.Cast(playerEntity.FindComponent(FactionAffiliationComponent));
				Faction playerFaction = playerFactionComp.GetAffiliatedFaction();
				
//				if (playerFaction == m_Factions[0])
//				{
//					m_wScoreRight.SetVisible(false);
//				}
//				else
//				{
//					m_wScoreLeft.SetVisible(false);
//				}
				
				if (!playerFaction)
				{
					playerFaction = GetFactionFromGroup(playerId);
				}
				
				Faction faction0 = factions[0];
				Faction faction1 = factions[1];
				
				switch (playerFaction)
				{
					case factions[0]:
					{
						m_wScoreLeft.SetVisible(true);
						m_wScoreRight.SetVisible(false);
						break;
					}
					
					case factions[1]:
					{
						m_wScoreLeft.SetVisible(false);
						m_wScoreRight.SetVisible(true);
						break;
					}
					
					default:
					{
						m_wScoreLeft.SetVisible(false);
						m_wScoreRight.SetVisible(false);
						break;
					}			
				}				
				
			}
		}
		else
		{
			Print("SCR_CaptureAndHoldDisplay could not create scoring layout!");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnMapClose(MapConfiguration config)
	{
		m_bMapOpen = false;
		Show(false);
		// Release all wrappers for objectives
//		ClearObjectiveDisplays(false);
//
//		// Clear all scoring elements
//		for (int i = m_aScoringElements.Count() - 1; i >= 0; i--)
//		{
//			m_aScoringElements.Remove(i);
//		}
//		
//		m_aScoringElements.Clear();
	}

	//------------------------------------------------------------------------------------------------
	Faction GetFactionFromGroup(int playerId)
	{
		if (!playerId)
			return null;
		
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
			return null;		
		
		SCR_AIGroup group = groupsManager.GetPlayerGroup(playerId);
		if (!group)
			return null;
		
		Faction faction = group.GetFaction();
		if (!faction)
		{
			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			array<int> playerIds = group.GetPlayerIDs();
			if (playerIds.IsEmpty())
				return null;
			
			foreach (int id : playerIds)
			{
				faction = factionManager.SGetPlayerFaction(id);
				if (faction)
					return faction;
			}
		}
		
		return faction;
	}
}
