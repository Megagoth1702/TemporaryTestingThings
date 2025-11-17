[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "Handles applying persistent injuries to player")]
class SCR_PersistentInjuryManagerComponentClass : SCR_BaseGameModeComponentClass
{
}

class SCR_PersistentInjuryManagerComponent : SCR_BaseGameModeComponent
{
	// Attached to gamemode authority
	[Attribute("0", desc: "Allow bleeding outside of Main base", category: "General")]
	protected bool m_bAllowBleeding;
	
	[Attribute("1", desc: "Allow injuries at Main base", category: "General")]
	protected bool m_bExcludeInjuriesFromMainBase;
	
	[Attribute("1", desc: "Allow injuries at other bases", category: "General")]
	protected bool m_bExcludeInjuriesFromBasesWithFieldHospital;
	
	[Attribute("WARNING: INJURIES SUSTAINED", UIWidgets.EditBox, "The string that is shown as the main message to the local player upon spawning into a location while injured.")]
	protected LocalizedString m_sLocalPlayerAlertTitle;

	[Attribute("You have been recovered from the battlefield and your injuries have been stabilized, but you may still need medical attention!", UIWidgets.EditBox, "The additional text that is shown as the main message to the local player upon spawning into a location while injured.")]
	protected LocalizedString m_sLocalPlayerAlertText;
	
	[Attribute("20", UIWidgets.EditBox, "The duration the message should show.")]
	protected int m_iDuration;
	
	protected ref map<int, ref map<string, float>> playerInjuries = new map<int, ref map<string, float>>();

	//------------------------------------------------------------------------------------------------
    bool GetAllowBleeding() 
	{
		return m_bAllowBleeding;
	}
	
	//------------------------------------------------------------------------------------------------
    void SetAllowBleeding(bool value) 
	{
		m_bAllowBleeding = value;
	}
	
	//------------------------------------------------------------------------------------------------
    bool GetExcludeInjuriesFromMainBase() 
	{
		return m_bExcludeInjuriesFromMainBase;
	}
	
	//------------------------------------------------------------------------------------------------
    void SetExcludeInjuriesFromMainBase(bool value) 
	{
		m_bExcludeInjuriesFromMainBase = value;
	}
	
	//------------------------------------------------------------------------------------------------
    bool GetExcludeInjuriesFromBases() 
	{
		return m_bExcludeInjuriesFromBasesWithFieldHospital;
	}
	
	//------------------------------------------------------------------------------------------------
    void SetExcludeInjuriesFromBases(bool value) 
	{
		m_bExcludeInjuriesFromBasesWithFieldHospital = value;
	}
	
	//------------------------------------------------------------------------------------------------
    void SaveInjuryState(int playerId, IEntity playerEntity) 
	{
		if (!playerId)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = GetDamageManagerComponent(playerEntity);
		if (!damageManager)
			return;
		
		if (!playerInjuries.Contains(playerId)) 
		{
	        playerInjuries[playerId] = new map<string, float>();
	    }
		
		else 
		{
			// Clear previous injury data
	        playerInjuries[playerId].Clear(); 
	    }
		
		// Access the player's injury map
    	map<string, float> injuries = playerInjuries[playerId];

       	array<HitZone> hitZones = {};
       	damageManager.GetAllHitZones(hitZones);
			
		// Clear list so its always fresh for Death Info Screen
		ClearPlayerInjuryList(playerId);
		
	    foreach (HitZone hitZone : hitZones) 
		{
		    if (hitZone && hitZone.GetDamageState() != EDamageState.UNDAMAGED) 
			{
			    injuries[hitZone.GetName()] = hitZone.GetHealth();
			    Print("Saving HitZone: " + hitZone.GetName() + ", Health: " + hitZone.GetHealth() + ", Max Health: " + hitZone.GetMaxHealth());
				
				// Save injury list for death info screen
				if (hitZone.GetName() != "Health" && hitZone.GetName() != "Resilience")
				{
					SetPlayerInjuryList(playerId, hitZone.GetName());
				}				
			}
	    }
    }
	
