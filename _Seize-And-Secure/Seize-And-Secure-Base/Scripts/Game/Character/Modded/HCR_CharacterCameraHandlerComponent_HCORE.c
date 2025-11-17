modded class SCR_CharacterCameraHandlerComponent
{
    override int CameraSelector()
    {
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_OwnerCharacter.GetCharacterController());
		
        if (IsInThirdPerson() && !m_OwnerCharacter.IsInVehicle() && !charController.IsUnconscious())
        {
            SetThirdPerson(false);
			
            return CharacterCameraSet.CHARACTERCAMERA_1ST;
        }
        
        if(m_OwnerCharacter.IsInVehicle())
        {
            CompartmentAccessComponent compartmentAccess = m_OwnerCharacter.GetCompartmentAccessComponent();
			IEntity vehicle = compartmentAccess.GetVehicleIn(m_OwnerCharacter);
			BaseCompartmentSlot currentCompartment = compartmentAccess.GetCompartment();
			
			if(currentCompartment && currentCompartment.GetType() == ECompartmentType.CARGO)
            {
				SetThirdPerson(false);
				
				return CharacterCameraSet.CHARACTERCAMERA_1ST_VEHICLE;
			}
			
			if (currentCompartment && currentCompartment.GetType() == ECompartmentType.TURRET)
			{
				SetThirdPerson(false);
				
				bool isTurretAds = false;
				bool inTurret = CheckIsInTurret(isTurretAds);
	
				if (isTurretAds)
					return CharacterCameraSet.CHARACTERCAMERA_ADS_VEHICLE;

				return CharacterCameraSet.CHARACTERCAMERA_1ST_TURRET;
			}
        }    

        return super.CameraSelector();
    };
};