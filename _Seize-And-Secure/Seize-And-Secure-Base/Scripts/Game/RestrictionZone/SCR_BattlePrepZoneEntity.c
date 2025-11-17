[EntityEditorProps(category: "GameScripted/Misc", description: "")]
class SCR_BattlePrepZoneEntityClass : SCR_EditorRestrictionZoneEntityClass
{
}

class SCR_BattlePrepZoneEntity : SCR_EditorRestrictionZoneEntity
{
//	[Attribute("90", UIWidgets.Slider, "Teams can not leave their main base area for this duration (seconds)", "1 300 1")] // This info should probably be in game mode so its only adjusted in one place for all teams.
	protected float BATTLE_PREP_TIME_S;
	
	protected bool m_bZoneRemoved;
	protected bool m_bIsServer;
	protected float m_fTimerForRemoval;
	
	SCR_GameModeCampaign CampaignInfo;
	
	//------------------------------------------------------------------------------------------------
	
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
//		if (m_bIsServer)
//		{
//			m_fTimerForRemoval += timeSlice;
//			
//			if(m_fTimerForRemoval > BATTLE_PREP_TIME_S * 1000 && m_bZoneRemoved == 0)
//			{
//				m_bZoneRemoved = 1;
//				m_fTimerForRemoval = 0;
//				BaseGameMode gamemode = GetGame().GetGameMode();
//				SCR_PlayersRestrictionZoneManagerComponent restrictionZoneManager = SCR_PlayersRestrictionZoneManagerComponent.Cast(gamemode.FindComponent(SCR_PlayersRestrictionZoneManagerComponent));
//				restrictionZoneManager.RemoveRestrictionZone(this);
//			}
//		}
	}


	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		if (!Replication.IsServer() || !GetGame().InPlayMode())
			return;
		
		m_bIsServer = Replication.IsServer();
		
		if (m_bIsServer)		
		{	
//			CampaignInfo = SCR_GameModeCampaign.GetInstance();
////			float BATTLE_PREP_TIME_S = CampaignInfo.GetBattlePrepTime();
//			BATTLE_PREP_TIME_S = CampaignInfo.GetBattlePrepTime();	
//			
////			int delay = BATTLE_PREP_TIME_S * 1000;
//			float delay = BATTLE_PREP_TIME_S * 1000;			
//			GetGame().GetCallqueue().CallLater(RemoveBattlePrepZone, delay, false);
			
			CampaignInfo = SCR_GameModeCampaign.GetInstance();
			CampaignInfo.GetBattlePrepEndedInvoker().Insert(RemoveBattlePrepZone);
		}
	
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void WarmupMessage(string message, int time)
	{	
		SCR_PopUpNotification popup = SCR_PopUpNotification.GetInstance();
		popup.PopupMsg(message, prio: SCR_ECampaignPopupPriority.RESPAWN, duration: time);
		CampaignInfo = SCR_GameModeCampaign.GetInstance();
		bool playCS = CampaignInfo.PlayClassicCS();
		if (playCS)
		{
			AudioSystem.PlaySound("{AAA7FC4C60D16496}Sounds/RadioProtocol/Samples/Eng/Male1/Movement/Move/CP_MOVE_ENGAGE_CONTACTS_S3_P1_V1.wav");
		}

	}
	
	//------------------------------------------------------------------------------------------------
	void RpcWrapper(string message, int time)
	{
		Rpc(WarmupMessage, message, time);
	}
	
	//------------------------------------------------------------------------------------------------
	void RemoveBattlePrepZone()
	{	
		BaseGameMode gamemode = GetGame().GetGameMode();
		SCR_PlayersRestrictionZoneManagerComponent restrictionZoneManager = SCR_PlayersRestrictionZoneManagerComponent.Cast(gamemode.FindComponent(SCR_PlayersRestrictionZoneManagerComponent));
		CampaignInfo = SCR_GameModeCampaign.GetInstance();
		bool showMessages = CampaignInfo.ShowBattlePrepTimer();
		if (showMessages)
		{		
			Rpc(WarmupMessage,"BATTLE PREP ENDED: GO, GO, GO!",10);
		}

		restrictionZoneManager.RemoveRestrictionZone(this);
		PrintFormat("Battle-Prep Zone Removed: %1", restrictionZoneManager);
	}	
	
	//------------------------------------------------------------------------------------------------
	WorldTimestamp ServerStartTime()
	{
		CampaignInfo = SCR_GameModeCampaign.GetInstance();
		return CampaignInfo.GetStartedTimestamp();
	}

	//------------------------------------------------------------------------------------------------
//	void SCR_BattlePrepZoneEntity(IEntitySource src, IEntity parent)
//	{
//		if (!Replication.IsServer() || SCR_Global.IsEditMode(this))
//			return;
//		
//		SetEventMask(EntityEvent.INIT | EntityEvent.FRAME);
//	}

	//------------------------------------------------------------------------------------------------
//	void ~SCR_BattlePrepZoneEntity()
//	{	
//		if (!Replication.IsServer() || SCR_Global.IsEditMode(this))
//			return;
//		
//		BaseGameMode gamemode = GetGame().GetGameMode();
//		
//		if (!gamemode)
//			return;
//		
//		SCR_PlayersRestrictionZoneManagerComponent restrictionZoneManager = SCR_PlayersRestrictionZoneManagerComponent.Cast(gamemode.FindComponent(SCR_PlayersRestrictionZoneManagerComponent));
//		
//		if (!restrictionZoneManager)
//			return;
//		
//		restrictionZoneManager.RemoveRestrictionZone(this);
//	}
	/*
	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		SetEventMask(owner, EntityEvent.FRAME);
	}
 
	override void EOnDeactivate(IEntity owner)
	{
		super.EOnDeactivate(owner);
		ClearEventMask(owner, EntityEvent.FRAME);
	}
	*/
}
