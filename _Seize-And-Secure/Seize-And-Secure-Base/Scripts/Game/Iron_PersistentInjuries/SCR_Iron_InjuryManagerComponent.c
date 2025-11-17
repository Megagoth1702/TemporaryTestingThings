[ComponentEditorProps(category: "GameScripted", description: "Handles injury data for Persistent Injury Component.")]
class SCR_Iron_InjuryManagerComponentClass : ScriptComponentClass
{
}

class SCR_Iron_InjuryManagerComponent : ScriptComponent
{
	[RplProp(condition: RplCondition.OwnerOnly)]
	protected ref array<ref string> m_sPlayerInjuries = {};
	
	//----------------------------------------------------------------------------------
	void SetPlayerInjuryList(string injury)
	{
		m_sPlayerInjuries.Insert(injury);
		Replication.BumpMe();		
	}
	
	//----------------------------------------------------------------------------------
	ref array<ref string> GetPlayerInjuryList()
	{
		return m_sPlayerInjuries;
	}
	
	//----------------------------------------------------------------------------------
	void ClearPlayerInjuryList()
	{
		m_sPlayerInjuries.Clear();
		Replication.BumpMe();
	}	
}