modded class SCR_WaveRespawnTimerComponent : SCR_RespawnTimerComponent
{	
	[Attribute(defvalue: "0", desc: "Use custom grace period for when wave respawn timer kicks in.", category: "Wave Respawn")]
	protected bool UseCustomGracePeriod;

	[Attribute(defvalue: "300", desc: "Grace period (in seconds) for when wave respawn timer kicks in. Recommended to set the same duration as battle-prep timer, if used.", category: "Wave Respawn")]
	protected float m_fCustomGracePeriod;

	SCR_GameModeCampaign m_CampaignInfo;
	WorldTimestamp m_ServerStartTime;
	protected SCR_UIInfoSpawnRequestResult m_UIInfoSpawnRequestResult;
	protected float m_fGracePeriod;
	protected bool m_bGracePeriodComplete;
	protected float m_fTimeDelta;
	protected ref array<int> m_aPlayersFirstSpawn = {};
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerConnected(int playerId)
	{
		if (!m_aPlayersFirstSpawn.Contains(playerId) && !m_aAllowedPlayers.Contains(playerId))
		{
			m_aPlayersFirstSpawn.Insert(playerId);
			m_aAllowedPlayers.Insert(playerId);
			Replication.BumpMe();
			return;
		}
		
		if (!m_aWaitingPlayers.Contains(playerId))
		{
			m_aWaitingPlayers.Insert(playerId);
			Replication.BumpMe();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!IsMaster())
			return;
				
		bool shouldBump;
		WorldTimestamp timeNow = GetCurrentTime();
		// Update timers
		int timersCount = m_aFactionRespawnTimers.Count();
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return;
		
		Faction playerFaction;
		for (int i = 0; i < timersCount; i++)
		{
			SCR_RespawnTimer timer = m_aFactionRespawnTimers[i];
			if (timer)
			{
				if (!m_bGracePeriodComplete)
				{
					m_bGracePeriodComplete = m_fTimeDelta >= m_fGracePeriod;
					m_fTimeDelta = Math.Clamp(timeNow.DiffMilliseconds(m_ServerStartTime), -1, m_fGracePeriod);	
#ifdef WORKBENCH	
//					PrintFormat("SCR_WaveRespawnTimerComponent: Grace Period: %1 | Server Time: %2 | Grace Period Complete: %3", m_fGracePeriod, timeNow.DiffMilliseconds(m_ServerStartTime), m_bGracePeriodComplete);
#endif			
				} 
				
				// If timer is finished, allow spawn for current wave of players
				if (timer.IsFinished(timeNow) || !m_bGracePeriodComplete)
				{
					// Add each waiting player of given faction into the "allow players" list
					for (int playerIndex = m_aWaitingPlayers.Count() -1; playerIndex >= 0; playerIndex--)
					{
						int playerId = m_aWaitingPlayers[playerIndex];
						playerFaction = factionManager.GetPlayerFaction(playerId);
						if (playerFaction)
						{
							int factionIndex = factionManager.GetFactionIndex(playerFaction);
							if (factionIndex == i)
							{
								// It would be nicer to use set<T>, but rpl codec is missing for that object
								if (!m_aAllowedPlayers.Contains(playerId))
									m_aAllowedPlayers.Insert(playerId);

								// Remove from waiting list, we are in ready list already
								m_aWaitingPlayers.Remove(playerIndex);
								
								// Make sure to propagate changes
								shouldBump = true;
							}
						}
					}
					
					// Start timer
					RpcDo_StartFactionTimer(i, timeNow);
					Rpc(RpcDo_StartFactionTimer, i, timeNow);
				}
			}
		}
		
		if (shouldBump)
			Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetAllWaitingPlayers(out array<int> outPlayers)
	{
		outPlayers = m_aWaitingPlayers;
		return outPlayers.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetWaitingPlayersInFaction(Faction faction, out array<int> outPlayers)
	{
		array<int>playersInFaction = {};
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		foreach (int player : m_aWaitingPlayers)
		{
			Faction playerFaction = factionManager.GetPlayerFaction(player);
			if (playerFaction == faction && !playersInFaction.Contains(player))
			{
				playersInFaction.Insert(player);
			}
		}
		
		outPlayers = playersInFaction;
		return outPlayers.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetWaitingPlayersInGroup(int playerId, out array<int> outPlayers)
	{					
		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupManager)
		{
			Print("GetWaitingPlayersInGroup: No Group Manager for Player ID: " + playerId);
			return 0;
		}
		
		SCR_AIGroup group = groupManager.GetPlayerGroup(playerId);
		if (!group)
		{
			Print("GetWaitingPlayersInGroup: No Group for Player ID: " + playerId);
			return 0;
		}
		
		array<int>playerIDsInGroup = group.GetPlayerIDs();
		array<int>playersWaitingInGroup = {};
		
		foreach (int player : playerIDsInGroup)
		{
			if (m_aWaitingPlayers.Contains(player) && !playersWaitingInGroup.Contains(player))
			{
				playersWaitingInGroup.Insert(player);
			}
		}
		
		outPlayers = playersWaitingInGroup;
		return playersWaitingInGroup.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Initialise this component.
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
		m_fGracePeriod = m_CampaignInfo.GetBattlePrepTime() * 1000;
		m_fCustomGracePeriod *= 1000;
		
		if (UseCustomGracePeriod)
		{
			m_fGracePeriod = m_fCustomGracePeriod;
		}
		
		m_ServerStartTime = GetCurrentTime();
	}
}
