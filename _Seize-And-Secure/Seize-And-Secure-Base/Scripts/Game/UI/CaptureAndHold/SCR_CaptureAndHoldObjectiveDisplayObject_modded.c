//------------------------------------------------------------------------------------------------
/*!
	This object serves as a wrapper for individual HUD objective widgets.
*/
modded class SCR_CaptureAndHoldObjectiveDisplayObject
{
	protected const float m_fTopDistance = 100.0; // 70.0
	protected ImageWidget m_wSignalIcon;
	protected TextWidget m_wSignalText;
	protected bool m_bMapIsOpen;
	protected bool m_bMapInvokerInitialized;
	
	protected SCR_CampaignMilitaryBaseComponent m_ClosestBase;
	
//	m_wSignalText = TextWidget.Cast(m_wRoot.FindAnyWidget("NoRadioSignal"));
//	m_wSignalIcon = ImageWidget.Cast(m_wRoot.FindAnyWidget("Icon_NoSignal"));
//	SCR_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
//	SCR_MapEntity.GetOnMapClose().Insert(OnMapClose);
	
	//------------------------------------------------------------------------------------------------
	//! Update this widget as static UI element, ie. not reprojecting it automatically.
	void UpdateStatic(SCR_CaptureAndHoldArea area, float timeSlice)
	{
		IEntity areaEntity = area.GetRootParent();
		IEntity playerEntity = SCR_PlayerController.GetLocalMainEntity();
		FactionAffiliationComponent playerFactionComp = FactionAffiliationComponent.Cast(playerEntity.FindComponent(FactionAffiliationComponent));
		Faction playerFaction = playerFactionComp.GetAffiliatedFaction();
		
		bool baseReceivingSignal = BaseIsReceivingRadioSignal(m_AffiliatedArea.GetClosestBase(), playerFaction);
		if (!baseReceivingSignal)
		{
			m_wRoot.SetVisible(false);
			return;
		}
		else
		{
			m_wRoot.SetVisible(true);
		}
		
		m_wDistanceText.SetVisible(false);
		UpdateImageState(m_wRoot, timeSlice);
		// Set objective symbol
		m_wNameText.SetText(m_AffiliatedArea.GetAreaSymbol());		

		m_wSignalText = TextWidget.Cast(m_wRoot.FindAnyWidget("NoRadioSignal"));
		m_wSignalIcon = ImageWidget.Cast(m_wRoot.FindAnyWidget("Icon_NoSignal"));
//		m_wSignalText.SetVisible(!HasRadioSignal(areaEntity, playerFaction));
//		m_wSignalIcon.SetVisible(!HasRadioSignal(areaEntity, playerFaction));
		
		m_wSignalText.SetVisible(!BaseIsReceivingRadioSignal(m_AffiliatedArea.GetClosestBase(), playerFaction));
		m_wSignalIcon.SetVisible(!BaseIsReceivingRadioSignal(m_AffiliatedArea.GetClosestBase(), playerFaction));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update this widget as dynamic UI element projected to screen space
	override void UpdateDynamic(IEntity playerEntity, float timeSlice)
	{
//		IEntity areaEntity = m_AffiliatedArea.GetRootParent();
//		UpdateRadioSignalStatus(areaEntity);

//		if (!m_bMapInvokerInitialized)
//		{
//			InitializeMapInvoker();
//		}
		
		SCR_MapEntity mapManager = SCR_MapEntity.GetMapInstance();
		if (mapManager.IsOpen())
		{
			m_bMapIsOpen = true;
		}
		else
		{
			m_bMapIsOpen = false;
		}
		
		m_wSignalText = TextWidget.Cast(m_wRoot.FindAnyWidget("NoRadioSignal"));
		m_wSignalIcon = ImageWidget.Cast(m_wRoot.FindAnyWidget("Icon_NoSignal"));
//		m_wSignalText.SetVisible(!HasRadioSignal(playerEntity));
//		m_wSignalIcon.SetVisible(!HasRadioSignal(playerEntity));
		
		FactionAffiliationComponent playerFactionComp = FactionAffiliationComponent.Cast(playerEntity.FindComponent(FactionAffiliationComponent));
		Faction playerFaction = playerFactionComp.GetAffiliatedFaction();
		m_wSignalText.SetVisible(!BaseIsReceivingRadioSignal(m_AffiliatedArea.GetClosestBase(), playerFaction));
		m_wSignalIcon.SetVisible(!BaseIsReceivingRadioSignal(m_AffiliatedArea.GetClosestBase(), playerFaction));
		
		vector objectiveWorldPosition = m_AffiliatedArea.GetWorldObjectiveCenter();
		vector projectedScreenPosition = m_wRoot.GetWorkspace().ProjWorldToScreen(objectiveWorldPosition, m_AffiliatedArea.GetWorld());

		vector cameraMatrix[4];
		m_AffiliatedArea.GetWorld().GetCurrentCamera(cameraMatrix);

		float alpha = 1.0;
		bool visible = true;

		// This case should rarely happen as we do not expect spawn protection area
		// to overlay a capture area, but regardless..
		bool isPopupDrawn;
		SCR_PopUpNotification popupNotifications = SCR_PopUpNotification.GetInstance();
		if (popupNotifications && popupNotifications.GetCurrentMsg())
			isPopupDrawn = true;

		// Center to screen and stay fixed
		if (playerEntity && m_AffiliatedArea.IsCharacterInside(SCR_ChimeraCharacter.Cast(playerEntity)) && !isPopupDrawn)
		{
			// Update screen space blend
			m_fBlendScreenPosition += (timeSlice * POSITION_BLEND_SPEED);
			visible = true; // Always draw inside
		}
		else
		{
			// Worldprojection
			vector dirToCamera = (objectiveWorldPosition - cameraMatrix[3]).Normalized();
			float dot = vector.Dot(dirToCamera, cameraMatrix[2]);
			if (dot < 0.6666 && m_fBlendScreenPosition <= 0) // Force animation
				visible = false;

			float alphaScale = Math.InverseLerp(0.667, 1.0, dot);
			alphaScale = Math.Clamp(alphaScale, 0.0, 1.0);

			if (m_fBlendScreenPosition <= 0.0)
				alpha = Math.Lerp(0.5, 1.0, alphaScale);
			else
				alpha = 1.0;

			// Blend the point out significantly in optics, because the projection is misaligned
			if (ArmaReforgerScripted.IsScreenPointInPIPSights(projectedScreenPosition, ArmaReforgerScripted.GetCurrentPIPSights()))
				alpha = Math.Min(alpha, 0.65);

			m_fBlendScreenPosition -= (timeSlice * POSITION_BLEND_SPEED);
		}

		// Clamp the blend so we don't overshoot
		m_fBlendScreenPosition = Math.Clamp(m_fBlendScreenPosition, 0.0, 1.0);

		// If not visible, do not draw and that's it
		if (!visible)
		{
			m_wRoot.SetVisible(false);
			return;
		}

		// Otherwise update widget
		m_wDistanceText.SetVisible(false); // true

		// Distance text
		float distance = vector.Distance(cameraMatrix[3], objectiveWorldPosition);
		distance = Math.Round(distance);
		m_wDistanceText.SetTextFormat("#AR-CAH-Objective_Distance", distance);

		// Update image state
		UpdateImageState(m_wRoot, timeSlice);

		// Opacity
		m_wRoot.SetOpacity(alpha);

		float x, y;
		float scale = 1.0;
		float textScale = 1.0;
		// Interpolate position
		if (m_fBlendScreenPosition > 0.0)
		{
			WorkspaceWidget ww = m_wRoot.GetWorkspace();
			int w = ww.GetWidth();
			int h = ww.GetHeight();
			float fixedX = ww.DPIUnscale(0.5 * w);
			float fixedY = ww.DPIUnscale(0.15 * h);

			SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
			if (editorManager && editorManager.IsOpened())
				fixedY += 40;

			x = Math.Lerp(projectedScreenPosition[0], fixedX, m_fBlendScreenPosition);
			y = Math.Lerp(projectedScreenPosition[1], fixedY, m_fBlendScreenPosition);
			scale = m_fBlendScreenPosition * 2.25;
			textScale = m_fBlendScreenPosition * 1.85;
		}
		else
		{
			x = projectedScreenPosition[0];
			y = projectedScreenPosition[1];
		}

		float xScale = m_fOriginalXSize * scale;
		float yScale = m_fOriginalYSize * scale;
		// Apply
		FrameSlot.SetPos(m_wRoot, x - 0.5 * xScale, y - 0.5 * yScale);

		// Scale
		FrameSlot.SetSize(m_wLayout, xScale, yScale);

		m_wNameText.SetExactFontSize(m_fOriginalTitleSize * textScale);
		m_wDistanceText.SetExactFontSize(m_fOriginalTextSize * textScale);

		// Set objective symbol
		m_wNameText.SetText(m_AffiliatedArea.GetAreaSymbol());

		// And make widget visible		
		if (distance > m_fTopDistance || m_bMapIsOpen)
		{
			m_wRoot.SetVisible(false);
		}
		
		else 
		{
			m_wRoot.SetVisible(true);
		}
	}
	
//	//------------------------------------------------------------------------------------------------
//	//! Update image state according to provided area state
//	protected override void UpdateImageState(Widget root, float timeSlice)
//	{
//		Faction owningFaction = m_AffiliatedArea.GetOwningFaction();
//
//		if (!BaseIsReceivingRadioSignal(m_AffiliatedArea.GetClosestBase(), SCR_FactionManager.SGetLocalPlayerFaction())) //Replication.IsClient()
//		{
//			m_wRoot.SetVisible(false);
//			return;
//		}
//		else
//		{
//			m_wRoot.SetVisible(true);
//		}
//		
//		Color targetColor;
//		if (!owningFaction)
//			targetColor = Color.FromRGBA(249, 210, 103, 255);
//		else
//			targetColor = owningFaction.GetFactionColor();
//
//		int rootColor = m_wRoot.GetColor().PackToInt();
//		Color currentColor = Color.FromInt(rootColor);
//		if (currentColor != targetColor)
//			currentColor.Lerp(targetColor, timeSlice * COLOR_BLEND_SPEED);
//
//		// Only show major symbol when area is marked as one
//		m_wMajorIcon.SetVisible(m_AffiliatedArea.IsMajor());
//
//		// Imageset for Base state icon
//		const string AtlasImageset = "{225B7CAD5CEC4AE3}UI/Imagesets/CaptureAndHold/CaptureAndHoldAtlas.imageset";
//
//		// Area is completely neutral
//		if (!owningFaction)
//		{
//			m_wIcon.LoadImageFromSet(0,AtlasImageset, "CAH_NEUTRAL_LARGE");
//			m_wBackdropIcon.LoadImageFromSet(0,AtlasImageset, "CAH_NEUTRAL_BACKDROP");
//
//			m_wRoot.SetColor(currentColor);
//			// With no owning faction, there is no contesting faction
//			m_wUnderAttackIcon.SetVisible(false);
//			return;
//		}
//
//		// TODO: Improvement desirable
//		// This is far from ideal and is not sandbox enough.
//		FactionKey factionKey = owningFaction.GetFactionKey();
//		if (factionKey == "US" || factionKey == "FIA")
//		{
//			m_wIcon.LoadImageFromSet(0,AtlasImageset, "CAH_BLUFOR_LARGE");
//			m_wBackdropIcon.LoadImageFromSet(0,AtlasImageset, "CAH_BLUFOR_BACKDROP");
//
//			m_wUnderAttackIcon.SetRotation(0);
//		}
//		else if (factionKey == "USSR")
//		{
//			m_wIcon.LoadImageFromSet(0,AtlasImageset, "CAH_OPFOR_LARGE");
//			m_wBackdropIcon.LoadImageFromSet(0,AtlasImageset, "CAH_OPFOR_BACKDROP");
//
//			m_wUnderAttackIcon.SetRotation(45);
//		}
//
//		m_wRoot.SetColor(currentColor);
//
//		Faction contestingFaction = m_AffiliatedArea.GetContestingFaction();
//		if (contestingFaction)
//		{
//			m_wUnderAttackIcon.SetVisible(true);
//			// Pulsing animation
//			//Color col = new Color(1.0, 1.0, 1.0, 1.0);
//			Color col = Color.FromRGBA(249, 210, 103, 255);
//			float val01 = Math.Sin( m_AffiliatedArea.GetWorld().GetWorldTime() * 0.01 ) * 0.5 + 0.5;
//			col.Lerp(contestingFaction.GetFactionColor(), val01);
//			m_wUnderAttackIcon.SetColor(col);
//
//			//Color selfCol = new Color(1.0, 1.0, 1.0, 1.0);
//			Color selfCol = Color.FromRGBA(249, 210, 103, 255);
//			selfCol.Lerp(owningFaction.GetFactionColor(), val01);
//			m_wRoot.SetColor(selfCol);
//		}
//		else
//		{
//			m_wUnderAttackIcon.SetVisible(false);
//		}
//	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateRadioSignalStatus(IEntity areaEntity)
	{
//		m_wSignalText.SetVisible(!HasRadioSignal(areaEntity, faction));
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasRadioSignal(IEntity areaEntity, Faction faction)
	{
		return SCR_GameModeCampaign.GetInstance().GetBaseManager().IsEntityInFactionRadioSignal(areaEntity, faction);
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasRadioSignal(IEntity playerEntity)
	{
		SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
		Faction characterFaction = playerCharacter.GetFaction();
		return SCR_GameModeCampaign.GetInstance().GetBaseManager().IsEntityInFactionRadioSignal(playerCharacter, characterFaction);
	}
	
	//------------------------------------------------------------------------------------------------
	bool BaseIsReceivingRadioSignal(SCR_CampaignMilitaryBaseComponent base, Faction characterFaction)
	{
		if (!characterFaction || !base)
		{
			PrintFormat("SCR_CaptureAndHoldObjectiveDisplayObject: BaseIsReceivingRadioSignal: Base: %1 | Character Faction: %2", base, characterFaction);
			return false;
		}
					
		return base.IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(characterFaction));
	}
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	//! \param[in] config
	protected void OnMapOpen(MapConfiguration config)
	{
			m_bMapIsOpen = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	//! \param[in] config
	protected void OnMapClose(MapConfiguration config)
	{
			m_bMapIsOpen = false;
	}
	
	//------------------------------------------------------------------------------------------------
	void InitializeMapInvoker()
	{
		if (!m_bMapInvokerInitialized)
		{				
			SCR_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
			SCR_MapEntity.GetOnMapClose().Insert(OnMapClose);
			
			m_bMapInvokerInitialized = true;
		}		
	}
}