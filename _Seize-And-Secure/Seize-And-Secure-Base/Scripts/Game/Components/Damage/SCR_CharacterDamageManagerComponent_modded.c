//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_ExtendedDamageManagerComponent
{
	protected WorldTimestamp m_UnconTimestamp;
	
	//-----------------------------------------------------------------------------------------------------------
	override void UpdateConsciousness()
	{
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(GetOwner().FindComponent(SCR_CharacterControllerComponent));
    
	    // Check if the character is conscious and not in a vehicle
	    if (!characterController.IsUnconscious() && !ChimeraCharacter.Cast(GetOwner()).IsInVehicle())
	    {
	        BaseWeaponManagerComponent weaponComponent = characterController.GetWeaponManagerComponent(); //BaseWeaponManagerComponent.Cast(characterController.GetWeaponManagerComponent());
	        bool wasArmed = false;
	        array<BaseWeaponComponent> weaponsList = {};
	        weaponComponent.GetWeapons(weaponsList);
	
	        foreach (BaseWeaponComponent weapon : weaponsList)
	        {
	            if (IsWeaponTypeValid(weapon.GetWeaponType()))
	            {
	                wasArmed = true;
	                break; // No need to continue checking once we find a valid weapon
	            }
				else
				{
					wasArmed = false;
				}
	        }
	
	        characterController.SetWasArmed(wasArmed);
	    }

    	super.UpdateConsciousness();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected bool IsWeaponTypeValid(EWeaponType weaponType)
	{
	    if (weaponType == EWeaponType.WT_RIFLE ||
        weaponType == EWeaponType.WT_COUNT || //??? Not sure what this one is
        weaponType == EWeaponType.WT_HANDGUN ||
        weaponType == EWeaponType.WT_MACHINEGUN ||
        weaponType == EWeaponType.WT_AUTOCANNON ||
        weaponType == EWeaponType.WT_FRAGGRENADE ||
        weaponType == EWeaponType.WT_SNIPERRIFLE ||
        weaponType == EWeaponType.WT_ROCKETLAUNCHER ||
        weaponType == EWeaponType.WT_GRENADELAUNCHER)
	    {
	        return true;
	    }
	    return false;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	void SetUnconTimestamp()
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
		if (!character)
			return;
		
		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		if (!controller)
			return;
		
		WorldTimestamp timeStamp = GetGame().GetWorld().GetTimestamp();
		controller.SetUnconTimestamp();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState, bool isJIP)
	{
		if (newLifeState == ECharacterLifeState.INCAPACITATED)
		{
			SetUnconTimestamp();
			SoundKnockout();
		}
		else if (newLifeState == ECharacterLifeState.DEAD)
		{
			SoundDeath(previousLifeState);
			RemoveAllBleedingParticlesAfterDeath();
		}
	}
};