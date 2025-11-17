modded class SCR_DamageBlurEffect : SCR_BaseScreenEffect
{
	protected ref IRON_SUP_ScreenEffects m_IronSuppressEffectsManager;

	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
//		SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
//		m_IronSuppressManager = Iron_SuppressionScreenEffect.Cast(screenEffectsManager.GetEffect(Iron_SuppressionScreenEffect));
		m_IronSuppressEffectsManager = IRON_SUP_ScreenEffects.Cast(GetGame().GetHUDManager().FindInfoDisplay(IRON_SUP_ScreenEffects));
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnDamage(BaseDamageContext damageContext)
	{
		SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay(); // 1.3 - SCR_ScreenEffectsManager screenEffectsManager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
		IRON_SUP_ScreenEffects suppressionEffectManager = IRON_SUP_ScreenEffects.Cast(screenEffectsManager.GetEffect(IRON_SUP_ScreenEffects));
		
//		m_IronSuppressEffectsManager = IRON_SUP_ScreenEffects.Cast(GetGame().GetHUDManager().FindInfoDisplay(IRON_SUP_ScreenEffects));
		if (suppressionEffectManager)
		{
			suppressionEffectManager.HeavyFlinchEffect();
		}
				
		if (damageContext.damageValue > m_fGaussBlurMinDamage)
		{
			m_fGaussBlurReduction = Math.Clamp(++m_fGaussBlurReduction, 0, 2);
		}		
	}
}
