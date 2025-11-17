modded class SCR_VideoSettingsSubMenu : SCR_SettingsSubMenuBase
{
	//------------------------------------------------------------------------------------------------
	override protected void SetupGrassDistance()
	{
		SCR_SliderComponent distance = SCR_SliderComponent.GetSliderComponent("GrassDistance", m_wRoot);
		if (!distance)
			return;

		int min = 125; //GetGame().GetMinimumGrassDistance();
		int max = GetGame().GetMaximumGrassDistance();
		int current = GetGame().GetGrassDistance();
		distance.SetMin(min);
		distance.SetMax(max);
		distance.SetStep(1);

		if (current < min)
		{
			current = min;
		}
		
		distance.SetValue(current);
		distance.GetOnChangedFinal().Insert(OnGrassDistanceChanged);
	}
}