	//------------------------------------------------------------------------------------------------ Original
//    void SaveInjuryState(int playerId, SCR_CharacterDamageManagerComponent damageManager) 
//	{
//		if (!playerId || !damageManager)
//			return;
//		
//		if (!playerInjuries.Contains(playerId)) 
//		{
//	        playerInjuries[playerId] = new map<string, float>();
//	    }
//		
//		else 
//		{
//			// Clear previous injury data
//	        playerInjuries[playerId].Clear(); 
//	    }
//		
//		// Access the player's injury map
//    	map<string, float> injuries = playerInjuries[playerId];
//
//       array<HitZone> hitZones = {};
//       damageManager.GetAllHitZones(hitZones);
//			
//	    foreach (HitZone hitZone : hitZones) 
//		{
//		    if (hitZone && hitZone.GetDamageState() != EDamageState.UNDAMAGED) 
//			{
//			    injuries[hitZone.GetName()] = hitZone.GetHealth();
//			    Print("Saving HitZone: " + hitZone.GetName() + ", Health: " + hitZone.GetHealth() + ", Max Health: " + hitZone.GetMaxHealth());
//			}
//	    }
//    }
	
	//------------------------------------------------------------------------------------------------ v2
	void ApplyInjuryState(int playerId, IEntity playerEntity) 
	{
		if (!playerInjuries.Contains(playerId)) 
			return;

		if (GetExcludeInjuriesFromMainBase() && IsNearMainBase(playerId) || GetExcludeInjuriesFromBases() && IsNearFriendlyBase(playerId))
		{
			return;
		}
		
		SCR_CharacterDamageManagerComponent damageManager = GetDamageManagerComponent(playerEntity);
		if (!damageManager)
			return;
		
		map<string, float> injuries = playerInjuries[playerId];
		
		if (injuries.Count() == 0)
		{
			return;
		}
			
/*		SCR_PopUpNotification popupNotifications = SCR_PopUpNotification.GetInstance();
		if (popupNotifications)
		{
			popupNotifications.PopupMsg(m_sLocalPlayerAlertTitle, m_iDuration, m_sLocalPlayerAlertText, 1000);
		} */
		
		Rpc(PushMessage, m_sLocalPlayerAlertTitle, m_sLocalPlayerAlertText, playerId, m_iDuration);
		
		Print("Applying Injury State for Player ID: " + playerId + "...");
		
	    float minHealthValue, maxHealthValue, setHealthValue;
	
	    foreach (string hitZoneName, float healthValue : injuries) 
		{
	        HitZone hitZone = damageManager.GetHitZoneByName(hitZoneName);
	        SCR_CharacterHitZone damagedHitZone = SCR_CharacterHitZone.Cast(hitZone);
	        string hitZoneIdentifier = hitZone.GetName(); // Store the hit zone's name once
	
			maxHealthValue = hitZone.GetMaxHealth();
			minHealthValue = healthValue/maxHealthValue;
			
	        // Set health limits based on the hit zone type
			if (injuries.Count() <= 1 && damageManager.GetHitZoneByName("Health"))
			{
				Print("Injury Count Low: Probably Suicide. Set Head Injury");
				HitZone head = damageManager.GetHitZoneByName("Head");
				head.SetHealthScaled(0.5);
			}
			
	        if (hitZoneIdentifier == "Blood" || hitZoneIdentifier == "Health") 
			{
				setHealthValue = Math.Clamp(minHealthValue, 0.4, 0.70);
				hitZone.SetHealthScaled(setHealthValue);
	        } 
			
			else 
			{
	            setHealthValue = Math.Clamp(minHealthValue, 0.05, 1); // 0.5
				hitZone.SetHealthScaled(setHealthValue);
	        }
	
#ifdef WORKBENCH
	        Print("Setting: " + hitZoneIdentifier + " Health: " + setHealthValue + " || Max Health: " + maxHealthValue);
#endif
	        
	        if (damagedHitZone) 
			{
	            damagedHitZone.SetHealthScaled(setHealthValue);
	            damageManager.AddBloodToClothes(damagedHitZone, Math.Clamp(damagedHitZone.GetMaxHealth(), 0, 255));
	
				float threshold = damagedHitZone.GetMaxHealth() * 0.40;
				
				if (GetAllowBleeding())
				{
					if (!IsNearMainBase(playerId) && damagedHitZone.GetHealth() < threshold) 
					{
		                damageManager.AddBleedingEffectOnHitZone(damagedHitZone);
		            }
		
		            // Ensure enough blood for bandaging if bleeding
		            if ((hitZoneIdentifier == "Head" || hitZoneIdentifier == "Neck" || hitZoneIdentifier == "Chest" || hitZoneIdentifier == "Abdomen") && damageManager.IsBleeding()) 
					{
		                HitZone playerBlood = damageManager.GetHitZoneByName("Blood");
						float bloodThreshold = playerBlood.GetMaxHealth() * 0.5;
						
						if (playerBlood.GetHealth() < bloodThreshold)
		                	playerBlood.SetHealthScaled(0.75);
		            }
				}
	            
	        }
	    }
	
	    // Set resilience to 80% so player doesn't go unconscious after spawning in
	    HitZone playerResilience = damageManager.GetHitZoneByName("Resilience");
	    playerResilience.SetHealthScaled(0.8);
	}
	
