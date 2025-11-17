//! This component serves as a base and interface for game mode scoring system.
//!
//! It keeps track of individual player scores and manages replication of such values to
//! all clients when necessary.
//!
//! Scoring is implemented in authoritative way, ie. only the server can update score.
//!
//! This component only keeps track of scores, but does not react or change the game flow in
//! any way. For specialised logic inherit this component and implement custom logic.
modded class SCR_BaseScoringSystemComponent : SCR_BaseGameModeComponent
{
	//------------------------------------------------------------------------------------------------
	//! Internal helper method.
	//! \return faction from provided index.
	override private Faction GetFactionByIndex(int factionIndex)
	{
		if (factionIndex < 0)
			return null;

		Faction faction = GetGame().GetFactionManager().GetFactionByIndex(factionIndex);
		if (!faction)
			return null;
		
		return faction;
	}
}
