enum LoadoutType 
{
    DEFAULT,
	SAVED,
    RIFLEMAN,
    SQUAD_LEADER,
    MEDIC,
    MACHINEGUNNER,
    AUTORIFLEMAN,
    COMBAT_ENGINEER,
	ANTI_TANK,
    SNIPER,
    GRENADIER,
	RADIO_OPERATOR,
	AMMO_BEARER,
	PILOT,
	CREWMAN,
	RECON,
	MORTARMAN,
    CUSTOM_1,
    CUSTOM_2,
    CUSTOM_3
}

[BaseContainerProps()]
class LoadoutStep : Managed
{
    [Attribute("", UIWidgets.EditBox, "Minimum squad size for this step")]
    int MinimumSquadSize;

    [Attribute("", UIWidgets.EditBox, "Maximum allowed loadouts for this step")]
    int MaximumLoadouts;
}

[BaseContainerProps(configRoot: true), BaseContainerCustomTitleField("m_sLoadoutName")]
modded class SCR_FactionPlayerLoadout : SCR_PlayerLoadout 
{

	[Attribute()] //[Attribute("", UIWidgets.EditBox, "Loadout Steps")]
	protected ref array<ref LoadoutStep> m_LoadoutSteps;

	[Attribute("", UIWidgets.ComboBox, "Loadout Type", enums: ParamEnumArray.FromEnum(LoadoutType))]
    protected LoadoutType m_LoadoutType;

	//------------------------------------------------------------------------------------------------
	ref array<ref LoadoutStep> GetLoadoutSteps()
	{
		return m_LoadoutSteps;
	}
	
	//------------------------------------------------------------------------------------------------v1
//	LoadoutType GetLoadoutType(SCR_BasePlayerLoadout loadout)
//	{
//		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
//		LoadoutType type = factionLoadout.m_LoadoutType;
//		
//		return type;
//	}
	
	//------------------------------------------------------------------------------------------------v2
	LoadoutType GetLoadoutType(SCR_BasePlayerLoadout loadout)
	{
		return m_LoadoutType;
	}
}