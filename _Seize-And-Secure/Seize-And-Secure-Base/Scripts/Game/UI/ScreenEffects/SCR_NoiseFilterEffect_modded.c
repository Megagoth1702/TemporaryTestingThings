//modded class SCR_NoiseFilterEffect : SCR_BaseScreenEffect
//{
//
//	[RplProp()]
//	protected bool m_bIsMuted;
//	
//	//------------------------------------------------------------------------------------------------
// 	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
//	{
//		SCR_Iron_EarplugManager.GetOnEarplugToggleInvoker().Insert(OnEarPlugStateChanged);
//		
//		super.DisplayControlledEntityChanged(from, to);
//	}
//	
//	//------------------------------------------------------------------------------------------------
//	// Check whether to apply or unapply the filter effect in case of ear plugs
//	static void OnEarPlugStateChanged(bool value)
//	{		
////		m_bIsMuted = value;
//
///*		AudioSystem.SetOutputStateSignal(
//			AudioSystem.DefaultOutputState,
//			AudioSystem.GetOutpuStateSignalIdx(AudioSystem.DefaultOutputState, "GEarPlugsInserted"),
//			value); */
//	}	
//	
//	//------------------------------------------------------------------------------------------------
//	protected override void ClearEffects()
//	{
//		
//		if (m_pDamageManager)
//			m_pDamageManager.GetOnDamageStateChanged().Remove(OnDamageStateChanged);
//
////		SCR_Iron_EarplugManager.ClearMutedState();
//	}
//};