modded class SCR_AIGroup : ChimeraAIGroup
{
	[RplProp(onRplName:"SLUpdateGroupFrequency")]
	protected int m_iSLSetGroupFrequency;
	
	//------------------------------------------------------------------------------------------------
	int GetSLGroupFrequency()
	{
		return m_iSLSetGroupFrequency;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetSLGroupFrequency(int value)
	{
		m_iSLSetGroupFrequency = value;
		SetRadioFrequency(value);
		SLUpdateGroupFrequency();
	}
	
	//------------------------------------------------------------------------------------------------
	void SLUpdateGroupFrequency()
	{		
		PrintFormat("SCR_AIGroup: SL Set Group Frequency to...%1", m_iSLSetGroupFrequency);
		Replication.BumpMe();
	}
}