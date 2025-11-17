//------------------------------------------------------------------------------------------------
//! Spawn point entity defines positions on which players can possibly spawn.
modded class SCR_SpawnPoint : SCR_Position
{
	[Attribute("0")]
	protected bool m_bHideInDeployMap;
	
	//------------------------------------------------------------------------------------------------
	override bool IsSpawnPointVisibleForPlayer(int pid)
	{
		if (m_bHideInDeployMap)
		{
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetHideInDeployMap()
	{
		return m_bHideInDeployMap;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetHideInDeployMap(bool value)
	{
		m_bHideInDeployMap = value;
	}
}
