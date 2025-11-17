class Seize_RadioOperatorPlayerComponent : SCR_MapMarkerDynamicWComponent
{
    protected bool m_bIsSymbolMode = false; // Flag for symbol mode
    protected Widget m_wSymbolRoot;         // Root widget for symbol
    protected Widget m_wSymbolOverlay;      // Overlay widget for the symbol
    protected bool m_bIsHovered = false;    // Flag for hover state
	protected TextWidget m_wMarkerTextPlayer;
	protected ImageWidget m_wOwnSquadIcon;

    //------------------------------------------------------------------------------------------------
    //! Set the visual mode for the military symbol
    void SetMilitarySymbolMode(bool isActive)
    {
        m_bIsSymbolMode = isActive;
        UpdateSymbolVisibility();
    }

    //------------------------------------------------------------------------------------------------
    //! Update the military symbol displayed
    void UpdateMilitarySymbol(SCR_MilitarySymbol militarySymbol)
    {
        SCR_MilitarySymbolUIComponent symbolComponent = SCR_MilitarySymbolUIComponent.Cast(m_wSymbolOverlay.FindHandler(SCR_MilitarySymbolUIComponent));
        if (symbolComponent)
        {
            symbolComponent.Update(militarySymbol);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Override the color setting
    override void SetColor(Color color)
    {
        super.SetColor(color);
        m_wSymbolOverlay.SetColor(color);
    }
	
	//------------------------------------------------------------------------------------------------
	//! \param[in] text
	override void SetText(string text)
	{
//		m_wMarkerText.SetText(text);
		m_wMarkerTextPlayer.SetText(text);
	}
	
	//------------------------------------------------------------------------------------------------
	//! \param[in] text
	void SetPlayerText(string text)
	{
		m_wMarkerTextPlayer.SetText(text);
	}

	//------------------------------------------------------------------------------------------------
	void SetSquadIcon(bool value)
	{
		m_wOwnSquadIcon.SetVisible(value);
	}
	
    //------------------------------------------------------------------------------------------------
    //! Attach the widget handlers
    override void HandlerAttached(Widget w)
    {
        super.HandlerAttached(w);
        m_wSymbolRoot = m_wRoot.FindAnyWidget("SymbolWidget");
        m_wSymbolOverlay = m_wSymbolRoot.FindAnyWidget("SymbolOverlay");
		m_wMarkerTextPlayer = TextWidget.Cast(m_wRoot.FindAnyWidget("MarkerTextPlayer"));
		m_wOwnSquadIcon = ImageWidget.Cast(m_wRoot.FindAnyWidget("OwnSquadIcon"));
    }

    //------------------------------------------------------------------------------------------------
    //! Called when the mouse enters the widget
    override bool OnMouseEnter(Widget w, int x, int y)
    {
        m_bIsHovered = true;
        UpdateTextVisibility();
        return super.OnMouseEnter(w, x, y);
    }

    //------------------------------------------------------------------------------------------------
    //! Called when the mouse leaves the widget
    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        m_bIsHovered = false;
        UpdateTextVisibility();
        return super.OnMouseLeave(w, x, y);
    }

    //------------------------------------------------------------------------------------------------
    //! Update the visibility of the text based on hover state
    void UpdateTextVisibility()
    {
        SetTextVisible(m_iLayerID <= 1 || m_bIsHovered);
    }

    //------------------------------------------------------------------------------------------------
    //! Handle mouse button interactions
    override bool OnMouseButtonDown(Widget w, int x, int y, int button)
    {
        if (button == 0) // Left Mouse Button only
        {
            GetGame().OpenGroupMenu();
            return true;
        }
		
        return false; // Other buttons are ignored
    }

    //------------------------------------------------------------------------------------------------
    //! Update visibility of symbol elements based on the current state
    protected void UpdateSymbolVisibility()
    {
        m_wSymbolRoot.SetEnabled(m_bIsSymbolMode);
        m_wSymbolRoot.SetVisible(m_bIsSymbolMode);
        m_wMarkerIcon.SetVisible(!m_bIsSymbolMode);
    }
}