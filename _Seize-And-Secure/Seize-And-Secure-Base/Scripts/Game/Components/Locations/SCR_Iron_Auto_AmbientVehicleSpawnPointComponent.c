class SCR_Iron_Auto_AmbientVehicleSpawnPointComponentClass : SCR_AmbientVehicleSpawnPointComponentClass
{
}

class SCR_Iron_Auto_AmbientVehicleSpawnPointComponent : SCR_AmbientVehicleSpawnPointComponent
{
	[Attribute("0", desc: "If true, will ignore distance check.")]
	protected bool m_bIgnoreDistance;
	
	[Attribute("250.0", UIWidgets.EditBox, "Distance to check for nearest base")]
	protected float m_fDistance;
		
	[Attribute("1.0", UIWidgets.EditBox, "Health of the vehicle to spawn in", "0 1.0")]
	protected float m_fHealthPercentage;
	
	[Attribute("1.0", UIWidgets.EditBox, "Fuel tank percentage of the vehicle to spawn in", "0 1.0")]
	protected float m_fFuelPercentage;
	
	[Attribute("2.0", UIWidgets.EditBox, "Distance to check for obstacles (meters)")]
	protected float m_fHeightCheck;
	
	[Attribute("1.0", UIWidgets.EditBox, "Distance to check for obstacles (meters)")]
	protected float m_fRadiusCheck;
	
	[Attribute("1.0", UIWidgets.EditBox, "Distance to check for obstacles (meters)")]
	protected float m_fCylinderRadiusCheck;
	
	[Attribute("0", desc: "If true, will spawn vehicle regardless of obstacles.")]
	protected bool m_bIgnoreTerrain;
	
	[Attribute("0", desc: "If true, will auto set faction to defending faction.")]
	protected bool m_bUseAADDefenderFaction;
	
	[Attribute("0", desc: "If true, will auto delete vehicle if no player is nearby for a certain amount of time.")]
	protected bool m_bResetIfTeamStranded;
	
	[Attribute("250.0", UIWidgets.EditBox, "")]
	protected float m_fDistanceFromMainBase;
	
	[Attribute("0.66", UIWidgets.EditBox, "What percentage of players must be near main base without a vehicle for this spawner")]
	protected float m_fStrandedTeamThreshold;
		
