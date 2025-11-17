//! Determinates if the owner entity is covered by the radio signal eminating from the component marked as Source with the same encryption key
modded class SCR_CoverageRadioComponent : ScriptedRadioComponent
{
	[Attribute("0", desc: "Is this base on a far away island?", category: "Coverage")]
	protected bool m_bIsOnIsland;
	
	[Attribute("0", desc: "Is this base receiving signal from island base?", category: "Coverage")]
	protected bool m_bIsReceivingFromIsland;
	
//	[Attribute("", desc: "The name of the base this radio is assigned to", category: "Coverage")]
//	string m_sAssignedTo;
	
	[Attribute(category: "Coverage")]
	protected ref array<ref string> m_aAssignedBaseNames;
	
	[Attribute(category: "Coverage")]
	protected ref array<ref string> m_aExcludedBaseNames;
	
	[Attribute("0", desc: "Is this base restricted to connected bases?", category: "Coverage")]
	protected bool m_bIsRestrictedToConnectedBases;
	
//	[Attribute("", desc: "The name of the base this radio can connect to", category: "Coverage")]
	protected ref array<ref string> m_sConnectedBasesArray = {};
	
	protected ref array<SCR_CoverageRadioComponent> m_aMOBSendingRadios = {};
	protected ref array<string> m_aMOBSendingKeys = {};
	protected ref array<SCR_CoverageRadioComponent> m_aMinorReceivingRadios = {};
	protected ref array<string> m_aMinorReceivingKeys = {};
	
	//------------------------------------------------------------------------------------------------
	bool GetIsSourceRadioOnIsland()
	{
		return m_bIsOnIsland;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsRestrictedToConnectedBases()
	{
		return m_bIsRestrictedToConnectedBases;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetIsReceivingFromIsland()
	{
		return m_bIsReceivingFromIsland;
	}
	
//	//------------------------------------------------------------------------------------------------
//	string GetBaseNameAssignedTo()
//	{
//		return m_sAssignedTo;
//	}
	
	//------------------------------------------------------------------------------------------------
	ref array<ref string> GetBaseNamesAssignedTo()
	{
		return m_aAssignedBaseNames;
	}
	
	//------------------------------------------------------------------------------------------------
	ref array<ref string> GetExcludedBaseNames()
	{
		return m_aExcludedBaseNames;
	}
	
//	//------------------------------------------------------------------------------------------------
//	ref array<ref string> GetConnectedBases()
//	{
//		return m_sConnectedBasesArray;
//	}
}
