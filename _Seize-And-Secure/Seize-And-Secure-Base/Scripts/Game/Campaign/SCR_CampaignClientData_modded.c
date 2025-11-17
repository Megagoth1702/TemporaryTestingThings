//------------------------------------------------------------------------------------------------
//! Used for storing client data to be reapplied for reconnecting clients
modded class SCR_CampaignClientData
{
	protected float m_fDeathRespawnPenalty;
	protected float m_fLastDeathTimestamp;
	protected float m_fLastOnDeathPenaltyDeductionTimestamp;
	protected int m_iNearestTeamMemberID;
	protected int m_iTeamMemberDistance;
	protected int m_iKillerID;

	//------------------------------------------------------------------------------------------------
	void SetDeathRespawnPenalty(float penalty)
	{
		m_fDeathRespawnPenalty = Math.Max(0, penalty);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDeathRespawnPenalty()
	{
		return m_fDeathRespawnPenalty;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetLastDeathTimestamp(float timestamp)
	{
		m_fLastDeathTimestamp = timestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetLastDeathTimestamp()
	{
		return m_fLastDeathTimestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetLastOnDeathPenaltyDeductionTimestamp(float timestamp)
	{
		m_fLastOnDeathPenaltyDeductionTimestamp = timestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetLastOnDeathPenaltyDeductionTimestamp()
	{
		return m_fLastOnDeathPenaltyDeductionTimestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetTeamMemberDistance()
	{
		return m_iTeamMemberDistance;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTeamMemberDistance(int distance)
	{
		m_iTeamMemberDistance = distance;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetLatestKillerID()
	{
		return m_iKillerID;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetLatestKillerID(int killerId)
	{
		m_iKillerID = killerId;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetNearestTeamMemberID()
	{
		return m_iNearestTeamMemberID;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetNearestTeamMemberID(int memberId)
	{
		m_iNearestTeamMemberID = memberId;
	}
};