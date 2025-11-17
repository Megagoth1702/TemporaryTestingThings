modded class SCR_MuzzleEffectComponent : MuzzleEffectComponent
{
	//----------------------------------------------------------------------------------
	//! Attached to weapons - set projectile info upon firing weapon 
	override void OnFired(IEntity effectEntity, BaseMuzzleComponent muzzle, IEntity projectileEntity)
	{
	    super.OnFired(effectEntity, muzzle, projectileEntity);
	
	    if (!Iron_SuppressionManager.IsClient())
		{
			Print("OnFired: Not Client. Returning...");
			return;
		}
	         	
	    ChimeraCharacter playerEntity = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());

	    if (!playerEntity)
		{
			Print("OnFired: No Player Entity. Returning...");
			return; 
		}
	        
	    // Retrieve muzzle transformation and calculate required directions
	    vector muzzleTransform[4];
	    muzzle.GetOwner().GetWorldTransform(muzzleTransform);
	    
	    vector muzzleDirection = muzzleTransform[2];
	    vector playerDirection = playerEntity.GetOrigin() - muzzleTransform[3];
	
	    IEntity gunOwnerEntity = muzzle.GetOwner().GetParent();
	    IEntity sourceEntity = SCR_EntityHelper.GetMainParent(muzzle.GetOwner());
	    Vehicle sourceVehicle = Vehicle.Cast(sourceEntity);
	    
	    IRON_SUP_ProjectileComponent projectile = IRON_SUP_ProjectileComponent.Cast(projectileEntity.FindComponent(IRON_SUP_ProjectileComponent));
	    if (!projectile)
		{
			Print("OnFired: No Projectile. Returning...");
			return;
		} 
				
		//PrintFormat("OnFired: Setting Bullet Owner: %1", gunOwnerEntity);
		projectile.SetBulletOwner(gunOwnerEntity);
		
	    // Check if the player is within 180 degrees of the muzzle direction
	    bool aimedAtPlayer = (vector.DotXZ(muzzleDirection, playerDirection) > 0.0);

	    if (aimedAtPlayer)
	    {
	        projectile.CauseFlinchUponImpact(true); 
	    }
	
	    // Track the projectile if aimed at the player and they are not a gunner
	    if (aimedAtPlayer)
	    {
			Iron_SuppressionManager.GetOnProjectileFlybyInvoker().Invoke(projectile, gunOwnerEntity);
	    }
	}
};