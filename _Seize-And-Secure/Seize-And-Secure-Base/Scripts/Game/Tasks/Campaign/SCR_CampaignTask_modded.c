//------------------------------------------------------------------------------------------------
//modded class SCR_CampaignTask : SCR_CampaignBaseTask
//{
//	
//	//------------------------------------------------------------------------------------------------
//	override void Finish(bool showMsg = true)
//	{
//		SCR_PersonalWalletManagerComponent walletManager = SCR_PersonalWalletManagerComponent.Cast(SCR_GameModeCampaign.GetInstance().FindComponent(SCR_PersonalWalletManagerComponent));
//
//		// Scoring system for Capture & Hold points
//		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
//		SCR_BaseScoringSystemComponent scoringSystem = gameMode.GetScoringSystemComponent();
//
//		
//		SCR_MilitaryBaseSystem system = SCR_MilitaryBaseSystem.GetInstance();
//		
//		if (system)
//			system.GetOnBaseFactionChanged().Remove(OnTargetBaseCaptured);
//
//		showMsg = SCR_FactionManager.SGetLocalPlayerFaction() == m_TargetFaction;
//		super.Finish(showMsg);
//		
//		SCR_XPHandlerComponent comp = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
//		
//		// Reward XP for seizing a base or reconfiguring a relay
//		if (comp && !GetTaskManager().IsProxy() && GetType() == SCR_CampaignTaskType.CAPTURE)
//		{
//			PlayerManager playerManager = GetGame().GetPlayerManager();
//			array<int> players = {};
//			playerManager.GetPlayers(players);
//			array<SCR_BaseTaskExecutor> assignees = {};
//			GetAssignees(assignees);
//			vector baseOrigin = m_TargetBase.GetOwner().GetOrigin();
//			int radius = m_TargetBase.GetRadius();
//			int radiusSq;
//			Faction playerFaction;
//			IEntity playerEntity;
//			bool isAssignee;
//			int assigneeID;
//			SCR_EXPRewards rewardID;
//			
//			if (m_TargetBase.GetType() == SCR_ECampaignBaseType.RELAY)
//				rewardID = SCR_EXPRewards.RELAY_RECONFIGURED;
//			else
//				rewardID = SCR_EXPRewards.BASE_SEIZED;
//			
//			if (m_TargetBase.GetType() == SCR_ECampaignBaseType.RELAY)
//				radiusSq = 50 * 50;
//			else
//				radiusSq = radius * radius;
//			
//			foreach (int playerId : players)
//			{
//				playerEntity = playerManager.GetPlayerControlledEntity(playerId);
//				
//				if (!playerEntity)
//					continue;
//				
//				playerFaction = SCR_CampaignReconfigureRelayUserAction.GetPlayerFaction(playerEntity);
//				
//				if (playerFaction != m_TargetFaction)
//					continue;
//				
//				if (vector.DistanceSq(playerEntity.GetOrigin(), baseOrigin) < radiusSq)
//				{
//					isAssignee = false;
//					
//					foreach (SCR_BaseTaskExecutor assignee : assignees)
//					{
//						assigneeID = SCR_BaseTaskExecutor.GetTaskExecutorID(assignee);
//						
//						if (assigneeID == playerId)
//						{
//							isAssignee = true;
//							break;
//						}
//					}
//					
//					float multiplier = 1.0;
//					
//					if (m_bIsPriority)
//						multiplier = 1.5;
//
//					comp.AwardXP(playerId, rewardID, multiplier, isAssignee);
//				}
//				
//				if (walletManager)
//				{
//					// Add supplies to personal wallet for each player based on rank
//					SCR_ECharacterRank userRank = SCR_CharacterRankComponent.GetCharacterRank(playerEntity);
//					int balanceToAdd = walletManager.GetBalanceToAdd();
//					float rankMultiplier = walletManager.GetWalletRankMultiplier(userRank);
//					
//					walletManager.AddWalletBalance(playerId, (balanceToAdd * rankMultiplier) * 1.5);
//				}
//		
//				// Add faction score for capturing a base...gives score multiplied by all players present! Find another way...
//				/* if (scoringSystem)
//				{
//					scoringSystem.AddFactionObjective(playerFaction, baseCaptureScore); 
//				} */
//				
//				
//				// Extra points for being near team member when doing it
//				if (comp.IsNearFactionMember(150, playerId))
//				{
//					GetGame().GetCallqueue().CallLater(comp.SeizeXPAward, 5000, false, playerId, SCR_EXPRewards.NEAR_SQUAD_MEMBER, 1.0);
//				}
//		
//				else
//				{
//					if (playerId != assigneeID && !isAssignee)
//					{
//						comp.AwardXP(playerId, SCR_EXPRewards.TEAM_CONTRIBUTION, 0.3);
//					}
//				}
//			}
//		}
//		
//		string baseName;
//		
//		if (m_TargetBase)
//			baseName = GetBaseNameWithCallsign();
//		
//		if (showMsg)
//		{
//			// TODO make this nicer
//			if (m_bIndividualTask)
//			{
//				if (IsAssignedToLocalPlayer())
//				{
//					if (DoneByAssignee())
//					{
//						SCR_PopUpNotification.GetInstance().PopupMsg(TASK_COMPLETED_TEXT + " " + GetTitle(), prio: SCR_ECampaignPopupPriority.TASK_DONE, param1: baseName, sound: SCR_SoundEvent.TASK_SUCCEED, text2: SCR_BaseTask.TASK_HINT_TEXT, text2param1: SCR_PopUpNotification.TASKS_KEY_IMAGE_FORMAT);
//					}
//					else
//					{
//						SCR_PopUpNotification.GetInstance().PopupMsg(TASK_FAILED_TEXT + " " + GetTitle(), prio: SCR_ECampaignPopupPriority.TASK_DONE, param1: baseName, sound: SCR_SoundEvent.TASK_FAILED);
//					}
//				}
//				else
//				{
//					if (m_aAssignees && m_aAssignees.Count() > 0)
//					{
//						string text;
//						string par1;
//						string par2;
//						string par3;
//						string par4;
//						string par5;
//						GetFinishTextData(text, par1, par2, par3, par4, par5);
//						SCR_PopUpNotification.GetInstance().PopupMsg(text, prio: SCR_ECampaignPopupPriority.TASK_DONE, param1: par1, param2: par2, param3: par3, param4: par4, sound: SCR_SoundEvent.TASK_SUCCEED);
//					}
//					else
//						SCR_PopUpNotification.GetInstance().PopupMsg(TASK_COMPLETED_TEXT + " " + GetTitle(), prio: SCR_ECampaignPopupPriority.TASK_DONE, param1: baseName, sound: SCR_SoundEvent.TASK_SUCCEED, text2: SCR_BaseTask.TASK_HINT_TEXT, text2param1: SCR_PopUpNotification.TASKS_KEY_IMAGE_FORMAT);
//				}
//			}
//			else
//				SCR_PopUpNotification.GetInstance().PopupMsg(TASK_COMPLETED_TEXT + " " + GetTitle(), prio: SCR_ECampaignPopupPriority.TASK_DONE, param1: baseName, sound: SCR_SoundEvent.TASK_SUCCEED, text2: SCR_BaseTask.TASK_HINT_TEXT, text2param1: SCR_PopUpNotification.TASKS_KEY_IMAGE_FORMAT);
//		}
//	}
//	
//};
