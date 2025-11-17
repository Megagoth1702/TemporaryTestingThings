class Iron_SetSquadFrequencyUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		if (playerId && IsSquadLeader(playerId))
		{
			return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
//		BaseRadioComponent radioComp = m_RadioComp.GetRadioComponent();
//		BaseTransceiver tranceiver = radioComp.GetTransceiver(0);
//		
//		int frequency = tranceiver.GetFrequency(); //transceiver.GetFrequency() + transceiver.GetFrequencyResolution()
//		SetGroupFrequency(playerId, frequency);
//		
//		PrintFormat("Iron_SetSquadFrequencyUserAction: Radio Tuned. Frequency: %1 | Player ID: %2", frequency, playerId);
		return true;
 	}
	
 	//------------------------------------------------------------------------------------------------
 	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
 	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		int frequency = GetRadioFrequency(playerId, pUserEntity);
		SetGroupFrequency(playerId, frequency);
 	}
	
	//------------------------------------------------------------------------------------------------
	void SetGroupFrequency(int playerId, int frequency)
	{
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		SCR_PlayerControllerGroupComponent playerControllerGroupComp = SCR_PlayerControllerGroupComponent.GetPlayerControllerComponent(playerId);
		if (groupsManager && playerControllerGroupComp)
		{		
			int groupId = playerControllerGroupComp.GetGroupID();
			SCR_AIGroup group = groupsManager.FindGroup(groupId);
			if (group)
			{
				group.SetSLGroupFrequency(frequency);				
				array<int>players = group.GetPlayerIDs();
				foreach(int id : players)
				{
					IEntity playerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(id);
					TunePlayersRadio(id, playerEntity, frequency);
				}
			}
		}				
	}
	
	//------------------------------------------------------------------------------------------------
	int GetRadioFrequency(int playerId, IEntity player)
	{
		PlayerController controller = GetGame().GetPlayerManager().GetPlayerController(playerId);
		if (!controller)
			return 0;
		
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(player);
		if (!gadgetManager)
			return 0;
		
		IEntity radio = gadgetManager.GetGadgetByType(EGadgetType.RADIO);
		if (!radio)
			return 0;
		
		BaseRadioComponent radioComponent = BaseRadioComponent.Cast(radio.FindComponent(BaseRadioComponent));
		if (!radioComponent)
			return 0;

		SCR_PlayerControllerGroupComponent groupComponent = SCR_PlayerControllerGroupComponent.Cast(controller.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupComponent)
			return 0;
		
		BaseTransceiver transceiver = radioComponent.GetTransceiver(0);
		if (!transceiver)
			return 0;
		
		return transceiver.GetFrequency();
	}		
	
	//------------------------------------------------------------------------------------------------
	void TunePlayersRadio(int playerId, IEntity player, int frequency)
	{
		PlayerController controller = GetGame().GetPlayerManager().GetPlayerController(playerId);
		if (!controller)
			return;
		
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(player);
		if (!gadgetManager)
			return;
		
		IEntity radio = gadgetManager.GetGadgetByType(EGadgetType.RADIO);
		if (!radio)
			return;
		
		BaseRadioComponent radioComponent = BaseRadioComponent.Cast(radio.FindComponent(BaseRadioComponent));
		if (!radioComponent)
			return;

		SCR_PlayerControllerGroupComponent groupComponent = SCR_PlayerControllerGroupComponent.Cast(controller.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupComponent)
			return;
		
		BaseTransceiver transceiver = radioComponent.GetTransceiver(0);
		if (!transceiver)
			return;
		
		transceiver.SetFrequency(frequency);
	}		
	
	//------------------------------------------------------------------------------------------------
	bool IsSquadLeader(int playerId)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId)); //GetPlayerController();
		if (!playerController)
		{
			Print("Iron_SetSquadFrequencyUserAction: IsPlayerLeader: No Player Controller for Player ID: " + playerId);
			return false;
		}
			
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupController)
		{
			Print("Iron_SetSquadFrequencyUserAction: IsPlayerLeader: No Group Controller for Player ID: " + playerId);
			return false;
		}
					
		SCR_AIGroup group = groupController.GetPlayersGroup(); // SCR_PlayerControllerGroupComponent.GetPlayerControllerComponent(playerId).GetPlayersGroup();
		if (!group)
		{
			Print("Iron_SetSquadFrequencyUserAction: IsPlayerLeader: No Group for Player ID: " + playerId);
			return false;
		}
		
//		Print("IsSquadLeader: Player Id: " + playerId + " - " + group.IsPlayerLeader(playerId));
		return group.IsPlayerLeader(playerId);
	}
}