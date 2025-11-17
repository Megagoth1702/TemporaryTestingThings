//------------------------------------------------------------------------------------------------
class SCR_Iron_Ragdoll_RepositionUserAction : ScriptedUserAction
{
    [Attribute(uiwidget: UIWidgets.ComboBox, desc: "Target pose for unconscious casualty", enums: ParamEnumArray.FromEnum(Iron_Ragdoll_EUnconsciousPose))]
    protected Iron_Ragdoll_EUnconsciousPose m_UnconsciousPosePosition;

    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        if (!super.CanBeShownScript(user) || !IsValidUser(user) || !IsValidOwner(GetOwner()))
            return false;

        SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
        SCR_CharacterControllerComponent ownerController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
        
        return ownerController && ownerController.IsUnconscious() && ownerController.GetUnconsciousPose() != m_UnconsciousPosePosition
				&& !ownerChar.GetAnimationComponent().IsRagdollActive();
    }

    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(pOwnerEntity);
        if (ownerChar)
        {
            SCR_CharacterControllerComponent ownerController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
            if (ownerController)
            {
                ownerController.Do_UnconPoseReposition(m_UnconsciousPosePosition);
            }
        }
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBroadcastScript()
    {
        return false;
    }

    //------------------------------------------------------------------------------------------------
    private bool IsValidUser(IEntity user)
    {
        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
        if (!userChar || userChar.IsInVehicle())
            return false;

        CharacterControllerComponent userController = userChar.GetCharacterController();
        return userController && !userController.IsSwimming() && !userController.IsFalling() && !userController.IsClimbing();
    }
    
    //------------------------------------------------------------------------------------------------
    private bool IsValidOwner(IEntity owner)
    {
        SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(owner);
        if (!ownerChar || ownerChar.IsInVehicle())
            return false;

        SCR_CharacterControllerComponent ownerController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
        return ownerController != null && !ownerController.IsDead();
    }
}