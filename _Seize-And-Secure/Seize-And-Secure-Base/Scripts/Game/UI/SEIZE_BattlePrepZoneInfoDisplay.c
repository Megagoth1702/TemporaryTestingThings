class SEIZE_BattlePrepZoneInfoDisplay : SCR_InfoDisplayExtended
{
	Widget m_wCountdownOverlay;
	RichTextWidget m_wCountdown;
	RichTextWidget m_wFlavour;
	
	SCR_GameModeCampaign m_gCampaign;
	SCR_GameModeCampaign CampaignInfo;
	WorldTimestamp prepTime;
//	SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//	SCR_BattlePrepZoneEntity PrepEntity;
//	int PrepTime = PrepEntity.GetBattlePrepTime();
//	SCR_BattlePrepZoneEntity PrepInfo = SCR_BattlePrepZoneEntity.Cast(CampaignInfo.FindComponent(SCR_BattlePrepZoneEntity));
//  WorldTimestamp prepTime = CampaignInfo.GetWorld().GetTimestamp().PlusMilliseconds(300000); //(BATTLE_PREP_TIME_S * 1000)	
	
	bool showBattlePrepTimer;
	bool isBattlePrepOver;

	
	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		RestrictedZoneMan = SCR_BattlePrepZoneEntity.Cast(GetGame().GetGameMode().Get);
//		prepTime = CampaignInfo.GetWorld().GetTimestamp().PlusMilliseconds(300000); //(BATTLE_PREP_TIME_S * 1000)
		float timeAdded = CampaignInfo.GetBattlePrepTime() * 1000;
		prepTime = CampaignInfo.GetWorld().GetTimestamp().PlusMilliseconds(timeAdded);
		showBattlePrepTimer = CampaignInfo.ShowBattlePrepTimer();	
		m_gCampaign = SCR_GameModeCampaign.Cast(GetGame().GetGameMode());
		
	if (!showBattlePrepTimer)
		Show(false);

		m_wCountdownOverlay = m_wRoot.FindAnyWidget("Countdown");
		m_wCountdown = RichTextWidget.Cast(m_wRoot.FindAnyWidget("CountdownWin"));
		m_wFlavour = RichTextWidget.Cast(m_wRoot.FindAnyWidget("FlavourText"));
		
			if (m_gCampaign)
				UpdateHUD();
	}
	
	override void DisplayUpdate(IEntity owner, float timeSlice)
	{

//		if (!m_wRoot || !world || !CampaignInfo || !CampaignInfo.ShowBattlePrepTimer())
//			return;
		if (!CampaignInfo)
			return;

			UpdateHUD();
	}	
	
	//------------------------------------------------------------------------------------------------
	protected void HideHUD()
	{
		Show(false, UIConstants.FADE_RATE_SLOW);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateHUDValues()
	{
		ChimeraWorld world = GetGame().GetWorld();
		WorldTimestamp serverTime = world.GetServerTimestamp();
		
		if (prepTime.DiffMilliseconds(serverTime) <= 0) 
			{
				isBattlePrepOver = true;
				m_wFlavour.SetVisible(false);
				m_wCountdown.SetVisible(false);
				m_wCountdownOverlay.SetVisible(false);
			}
		
		else
			{
				float prepCountdown;
	
				prepCountdown = prepTime.DiffMilliseconds(serverTime);
				m_wCountdown.SetColor(Color.FromInt(Color.WHITE));
		
				prepCountdown = Math.Max(0, Math.Ceil(prepCountdown / 1000));
				
				string shownTime = SCR_FormatHelper.GetTimeFormatting(prepCountdown, ETimeFormatParam.DAYS | ETimeFormatParam.HOURS, ETimeFormatParam.DAYS | ETimeFormatParam.HOURS | ETimeFormatParam.MINUTES);
				m_wCountdown.SetText(shownTime);
				SCR_PopUpNotification.GetInstance().Offset(true);
				
				m_wFlavour.SetTextFormat("BATTLE PREP ONGOING");		
				m_wCountdownOverlay.SetVisible(true);
				m_wFlavour.SetVisible(true);
			} 
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateHUD()
	{

//		if (!m_wRoot || !world || !CampaignInfo || !CampaignInfo.ShowBattlePrepTimer())
//			return;
		
		if (!showBattlePrepTimer)
			{
			Show(false);
				return;
			}
		
		if (isBattlePrepOver)
		{
			GetGame().GetCallqueue().CallLater(HideHUD);
		}
		
		else
		{
			GetGame().GetCallqueue().Remove(HideHUD);
			Show(true, UIConstants.FADE_RATE_FAST);
			UpdateHUDValues();
		}
	}
}
