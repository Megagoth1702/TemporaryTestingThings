class Iron_SuppressionManager : GameSystem
{
	protected static ref ScriptInvoker OnProjectileFlybyInvoker;
	protected static ref ScriptInvoker OnProjectileImpactInvoker;
	protected static ref ScriptInvoker OnExplosionInvoker;
	private ref array<IRON_SUP_ProjectileComponent> m_aProjectiles = {};
	
	private IEntity m_LocalPlayer;
	private ChimeraCharacter m_ChimeraPlayerCharacter;
	private CharacterControllerComponent m_CharControl;
	
	protected static float m_fSuppressionLevel;
	protected float m_fElapsedTime;
	protected float m_fRecoveryDelay = 200;
	protected float m_fRecoveryRate = 0.01;
	private float m_fSuppressionFlinchTime = 0;	
	private float m_fSuppressionFirstFlinchTime = 0;	
	
	// higher is slower
	private float m_fFlinchRateMin = 1.7; // 0.7
	private float m_fFlinchRateMax = 2.2; // 0.1.2
	private float m_fFlybyRateMin = 0.2; // 0.4
	private float m_fFlybyRateMax = 0.38; // 0.48
	
	private float m_fFirstFlinchRateMin = 200;
	private float m_fFirstFlinchRateMax = 500;
	
	//------------------------------------------------------------------------------------------------
	static ScriptInvoker GetOnProjectileFlybyInvoker() 
	{
		if (!OnProjectileFlybyInvoker)
		{
			OnProjectileFlybyInvoker = new ScriptInvoker();
		}
		
		return OnProjectileFlybyInvoker;
	}
	
	//------------------------------------------------------------------------------------------------
	static ScriptInvoker GetOnProjectileImpactInvoker() 
	{
		if (!OnProjectileImpactInvoker)
		{
			OnProjectileImpactInvoker = new ScriptInvoker();
		}
		
		return OnProjectileImpactInvoker;
	}
	
	//------------------------------------------------------------------------------------------------
	static ScriptInvoker GetOnExplosionInvoker() 
	{
		if (!OnExplosionInvoker)
		{
			OnExplosionInvoker = new ScriptInvoker();
		}
		
		return OnExplosionInvoker;
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnInit()
	{
		OnProjectileFlybyInvoker = GetOnProjectileFlybyInvoker();
		OnProjectileImpactInvoker = GetOnProjectileImpactInvoker();
		OnExplosionInvoker = GetOnExplosionInvoker();
		
		OnProjectileFlybyInvoker.Insert(RegisterProjectileFlyby);
		OnProjectileImpactInvoker.Insert(RegisterProjectileImpact);
		OnExplosionInvoker.Insert(RegisterExplosion);
		
		m_aProjectiles = new array<IRON_SUP_ProjectileComponent>();

		m_LocalPlayer = GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId());
		m_ChimeraPlayerCharacter = ChimeraCharacter.Cast(m_LocalPlayer);

		Print("Iron_SuppressionManager: Is Initialized...");
	}
	
	//------------------------------------------------------------------------------------------------
	override event protected void OnCleanup()
	{		
		super.OnCleanup();
		
		OnProjectileFlybyInvoker.Remove(RegisterProjectileFlyby);	
		OnProjectileImpactInvoker.Remove(RegisterProjectileImpact);
		OnExplosionInvoker.Remove(RegisterExplosion);
	}
	
	//------------------------------------------------------------------------------------------------
    // Call this method when a projectile is created
    void RegisterProjectileFlyby(IRON_SUP_ProjectileComponent projectile, IEntity owner)
    {
        if (projectile)
        {
           AddProjectile(projectile, owner);
        }
    }
	
	//------------------------------------------------------------------------------------------------
    // Call this method when a projectile impact is created
    void RegisterProjectileImpact(IRON_SUP_ProjectileComponent projectile, IEntity owner)
    {
        if (projectile)
        {
           OnProjectileImpact(projectile);
        }
    }
	
	//------------------------------------------------------------------------------------------------
    // Call this method when an explosion occurs
    void RegisterExplosion(IRON_SUP_ExplosionComponent explosion)
    {
        if (explosion)
        {
           OnExplosion(explosion);
        }
    }
	
	//------------------------------------------------------------------------------------------------
	void AddProjectile(IRON_SUP_ProjectileComponent projectile, IEntity owner)
	{
		m_aProjectiles.Insert(projectile);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ESystemPoint point)
	{
//		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());		
//		if (!gameMode.IsRunning())
//			return;
		
		m_LocalPlayer = GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId());
		if (!m_LocalPlayer)
			return;
		
		m_ChimeraPlayerCharacter = ChimeraCharacter.Cast(m_LocalPlayer);
		if (!m_ChimeraPlayerCharacter)
			return;
		
		m_CharControl = m_ChimeraPlayerCharacter.GetCharacterController();
		if (!m_CharControl || m_CharControl.IsDead())
		{
			m_fSuppressionLevel = 0;
			return;
		}
		
		m_fSuppressionLevel = GetSuppressionLevel();
		m_fSuppressionFlinchTime = Math.Clamp(m_fSuppressionFlinchTime, 0, m_fFlinchRateMax);
		m_fSuppressionFirstFlinchTime = Math.Clamp(m_fSuppressionFirstFlinchTime, 0, m_fFirstFlinchRateMax);
		
		if (m_fSuppressionFlinchTime > 0)
		{
			m_fSuppressionFlinchTime -= 0.1;
		}
		
		if (m_fSuppressionFirstFlinchTime > 0)
		{
			m_fSuppressionFirstFlinchTime -= 0.05;
		}
		
		if (m_fSuppressionLevel > 0)
		{
//			PrintFormat("Suppression Level: %1", m_fSuppressionLevel);

			m_fElapsedTime += 1;
			
//			PrintFormat("Elapsed Time: %1", m_fElapsedTime);
			if (m_fElapsedTime >= GetRecoveryDelay())
			{
				ReduceSuppression(GetRecoveryRate());
			}
			
			if (m_fSuppressionLevel == 0)
			{
				m_fElapsedTime = 0;
			}
		}
		
	 	int projectileCount = m_aProjectiles.Count();
	    if (projectileCount == 0)
	    {
	        return;
	    }
	    
//		PrintFormat("Projectile Count: %1", projectileCount);
		
	    for (int i = projectileCount - 1; i >= 0; --i)
	    {
	        IRON_SUP_ProjectileComponent projectile = m_aProjectiles[i];
	        
	        if (projectile)
	        {
	            TrackProjectile(projectile);
	        }
			
	        else
	        {
	            m_aProjectiles.Remove(i);
	        }
	    }		
	}
	
	//------------------------------------------------------------------------------------------------
	void TrackProjectile(IRON_SUP_ProjectileComponent projectile)
	{
		ChimeraCharacter playerEntity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId()));
	    if (!playerEntity)
	    {
	        Print("Player entity not found!");
	        return;
	    }
	
		if (!projectile)
		{
			Print("Invalid projectile...removing");
			RemoveProjectile(projectile);
	        return;
		}
		
	    vector playerTransform[4];
	    playerEntity.GetWorldTransform(playerTransform);

        vector projectileTransform[4];
        projectile.GetOwner().GetWorldTransform(projectileTransform);

        // Calculate vectors and check projectile direction
        vector toPlayer = playerTransform[3] - projectileTransform[3]; // Vector to player
        vector projectileDirection = projectileTransform[2]; // Projectile direction

        if (IsProjectileHeadingTowardsPlayer(toPlayer, projectileDirection))
        {
            float distance = vector.Distance(playerTransform[3], projectileTransform[3]); // playerTransform[3], projectileTransform[3]			
//			float distance = vector.DistanceXZ(toPlayer, projectileDirection);			
//			PrintFormat("Projectile Distance: %1", distance);
			
            if (distance <= projectile.GetMaximumFlybyRange())
            {
				OnProjectileFlyby(projectile, distance);
            }
        }
	}
	
	//------------------------------------------------------------------------------------------------
	void RemoveProjectile(IRON_SUP_ProjectileComponent projectile)
	{
		if (m_aProjectiles.Contains(projectile) || projectile == null)
		{
			m_aProjectiles.RemoveItem(projectile);
//			PrintFormat("Projectile Removed: %1", projectile);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	void RemoveAllProjectiles()
	{
		m_aProjectiles.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnExplosion(IRON_SUP_ExplosionComponent explosion)
	{
		ChimeraCharacter playerEntity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId()));
	    if (!playerEntity)
		{
			Print("OnExplosion: No Player Entity");
			return;
		}
		
		if (!explosion)
		{
			Print("OnExplosion: No Explosion Component!");
			return;
		}

	    // Gather explosion data
	    float forceMultiplier = Math.Max(0, explosion.GetForceMultiplier());
	    float explosionDistanceSquared = vector.DistanceSq(playerEntity.EyePosition(), explosion.GetOwner().GetOrigin());
	    float rangeMinSquared = explosion.GetExplosionRangeMinSq();
	    float rangeMaxSquared = explosion.GetExplosionRangeMaxSq();
	    float explosionDistance = vector.DistanceXZ(playerEntity.GetOrigin(), explosion.GetOwner().GetOrigin());
	
		float suppressionResults = CalculateExplosionSuppression(playerEntity, explosion, explosionDistanceSquared, rangeMinSquared, rangeMaxSquared, forceMultiplier);
		float expDampening;
		
		if (playerEntity.IsInVehicle())
		{
			IEntity vehicle = CompartmentAccessComponent.GetVehicleIn(playerEntity);
			IRON_SUP_VehicleComponent vehComp = IRON_SUP_VehicleComponent.Cast(vehicle.FindComponent(IRON_SUP_VehicleComponent));
			if (vehComp)
			{
				expDampening = vehComp.GetExplosionDampening();
			}
		}
		
		// Check for camera shake based on distance
	   	HandleCameraShake(explosionDistanceSquared, explosion, suppressionResults, expDampening);
		
	    if (explosionDistanceSquared < rangeMaxSquared)
	    {
	        if (suppressionResults > 0)
	        {
	            AddSuppression(suppressionResults * GetResiliance(playerEntity) - expDampening);
				
				SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay(); // 1.3 - SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
				IRON_SUP_ScreenEffects suppressionEffectManager = IRON_SUP_ScreenEffects.Cast(screenEffectsManager.GetEffect(IRON_SUP_ScreenEffects)); // 1.3 - Iron_SuppressionScreenEffect suppressionEffectManager = Iron_SuppressionScreenEffect.Cast(screenEffectsManager.GetEffect(Iron_SuppressionScreenEffect));
				if (suppressionEffectManager)
				{
					suppressionEffectManager.HeavyFlinchEffect();
				}				
	        }
	    }
	}
	
	//------------------------------------------------------------------------------------------------
	float CalculateExplosionSuppression(ChimeraCharacter playerEntity, IRON_SUP_ExplosionComponent explosion, float distanceSquared, float rangeMinSquared, float rangeMaxSquared, float forceMultiplier)
	{
	    float range = rangeMaxSquared - rangeMinSquared;
	    float t = 0;
	    float coef = 1;

		if (playerEntity.IsInVehicle())
		{
			IEntity vehicle = CompartmentAccessComponent.GetVehicleIn(playerEntity);
			IRON_SUP_VehicleComponent vehComp = IRON_SUP_VehicleComponent.Cast(vehicle.FindComponent(IRON_SUP_VehicleComponent));
			if (vehComp)
			{
				coef = 1 - vehComp.GetExplosionDampening();
			}	        
	    }
	
	    if (forceMultiplier > 0 && range > 0)
	    {
	        t = 1 - Math.Clamp((distanceSquared - rangeMinSquared) / range, 0, 1);
	    }
	
	    // Calculate the suppression value
	    float suppression = forceMultiplier * t * coef * explosion.GetExplosionStrength();
	
#ifdef WORKBENCH
	    Print("Explosion Suppression: " + suppression);
#endif
	    return suppression;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnProjectileImpact(IRON_SUP_ProjectileComponent projectile)
	{
		ChimeraCharacter playerEntity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId()));
	    if (!playerEntity)
	    {
			Print("OnProjectileImpact: No Player Entity");
	        return;
	    }
		
		if (!projectile)
	    {
			Print("OnProjectileImpact: No Projectile. Removing...");
			RemoveProjectile(projectile);
	        return;
	    }
		
		float caliberMultiplier = Math.Max(0, projectile.GetCaliberMultiplier());
		float distance = vector.DistanceXZ(playerEntity.GetOrigin(), projectile.GetOwner().GetOrigin());
		float rangeMax = Math.Min(1, caliberMultiplier) * projectile.GetMaximumImpactRange();
		float rangeMin = Math.Min(1, caliberMultiplier) * projectile.GetMinimumImpactRange();
	    float range = rangeMax - rangeMin;
		
		if (distance <= rangeMax)
	    {
//			PrintFormat("OnProjectileImpact: Impact!");
			HandleSuppressionImpact(playerEntity, projectile, caliberMultiplier, distance);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnProjectileFlyby(IRON_SUP_ProjectileComponent projectile, float distance)
	{
	    ChimeraCharacter playerEntity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId()));
	    if (!playerEntity)
	    {
			Print("OnProjectileFlyby: No Player Entity");
	        return;
	    }
	
	    // Get transformations for player and projectile
	    vector playerTransform[4];
	    playerEntity.GetWorldTransform(playerTransform);
	
	    vector projectileTransform[4];
	    projectile.GetOwner().GetWorldTransform(projectileTransform);
		
	    vector toPlayer = playerTransform[3] - projectileTransform[3];
		
	    HandleSuppressionFlyby(playerEntity, projectile, distance, projectileTransform, toPlayer);
		RemoveProjectile(projectile);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsProjectileHeadingTowardsPlayer(vector toPlayer, vector projectileDirection)
	{
	    float dotProduct = vector.DotXZ(toPlayer, projectileDirection);
//		PrintFormat("Projectile Vector: %1 | Headed Towards Player: %2", dotProduct, dotProduct < 0);
		
	    return dotProduct < 0;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsFriendlyBulletNearby(float distanceFactor, FlinchTotal screenEffectPosition, FlinchTotal calculatedFlinchDirection, ChimeraCharacter playerEntity, IRON_SUP_ProjectileComponent projectile)
	{
		// Flyby screen effect
		HandleFlyby(distanceFactor, screenEffectPosition, calculatedFlinchDirection);
		
	    IEntity bulletOwner = projectile.GetBulletOwner();
	    if (bulletOwner && projectile.IsBulletOwnerFriendlyToPlayer(playerEntity))
	    {
	        float delta = vector.DistanceXZ(playerEntity.GetOrigin(), bulletOwner.GetOrigin());
	        if (delta < projectile.GetFriendlyFlybyRange()) // Within friendly fire range
	        {
#ifdef WORKBENCH
	            Print("Bullet Shot by Friendly and Within Range! Range: " + delta);
#endif
	            return true; // Friendly bullet wont cause full suppression
	        }
	    }
		
	    return false;
	}
	
	//------------------------------------------------------------------------------------------------
	void HandleSuppressionFlyby(ChimeraCharacter playerEntity, IRON_SUP_ProjectileComponent projectile, float distance, vector projectileTransform[4], vector toPlayer)
	{	
		vector eyePos = playerEntity.EyePosition();
		vector eyeToProjectile = playerEntity.EyePosition() - projectileTransform[3];
		
		ProjectileOrigin projectileOrigin = GetProjectileOrigin(playerEntity, projectile);
		FlinchHeight flinchHeight = GetFlinchHeight(eyePos, projectileTransform, toPlayer); // eyeToProjectile

		vector playerTransform[4];
	    playerEntity.GetWorldTransform(playerTransform);
		
		FlinchTotal screenEffectPosition = GetProjectileScreenLocation(eyeToProjectile, playerTransform, projectileOrigin, flinchHeight); //eyeToProjectile
		FlinchTotal calculatedFlinchDirection = GetCalculatedFlinchDirection(screenEffectPosition);
		
		if (IsFriendlyBulletNearby(distance, screenEffectPosition, calculatedFlinchDirection, playerEntity, projectile))
		{
			return;
		}
		
//		float resiliance = GetResiliance(playerEntity);
//		float suppressionFactor = (projectile.GetMinimumFlybyRange() / distance) * projectile.GetCaliberMultiplier();
//		float suppression = Math.Clamp(suppressionFactor * resiliance, 0.05, 0.5);
		float vehDampening;
		
		if (playerEntity.IsInVehicle())
		{
			IEntity vehicle = CompartmentAccessComponent.GetVehicleIn(playerEntity);
			IRON_SUP_VehicleComponent vehComp = IRON_SUP_VehicleComponent.Cast(vehicle.FindComponent(IRON_SUP_VehicleComponent));
			if (vehComp)
			{
				vehDampening = vehComp.GetSuppressionDampening();
			}
		}
		
		float distanceFactor = Math.Clamp(projectile.GetMinimumFlybyRange() / distance, 0.1, 0.7);
		float caliberImpact = projectile.GetCaliberMultiplier(); //Math.Clamp(projectile.GetCaliberMultiplier() * (1.0 * distanceFactor), 0.15, 0.5);
		float resilience = GetResiliance(playerEntity);
		
		float suppression = Math.Clamp((distanceFactor * caliberImpact * resilience) - vehDampening, 0.05, 0.5); //Math.Clamp(((caliberImpact + distanceFactor) * resilience) - vehDampening, 0.05, 0.5);
		
		AddSuppression(suppression);
		HandleFlybyFlinchEffect(playerEntity, projectile, distance, screenEffectPosition, calculatedFlinchDirection);
	}
	
	//------------------------------------------------------------------------------------------------
	void HandleSuppressionImpact(ChimeraCharacter playerEntity, IRON_SUP_ProjectileComponent projectile, float caliberMultiplier, float distance)
	{	
		if (!playerEntity)
		{
			Print("HandleSuppressionImpact: No Player Entity!");
			return;
		}
		
		float vehDampening;		
        if (playerEntity.IsInVehicle())
		{
			IEntity vehicle = CompartmentAccessComponent.GetVehicleIn(playerEntity);
			IRON_SUP_VehicleComponent vehComp = IRON_SUP_VehicleComponent.Cast(vehicle.FindComponent(IRON_SUP_VehicleComponent));
			if (vehComp)
			{
				vehDampening = vehComp.GetSuppressionDampening();
			}
		}
		
//		float distanceFactor = Math.Clamp(projectile.GetMinimumImpactRange() / distance, 0, 1);
//		float caliberImpact = caliberMultiplier * (1.0 + (1.0 - distanceFactor));
//		float resilience = GetResiliance(playerEntity);
//		
//		float suppression = Math.Clamp(((caliberImpact + distanceFactor) * resilience) - vehDampening, 0.05, 1.0);
		
		float distanceFactor = Math.Clamp(projectile.GetMinimumImpactRange() / distance, 0.1, 0.5);
		float resilience = GetResiliance(playerEntity);
		
		float suppression = Math.Clamp((distanceFactor * caliberMultiplier * resilience) - vehDampening, 0.05, 0.5);

		AddSuppression(suppression);
        HandleImpactFlinchEffect(distanceFactor);
	}
	
	//------------------------------------------------------------------------------------------------
	void HandleImpactFlinchEffect(float suppressionFactor)
	{
	    if (suppressionFactor >= 0.3)
	    {
			ChimeraCharacter playerEntity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId()));
			if (playerEntity.IsInVehicle())
			{
				IEntity vehicle = CompartmentAccessComponent.GetVehicleIn(playerEntity);
				IRON_SUP_VehicleComponent vehComp = IRON_SUP_VehicleComponent.Cast(vehicle.FindComponent(IRON_SUP_VehicleComponent));
				if (!vehComp.GetAllowImpactFlinch())
				{
					return;
				}
			}
			
			if (m_fSuppressionFlinchTime <= 0)
		    {
		        m_fSuppressionFlinchTime = GetFlinchRate();
		
				SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay(); // 1.3 - SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
				IRON_SUP_ScreenEffects suppressionEffectManager = IRON_SUP_ScreenEffects.Cast(screenEffectsManager.GetEffect(IRON_SUP_ScreenEffects)); // 1.3 - Iron_SuppressionScreenEffect suppressionEffectManager = Iron_SuppressionScreenEffect.Cast(screenEffectsManager.GetEffect(Iron_SuppressionScreenEffect));				if (suppressionEffectManager)
				{
					suppressionEffectManager.OnSuppressionFlinch();
				}
		    }
	    }
	}
	
	//------------------------------------------------------------------------------------------------
	void HandleFlybyFlinchEffect(ChimeraCharacter playerEntity, IRON_SUP_ProjectileComponent projectile, float distance, FlinchTotal screenEffectPos, FlinchTotal flinchDirection)
	{
		// First flinch effect
		SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
		IRON_SUP_ScreenEffects suppressionEffectManager = IRON_SUP_ScreenEffects.Cast(screenEffectsManager.GetEffect(IRON_SUP_ScreenEffects));
		if (suppressionEffectManager && m_fSuppressionFirstFlinchTime <= 0)
		{
			suppressionEffectManager.HeavyFlinchEffect();
		}
		
		m_fSuppressionFirstFlinchTime = GetFirstFlinchRate();
		
	    if (projectile.CanCauseFlybyFlinch())
	    {
			if (playerEntity.IsInVehicle())
			{
				IEntity vehicle = CompartmentAccessComponent.GetVehicleIn(playerEntity);
				IRON_SUP_VehicleComponent vehComp = IRON_SUP_VehicleComponent.Cast(vehicle.FindComponent(IRON_SUP_VehicleComponent));
				if (!vehComp.GetAllowFlybyFlinch())
				{
					return;
				}
			}
			
			if (suppressionEffectManager && m_fSuppressionFlinchTime <= 0)
		    {
		        m_fSuppressionFlinchTime = GetFlinchRate();

				suppressionEffectManager.OnProcessFlyby(screenEffectPos, flinchDirection);
		    }
	    }
	}
	
	//------------------------------------------------------------------------------------------------
	void HandleFlyby(float distanceFactor, FlinchTotal screenEffectPos, FlinchTotal flinchDirection)
	{
		ChimeraCharacter playerEntity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId()));
		if (playerEntity.IsInVehicle())
		{
			IEntity vehicle = CompartmentAccessComponent.GetVehicleIn(playerEntity);
			IRON_SUP_VehicleComponent vehComp = IRON_SUP_VehicleComponent.Cast(vehicle.FindComponent(IRON_SUP_VehicleComponent));
			if (!vehComp.GetAllowFlybyFlinch())
			{
				return;
			}
		}
			
		SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
		IRON_SUP_ScreenEffects suppressionEffectManager = IRON_SUP_ScreenEffects.Cast(screenEffectsManager.GetEffect(IRON_SUP_ScreenEffects));
		if (suppressionEffectManager)
		{
			suppressionEffectManager.OnProcessFlyby(screenEffectPos, flinchDirection);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void HandleCameraShake(float explosionDistance, IRON_SUP_ExplosionComponent explosion, float calculatedSuppression, float dampening = 0)
	{
		float linearIntensity = calculatedSuppression - dampening * Math.RandomFloat(0.1, 6); 
		float angularIntensity = calculatedSuppression - dampening * Math.RandomFloat(0.1, 6);
		
		float effectIntensityLinear = Math.Clamp(linearIntensity, 0, 10);
		float effectIntensityAngular = Math.Clamp(angularIntensity, 0, 10);
		
		PrintFormat("HandleCameraShake: LinearIntensity: %1 | AngularIntensity: %2 | LinearEffect: %3 | AngularEffect: %4 | Dampening: %5", linearIntensity, angularIntensity, effectIntensityLinear, effectIntensityAngular, dampening);
		
		// Far away explosion
	    if (explosionDistance < (explosion.GetExplosionRangeMaxSq() * 3) && explosionDistance > explosion.GetExplosionRangeMaxSq())
	    {
			float farLinear = Math.RandomFloat(0.1, 0.5); //Math.Clamp(effectIntensityLinear, 0.1, 0.5);
			float farAngular = Math.RandomFloat(0.1, 0.5); //Math.Clamp(effectIntensityAngular, 0.1, 0.5);
						
//	        SCR_CameraShakeManagerComponent.AddCameraShake(Math.Lerp(0, 1, 2), Math.Lerp(0, 2, 2), 0.01, 0.2, 0.3);
			SCR_CameraShakeManagerComponent.AddCameraShake(farLinear, farAngular, 0.01, 0.2, 0.3);
			
			PrintFormat("HandleCameraShake: Far Away: Linear: %1 | Angular: %2", farLinear, farAngular);
	    }
	
		else if (explosionDistance < explosion.GetExplosionRangeMaxSq())
		{
			float closeLinear = Math.Clamp(effectIntensityLinear - dampening, 0.1, 3);
			float closeAngular = Math.Clamp(effectIntensityAngular - dampening, 0.1, 3);
			
			SCR_CameraShakeManagerComponent.AddCameraShake(closeLinear, closeAngular, 0.01, 0.2, 0.3);
			
			PrintFormat("HandleCameraShake: Close: Linear: %1 | Angular: %2 ", closeLinear, closeAngular);
		}	
	}
	
	//----------------------------------------------------------------------------------
	static bool IsClient()
	{
		if (Replication.IsClient())
		{
			return true;
		}
		
		else if (RplSession.Mode() != RplMode.Dedicated)
		{
			return true;
		}
		
		return false;
	}
	
	//----------------------------------------------------------------------------------
	void AddSuppression(float value)
	{
//		PrintFormat("AddSuppression: %1", value);

		m_fSuppressionLevel += value;
		m_fSuppressionLevel = Math.Clamp(m_fSuppressionLevel, 0, 1.0);
		m_fElapsedTime = 0;
	}
	
	//----------------------------------------------------------------------------------
	void ReduceSuppression(float value)
	{
		m_fSuppressionLevel -= value;
		m_fSuppressionLevel = Math.Clamp(m_fSuppressionLevel, 0, 1.0);
	}
	
	//----------------------------------------------------------------------------------
	float GetRecoveryDelay()
	{
		float recoveryDelay;
		ChimeraCharacter playerEntity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId()));	
		recoveryDelay = m_fRecoveryDelay * GetResiliance(playerEntity);
		
		return recoveryDelay;
	}
	
	//----------------------------------------------------------------------------------
	void SetRecoveryDelay(float time)
	{
		m_fRecoveryDelay = time;
	}
	
	//----------------------------------------------------------------------------------
	float GetRecoveryRate()
	{
		float recoveryRate;
		ChimeraCharacter playerEntity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(SCR_PlayerController.GetLocalPlayerId()));
		recoveryRate = Math.Clamp(m_fRecoveryRate / GetResiliance(playerEntity), 0.01, 0.1);
		return recoveryRate;
	}
	
	//----------------------------------------------------------------------------------
	void SetRecoveryRate(float value)
	{
		m_fRecoveryRate = value;
	}
	
	//----------------------------------------------------------------------------------
	static float GetSuppressionLevel()
	{
		return Math.Clamp(m_fSuppressionLevel, 0, 1.0);
	}
	
	//----------------------------------------------------------------------------------
	float GetResiliance(ChimeraCharacter playerEntity)
	{
		if (!playerEntity)
		{
			return 1;
		}
		
		SCR_ECharacterRank charRank = SCR_CharacterRankComponent.GetCharacterRank(playerEntity);
		if (charRank >= SCR_ECharacterRank.SERGEANT)
		{
			return 0.7;
		}
		else if (charRank >= SCR_ECharacterRank.LIEUTENANT)
		{
			return 0.5;
		}
		else if (charRank >= SCR_ECharacterRank.CAPTAIN)
		{
			return 0.3;
		}
		else
		{
			return 1;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	float GetFlinchRate()
	{
		return Math.RandomFloat(m_fFlinchRateMin, m_fFlinchRateMax);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetFirstFlinchRate()
	{
		return Math.RandomFloat(m_fFirstFlinchRateMin, m_fFirstFlinchRateMax);
	}
	
	//------------------------------------------------------------------------------------------------ v2
	ProjectileOrigin GetProjectileOrigin(ChimeraCharacter playerEntity, IRON_SUP_ProjectileComponent projectile)
	{
		IEntity projectileOwner = projectile.GetBulletOwner();
		vector projectileOrigin = projectileOwner.GetOrigin();
		vector playerOrigin = playerEntity.GetOrigin();
		vector playersAimingDirection = GetPlayersDirection();

	    // Get transforms for player and projectile
	    vector playerTransform[4];
	    playerEntity.GetWorldTransform(playerTransform);

	    vector projectileTransform[4];
	    projectile.GetOwner().GetWorldTransform(projectileTransform);

		// Calculate direction from player to target
		vector directionToTarget = vector.Direction(playerOrigin, projectileOrigin);
		float projectileDirection = directionToTarget.VectorToAngles()[0] - playersAimingDirection.VectorToAngles()[0];

//		PrintFormat("Projectile Direction: %1", projectileDirection);
		
		//projectileDirection
	/*	Front: 0 - center, 320 - frontleft, 40 - frontright
		Right: 40+, rightcenter - 90, rightbehind - 130
		Behind: 130+, behindcenter - 180, leftbehind - 225
		Left: 225+, leftcenter - 270, frontleft - 320 
	*/
		
		if (projectileDirection >= -50 && projectileDirection <= 50)
		{
//			Print("Origin: Front");
			return ProjectileOrigin.Front;
		}
		
		else if (projectileDirection > 50 && projectileDirection <= 130)
		{
//			Print("Origin: Right");
			return ProjectileOrigin.Right;
		}
		
		else if (projectileDirection > 130 && projectileDirection <= 225)
		{
//			Print("Origin: Behind");
			return ProjectileOrigin.Behind;
		}
		
		else
		{
//			Print("Origin: Left");
			return ProjectileOrigin.Left;
		}
	}
	
	//------------------------------------------------------------------------------------------------ v3
	FlinchTotal GetCalculatedFlinchDirection(FlinchTotal combinedFlinch)
	{
		FlinchTotal direction = FlinchTotal.LevelCenter;
		
		if (combinedFlinch == FlinchTotal.BelowLeft || combinedFlinch == FlinchTotal.LevelLeft || combinedFlinch == FlinchTotal.AboveLeft)
		{
			direction = FlinchTotal.LevelRight;
		}
		
		if (combinedFlinch == FlinchTotal.BelowRight || combinedFlinch == FlinchTotal.LevelRight || combinedFlinch == FlinchTotal.AboveRight)
		{
			direction = FlinchTotal.LevelLeft;
		}
		
//		PrintFormat("GetCalculatedFlinchDirection: %1", direction);
		return direction;
	}
	
	//------------------------------------------------------------------------------------------------
	FlinchHeight GetFlinchHeight(vector eyePos, vector projectileTransform[4], vector toPlayer)
	{
		FlinchHeight flinchHeight;
		float heightDelta = projectileTransform[3][1] - eyePos[1]; //(projectileTransform[3][1] - playerTransform[3][1]);
		
		if (heightDelta > 0.6) // 0.6
		{
			flinchHeight = FlinchHeight.Above;
		}
		
		else if (heightDelta > -0.3) // -0.3
		{
			flinchHeight = FlinchHeight.Level;
		}
		
		else
		{
			flinchHeight = FlinchHeight.Below;
		}
		
		return flinchHeight;
	}
	
	//------------------------------------------------------------------------------------------------
	FlinchTotal GetProjectileScreenLocation(vector eyePos, vector playerTransform[4], ProjectileOrigin projectileOrigin, FlinchHeight projectileHeight)
	{
		/*
	    eyePos[2] = Left/Right (-/+)
	    eyePos[1] = Height (- for higher, + for lower)
	    eyePos[0] = ???
	    */
	
	    FlinchDirection flinchDirection;
//	    PrintFormat("Projectile Origin: %1", projectileOrigin);
	
	    // Use the player's right vector to identify left/right based on eye position
	    vector playerForward = playerTransform[2]; // Assuming index 2 is the forward vector
	    vector playerRight = playerForward.Perpend(); // This gives us the right direction
	
		/*
		Front Shot: Right
		Projectile Direction: 7.89722
		Flinch Direction Eye Pos: 1.12891
		GetProjectileScreenLocation: Left
		
		Front Shot: Left
		Projectile Direction: 5.40162
		Flinch Direction Eye Pos: 0.724121
		GetProjectileScreenLocation: Left
		
		Rear Shot: Right
		Projectile Direction: -168.304
		Flinch Direction Eye Pos: 3.25879
		GetProjectileScreenLocation: Right
		
		Rear Shot: Left
		Projectile Direction: -168.304
		Flinch Direction Eye Pos: 9.72314
		GetProjectileScreenLocation: Right
		*/
		
	    // Determine additional directions based on eyePos
//		PrintFormat("Eye Pos: %1", eyePos);
		
		if (projectileOrigin == ProjectileOrigin.Front) 
		{
			if (eyePos[0] >= 0.3) // Right side detected // -0.3
			{
//				Print("GetProjectileScreenLocation: Right"); //Right
	            flinchDirection = FlinchDirection.Right;
			}
			
			else if (eyePos[0] <= -0.3)
			{
//				Print("GetProjectileScreenLocation: Left"); //Left
	            flinchDirection = FlinchDirection.Left;
			}
				
			else // Center positions
		    {
//				Print("GetProjectileScreenLocation: Center");
		        flinchDirection = FlinchDirection.Center;
		    }	
		}
		
		else if (projectileOrigin == ProjectileOrigin.Behind)
		{
			if (eyePos[0] <= -0.3) // Right side detected // 3.25
			{
//				Print("GetProjectileScreenLocation: Right"); // Right
	            flinchDirection = FlinchDirection.Right;
			}
			
			else if (eyePos[0] >= 0.3) // Left side detected // 9.72
			{
//				Print("GetProjectileScreenLocation: Left"); // Left
	            flinchDirection = FlinchDirection.Left;
			}	
			
			else // Center positions
		    {
//				Print("GetProjectileScreenLocation: Center");
		        flinchDirection = FlinchDirection.Center;
		    }
		}
		
		else if (projectileOrigin == ProjectileOrigin.Left)
		{
//			Print("GetProjectileScreenLocation: Left");
			flinchDirection = FlinchDirection.Left;
		}
		
		else if(projectileOrigin == ProjectileOrigin.Right)
		{
//			Print("GetProjectileScreenLocation: Right");
			flinchDirection = FlinchDirection.Right; //Right
		}

		return CombineFlinch(flinchDirection, projectileHeight);
	}
	
	//------------------------------------------------------------------------------------------------
	FlinchTotal CombineFlinch(FlinchDirection flinchDirection, FlinchHeight flinchHeight)
	{
//		PrintFormat("CombineFlinch: Direction: %1 | Height: %2", flinchDirection, flinchHeight);
	    switch (flinchDirection) 
		{
	        case FlinchDirection.Center:
	            switch (flinchHeight) 
				{
	                case FlinchHeight.Above:
	                    return FlinchTotal.AboveCenter;
	                case FlinchHeight.Level:
	                    return FlinchTotal.LevelCenter;
	                case FlinchHeight.Below:
	                    return FlinchTotal.BelowCenter;
	        	}
	            break;
			
	        case FlinchDirection.Left:
	            switch (flinchHeight) 
				{
	                case FlinchHeight.Above:
	                    return FlinchTotal.AboveLeft;
	                case FlinchHeight.Level:
	                    return FlinchTotal.LevelLeft;
	                case FlinchHeight.Below:
	                    return FlinchTotal.BelowLeft;
	            }
	            break;
			
	        case FlinchDirection.Right:
	            switch (flinchHeight) 
				{
	                case FlinchHeight.Above:
	                    return FlinchTotal.AboveRight;
	                case FlinchHeight.Level:
	                    return FlinchTotal.LevelRight;
	                case FlinchHeight.Below:
	                    return FlinchTotal.BelowRight;
	            }
	            break;
	    }
		
	    return FlinchTotal.None; // Default case if not matched
	}
	
	//------------------------------------------------------------------------------------------------
	private vector GetPlayersDirection()
	{
		vector aimMat[4];
		ChimeraCharacter character = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		Math3D.AnglesToMatrix(CharacterControllerComponent.Cast(character.FindComponent(CharacterControllerComponent)).GetInputContext().GetAimingAngles() * Math.RAD2DEG, aimMat);
		return aimMat[2];
	}
}

	//------------------------------------------------------------------------------------------------
	enum ProjectileOrigin 
	{
	    Front,
	    Left,
	    Right,
	    Behind
	};

	//------------------------------------------------------------------------------------------------
	enum FlinchDirection 
	{
	    Center, 
		Left, 
		Right
	};

	//------------------------------------------------------------------------------------------------
	enum FlinchHeight 
	{
	    Above, 
		Level, 
		Below
	};

	//------------------------------------------------------------------------------------------------
	enum FlinchTotal 
	{
	    AboveCenter, 	// 0
		AboveLeft, 		// 1
		AboveRight, 	// 2
	    LevelCenter, 	// 3
		LevelLeft, 		// 4
		LevelRight, 	// 5
	    BelowCenter, 	// 6
		BelowLeft, 		// 7
		BelowRight, 	// 8
	    None 			// 9
	};