	//------------------------------------------------------------------------------------------------ Original
//	void ApplyInjuryState(int playerId, SCR_CharacterDamageManagerComponent damageManager) 
//	{
//		if (!playerInjuries.Contains(playerId)) 
//			return;
//		
//		map<string, float> injuries = playerInjuries[playerId];
//		
//	    if (injuries.Count() == 0) 
//			return;
//	
//		Print("Applying Injury State...");
//		
//	    float minHealthValue, maxHealthValue, setHealthValue;
//	
//	    foreach (string hitZoneName, float healthValue : injuries) 
//		{
//	        HitZone hitZone = damageManager.GetHitZoneByName(hitZoneName);
//	        SCR_CharacterHitZone damagedHitZone = SCR_CharacterHitZone.Cast(hitZone);
//	        string hitZoneIdentifier = hitZone.GetName(); // Store the hit zone's name once
//	
//			maxHealthValue = hitZone.GetMaxHealth();
//			minHealthValue = healthValue/maxHealthValue;
//			
//	        // Set health limits based on the hit zone type
//	        if (hitZoneIdentifier == "Blood" || hitZoneIdentifier == "Health") 
//			{
//				setHealthValue = Math.Clamp(minHealthValue, 0.4, 0.70);
//				hitZone.SetHealthScaled(setHealthValue);
//	        } 
//			
//			else 
//			{
//	            setHealthValue = Math.Clamp(minHealthValue, 0.05, 1); // 0.5
//				hitZone.SetHealthScaled(setHealthValue);
//	        }
//	
//#ifdef WORKBENCH
//	        Print("Setting: " + hitZoneIdentifier + " Health: " + setHealthValue + " || Max Health: " + maxHealthValue);
//#endif
//	        
//	        if (damagedHitZone) 
//			{
//	            damagedHitZone.SetHealthScaled(setHealthValue);
//	            damageManager.AddBloodToClothes(damagedHitZone, Math.Clamp(damagedHitZone.GetMaxHealth(), 0, 255));
//	
//				float threshold = damagedHitZone.GetMaxHealth() * 0.65;
//				
//	            if (!IsNearMainBase(playerId) && damagedHitZone.GetHealth() < threshold) 
//				{
//	                damageManager.AddBleedingEffectOnHitZone(damagedHitZone);
//	            }
//	
//	            // Ensure enough blood for bandaging if bleeding
//	            if ((hitZoneIdentifier == "Head" || hitZoneIdentifier == "Neck" || hitZoneIdentifier == "Chest" || hitZoneIdentifier == "Abdomen") && damageManager.IsBleeding()) 
//				{
//	                HitZone playerBlood = damageManager.GetHitZoneByName("Blood");
//					float bloodThreshold = playerBlood.GetMaxHealth() * 0.5;
//					
//					if (playerBlood.GetHealth() < bloodThreshold)
//	                	playerBlood.SetHealthScaled(0.75);
//	            }
//	        }
//	    }
//	
//	    // Set resilience to 80% so player doesn't go unconscious after spawning in
//	    HitZone playerResilience = damageManager.GetHitZoneByName("Resilience");
//	    playerResilience.SetHealthScaled(0.8);
//	}

	//------------------------------------------------------------------------------------------------
	bool IsNearMainBase(int playerId)
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		SCR_CampaignFaction playerFaction = SCR_CampaignFaction.Cast(factionManager.GetPlayerFaction(playerId));
		
		if (!playerFaction)
			return false;
		
		SCR_CampaignMilitaryBaseComponent baseHQ = playerFaction.GetMainBase();
		if (!baseHQ)
			return false;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return false;
		
		IEntity player = playerController.GetControlledEntity();
		if (!player)
			return false;
		
