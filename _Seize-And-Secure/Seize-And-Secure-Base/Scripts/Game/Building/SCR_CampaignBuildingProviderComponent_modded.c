modded class SCR_CampaignBuildingProviderComponent : SCR_MilitaryBaseLogicComponent
{
	[Attribute("1", UIWidgets.CheckBox, "MHQ needs to be deployed before building mode is accessible.")]
	protected bool m_bRequiresDeployment;

	//------------------------------------------------------------------------------------------------
	bool RequiresDeployment()
	{
		return m_bRequiresDeployment;
	}
	
	//------------------------------------------------------------------------------------------------
//	override void EOnInit(IEntity owner)
//	{
//		super.EOnInit(owner);

//		if (System.IsConsoleApp() || !GetGame().GetPlayerController())
//			SetOnProviderFactionChangedEvent();
//	}

	//------------------------------------------------------------------------------------------------
	//! Requesting a building mode. If trigger exist (was spawned with provider, because "Y" can be used to enter the mode, it will open the mode directly. If not, it 1st spawn the building area trigger.
	//! \param[in] playerID
	//! \param[in] userActionUsed

	//------------------------------------------------------------------------------------------------
	//! Check if given character faction is a hostile to player, or not.
	//! \param[in] char Character to be evaluated if is enemy or not.

}
