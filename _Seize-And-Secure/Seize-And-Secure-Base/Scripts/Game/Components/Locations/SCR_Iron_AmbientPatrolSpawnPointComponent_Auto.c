class SCR_Iron_AmbientPatrolSpawnPointComponent_AutoClass : SCR_AmbientPatrolSpawnPointComponentClass
{

}

class SCR_Iron_AmbientPatrolSpawnPointComponent_Auto : SCR_AmbientPatrolSpawnPointComponent
{
	//------------------------------------------------------------------------------------------------
	//!
	override void SpawnPatrol()
	{
		SCR_FactionAffiliationComponent comp = SCR_FactionAffiliationComponent.Cast(GetOwner().FindComponent(SCR_FactionAffiliationComponent));
		if (!comp)
		{
			return;
		}			

//		SCR_Faction faction = SCR_Faction.Cast(comp.GetAffiliatedFaction());

		IEntity owner = comp.GetOwner();
		if (!owner)
		{
			return;
		}

		vector ownerOrigin = owner.GetOrigin();
		if (!ownerOrigin)
		{
			return;
		}
	
		SCR_GameModeCampaign m_Campaign = SCR_GameModeCampaign.GetInstance();
		if (!m_Campaign)
		{
			PrintFormat("SCR_Iron_AmbientPatrolSpawnPointComponent_Auto: SpawnPatrol: No Campaign Info...%1", this);
			return;
		}

		SCR_CampaignMilitaryBaseComponent base = m_Campaign.GetBaseManager().FindClosestBase(ownerOrigin);
		if (!base)
		{
			PrintFormat("SCR_Iron_AmbientPatrolSpawnPointComponent_Auto: SpawnPatrol: No Base...%1", this);
			return;
		}
	
		Faction baseFaction = base.GetFaction();
		if (!baseFaction)
		{
			//PrintFormat("SCR_Iron_AmbientPatrolSpawnPointComponent_Auto: SpawnPatrol: No Base Faction...%1", this);
			return;
		}
	
		SCR_Faction faction = SCR_Faction.Cast(baseFaction);
		if (!faction)
			faction = SCR_Faction.Cast(comp.GetDefaultAffiliatedFaction());

		if (faction != m_SavedFaction || m_iRespawnPeriod > 0)
		{
			Update(faction);
		}		

		m_bSpawned = true;
		m_bGroupActive = true;

		if (m_sPrefab.IsEmpty())
		{
			PrintFormat("SCR_Iron_AmbientPatrolSpawnPointComponent_Auto: SpawnPatrol: No Prefab... %1", this);
			return;
		}
			
		Resource prefab = Resource.Load(m_sPrefab);

		if (!prefab || !prefab.IsValid())
		{
			PrintFormat("SCR_Iron_AmbientPatrolSpawnPointComponent_Auto: SpawnPatrol: No Prefab or Is Not Valid... %1", this);
			return;
		}			

		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = GetOwner().GetOrigin();

		if (m_iRespawnPeriod == 0 && m_Waypoint && Math.RandomFloat01() >= 0.5)
		{
			AIWaypointCycle cycleWP = AIWaypointCycle.Cast(m_Waypoint);

			if (cycleWP)
			{
				array<AIWaypoint> waypoints = {};
				cycleWP.GetWaypoints(waypoints);
				params.Transform[3] = waypoints.GetRandomElement().GetOrigin();
			}
		}

		m_Group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(prefab, null, params));

		if (!m_Group)
		{
			PrintFormat("SCR_Iron_AmbientPatrolSpawnPointComponent_Auto: SpawnPatrol: No Group... %1", this);
			return;
		}			

		if (!m_Group.GetSpawnImmediately())
		{
			if (m_iMembersAlive > 0)
				m_Group.SetMaxUnitsToSpawn(m_iMembersAlive);

			m_Group.SpawnUnits();
		}

		m_Group.AddWaypoint(m_Waypoint);

//		if (m_iRespawnPeriod != 0)
//			m_Group.GetOnAgentRemoved().Insert(OnAgentRemoved); // Removed in vanilla 1.6 exp
	}
}
