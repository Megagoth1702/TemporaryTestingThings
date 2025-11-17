modded class SCR_GameOverScreenContentUIComponent: ScriptedWidgetComponent
{
	/*!
	Fills the widgets of the gameover screen content
	\param endScreenUIContent contains the layout and any neccessary information for the endscreen content widget
	*/
	override void InitContent(SCR_GameOverScreenUIContentData endScreenUIContent)
	{
		super.InitContent(endScreenUIContent);
		
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (CampaignInfo)
		{
			SCR_CaptureAndHoldManager chManager = SCR_CaptureAndHoldManager.Cast(CampaignInfo.FindComponent(SCR_CaptureAndHoldManager));
			
			if (chManager && chManager.TicketScoreSystem())
			{				
				Faction winningFaction = EndGameAction.GetWinningFaction();
				if (!winningFaction)
					return;
				
				SCR_BaseScoringSystemComponent scoring = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
				int score = scoring.GetFactionScore(winningFaction);
				TextWidget ticketsRemainingWidget = TextWidget.Cast(m_wRoot.FindAnyWidget("GameOver_TicketsRemaining"));
		
				if (ticketsRemainingWidget)
					ticketsRemainingWidget.SetTextFormat("%1 Tickets Remaining: %2", winningFaction.GetFactionName(), score);
			}
			
		}
		
	}
};
