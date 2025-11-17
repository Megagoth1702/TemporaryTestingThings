[BaseContainerProps(configRoot: true), BaseContainerCustomCheckIntTitleField("m_bEnabled", "Arsenal Data", "DISABLED - Arsenal Data", 1)]
modded class SCR_ArsenalItem : SCR_BaseEntityCatalogData
{	
//	[Attribute(LoadoutType.DEFAULT.ToString(), desc: "If items require roles, which S&S loadout type does the player need to have in order to obtain the item\n\nNote: Grenades, medical items and ammo that can be used by the arsenal resupply action should have a rank if Private as those actions only check for Renegade or not", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(LoadoutType))]
	[Attribute(desc: "If items require roles, which S&S loadout type does the player need to have in order to obtain the item\n\nNote: Grenades, medical items and ammo that can be used by the arsenal resupply action should have a rank if Private as those actions only check for Renegade or not", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(LoadoutType))]
	protected ref array<ref LoadoutType> m_eRequiredRoles;
	
	[Attribute(desc: "If items require roles, which S&S loadout type is excluded", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(LoadoutType))]
	protected ref array<ref LoadoutType> m_eExcludedRoles;
	
	[Attribute(desc: "Which S&S loadout type is has special priviledges", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(LoadoutType))]
	protected ref array<ref LoadoutType> m_ePriviledgedRoles;
	
	[Attribute(defvalue: "0", desc: "...", category: "")]
	protected bool m_bIgnoreRankIfRole;
	
	//------------------------------------------------------------------------------------------------
	//! \return S&S Role required of player in order to obtain the item from the arsenal. Only valid when Item's roles are required
	array<ref LoadoutType> GetRequiredRoles()
	{		
		return m_eRequiredRoles;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref LoadoutType> GetExcludedRoles()
	{		
		return m_eExcludedRoles;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref LoadoutType> GetPriviledgedRoles()
	{		
		return m_ePriviledgedRoles;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIgnoreRankIfRole()
	{
		return m_bIgnoreRankIfRole;
	}
}
