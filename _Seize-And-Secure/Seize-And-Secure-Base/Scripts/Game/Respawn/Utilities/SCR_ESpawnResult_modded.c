/*
	This enum represents possible results/responses sent from the authority after a SCR_SpawnRequestComponent
	issues a request to respawn to a SCR_SpawnHandlerComponent.

	When the spawning process is successfull, the return value is SCR_ESpawnResult.OK,
	anything else is a request which has failed and resulted in the player not spawning.
*/
modded enum SCR_ESpawnResult
{
	NOT_ALLOWED_PERSONAL_ALLOWANCE,
	NOT_ALLOWED_BASE_THRESHOLD,
};
