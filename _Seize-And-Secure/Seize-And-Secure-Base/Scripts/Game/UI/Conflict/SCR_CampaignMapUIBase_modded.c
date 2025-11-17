//------------------------------------------------------------------------------------------------
modded class SCR_CampaignMapUIBase : SCR_CampaignMapUIElement
{
	protected SCR_MapEntity m_PlayerMapEntity;
	protected ImageWidget m_RadioRangeWidget;
	protected float m_fRadioRange;
	protected float m_fCurrentZoom;
	protected float m_fPreviousZoom;
	protected float m_fCalculatedSize;
	protected const float SCALE = 2.3529;
	protected SCR_CampaignFaction m_CampaignFaction;
	protected ref Color m_FactionColor;
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		m_RadioRangeWidget = ImageWidget.Cast(w.FindAnyWidget("Iron_RadioRange"));	
	}
	
	//------------------------------------------------------------------------------------------------
	protected WorkspaceWidget GetUIWorkspace()
    {
        if (m_RadioRangeWidget)
            return m_RadioRangeWidget.GetWorkspace();
        
		return null; // Fallback if widget not ready
    }
	
	//------------------------------------------------------------------------------------------------
	protected override void InitBaseIcon()
	{
		if (!m_Base)
			return;

		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (m_PlayerFaction && CampaignInfo && CampaignInfo.GetHideBasesOutsideRadioRange())
		{
			bool hasRadioRange = m_Base.IsHQRadioTrafficPossible(m_PlayerFaction);
			
			m_wBaseIcon.SetVisible(hasRadioRange);
			m_wImageOverlay.SetVisible(hasRadioRange);
			m_wBaseFrame.SetVisible(hasRadioRange);
			m_wBaseIcon.SetVisible(hasRadioRange);
			m_wBaseName.SetVisible(hasRadioRange);
			m_wCallsignName.SetVisible(hasRadioRange);
			m_wBaseNameDialog.SetVisible(hasRadioRange);
			m_wCallsignNameDialog.SetVisible(hasRadioRange);		
		}
		
		SCR_CaptureAndHoldManager chManager = SCR_CaptureAndHoldManager.GetAreaManager();
		if (m_PlayerFaction && chManager && CampaignInfo && CampaignInfo.GetHideCAHZonesOutsideRadioRange())
		{
			array<SCR_CaptureAndHoldArea> chAreas = {};
			int areas = chManager.GetAreas(chAreas);
			
			foreach (SCR_CaptureAndHoldArea area : chAreas)
			{
				SCR_CampaignMilitaryBaseComponent closestBase = area.GetClosestBase();
				if (closestBase)// && closestBase.IsHQRadioTrafficPossible(m_PlayerFaction))
				{
					SCR_MapDescriptorComponent mapTarget = SCR_MapDescriptorComponent.Cast(area.FindComponent(SCR_MapDescriptorComponent));
					if (!mapTarget)
					{
						continue;
					}

					MapItem mapIcon = mapTarget.Item();
					mapIcon.SetVisible(closestBase.IsHQRadioTrafficPossible(m_PlayerFaction));
				}
			}
		}
		
		m_fRadioRange = m_Base.GetRadioRange();
		m_PlayerMapEntity = SCR_MapEntity.GetMapInstance();
		
		super.InitBaseIcon();
	}
	
	//------------------------------------------------------------------------------------------------
	override void ShowName(bool visible)
	{
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (m_Base && m_PlayerFaction && CampaignInfo && CampaignInfo.GetHideBasesOutsideRadioRange())
		{
			bool hasRadioRange = m_Base.IsHQRadioTrafficPossible(m_PlayerFaction);			
			super.ShowName(hasRadioRange);
		}
		
		else
		{
			super.ShowName(visible);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	//! \param[in] visible
	override void ChangeNameSize(bool visible)
	{
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (m_Base && m_PlayerFaction && CampaignInfo && CampaignInfo.GetHideBasesOutsideRadioRange())
		{
			bool hasRadioRange = m_Base.IsHQRadioTrafficPossible(m_PlayerFaction);
			super.ChangeNameSize(hasRadioRange);
		}		
		else
		{
			super.ChangeNameSize(visible);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnMapZoom(float pixelPerUnit)
	{
		WorkspaceWidget workspace = GetUIWorkspace();
        if (workspace)
        {
            float scaledSize = (m_fRadioRange * pixelPerUnit) * 2; // Diameter in scaled pixels (adjust multiplier if needed for radius/diameter)
            m_fCalculatedSize = workspace.DPIUnscale(scaledSize); // Unscale for SetSize
        }
        else
        {
            m_fCalculatedSize = (m_fRadioRange * pixelPerUnit) * 2; // Fallback without scaling (but this won't fix the issue)
        }
		
//		m_fCurrentZoom = m_PlayerMapEntity.GetCurrentZoom();
//		m_fPreviousZoom = m_fCurrentZoom;
//		m_fCalculatedSize = (m_fRadioRange * pixelPerUnit) * SCALE;
		
		//PrintFormat("SCR_CampaignMapUIBase: OnMapZoom: Current Zoom: %1 | Pixel Per Unit: %2", m_fCurrentZoom, pixelPerUnit);
		m_RadioRangeWidget.SetSize(m_fCalculatedSize, m_fCalculatedSize);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (m_MobileAssembly)
		{
			return super.OnMouseEnter(w, x, y);	
		}
		
		if (m_RadioRangeWidget)
		{
			WorkspaceWidget workspace = GetUIWorkspace();
			float currentZoom = m_PlayerMapEntity.GetCurrentZoom();
			float calculatedSize = (m_fRadioRange * currentZoom) * SCALE;
			
			if (workspace)
            {
                float scaledSize = (m_fRadioRange * currentZoom) * 2; // Diameter in scaled pixels
                calculatedSize = workspace.DPIUnscale(scaledSize); 		// Unscale for SetSize
            }
            else
            {
                calculatedSize = (m_fRadioRange * currentZoom) * 2; // Fallback
            }
			
			SCR_CampaignFaction m_CampaignFaction = SCR_CampaignFaction.Cast(m_Base.GetFaction());
			m_FactionColor = m_CampaignFaction.GetFactionColor();

			m_RadioRangeWidget.SetSize(calculatedSize, calculatedSize);
			if (m_CampaignFaction)
			{
				//m_RadioRangeWidget.SetColor(m_FactionColor);
			}
			
			SCR_GameModeCampaign campaignInfo = SCR_GameModeCampaign.GetInstance();
			bool isInRangeNow;
			//IEntity player = SCR_PlayerController.GetLocalControlledEntity();
			IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId());
			if (player)
			{
				SCR_Faction playerFaction = SCR_Faction.Cast(SCR_Faction.GetEntityFaction(player));
				isInRangeNow = campaignInfo.GetBaseManager().IsEntityInFactionRadioSignal(m_Base.GetOwner(), playerFaction);
			}
			else if (!player && m_Base.IsHQ())
			{
				// Probably in the deployment menu
				isInRangeNow = true;
			}
			
			m_RadioRangeWidget.SetVisible(m_fRadioRange > 0 && isInRangeNow);
			m_PlayerMapEntity.GetOnMapZoom().Insert(OnMapZoom);
		}
		
		return super.OnMouseEnter(w, x, y);	
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{	
		if (m_RadioRangeWidget)
		{
			m_RadioRangeWidget.SetVisible(false);
			m_PlayerMapEntity.GetOnMapZoom().Remove(OnMapZoom);
		}
		
		return super.OnMouseLeave(w, enterW, x, y);	
	}
};
