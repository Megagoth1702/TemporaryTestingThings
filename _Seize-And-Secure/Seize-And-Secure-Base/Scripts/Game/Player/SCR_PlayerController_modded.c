void OnControlledEntityChangedServer(int playerId, IEntity from, IEntity to);
typedef func OnControlledEntityChangedServer;
typedef ScriptInvokerBase<OnControlledEntityChangedServer> OnControlledEntityChangedServerInvoker;
	
modded class SCR_PlayerController : PlayerController
{

	ref OnControlledEntityChangedServerInvoker m_Seize_OnControlledEntityChangedServer = new OnControlledEntityChangedServerInvoker();
	protected CharacterControllerComponent m_CharacterController;
	protected SCR_CommunicationSoundComponent m_CommunicationSound;
	protected SCR_GameModeCampaign m_CampaignInfo;
	
	private float m_fHealth;
	private float m_fMovementDamage;
	private float m_fAimingDamage;
	private float m_fElapsedTime = 0.0;
	private float m_fElapsedTimeLimp = 0.0;
	private const float m_fTimeInterval = 3.0;
	private const float m_fLimpInterval = 0.5;

	// Bushes slowing you down
	protected bool m_bBushSlowingEnabled;
	const float MIN_SPEED = 0.35;
	const float MAX_BUSH_SIGNAL = 3.0;
	
	protected float m_fBushSignalTime;
	protected float m_fBushSignalTimeTick = 2.5;
	protected float m_fBushSignalTimeTickMin = 1.0;
	protected float m_fBushSignalTimeTickMax = 4.0;
	
	// Strafe spamming
	protected bool m_bStrafeSpamPenaltyEnabled;
	protected vector m_PreviousStrafeVelocity;
	protected bool m_bIsStrafeSpamming;
	protected float m_fStrafeSpamTime;
	protected float m_fStrafeSpamTimeTick = 1.0;
	protected float m_fStrafeSpamForgivenessTimeTick = 2.0;
	
	ref array<WorldTimestamp> m_aStrafeChangeTimes = {}; // Tracks timestamps of direction changes
	float m_fPreviousStrafe = 0.0; 						// Previous frame's strafe component
	const float STRAFE_WINDOW = 3.0; 					// Time window in seconds to check for spam
	const int MAX_CHANGES = 2; 							// Max direction changes allowed in the window before tripping
	const float STRAFE_THRESHOLD = 0.3; 				// Minimum strafe speed to consider a valid change (prevents noise from tiny movements) Crouch = 0.3
	float m_fPreviousYaw = 0.0; 						// Previous frame's yaw angle
	const float MAX_YAW_CHANGE = 15.0; 					// Max degrees rotation per frame to consider "real" strafe
	
	// Diving/Crouch sliding
	protected bool m_bIsSliding;
	protected float m_fSlidingTime;
	protected float m_fSlidingTimeTick = 1.0;
	protected bool m_bIsDiving;
	protected vector m_SlidingVelocity;
	protected bool m_bSlidingTriggered;
	
	protected bool m_bIsPlayingAudio;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (m_CampaignInfo)
		{
			m_bBushSlowingEnabled = m_CampaignInfo.GetBushSlowingPlayerEnabled();
			m_bStrafeSpamPenaltyEnabled = m_CampaignInfo.GetStrafeSpamPenaltyEnabled();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		super.OnControlledEntityChanged(from, to);
		
		if (!Replication.IsRunning() || Replication.IsServer())
			m_Seize_OnControlledEntityChangedServer.Invoke(GetPlayerId(), from, to);
	}
	
/*	protected ref array<ref string> m_sPlayerInjuries = {};
	
	//----------------------------------------------------------------------------------
	void SetPlayerInjuryList(string injury)
	{
		m_sPlayerInjuries.Insert(injury);		
	}
	
	//----------------------------------------------------------------------------------
	ref array<ref string> GetPlayerInjuryList()
	{
		return m_sPlayerInjuries;
	}
	
	//----------------------------------------------------------------------------------
	void ClearPlayerInjuryList()
	{
		m_sPlayerInjuries.Clear();
	} */
	
	//------------------------------------------------------------------------------------------------
	void UpdateMovementAndAimingState(float timeSlice)
	{
		IEntity player = GetLocalMainEntity();
		if (!player)
			return;
		
		ChimeraCharacter character = ChimeraCharacter.Cast(player);
		if (!character)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
		if (!damageManager)
			return;
		
		m_CharacterController = CharacterControllerComponent.Cast(player.FindComponent(CharacterControllerComponent));
		if (!m_CharacterController || m_CharacterController.IsDead())
			return;
				
		m_fHealth = damageManager.GetHealth();

		if (m_fHealth <= 99) // && (!damageManager.GetGroupTourniquetted(ECharacterHitZoneGroup.LEFTLEG) || !damageManager.GetGroupTourniquetted(ECharacterHitZoneGroup.RIGHTLEG) || !damageManager.GetGroupTourniquetted(ECharacterHitZoneGroup.LEFTARM) || !damageManager.GetGroupTourniquetted(ECharacterHitZoneGroup.RIGHTARM)))
		{
			m_fMovementDamage = Math.Clamp((100-m_fHealth) * 0.01, 0, 0.5);
			m_fAimingDamage = Math.Clamp((100-m_fHealth) * 0.1, 0, 3); // 10 - might be too high
			
			m_fElapsedTime += timeSlice;
			
			array<ref SCR_PersistentDamageEffect> effects = damageManager.GetAllPersistentEffectsOfType(SCR_MorphineDamageEffect);
			if (!effects.IsEmpty() || Iron_SuppressionManager.GetSuppressionLevel() > 0)
			{
				m_fMovementDamage *= 0.5;
				m_fAimingDamage *= 0.5;
			}
			
	   		if (m_fElapsedTime >= m_fTimeInterval) 
			{
#ifdef WORKBENCH
			Print("Health: " + m_fHealth + " Movement Damage: " + m_fMovementDamage + " Aiming Damage: " + m_fAimingDamage);
#endif
				damageManager.SetMovementDamage(m_fMovementDamage);
				damageManager.SetAimingDamage(m_fAimingDamage);
				m_fElapsedTime = 0;
			}
			
			// Do a back-limp!
			if (GetAreLegsInjured(damageManager))
			{
				m_fElapsedTimeLimp += timeSlice;
				if (m_CharacterController.GetStance() != ECharacterStance.PRONE)
				{
					if (m_fElapsedTimeLimp >= m_fLimpInterval && m_fHealth <= 85)
					{
						damageManager.SetMovementDamage(m_fMovementDamage * 1.30);
						m_fElapsedTimeLimp = 0;
					}
					
					else if (m_fElapsedTimeLimp >= m_fLimpInterval * 0.5)
					{
						damageManager.SetMovementDamage(m_fMovementDamage);
					}
					
				}		
			}				
		}
		
		else
		{
			damageManager.SetMovementDamage(0);
			damageManager.SetAimingDamage(0);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetAreLegsInjured(SCR_CharacterDamageManagerComponent damageMan)
	{
		if (!damageMan)
			return false;
		
		bool isInjured = false;
		
		array<HitZone> legHitzones = {damageMan.GetHitZoneByName("LThigh"), damageMan.GetHitZoneByName("RThigh"),
									damageMan.GetHitZoneByName("RCalf"), damageMan.GetHitZoneByName("LCalf"),
									damageMan.GetHitZoneByName("RFoot"), damageMan.GetHitZoneByName("LFoot")};
		
		foreach (HitZone hitZone : legHitzones)
		{
			float health = hitZone.GetHealthScaled();
			if (health <= 0.9)
			{
				isInjured = true;
				break;
			}
		}
				
		return isInjured;
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_RepackMagazine(int playerId, RplId invManagerComp, RplId storageTo, RplId storageFrom, RplId fromMag, RplId toMag)
	{
		SCR_InventoryStorageManagerComponent m_InventoryManager = SCR_InventoryStorageManagerComponent.Cast(Replication.FindItem(invManagerComp));
		BaseInventoryStorageComponent storageToComp = BaseInventoryStorageComponent.Cast(Replication.FindItem(storageTo));
		BaseInventoryStorageComponent storageFromComp = BaseInventoryStorageComponent.Cast(Replication.FindItem(storageFrom));
		MagazineComponent fromMagazine = MagazineComponent.Cast(Replication.FindItem(fromMag));
		MagazineComponent toMagagazine = MagazineComponent.Cast(Replication.FindItem(toMag));

		Do_RepackMagazine(playerId, invManagerComp, storageTo, storageFrom, fromMag, toMag);
		Rpc(Do_RepackMagazine, playerId, invManagerComp, storageTo, storageFrom, fromMag, toMag); // Wants encoding 'n stuff
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Do_RepackMagazine(int playerId, RplId invManagerComp, RplId storageTo, RplId storageFrom, RplId fromMag, RplId toMag)
	{
		PrintFormat("Do_RepackMagazine: Server repacking Mag | Player ID: %1", playerId);
		IEntity playerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (!playerEntity)
		{
			PrintFormat("Do_RepackMagazine: No playerEntity");
			return;
		}
		
		ChimeraCharacter chimeraPlayer = ChimeraCharacter.Cast(playerEntity);
		if (!chimeraPlayer)
		{
			PrintFormat("Do_RepackMagazine: No chimeraPlayer");
			return;
		}
		
		//SCR_InventoryStorageManagerComponent m_InventoryManager = SCR_InventoryStorageManagerComponent.Cast(Replication.FindItem(invManagerComp));
		SCR_InventoryStorageManagerComponent m_InventoryManager = SCR_InventoryStorageManagerComponent.Cast(chimeraPlayer.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!m_InventoryManager)
		{
			PrintFormat("Do_RepackMagazine: No Inventory Manager");
			return;
		}
		
		BaseInventoryStorageComponent storageToComp = BaseInventoryStorageComponent.Cast(Replication.FindItem(storageTo));
		if (!storageToComp)
		{
			PrintFormat("Do_RepackMagazine: No storageToComp");
		}
		
		BaseInventoryStorageComponent storageFromComp = BaseInventoryStorageComponent.Cast(Replication.FindItem(storageFrom));
		if (!storageFromComp)
		{
			PrintFormat("Do_RepackMagazine: No storageFromComp");
		}
		
		MagazineComponent fromMagazine = MagazineComponent.Cast(Replication.FindItem(fromMag));
		if (!fromMagazine)
		{
			PrintFormat("Do_RepackMagazine: No fromMagazine");
		}
		
		MagazineComponent toMagagazine = MagazineComponent.Cast(Replication.FindItem(toMag));
		if (!toMagagazine)
		{
			Print("PlayerController.Do_RepackMagazine: No toMagagazine!", LogLevel.ERROR);
			return;
		}

		IEntity fromEntity = fromMagazine.GetOwner();
		IEntity toEntity = toMagagazine.GetOwner();
		
		int fromMagCount = fromMagazine.GetAmmoCount();
		int toMagCount = toMagagazine.GetAmmoCount();
		int maxMagCount = toMagagazine.GetMaxAmmoCount();
		
		if (fromMagCount + toMagCount <= maxMagCount)
		{
#ifdef WORKBENCH
			Print("Do_RepackMagazine: Setting From Mag (Ammo): " + fromMagCount + " To New Magazine (Ammo): " + toMagCount + " Total: " + (fromMagCount + toMagCount));
#endif
			toMagagazine.SetAmmoCount(fromMagCount + toMagCount);
			fromMagazine.SetAmmoCount(0);
			m_InventoryManager.TrySwapItemStorages(fromEntity, toEntity); // m_pCallBack); 
		}
		
		else
		{		
			int remainder = (fromMagCount + toMagCount) % maxMagCount;
						
			toMagagazine.SetAmmoCount(maxMagCount);

			if (remainder > 0)
			{
				fromMagazine.SetAmmoCount(remainder);
				m_InventoryManager.TrySwapItemStorages(fromEntity, toEntity);
			}
		}
		
		if (fromMagazine.GetAmmoCount() == 0)
			m_InventoryManager.TryRemoveItemFromInventory(fromEntity);
	}

	//----------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		if (!s_pLocalPlayerController)
			UpdateLocalPlayerController();

		if (m_bIsLocalPlayerController)
		{
			UpdateControls();
			//UpdateUI();
		}

		UpdateMovementEffects(timeSlice);
		UpdateMovementAndAimingState(timeSlice);
	}
	
	//----------------------------------------------------------------------------------
	void UpdateMovementEffects(float timeSlice)
	{
		if (m_CharacterController)
		{
			IEntity playerEntity = m_CharacterController.GetOwner();
			if (playerEntity)
			{
				int bushContactIdx;
				int bushHeightIdx;
				int surfaceIdx;
				int speedIdx;
				bool isWet;
				
				if (!m_CommunicationSound)
				{
					m_CommunicationSound = SCR_CommunicationSoundComponent.Cast(playerEntity.FindComponent(SCR_CommunicationSoundComponent));
				}
				
				CharacterSoundComponent charSoundComponent = CharacterSoundComponent.Cast(playerEntity.FindComponent(CharacterSoundComponent));
				if (charSoundComponent)
				{
					//BushContact
					//BushHeight

					speedIdx = charSoundComponent.GetSignalIndex("Speed");
					surfaceIdx = charSoundComponent.GetSignalIndex("Surface");
					bushContactIdx = charSoundComponent.GetSignalIndex("BushContact");
					bushHeightIdx = charSoundComponent.GetSignalIndex("BushHeight");
					//PrintFormat("%1 | %2", bushContactIdx, bushHeightIdx);
										
					SignalsManagerComponent signalManager = SignalsManagerComponent.Cast(playerEntity.FindComponent(SignalsManagerComponent));
					if (signalManager)
					{	
						float surfaceSignal = signalManager.GetSignalValue(surfaceIdx);
						float currentMovementSpeed = signalManager.GetSignalValue(speedIdx);
						ECharacterStance charStance = m_CharacterController.GetStance();
												
						if (currentMovementSpeed > 2)
						{			
							Physics playerPhysics = playerEntity.GetPhysics();
							if (playerPhysics)
							{
								//vector playerVelocity;
//								if (!m_bIsSliding && !m_bSlidingTriggered)
//								{
//									m_bSlidingTriggered = true;
//									m_SlidingVelocity = playerPhysics.GetVelocity();
//									GetGame().GetCallqueue().CallLater(SetIsSliding, 1000, false, false);									
//								}									
								
								vector localVelocity = playerEntity.VectorToLocal(m_SlidingVelocity);
								if (charStance == ECharacterStance.CROUCH && !m_CharacterController.IsUnconscious() && currentMovementSpeed > 4.5 && m_CharacterController.IsSprinting())
								{
									if (!m_bIsSliding && !m_bSlidingTriggered)
									{
										m_bSlidingTriggered = true;
										m_bIsSliding = true;
										m_SlidingVelocity = playerPhysics.GetVelocity();
										GetGame().GetCallqueue().CallLater(SetIsSliding, 1000, false, false);
									}
									
									SlidePlayer(surfaceSignal, 1, 1.0, localVelocity);
									if (!m_bIsPlayingAudio)
									{
										m_bIsPlayingAudio = true;
										PlaySound(surfaceSignal);
									}																
								}						
								else if (charStance == ECharacterStance.PRONE && !m_CharacterController.IsUnconscious() && !m_bIsDiving && m_CharacterController.IsSprinting())
								{								
									DivePlayer();
								}	
								
								if (charStance == ECharacterStance.PRONE && !m_CharacterController.IsUnconscious() && m_bIsDiving)
								{
									SlidePlayer(surfaceSignal, 1, 1.0, localVelocity);
								}		
							}																	
						}
						
						// Detect Strafe Spam and punish severely
						if (m_bStrafeSpamPenaltyEnabled)
						{
							if (currentMovementSpeed >= 2.0)
							{																					
							    if (playerEntity)
							    {
							        Physics playerPhysics = playerEntity.GetPhysics();
							        if (playerPhysics && !m_CharacterController.IsUnconscious())
							        {
							            vector currentVelocity = playerPhysics.GetVelocity();
							            vector localVelocity = playerEntity.VectorToLocal(currentVelocity);
							            float currStrafe = localVelocity[0]; // Local x is strafe (left/right)
	
							            // Check for direction change
							            bool isDirectionChange = false;
							            if ((m_fPreviousStrafe * currStrafe < 0) && (Math.AbsFloat(m_fPreviousStrafe) > STRAFE_THRESHOLD) && (Math.AbsFloat(currStrafe) > STRAFE_THRESHOLD))
							            {
							                isDirectionChange = true;
							            }
										
							            m_fPreviousStrafe = currStrafe;
							            if (isDirectionChange)
							            {
											//PrintFormat("IsDirectionChange: True");
							                m_aStrafeChangeTimes.Insert(GetGame().GetWorld().GetTimestamp()); // GetTime()); // Insert current time in seconds
							            }
							            
							            // Clean up old changes outside the window
							            while (m_aStrafeChangeTimes.Count() > 0 &&  GetGame().GetWorld().GetTimestamp().DiffSeconds(m_aStrafeChangeTimes[0]) > STRAFE_WINDOW)
							            {
											//PrintFormat("Removing Direction Changes...");
							                m_aStrafeChangeTimes.Remove(0);
							            }
							            
							            // Check if spamming (too many changes in window)
							            if (m_aStrafeChangeTimes.Count() > MAX_CHANGES)
							            {
							                if (m_CommunicationSound)
							                {	
							                    m_CommunicationSound.SoundEventHit(false, EDamageType.MELEE);									
							                }
							                
											SetStrafeSpamState(0.25, true);
											GetGame().GetCallqueue().CallLater(SetStrafeSpamState, 3000, false, 1.0, false);
							                m_aStrafeChangeTimes.Clear();
							            }
							        }
							    }
							}
						}
						
						if (m_bBushSlowingEnabled)
						{
							float bushSignal = signalManager.GetSignalValue(bushContactIdx);
							float bushHeightSignal = signalManager.GetSignalValue(bushHeightIdx);
		                    float speedMul;
	
		                    if (bushSignal <= 0.0)
		                    {
		                        speedMul = 1.0; // Full speed when no bush contact
		                    }
		                    else
		                    {
								//float clampedSignal = Math.Min(bushSignal, MAX_BUSH_SIGNAL);
								float clampedSignal = Math.Min(bushHeightSignal, MAX_BUSH_SIGNAL);
		                        float normalized = clampedSignal / MAX_BUSH_SIGNAL;
		                        speedMul = MIN_SPEED + (1.0 - normalized) * (1.0 - MIN_SPEED);	
		                    }
	
							if (speedMul < 1.0)
							{
								// Contact type 8 = grass? 3 = tree?
								if (bushSignal == 8 || bushSignal == 3)
								{
									speedMul = 0.65;
								}
								
								m_fBushSignalTime += timeSlice;
								if (m_fBushSignalTime > m_fBushSignalTimeTick)
								{
									m_fBushSignalTime = 0;
									m_fBushSignalTimeTick = Math.RandomIntInclusive(m_fBushSignalTimeTickMin, m_fBushSignalTimeTickMax);
									
									int randomInt = Math.RandomIntInclusive(1, 50);
									if (randomInt >= 20 && currentMovementSpeed > 1 && bushSignal != 8 && bushSignal != 3)
									{
										if (m_CommunicationSound)
										{	
											//m_CommunicationSound.SoundEventHit(true, EDamageType.MELEE);
											m_CommunicationSound.SoundEventDeath(false);
										}	
									}
								}
								
								//PrintFormat("Contact Signal: %1 | Height: %3 | Speed: %2", bushSignal, speedMul, bushHeightSignal);
							}
							
							if (!m_bIsStrafeSpamming)
	                    		m_CharacterController.OverrideMaxSpeed(speedMul);
						}												
					}
				}								
			}
		}
	}
	
	//----------------------------------------------------------------------------------
	void SetStrafeSpamState(float speed, bool value)
	{
		m_CharacterController.OverrideMaxSpeed(speed);
		m_bIsStrafeSpamming = value;
	}
	
	//----------------------------------------------------------------------------------
	void DivePlayer()
	{
		if (m_CharacterController)
		{			
			IEntity playerEntity = m_CharacterController.GetOwner();
			if (playerEntity)
			{
				Physics playerPhysics = playerEntity.GetPhysics();
				if (playerPhysics && !m_CharacterController.IsUnconscious())
				{
					vector velocity = playerPhysics.GetVelocity();
					int vel = 150;
				
					string playerSlip1 = string.ToString(velocity[0] * 30);
					string playerSlip2 = vel.ToString();
					string playerSlip3 = string.ToString(velocity[2] * 30);

					string str = string.Format("%1 %2 %3", playerSlip1, playerSlip2, playerSlip3);
					vector force = str.ToVector();
					
					//PrintFormat("Velocity: %1", velocity);
					m_bIsDiving = true;
					playerPhysics.ApplyImpulse(force);
					GetGame().GetCallqueue().CallLater(SetIsDiving, 1000, false, false);										
				}
			}
		}		
	}
	
	//----------------------------------------------------------------------------------
	void SlidePlayer(int surfaceSignal, int mult, float delta, vector velocity)
	{
		if (m_CharacterController)
		{			
			IEntity playerEntity = m_CharacterController.GetOwner();
			if (playerEntity)
			{
				Physics playerPhysics = playerEntity.GetPhysics();
				if (playerPhysics && !m_CharacterController.IsUnconscious())
				{
					//vector velocity = playerPhysics.GetVelocity();
					int vel = 100;
					if (mult == 1)
						vel = 30;

					if (delta > 0.80 && vel == 30)
					{
						vel = 20;
						//PrintFormat("Delta Triggered: %1", delta);
					}
					
					string playerSlip1 = string.ToString(velocity[0] * (100 * mult));
					string playerSlip2 = vel.ToString();
					string playerSlip3 = string.ToString(velocity[2] * (100 * mult));

					string str = string.Format("%1 %2 %3", playerSlip1, playerSlip2, playerSlip3);
					vector force = str.ToVector();
					
					//PrintFormat("Velocity: %1", velocity);					
					playerPhysics.ApplyTorque(force);
					m_bIsSliding = true;														
				}
			}
		}
	}

	//----------------------------------------------------------------------------------
	void PlaySound(int surfaceSignal)
	{
		string sound;
		if (surfaceSignal == 1500 || surfaceSignal == 1600 || surfaceSignal == 1200 ||
		surfaceSignal == 2600 || surfaceSignal == 2500 || surfaceSignal == 2710)
		{
			sound = "{1062051EE4B6A41D}Sounds/Character/Movement/Bodyfalls/Samples/Gravel/Bodyfall_Gravel_Slide_4.wav";
		}
		else if (surfaceSignal == 5100 || surfaceSignal == 7610 || surfaceSignal == 7620)
		{
			sound = "{C1854BEDE1902F07}Sounds/Weapons/Grenades/_SharedData/Samples/Impacts/Weapons_Grenade_RGD5_Hit_Wood_Hard_02.wav";
		}
		else if (surfaceSignal == 2100 || surfaceSignal == 2311 || surfaceSignal == 2800)
		{
			sound = "{690A1A43565F6928}Sounds/Character/Footsteps/Samples/Sprint/Wet_Muddy/Footstep_Sprint_Smear_Wet_Muddy_05.wav";
		}
		else if (surfaceSignal == 6100 || surfaceSignal == 6110)
		{
			sound = "{0FC495B6A9ABB88A}Sounds/Character/Movement/Bodyfalls/Samples/Metal/Bodyfall_Metal_Slide_1.wav";
		}
		else if (surfaceSignal == 3500)
		{
			sound = "{E80699ED5F634500}Sounds/Weapons/_SharedData/BulletHits/samples/Leaves_Dry_debris_02.wav";
		}
		
		if (m_bIsSliding)
		{
			sound = "{7B0A2957FE00BB48}Sounds/Character/Movement/Bodyfalls/Samples/Concrete/Bodyfall_Concrete_Slide_3_Long.wav";
		}
		
		AudioSystem.PlaySound(sound);
		GetGame().GetCallqueue().CallLater(SetIsPlayingAudio, 1000, false, false);
	}
	
	//----------------------------------------------------------------------------------
	bool GetIsDiving()
	{
		return m_bIsDiving;
	}
	
	//----------------------------------------------------------------------------------
	void SetIsDiving(bool value)
	{
		m_bIsDiving = value;
	}
	
	//----------------------------------------------------------------------------------
	void SetIsSliding(bool value)
	{
		m_bIsSliding = value;
		m_bSlidingTriggered = value;
	}
	
	//----------------------------------------------------------------------------------
	void SetIsPlayingAudio(bool value)
	{
		m_bIsPlayingAudio = value;
	}
}
