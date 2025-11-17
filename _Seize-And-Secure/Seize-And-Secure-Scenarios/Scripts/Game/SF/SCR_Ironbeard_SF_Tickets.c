[BaseContainerProps(), SCR_ContainerActionTitle()]
class SCR_Ironbeard_SF_Tickets : SCR_ScenarioFrameworkActionBase
{
	[Attribute(desc: "Entity to be deleted (Optional if action is attached on Slot that spawns target entity)")]
	ref SCR_ScenarioFrameworkGet m_Getter;
	
	[Attribute(defvalue: "", UIWidgets.EditBox, desc: "Faction Name", category: "")]
	protected FactionKey m_sFactionKey;
	
	[Attribute(defvalue: "", UIWidgets.Auto, desc: "Ticket Reward", category: "")]
	protected int m_iReward;

	//------------------------------------------------------------------------------------------------
	override void OnActivate(IEntity object) // IEntity object
	{
//		if (!CanActivate())
//			return;
//
//		IEntity entity;
//		if (!ValidateInputEntity(object, m_Getter, entity))
//			return;

		SCR_BaseScoringSystemComponent scoringSystem = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).GetScoringSystemComponent();
		FactionManager factionManager = GetGame().GetFactionManager();
		Faction faction = factionManager.GetFactionByKey(m_sFactionKey);

		scoringSystem.AddFactionObjective(faction, m_iReward);
	}
}