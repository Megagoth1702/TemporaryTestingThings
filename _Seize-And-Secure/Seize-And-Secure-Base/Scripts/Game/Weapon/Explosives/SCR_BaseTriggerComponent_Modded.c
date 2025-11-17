modded class SCR_BaseTriggerComponent : BaseTriggerComponent
{
	//------------------------------------------------------------------------------------------------
	//!
	// Only call this on the server
	override void ActivateTrigger()
	{
		GenericEntity owner = GenericEntity.Cast(GetOwner());
		RplComponent rplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (!rplComponent || rplComponent.IsProxy())
			return;

		if (GetGame().InPlayMode())
		{
			auto garbageSystem = SCR_GarbageSystem.GetByEntityWorld(owner);
			if (garbageSystem)
				garbageSystem.Withdraw(owner);
		}
		
		SetLive();
		m_bActivated = true;
		ShowFuse();
		Replication.BumpMe();
	}
}