	//------------------------------------------------------------------------------------------------
	bool GetResetIfTeamStranded()
	{
		return m_bResetIfTeamStranded;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDistanceFromMainBase()
	{
		return m_fDistanceFromMainBase;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetStrandedTeamThreshold()
	{
		return m_fStrandedTeamThreshold;
	}
		
	//------------------------------------------------------------------------------------------------ v1
	//!
	//! \return the created vehicle
//	override Vehicle SpawnVehicle()
//	{
//		SCR_FactionAffiliationComponent comp = SCR_FactionAffiliationComponent.Cast(GetOwner().FindComponent(SCR_FactionAffiliationComponent));
//		if (!comp)
//			return null;
//
//		SCR_GameModeCampaign m_Campaign = SCR_GameModeCampaign.GetInstance();
////		if (m_Campaign)
////			return null;
//		
//		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
////		if (!factionManager)
////			return null;
//		
//		IEntity owner = comp.GetOwner();
////		if (!owner)
////			return null;
//				
//		vector ownerOrigin = owner.GetOrigin();
////		if (!ownerOrigin)
////			return null;
//		
//		if (m_Campaign.GetIsMatchOver() || !m_Campaign.IsRunning())
//		{
//			Print("SpawnVehicle: Match is over. Returning...");
//			return null;
//		}		
//		
//		SCR_CampaignMilitaryBaseComponent base = m_Campaign.GetBaseManager().FindClosestBase(ownerOrigin);
//		Faction civFaction = factionManager.GetFactionByKey("CIV");	
//		vector baseOrigin = base.GetOwner().GetOrigin();
//		Faction baseFaction = base.GetFaction();
//		
//		float distance = vector.DistanceXZ(ownerOrigin, baseOrigin);
//		
//		if (!m_bIgnoreDistance && distance > m_fDistance)
//		{
////			baseFaction = civFaction;
//			return null;						
//		}
//
////		SCR_Faction faction = SCR_Faction.Cast(comp.GetAffiliatedFaction());
//		SCR_Faction faction;
//		if (m_bUseAADDefenderFaction)
//		{
//			SCR_Iron_AAD_FactionAffiliationComponent_Auto autoFaction = SCR_Iron_AAD_FactionAffiliationComponent_Auto.Cast(owner.FindComponent(SCR_Iron_AAD_FactionAffiliationComponent_Auto));
//			if (!autoFaction)
//			{
//				return null;
//			}
//			else
//			{
//				faction = SCR_Faction.Cast(autoFaction.GetDefendingFaction());
//			}
//		}
//		
//		else
//		{
//			faction = SCR_Faction.Cast(baseFaction);
//		}
//		
//		if (!faction)
//		{
//			faction = SCR_Faction.Cast(civFaction); // SCR_Faction.Cast(comp.GetDefaultAffiliatedFaction());
//		}
//
//		if (faction != m_SavedFaction || (!faction && m_sPrefab.IsEmpty()))
//			Update(faction);
//
//		if (m_sPrefab.IsEmpty())
//		{
//			return null;
//		}
//
//		Resource prefab = Resource.Load(m_sPrefab);
//
//		if (!prefab || !prefab.IsValid())
//		{
//			return null;
//		}
//			
//		if (!m_bIgnoreTerrain)
//		{
//			vector pos;
//			bool spawnEmpty = SCR_WorldTools.FindEmptyTerrainPosition(pos, GetOwner().GetOrigin(), m_fRadiusCheck, m_fCylinderRadiusCheck, m_fHeightCheck); //SPAWNING_RADIUS, SPAWNING_RADIUS
//	
//			if (!spawnEmpty)
//			{
//	#ifdef WORKBENCH
//				Print("SCR_AmbientVehicleSpawnPointComponent: FindEmptyTerrainPosition failed at " + GetOwner().GetOrigin().ToString(), LogLevel.WARNING);
//	#endif
//				// In case this spawnpoint is blocked from the start, don't process it anymore
//				// Prevents unexpected behavior such as vehicles spawning on a spot where a service composition has been built and after a session load dismantled
//				if (!m_bFirstSpawnDone)
//					m_bDepleted = true;
//	
//				return null;
//			}
//		}
//		
//		EntitySpawnParams params = EntitySpawnParams();
//		params.TransformMode = ETransformMode.WORLD;
//		GetOwner().GetTransform(params.Transform);
//
//		if (m_Vehicle)
//			RemoveInteractionHandlers(m_Vehicle);
//
//		m_Vehicle = Vehicle.Cast(GetGame().SpawnEntityPrefab(prefab, null, params));
//		m_fRespawnTimestamp = null;
//		m_bFirstSpawnDone = true;
//		m_bSpawnProcessed = true;
//
//		if (!m_Vehicle)
//			return null;
//
//		m_fSpawnTimestamp = GetGame().GetWorld().GetWorldTime();
//
//		CarControllerComponent carController = CarControllerComponent.Cast(m_Vehicle.FindComponent(CarControllerComponent));
//
//		// Activate handbrake so the vehicles don't go downhill on their own when spawned
//		if (carController)
//			carController.SetPersistentHandBrake(true);
//
//		Physics physicsComponent = m_Vehicle.GetPhysics();
//
//		// Snap to terrain
//		if (physicsComponent)
//			physicsComponent.SetVelocity("0 -1 0");
//
//		EventHandlerManagerComponent handler = EventHandlerManagerComponent.Cast(m_Vehicle.FindComponent(EventHandlerManagerComponent));
//
//		if (handler)
//			handler.RegisterScriptHandler("OnDestroyed", this, OnVehicleDestroyed);
//
//		if (m_bStopDespawnOnInteraction)
//			AddInteractionHandlers(m_Vehicle);
//		
//		GetGame().GetCallqueue().CallLater(HealVehicle, 3000, false, m_Vehicle, m_fHealthPercentage, m_fFuelPercentage);
//		
//		return m_Vehicle;
//	}

	//------------------------------------------------------------------------------------------------ v2
	override Vehicle SpawnVehicle()
	{
	    // Prevent spawning if a vehicle is already active
	    if (m_Vehicle != null)
	        return null;
	
	    // Get faction affiliation component from the owner entity
	    SCR_FactionAffiliationComponent comp = SCR_FactionAffiliationComponent.Cast(GetOwner().FindComponent(SCR_FactionAffiliationComponent));
	    if (!comp)
	        return null;
	
	    // Get campaign game mode instance
	    SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
	    if (campaign.GetIsMatchOver() || !campaign.IsRunning())
	    {
	        Print("SpawnVehicle: Match is over or not running. Aborting spawn.");
	        return null;
	    }
	
	    // Get faction manager and owner details
	    SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
	    IEntity owner = GetOwner();
	    vector ownerOrigin = owner.GetOrigin();
	
	    // Find the closest military base
	    SCR_CampaignMilitaryBaseComponent base = campaign.GetBaseManager().FindClosestBase(ownerOrigin);
	    Faction civFaction = factionManager.GetFactionByKey("CIV");
	    vector baseOrigin = base.GetOwner().GetOrigin();
	    Faction baseFaction = base.GetFaction();
	
	    // Check distance constraint
	    float distance = vector.DistanceXZ(ownerOrigin, baseOrigin);
	    if (!m_bIgnoreDistance && distance > m_fDistance)
	        return null;
	
	    // Determine the faction for spawning
	    SCR_Faction faction;
	    if (m_bUseAADDefenderFaction)
	    {
	        SCR_Iron_AAD_FactionAffiliationComponent_Auto autoFaction = SCR_Iron_AAD_FactionAffiliationComponent_Auto.Cast(owner.FindComponent(SCR_Iron_AAD_FactionAffiliationComponent_Auto));
	        if (!autoFaction)
	            return null;
	        faction = SCR_Faction.Cast(autoFaction.GetDefendingFaction());
	    }
	    else
	    {
	        faction = SCR_Faction.Cast(baseFaction);
	    }
	
	    // Fallback to civilian faction if no faction is determined
	    if (!faction)
	        faction = SCR_Faction.Cast(civFaction);
	
	    // Update prefab if faction changes or prefab is empty
	    if (faction != m_SavedFaction || (!faction && m_sPrefab.IsEmpty()))
	        Update(faction);
	
	    if (m_sPrefab.IsEmpty())
	        return null;
	
	    // Load vehicle prefab
	    Resource prefab = Resource.Load(m_sPrefab);
	    if (!prefab || !prefab.IsValid())
	        return null;
	
	    // Check for suitable spawn position if terrain constraints apply
	    if (!m_bIgnoreTerrain)
	    {
	        vector pos;
	        bool spawnEmpty = SCR_WorldTools.FindEmptyTerrainPosition(pos, ownerOrigin, m_fRadiusCheck, m_fCylinderRadiusCheck, m_fHeightCheck);
	        if (!spawnEmpty)
	        {
	#ifdef WORKBENCH
	            Print("SCR_AmbientVehicleSpawnPointComponent: FindEmptyTerrainPosition failed at " + ownerOrigin.ToString(), LogLevel.WARNING);
	#endif
	            if (!m_bFirstSpawnDone)
	                m_bDepleted = true;
	            return null;
	        }
	    }
	
	    // Set up spawn parameters
	    EntitySpawnParams params = EntitySpawnParams();
	    params.TransformMode = ETransformMode.WORLD;
	    GetOwner().GetTransform(params.Transform);
	
	    // Spawn the vehicle
	    m_Vehicle = Vehicle.Cast(GetGame().SpawnEntityPrefab(prefab, null, params));
	    if (!m_Vehicle)
		{
			PrintFormat("SCR_AmbientVehicleSpawnPointComponent: Tried to spawn vehicle: %1 : but is null. Returning...", m_Vehicle.ToString());
			return null;
		}
	        	
	    // Update spawn state
	    m_fRespawnTimestamp = null;
	    m_fSpawnTimestamp = GetGame().GetWorld().GetWorldTime();
	    m_bFirstSpawnDone = true;
	    m_bSpawnProcessed = true;
	
	    // Configure vehicle components
	    CarControllerComponent carController = CarControllerComponent.Cast(m_Vehicle.FindComponent(CarControllerComponent));
	    if (carController)
	        carController.SetPersistentHandBrake(true);
	
	    Physics physicsComponent = m_Vehicle.GetPhysics();
	    if (physicsComponent)
	        physicsComponent.SetVelocity("0 -1 0"); // Snap to terrain
	
	    // Register destruction event handler
	    EventHandlerManagerComponent handler = EventHandlerManagerComponent.Cast(m_Vehicle.FindComponent(EventHandlerManagerComponent));
	    if (handler)
	        handler.RegisterScriptHandler("OnDestroyed", this, OnVehicleDestroyed);
	
	    // Add interaction handlers if despawn on interaction is disabled
	    if (m_bStopDespawnOnInteraction)
		{
			AddInteractionHandlers(m_Vehicle);
		}
	        	
	    // Schedule vehicle healing
	    GetGame().GetCallqueue().CallLater(HealVehicle, 3000, false, m_Vehicle, m_fHealthPercentage, m_fFuelPercentage);
	
	    return m_Vehicle;
	}
	
	//------------------------------------------------------------------------------------------------
	void HealVehicle(Vehicle vehicle, float fuelPercentage, float healthPercentage)
	{
		SCR_DamageManagerComponent damageManager = vehicle.GetDamageManager(); //SCR_DamageManagerComponent.Cast(vehicle.GetDamageManager());
		if (!damageManager)
		{
			Print("SCR_Iron_Auto_AmbientVehicleSpawnPointComponent: HealVehicle: Missing Manager");
			return;
		}
		
		VehicleControllerComponent vehController = VehicleControllerComponent.Cast(vehicle.FindComponent(VehicleControllerComponent));
		if (vehController)
		{
			SCR_FuelManagerComponent fuelManager = SCR_FuelManagerComponent.Cast(vehController.GetFuelManager());
			if (fuelManager)
			{
				fuelManager.SetTotalFuelPercentage(fuelPercentage);
			}
		}
		
		if (healthPercentage == 1.0)
		{
			damageManager.FullHeal();
		}
		else
		{
			damageManager.SetHealthScaled(healthPercentage);
		}
				
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterVehicleDestroyed(IEntity vehicle)
	{
		EventHandlerManagerComponent handler = EventHandlerManagerComponent.Cast(vehicle.FindComponent(EventHandlerManagerComponent));
	    if (handler)
	        handler.RemoveScriptHandler("OnDestroyed", this, OnVehicleDestroyed);

//		OnVehicleDestroyed(vehicle);
		
		m_Vehicle = null;
		m_bAllowDespawn = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! \param[in] vehicle
	protected override void OnVehicleDestroyed(IEntity vehicle)
	{
		super.OnVehicleDestroyed(vehicle);
	}

}
