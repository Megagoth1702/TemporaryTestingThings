[EntityEditorProps(category: "GameScripted/Markers")]
class Seize_RadioOperatorMapMarkerPlayerClass : SCR_MapMarkerEntityClass
{
}

class Seize_RadioOperatorMapMarkerPlayer : SCR_MapMarkerEntity
{
	protected static const ref const Color COLOR_INCAPACITATED = Color.FromRGBA(245, 170, 39, 255); // FromInt(0xFFFCBA03); //0xFFFF3636 #fcba03 125
	protected static const ref const Color COLOR_DEAD = Color.Gray;
	
	[RplProp(onRplName: "UpdateMarker")]
	protected ref SCR_MilitarySymbol m_MilitarySymbol;
	
	[RplProp(onRplName: "UpdateMarker")]
	protected int m_iColor = Color.WHITE;
	
	[RplProp(onRplName: "UpdateMarker")]
	protected string m_sSyncedText = string.Empty;
	
	[RplProp(onRplName: "SetMarkerColor")]
	protected int m_iColorAlive;
	
	protected int m_iGroupColor = Color.GREEN;
	
	protected Seize_RadioOperatorPlayerComponent m_MarkerWidgetComponent;

	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
	    super.EOnInit(owner);
	    InitializeMilitarySymbol();
	}
	
	//------------------------------------------------------------------------------------------------
	void InitializeMilitarySymbol()
	{
	    m_MilitarySymbol = new SCR_MilitarySymbol();
	    m_MilitarySymbol.SetIdentity(EMilitarySymbolIdentity.BLUFOR);
	    m_MilitarySymbol.SetDimension(EMilitarySymbolDimension.LAND);
	    m_MilitarySymbol.SetIcons(EMilitarySymbolIcon.INFANTRY);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMarkerColor()
	{
	    m_iColor = m_iColorAlive;
		UpdateMarker();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetGroupColor()
	{
	    m_iColorAlive = m_iGroupColor;
		SetMarkerColor();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMarkerColorFaction(Faction faction)
	{
		m_iColorAlive = faction.GetFactionColor().PackToInt();
	    SetMarkerColor();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetSquadIcon(bool value)
	{
		if (m_MarkerWidgetComponent)
	    	m_MarkerWidgetComponent.SetSquadIcon(value);
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnCreateMarker()
	{
	    super.OnCreateMarker();
		
	    m_MarkerWidgetComponent = Seize_RadioOperatorPlayerComponent.Cast(m_MarkerWidgetComp);
	    if (m_MarkerWidgetComponent)
	    {
	        ConfigureMarkerWidget();
	    }
	}
	
	//------------------------------------------------------------------------------------------------
	void ConfigureMarkerWidget()
	{
	    m_MarkerWidgetComponent.SetMilitarySymbolMode(true);
	    m_MarkerWidgetComponent.UpdateMilitarySymbol(m_MilitarySymbol);
	    m_MarkerWidgetComponent.SetColor(Color.FromInt(m_iColor));
	    m_MarkerWidgetComponent.SetText(m_sSyncedText);
//		m_MarkerWidgetComponent.SetSquadIcon(true);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateMilitarySymbol(EMilitarySymbolIcon icons)
	{
	    SetMilitarySymbolIcons(icons);
	    Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMilitarySymbolIcons(EMilitarySymbolIcon icons)
	{
	    m_MilitarySymbol.SetIcons(icons);
	    UpdateMarker();
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateColor(Color newColor)
	{
	    m_iColor = newColor.PackToInt();
	    UpdateMarker();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetGlobalColor(Color newColor)
	{
	    UpdateColor(newColor);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateText(string text)
	{
	    m_sSyncedText = text;
	    UpdateMarker();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetGlobalText(string text)
	{
	    UpdateText(text);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateMarker()
	{
	    if (m_MarkerWidgetComponent)
	    {
	        m_MarkerWidgetComponent.UpdateMilitarySymbol(m_MilitarySymbol);
	        m_MarkerWidgetComponent.SetColor(Color.FromInt(m_iColor));
//			m_MarkerWidgetComponent.SetPlayerText(m_sSyncedText);
	        m_MarkerWidgetComponent.SetText(m_sSyncedText);
			Replication.BumpMe();
	    }
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsGlobalVisible()
	{
	    return m_bIsGlobalVisible;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnLifeStateChanged(ECharacterLifeState previousState, ECharacterLifeState newState)
	{
	    switch (newState)
	    {
	        case ECharacterLifeState.ALIVE:
	            SetGlobalColor(Color.FromInt(m_iColorAlive));
	            break;
	        case ECharacterLifeState.INCAPACITATED:
	            SetGlobalColor(COLOR_INCAPACITATED);
	            break;
	        case ECharacterLifeState.DEAD:
	            SetGlobalColor(COLOR_DEAD);
	            break;
	    }
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetFaction(Faction faction)
	{
	    super.SetFaction(faction);
	    UpdateFactionSymbol(faction);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateFactionSymbol(Faction faction)
	{
	    SCR_GroupIdentityCore groupIdentity = SCR_GroupIdentityCore.Cast(SCR_GroupIdentityCore.GetInstance(SCR_GroupIdentityCore));
	    if (groupIdentity)
	    {
	        SCR_MilitarySymbolRuleSet symbolRuleSet = groupIdentity.GetSymbolRuleSet();
	        if (symbolRuleSet)
	        {
	            symbolRuleSet.UpdateSymbol(m_MilitarySymbol, SCR_Faction.Cast(faction));
	            m_iColorAlive = faction.GetFactionColor().PackToInt();
	            SetMarkerColor();
	            Replication.BumpMe();
	        }
	    }
	}
}