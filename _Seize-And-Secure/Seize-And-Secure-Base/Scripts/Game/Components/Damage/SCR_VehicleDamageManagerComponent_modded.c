modded class SCR_VehicleDamageManagerComponent : SCR_DamageManagerComponent
{	
	//------------------------------------------------------------------------------------------------
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (s_OnVehicleDestroyed && newState == EDamageState.DESTROYED)
		{
			IRON_SUP_ExplosionComponent explosionComponent = IRON_SUP_ExplosionComponent.Cast(GetOwner().FindComponent(IRON_SUP_ExplosionComponent));
			if (explosionComponent)
			{
				explosionComponent.ApplyEffect();
			}
		}
	}
}