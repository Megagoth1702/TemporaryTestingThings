[ComponentEditorProps(category: "Iron", description: "Ironbeard Suppression")]
class IRON_SUP_ProjectileComponentClass : ScriptComponentClass
{
}

class IRON_SUP_ProjectileComponent : ScriptComponent
{	
	// Located in projectile prefabs 	
	[Attribute(defvalue: "1.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fCaliberMultiplier;
	
	[Attribute(defvalue: "15.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fFriendlyFlybyRange;
	
	[Attribute(defvalue: "1.5", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fMinimumFlybyRange;
	
	[Attribute(defvalue: "8.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fMaximumFlybyRange;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.CheckBox, category: "Suppression")]
	private bool m_bCauseFlinchUponImpact;
	
	[Attribute(defvalue: "2.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fMinimumImpactRange;
	
	[Attribute(defvalue: "6.0", uiwidget: UIWidgets.EditBox, category: "Suppression")]
	private float m_fMaximumImpactRange;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.CheckBox, category: "Suppression")]
	private bool m_bCausesFlybyFlinch;
	
	private Vehicle m_pFiredFromVehicle;
	protected IEntity m_iBulletOwner;
	
	//------------------------------------------------------------------------------------------------
	void SetBulletOwner(IEntity owner)
	{
		m_iBulletOwner = owner;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetBulletOwner()
	{
		return m_iBulletOwner;
	}

	//------------------------------------------------------------------------------------------------
	bool IsBulletOwnerFriendlyToPlayer(IEntity playerEntity)
	{
	    if (!m_iBulletOwner)
	        return false;
	
	    SCR_ChimeraCharacter bulletOwner = SCR_ChimeraCharacter.Cast(m_iBulletOwner);
	    if (!bulletOwner)
			return false;

		 // Handle player-controlled entity
        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(playerEntity);
        int bulletId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_iBulletOwner);
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(playerManager.GetPlayerControlledEntity(playerId));
		SCR_ChimeraCharacter bullet = SCR_ChimeraCharacter.Cast(playerManager.GetPlayerControlledEntity(bulletId));
        
		if (bullet)
		{
			Faction playerFaction = player.GetFaction();
	        Faction bulletFaction = bullet.GetFaction();
	
	        return playerFaction == bulletFaction;
		}
		
		else
		{
		     // Handle the case for AI-controlled entities
			CharacterControllerComponent charController = bulletOwner.GetCharacterController();
	        AIControlComponent aiControl = charController.GetAIControlComponent();
	        if (aiControl)
	        {
	            // Logic to check AI faction; assume we have a method to get faction for AI
	            Faction bulletFaction = GetAIEntityFaction(bulletOwner);
	            Faction playerFaction = SCR_FactionManager.SGetLocalPlayerFaction();
	            return bulletFaction == playerFaction;
	        }
			
	        else
	        {
	            return false; // If owner is null and not AI, treated as non-friendly
	        }
		}
		
		return false;		
	}
	
	//------------------------------------------------------------------------------------------------
	// Additional helper method to get AI faction if necessary
	Faction GetAIEntityFaction(SCR_ChimeraCharacter aiEntity)
	{
	    FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast(aiEntity.FindComponent(FactionAffiliationComponent));
	    if (factionComponent)
	    {
	        return factionComponent.GetAffiliatedFaction();
	    }
		
	    return null; // Default case if no faction is found
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
	    // Check if the game is in play mode and if we're on the client side
	    if (!GetGame().InPlayMode())// || !Iron_SuppressionHelper.IsClient())
	    {
	        return;
	    }
	
	    IEntity playerEntity = SCR_PlayerController.GetLocalControlledEntity();
	    
	    // Don't suppress yourself
	    if (m_iBulletOwner == playerEntity)
	    {
	        return;
	    }
	
	    // If impact is set to trigger on deletion, handle the impact
	    if (IsImpactWhenDeleted())
	    {
			Iron_SuppressionManager.GetOnProjectileImpactInvoker().Invoke(this, m_iBulletOwner);
	    }
	}
	
	//----------------------------------------------------------------------------------
	float GetCaliberMultiplier()
	{
		return m_fCaliberMultiplier;
	}
	
	//----------------------------------------------------------------------------------
	float GetFriendlyFlybyRange()
	{
		return m_fFriendlyFlybyRange;
	}
	
	//----------------------------------------------------------------------------------
	float GetMinimumFlybyRange()
	{
		return m_fMinimumFlybyRange;
	}
	
	//----------------------------------------------------------------------------------
	float GetMaximumFlybyRange()
	{
		return m_fMaximumFlybyRange;
	}
	
	//----------------------------------------------------------------------------------
	bool IsImpactWhenDeleted()
	{
		return m_bCauseFlinchUponImpact;
	}
	
	//----------------------------------------------------------------------------------
	void CauseFlinchUponImpact(bool value)
	{
		m_bCauseFlinchUponImpact = value;
	}
	
	//----------------------------------------------------------------------------------
	float GetMinimumImpactRange()
	{
		return m_fMinimumImpactRange;
	}
	
	//----------------------------------------------------------------------------------
	float GetMaximumImpactRange()
	{
		return m_fMaximumImpactRange;
	}
	
	//----------------------------------------------------------------------------------
	bool CanCauseFlybyFlinch()
	{
		return m_bCausesFlybyFlinch;
	}
	
	//----------------------------------------------------------------------------------
	void SetCausesFlybyFlinch(bool value)
	{
		m_bCausesFlybyFlinch = value;
	}
}