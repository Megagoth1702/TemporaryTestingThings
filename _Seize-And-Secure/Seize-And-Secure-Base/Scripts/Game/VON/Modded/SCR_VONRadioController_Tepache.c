//------------------------------------------------------------------------------------------------
//! Scripted VON input and control, attached to SCR_PlayerController
//modded class SCR_VONController : ScriptComponent
//{
// 	// TODO more robust von component selection management
//
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_Start_1.wav", params: "wav")]
//	protected ResourceName m_sRadioSound0;
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_Start_2.wav", params: "wav")]
//	protected ResourceName m_sRadioSound1;
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_Start_3.wav", params: "wav")]
//	protected ResourceName m_sRadioSound2;
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_Start_4.wav", params: "wav")]
//	protected ResourceName m_sRadioSound3;
//	
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_Start_1_LR.wav", params: "wav")]
//	protected ResourceName m_sRadioSoundLR0;
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_Start_2_LR.wav", params: "wav")]
//	protected ResourceName m_sRadioSoundLR1;
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_Start_3_LR.wav", params: "wav")]
//	protected ResourceName m_sRadioSoundLR2;
//	
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_End_1.wav", params: "wav")]
//	protected ResourceName m_sRadioSoundEnd0;
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_End_2.wav", params: "wav")]
//	protected ResourceName m_sRadioSoundEnd1;
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_End_3.wav", params: "wav")]
//	protected ResourceName m_sRadioSoundEnd2;
//	[Attribute(defvalue: "Sounds/VON/Samples/Radio_End_4.wav", params: "wav")]
//	protected ResourceName m_sRadioSoundEnd3;
//	
//	protected AudioHandle m_AudioHandleRadioSqawk;
//	protected AudioHandle m_AudioHandleRadioSqawkRadioSqawkOff;
//	
//	//------------------------------------------------------------------------------------------------
//	override protected void ActionVONBroadcast(float value, EActionTrigger reason = EActionTrigger.UP)
//	{
//	
//		super.ActionVONBroadcast(value, reason);
//		
//		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
//		if (!player)
//			return;
//		
//		ChimeraCharacter character = ChimeraCharacter.Cast(player);	
//		if (!character)
//		{
//			return;
//		}
//		
//		CharacterControllerComponent controller = character.GetCharacterController();
//		if (!controller)
//		{
//			return;
//		}
//		
//		if (controller.IsUnconscious() || controller.IsDead())
//		{
//			return;
//		}
//		
//		// Tepache Radio - integrated into S&S with permission (Ask Clay...)
//		if (reason == EActionTrigger.DOWN && !m_bIsLongRangeToggled)
//		{
//			if (!AudioSystem.IsSoundPlayed(m_AudioHandleRadioSqawk))
//					AudioSystem.TerminateSound(m_AudioHandleRadioSqawk);
//			
//			RandomGenerator gen = new RandomGenerator();
//			int sampleRadioStartIndex =  gen.RandIntInclusive(0, 3);
//			
//			if (sampleRadioStartIndex==0)
//				m_AudioHandleRadioSqawk = AudioSystem.PlaySound(m_sRadioSound0);	
//			if (sampleRadioStartIndex==1)
//				m_AudioHandleRadioSqawk = AudioSystem.PlaySound(m_sRadioSound1);	
//			if (sampleRadioStartIndex==2)
//				m_AudioHandleRadioSqawk = AudioSystem.PlaySound(m_sRadioSound2);	
//			if (sampleRadioStartIndex==3)
//				m_AudioHandleRadioSqawk = AudioSystem.PlaySound(m_sRadioSound3);			
//
//			SetVONBroadcast(true);
//		}
//		
//		else if (reason == EActionTrigger.DOWN && m_bIsLongRangeToggled)
//		{
//		
//			if (!AudioSystem.IsSoundPlayed(m_AudioHandleRadioSqawk))
//					AudioSystem.TerminateSound(m_AudioHandleRadioSqawk);
//			
//			RandomGenerator gen = new RandomGenerator();
//			int sampleRadioStartIndex =  gen.RandIntInclusive(0, 2);
//			
//			if (sampleRadioStartIndex==0)
//				m_AudioHandleRadioSqawk = AudioSystem.PlaySound(m_sRadioSoundLR0);	
//			if (sampleRadioStartIndex==1)
//				m_AudioHandleRadioSqawk = AudioSystem.PlaySound(m_sRadioSoundLR1);	
//			if (sampleRadioStartIndex==2)
//				m_AudioHandleRadioSqawk = AudioSystem.PlaySound(m_sRadioSoundLR2);				
//
//			SetVONBroadcast(true);
//		}
//		
//		else 
//		{
//			AudioSystem.TerminateSound(m_AudioHandleRadioSqawk);
//			
//			if (!AudioSystem.IsSoundPlayed(m_AudioHandleRadioSqawkRadioSqawkOff))
//					AudioSystem.TerminateSound(m_AudioHandleRadioSqawkRadioSqawkOff);
//
//			RandomGenerator gen = new RandomGenerator();
//			int sampleRadioEndIndex =  gen.RandIntInclusive(0, 3);
//			
//			if (sampleRadioEndIndex==0)
//				m_AudioHandleRadioSqawkRadioSqawkOff = AudioSystem.PlaySound(m_sRadioSoundEnd0);	
//			if (sampleRadioEndIndex==1)
//				m_AudioHandleRadioSqawkRadioSqawkOff = AudioSystem.PlaySound(m_sRadioSoundEnd1);	
//			if (sampleRadioEndIndex==2)
//				m_AudioHandleRadioSqawkRadioSqawkOff = AudioSystem.PlaySound(m_sRadioSoundEnd2);	
//			if (sampleRadioEndIndex==3)
//				m_AudioHandleRadioSqawkRadioSqawkOff = AudioSystem.PlaySound(m_sRadioSoundEnd3);	
//			
//			SetVONBroadcast(false);
//		}
//	}
//	
//	//------------------------------------------------------------------------------------------------
//	//! Set transmission method depending on entry type when starting VON transmit
//	override protected void SetActiveTransmit(notnull SCR_VONEntry entry)
//	{	
//		// Null errors/crashing due to m_VONComp when exiting game in 1.3 experimental
//		if (!m_VONComp)
//		{
//			return;
//		}
//			
//		super.SetActiveTransmit(entry);
//	}
//};
