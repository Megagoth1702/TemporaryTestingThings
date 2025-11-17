[ComponentEditorProps(category: "Suppression", description: "Sets suppression settings for each compartment of a vehicle.")]
class IRON_SUP_VehicleComponentClass : ScriptComponentClass
{
}

class IRON_SUP_VehicleComponent : ScriptComponent
{
	// Located in helicopter/vehicle prefabs - customize suppression effects per vehicle/compartment	
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.CheckBox, category: "Suppression")]
	private bool m_bAllowImpactFlinch;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.CheckBox, category: "Suppression")]
	private bool m_bAllowFlybyFlinch;
	
	[Attribute(defvalue: "0.5", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fSuppressionDampening;
	
	[Attribute(defvalue: "0.5", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fExplosionDampening;
	
	//----------------------------------------------------------------------------------
	bool GetAllowImpactFlinch()
	{
		return m_bAllowImpactFlinch;
	}
	
	//----------------------------------------------------------------------------------
	bool GetAllowFlybyFlinch()
	{
		return m_bAllowFlybyFlinch;
	}
	
	//----------------------------------------------------------------------------------
	float GetSuppressionDampening()
	{
		return m_fSuppressionDampening;
	}
	
	//----------------------------------------------------------------------------------
	float GetExplosionDampening()
	{
		return m_fExplosionDampening;
	}
}