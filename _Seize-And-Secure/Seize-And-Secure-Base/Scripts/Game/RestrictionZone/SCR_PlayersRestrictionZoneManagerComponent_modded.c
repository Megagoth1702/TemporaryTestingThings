modded class SCR_PlayersRestrictionZoneManagerComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//Kill the player that walked outside of the zone
	override protected void KillPlayerOutOfZone(int playerID, IEntity playerEntity)
	{
		super.KillPlayerOutOfZone(playerID, playerEntity);

		SCR_XPHandlerComponent compXP = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
		
		if (compXP)
			compXP.AwardXP(playerID, SCR_EXPRewards.LEFT_WARMUP_ZONE, 1.0, false);
	}	
}
