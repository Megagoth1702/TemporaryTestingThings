modded class SCR_RadioCoverageSystem : GameSystem
{
	//------------------------------------------------------------------------------------------------ v1
//	override protected void HandleRanges(notnull SCR_CoverageRadioComponent component, float range = -1)
//	{
//		if (range < 0)
//			range = component.GetTransceiver(0).GetRange();
//
//		float rangeSq = range * range;
//
//		vector position = component.GetOwner().GetOrigin();
//		int otherRange;
//		int distanceSq;
//
//		foreach (SCR_CoverageRadioComponent radio : m_aRadioComponents)
//		{
//			if (radio == component || !radio.IsPowered())
//				continue;
//
//			distanceSq = vector.DistanceSqXZ(radio.GetOwner().GetOrigin(), position);
//
//			ref array<ref string> assignedBases = component.GetConnectedBases(); // This isn't working as expected at the moment
//			
////			if (distanceSq <= rangeSq || (component.GetIsReceivingFromIsland() && radio.GetIsSourceRadioOnIsland() && component.GetEncryptionKey() == radio.GetEncryptionKey()))
//			if (distanceSq <= rangeSq || (component.GetBaseNameAssignedTo() == radio.GetOwner().GetName() && !component.GetBaseNameAssignedTo().IsEmpty())) //(component.GetBaseNameAssignedTo() == radio.GetOwner().GetName()))
//			{
//				component.AddRadioInRange(radio);
//				radio.AddRadioInRangeOf(component);
//			}
//			else
//			{
//				component.RemoveRadioInRange(radio);
//				radio.RemoveRadioInRangeOf(component);	
//			}
//
//			otherRange = radio.GetTransceiver(0).GetRange();
//
//			if (distanceSq <= (otherRange * otherRange))
//			{
//				radio.AddRadioInRange(component);
//				component.AddRadioInRangeOf(radio);
//			}
//			
//		}
//	}
	
	//------------------------------------------------------------------------------------------------ v4
	override protected void HandleRanges(notnull SCR_CoverageRadioComponent component, float range = -1)
	{
	    if (range < 0)
	        range = component.GetTransceiver(0).GetRange();
	
		// This is the only class/method that works for setting MHQ range
		SCR_CampaignMobileAssemblyStandaloneComponent standaloneComp = SCR_CampaignMobileAssemblyStandaloneComponent.Cast(component.GetOwner().FindComponent(SCR_CampaignMobileAssemblyStandaloneComponent));
		if (standaloneComp)
		{
			SCR_GameModeCampaign campaignInfo = SCR_GameModeCampaign.GetInstance();
			if (campaignInfo)
			{
				range = campaignInfo.GetMHQRange();
				component.GetTransceiver(0).SetRange(range);
				//PrintFormat("SCR_RadioCoverageSystem: HandleRanges: Setting Range for MHQ: %1", range);
			}			
		}
		
	    float rangeSq = range * range;
	    vector position = component.GetOwner().GetOrigin();
	
	    // Cache assigned and excluded base names
	    ref array<ref string> assignedBases = component.GetBaseNamesAssignedTo();
	    ref array<ref string> excludedBases = component.GetExcludedBaseNames();
	
	    foreach (SCR_CoverageRadioComponent radio : m_aRadioComponents)
	    {
	        if (radio == component || !radio.IsPowered())
	            continue;
	
	        string radioBaseName = radio.GetOwner().GetName();
	        vector radioPosition = radio.GetOwner().GetOrigin();
	        float distanceSq = vector.DistanceSqXZ(radioPosition, position);
	
	        bool isExcluded = excludedBases.Contains(radioBaseName);
	        bool isAssigned = assignedBases.Contains(radioBaseName);
	        bool isWithinRange = distanceSq <= rangeSq;
	
	        // Forward check: Can component transmit to radio?
	        if (!isExcluded && (isWithinRange || isAssigned))
	        {
//	            PrintFormat("SCR_RadioCoverageSystem: HandleRanges: Component.AddRadioInRange: Radio(%1)", radioBaseName);
	            component.AddRadioInRange(radio);    // Component can transmit to radio
	            radio.AddRadioInRangeOf(component);  // Radio can receive from component
	        }
	        else
	        {
//	            PrintFormat("SCR_RadioCoverageSystem: HandleRanges: Component.RemoveRadioInRange: Radio(%1)", radioBaseName);
	            component.RemoveRadioInRange(radio);
	            radio.RemoveRadioInRangeOf(component);
	        }
	
	        // Reverse check: Can radio transmit to component?
	        float otherRange = radio.GetTransceiver(0).GetRange();
	        float otherRangeSq = otherRange * otherRange;
	
	        // Allow transmission if within range OR if radio is assigned to component
//	        if (distanceSq <= otherRangeSq || isAssigned)
			if (distanceSq <= otherRangeSq && !isExcluded || isAssigned)
	        {
//	            PrintFormat("SCR_RadioCoverageSystem: HandleRanges: Radio.AddRadioInRange: Component(%1)", component.GetOwner().GetName());
	            radio.AddRadioInRange(component);    // Radio can transmit to component
	            component.AddRadioInRangeOf(radio);  // Component can receive from radio
	        }
	        else
	        {
//	            PrintFormat("SCR_RadioCoverageSystem: HandleRanges: Radio.RemoveRadioInRange: Component(%1)", component.GetOwner().GetName());
	            radio.RemoveRadioInRange(component);
	            component.RemoveRadioInRangeOf(radio);
	        }
	    }
	}
}