		vector playerLocation = player.GetOrigin();
		vector HQDistance = baseHQ.GetOwner().GetOrigin();
		float distance = vector.DistanceXZ(HQDistance,playerLocation);
		
#ifdef WORKBENCH
		Print("Player ID: " + playerId + " Distance From Main Base: " + distance);
#endif
		return distance <= 150;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsNearFriendlyBase(int playerId)
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		SCR_CampaignFaction playerFaction = SCR_CampaignFaction.Cast(factionManager.GetPlayerFaction(playerId));
		
		if (!playerFaction)
			return false;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return false;
		
		IEntity player = playerController.GetControlledEntity();
		if (!player)
			return false;

		SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
	    if (!baseManager || !CampaignInfo)
		{
			return false;
		}
		
		array<SCR_CampaignMilitaryBaseComponent> campaignBases = {};
	    array<SCR_MilitaryBaseComponent> bases = {};
	    baseManager.GetBases(bases);
	
		vector playerLocation = player.GetOrigin();
		
		SCR_CampaignMilitaryBaseComponent closestBase = CampaignInfo.GetBaseManager().FindClosestBase(playerLocation);
		vector basePosition = closestBase.GetOwner().GetOrigin();
		float distance = vector.DistanceXZ(playerLocation, basePosition);
		SCR_ServicePointDelegateComponent hospitalService = closestBase.GetServiceDelegateByType(SCR_EServicePointType.FIELD_HOSPITAL);
//		bool serviceStatus = hospitalService.IsActive();

		if (playerFaction == closestBase.GetFaction() && distance <= 100 && hospitalService && hospitalService.IsActive()) //serviceStatus) // GetHasFieldHospital())
		{
#ifdef WORKBENCH
			Print("IsNearFriendlyBase: Has Field Hospital...returning TRUE");
#endif
			return true;
		}

#ifdef WORKBENCH
		Print("Player ID: " + playerId + " Distance From Friendly Base: " + distance);
#endif
		return false;
	}
	
	//------------------------------------------------------------------------------------------------ v2
	override void OnPlayerKilled(notnull SCR_InstigatorContextData instigatorContextData)
	{
		int playerId = instigatorContextData.GetVictimPlayerID();
		Print("Player ID: " + playerId + " Killed...");
		SaveInjuryState(playerId, instigatorContextData.GetVictimEntity());
	}
	
	//------------------------------------------------------------------------------------------------ Original
//	override void OnPlayerKilled(notnull SCR_InstigatorContextData instigatorContextData)
//	{
//		int playerId = instigatorContextData.GetVictimPlayerID();
//		Print("Player ID: " + playerId + " Killed...");
//		ChimeraCharacter character = ChimeraCharacter.Cast(instigatorContextData.GetVictimEntity());
//		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
//				
//		SaveInjuryState(playerId, damageManager);
//	}
	
	//------------------------------------------------------------------------------------------------ v2
	override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(entity);	
	    ApplyInjuryState(playerId, entity);
	}
	
	//------------------------------------------------------------------------------------------------ Original
//	override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
//	{
//		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(entity);
//		ChimeraCharacter character = ChimeraCharacter.Cast(entity);
//		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
//				
//	    ApplyInjuryState(playerId, damageManager);
//	}

	//------------------------------------------------------------------------------------------------
	SCR_CharacterDamageManagerComponent GetDamageManagerComponent(IEntity playerEntity)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(playerEntity);
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
		
		return damageManager;
	}
	
	//------------------------------------------------------------------------------------------------	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void PushMessage(string messageTitle, string messageText, int playerId, int duration)
	{	
		int playerCheck = SCR_PlayerController.GetLocalPlayerId();
		if (playerId != playerCheck)
			return;
		
		SCR_PopUpNotification.GetInstance().PopupMsg(messageTitle, duration, messageText, prio: 1000);
	}
	
	//------------------------------------------------------------------------------------------------
	void ClearPlayerInjuryList(int playerId)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		SCR_Iron_InjuryManagerComponent injuryComponent = SCR_Iron_InjuryManagerComponent.Cast(playerController.FindComponent(SCR_Iron_InjuryManagerComponent));
		
		if (injuryComponent)
		{
			injuryComponent.ClearPlayerInjuryList();	
		}			
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPlayerInjuryList(int playerId, string injury)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		SCR_Iron_InjuryManagerComponent injuryComponent = SCR_Iron_InjuryManagerComponent.Cast(playerController.FindComponent(SCR_Iron_InjuryManagerComponent));
		
		if (injuryComponent)
		{
			injuryComponent.SetPlayerInjuryList(injury);
		}
		
	}

}