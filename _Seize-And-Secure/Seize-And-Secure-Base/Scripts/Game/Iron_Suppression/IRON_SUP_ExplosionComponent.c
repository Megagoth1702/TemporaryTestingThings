[ComponentEditorProps(category: "Ironbeard", description: "Ironbeard Suppression")]
class IRON_SUP_ExplosionComponentClass : ScriptComponentClass
{
}

class IRON_SUP_ExplosionComponent : ScriptComponent
{	
	// Located in Suppression_Explosion_Base.et and vehicle prefabs, spawned by warhead
	[Attribute(defvalue: "3.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fExplosionStrength;
	
	[Attribute(defvalue: "1.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fForceMultiplier;
	
	[Attribute(defvalue: "10.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fExplosionRangeMin;
	
	[Attribute(defvalue: "20.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fExplosionRangeMax;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.CheckBox, category: "Suppression")]
	private bool m_bActivateOnInitialization;
	
	//----------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (!GetGame().InPlayMode() || !Iron_SuppressionManager.IsClient())
		{
			return;
		}	
		
		if (GetActivateOnInitialization())
		{
			ApplyEffect();
		}			
	}
	
	//----------------------------------------------------------------------------------
	void ApplyEffect()
	{
		Iron_SuppressionManager.GetOnExplosionInvoker().Invoke(this);
	}
	
	//----------------------------------------------------------------------------------
	float GetExplosionStrength()
	{
		return m_fExplosionStrength;
	}
	
	//----------------------------------------------------------------------------------
	float GetForceMultiplier()
	{
		return m_fForceMultiplier;
	}
	
	//----------------------------------------------------------------------------------
	float GetExplosionRangeMin()
	{
		return m_fExplosionRangeMin;
	}
	
	//----------------------------------------------------------------------------------
	float GetExplosionRangeMinSq()
	{
		return m_fExplosionRangeMin * m_fExplosionRangeMin;
	}
	
	//----------------------------------------------------------------------------------
	float GetExplosionRangeMax()
	{
		return m_fExplosionRangeMax;
	}
	
	//----------------------------------------------------------------------------------
	float GetExplosionRangeMaxSq()
	{
		return m_fExplosionRangeMax * m_fExplosionRangeMax;
	}
	
	//----------------------------------------------------------------------------------
	bool GetActivateOnInitialization()
	{
		return m_bActivateOnInitialization;
	}
}