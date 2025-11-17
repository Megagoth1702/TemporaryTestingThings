

[EntityEditorProps(category: "GameScripted/Groups", description: "Player groups manager, attach to game mode entity!.")]
modded class SCR_GroupsManagerComponentClass : SCR_BaseGameModeComponentClass
{
}

modded class SCR_GroupsManagerComponent : SCR_BaseGameModeComponent
{

	//------------------------------------------------------------------------------------------------
	//! \return array of SCR_EGroupRole, available group roles
	override array<SCR_EGroupRole> GetAvailableGroupRoles(notnull Faction faction)
	{
		array<SCR_EGroupRole> availableGroupRoles = {};

		SCR_Faction scrFaction = SCR_Faction.Cast(faction);
		if (!scrFaction)
			return availableGroupRoles;

		PlayerController pc = GetGame().GetPlayerController();
		int playerId;
		if (pc)
			playerId = pc.GetPlayerId();

		bool isLocalPlayerCommander = SCR_FactionCommanderPlayerComponent.IsLocalPlayerCommander();

		array<SCR_GroupRolePresetConfig> availableGroupRolePresetConfigs = {};
		scrFaction.GetGroupRolePresetConfigs(availableGroupRolePresetConfigs);
		foreach (SCR_GroupRolePresetConfig preset : availableGroupRolePresetConfigs)
		{
			if (!preset.CanBeCreatedByPlayer())
				continue;

			if (isLocalPlayerCommander || (!isLocalPlayerCommander /*&& HasPlayerRequiredRank(preset, playerId, true)*/))
				availableGroupRoles.Insert(preset.GetGroupRole());
		}

		array<SCR_AIGroup> playableGroups = GetPlayableGroupsByFaction(faction);

		// faction commander can create all group roles
/*		if (isLocalPlayerCommander)
			return availableGroupRoles;

		for (int i = availableGroupRoles.Count() - 1; i >= 0; i--)
		{
			foreach (SCR_AIGroup group : playableGroups)
			{
				if (!group)
					continue;

				if (group.GetGroupRole() != availableGroupRoles[i])
					continue;

				if (group.IsPrivate())
					continue;

				// remove group role if is in group more members than half of it's max group size
				if (group.GetPlayerCount() > (group.GetMaxMembers() * 0.5))
					continue;

				availableGroupRoles.RemoveOrdered(i);
				break;
			}
		}*/

		return availableGroupRoles;
	}

	//------------------------------------------------------------------------------------------------
	//! \param[in] groupRole
	//! \param[in] faction
	//! \return

}
