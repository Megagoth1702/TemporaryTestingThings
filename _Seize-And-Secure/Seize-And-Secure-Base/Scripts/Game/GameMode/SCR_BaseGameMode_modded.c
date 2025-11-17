//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{

	SCR_GameModeCampaign CampaignInfo;

	//------------------------------------------------------------------------------------------------
	override protected void OnGameModeStart()
	{
		super.OnGameModeStart();
		EnforceMinVideoSettings();
		
		SCR_NotificationSenderComponent sender = SCR_NotificationSenderComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_NotificationSenderComponent));
		if (!sender)
			return;
		
		sender.SetKillFeedType(EKillFeedType.DISABLED);
		sender.SetReceiveKillFeedType(EKillFeedReceiveType.ALL);
	}
	
	//------------------------------------------------------------------------------------------------
	void EnforceMinVideoSettings()
	{	
		BaseContainer videoSettings = GetGame().GetEngineUserSettings().GetModule("VideoUserSettings");
		BaseContainer grassSettings = GetGame().GetEngineUserSettings().GetModule("GrassMaterialSettings");		
		int distantShadows, distantShadowsQuality, grassLod, grassLOD, grassDistance;
		
		videoSettings.Get("DistantShadows", distantShadows); 				// "VideoUserSettings", "DistantShadowsQuality", "DistantShadows"
		videoSettings.Get("DistantShadowsQuality", distantShadowsQuality); 	// "VideoUserSettings", "DistantShadowsQuality", "DistantShadows"
		grassSettings.Get("Lod", grassLod); 								// "GrassMaterialSettings", "Lod", "GrassLOD"
		grassSettings.Get("GrassLOD", grassLOD); 							// "GrassMaterialSettings", "Lod", "GrassLOD"
		grassSettings.Get("Distance", grassDistance); 						// "GrassMaterialSettings", "Distance", "GrassDistance"
		
		// Enforce minimum video settings
		if (videoSettings)
		{
			if (distantShadowsQuality < 1)
			{
				videoSettings.Set("DistantShadowsQuality", 1);
				Print("EnforceVideoSettings: Distant Shadows Quality Enforced");
				GetGame().UserSettingsChanged();
			}					
		}

		// Enforce minimum grass settings
		if (grassSettings)
		{
			if (grassLod < 2)
			{
				grassSettings.Set("Lod", 2);
				Print("EnforceVideoSettings: Grass LODs Enforced");
				GetGame().UserSettingsChanged();	
			}	
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		CampaignInfo = SCR_GameModeCampaign.GetInstance();
	}
}
