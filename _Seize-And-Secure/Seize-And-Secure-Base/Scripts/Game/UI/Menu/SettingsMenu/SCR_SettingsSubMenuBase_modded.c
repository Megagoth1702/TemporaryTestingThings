modded class SCR_SettingsSubMenuBase: SCR_SubMenuBase
{
	//------------------------------------------------------------------------------------------------
	override protected void OnMenuItemChanged(SCR_SettingsBindingBase binding)
	{
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode && gameMode.IsRunning())
		{
			gameMode.EnforceMinVideoSettings();
		}

		super.OnMenuItemChanged(binding);
	}
};