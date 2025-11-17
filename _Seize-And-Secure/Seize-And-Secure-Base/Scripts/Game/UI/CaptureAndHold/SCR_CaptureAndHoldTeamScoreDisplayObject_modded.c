//! This object serves as a wrapper for individual HUD team score widgets.
modded class SCR_CaptureAndHoldTeamScoreDisplayObject
{
	protected ref array<Faction> m_Factions = {};
	
	protected Widget m_wScoreLeft;
	protected FrameWidget m_wScoreRight;
	
	protected ImageWidget m_wIcon_Left;
	protected ImageWidget m_wIcon_Right;
	
	protected ImageWidget m_wBackdropIcon_Left;
	protected ImageWidget m_wBackdropIcon_Right;
	
	protected TextWidget m_wScoreText_Left;
	protected TextWidget m_wScoreText_Right;
	
	//------------------------------------------------------------------------------------------------
	//! Finds a widget by name, either available variant (default, _Left, _Right)
//	override protected Widget FindScoreWidget(notnull Widget parent, string name)
//	{
//		Widget defaultWidget = parent.FindAnyWidget(name);
//		if (defaultWidget)
//			return defaultWidget;
//
//		Widget leftWidget = parent.FindAnyWidget(name + "_Left");
//		if (leftWidget)
//			return leftWidget;
//
//		Widget rightWidget = parent.FindAnyWidget(name + "_Right");
//		if (rightWidget)
//			return rightWidget;
//
//		return null;
//	}
//	
//	//------------------------------------------------------------------------------------------------
//	//! Creates new score object wrapper for provided faction.
//	void SCR_CaptureAndHoldTeamScoreDisplayObject(notnull Widget root, notnull Faction faction)
//	{
//		m_wRoot = root;
//		m_wFactionImage = ImageWidget.Cast(FindScoreWidget(root, "Icon_Faction"));
//		m_wBarImage = ImageWidget.Cast(FindScoreWidget(root, "ScoreBar_Fill"));
//		m_wScoreText = TextWidget.Cast(FindScoreWidget(root, "ScoreBar_Text"));
//
//		m_pAffiliatedFaction = faction;
//
//		ResourceName iconResource;
//		SCR_Faction scrFaction = SCR_Faction.Cast(faction);
//		if (scrFaction)
//		{
//			iconResource = scrFaction.GetFactionFlag();
//		}
//		else
//		{
//			// Set icon directly
//			UIInfo factionInfo = faction.GetUIInfo();
//			if (factionInfo)
//			iconResource = factionInfo.GetIconPath();
//		}
//
//		int factionColor = faction.GetFactionColor().PackToInt();
//		if (!iconResource.IsEmpty())
//		{
//			int whiteColor = Color.White.PackToInt();
//			m_wFactionImage.SetColor(Color.FromInt(whiteColor));
//			m_wFactionImage.LoadImageTexture(0, iconResource);
//			m_wFactionImage.SetImage(0);
//		}
//		else
//			m_wFactionImage.SetColor(Color.FromInt(factionColor));
//
//		// Color elements
//		m_wRoot.SetColor(faction.GetFactionColor());
//	}
//	
	
	//------------------------------------------------------------------------------------------------
	//! Updates state of this object.
	override void Update(int score, int maxScore)
	{
		float progress01;
		if (maxScore > 0)
			progress01 = score / (float)maxScore;

		SCR_CaptureAndHoldManager chManager;
		SCR_GameModeCampaign gameModeCampaign = SCR_GameModeCampaign.GetInstance();
		if (gameModeCampaign)
		{
			chManager = SCR_CaptureAndHoldManager.Cast(gameModeCampaign.FindComponent(SCR_CaptureAndHoldManager));
		}

		if (chManager && chManager.TicketScoreSystem())
		{
			progress01 = score / chManager.GetFactionStartScore();
		}
		
		m_wBarImage.SetMaskProgress(progress01);		
		m_wScoreText.SetText(score.ToString());
	}
}
