class Iron_ViewBackpackInventoryUserAction : SCR_ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript() { return false; };
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript() { return true; };
	
	//------------------------------------------------------------------------------------------------
    // Check if the action can be performed
    override bool CanBePerformedScript(IEntity user)
    {
        if (!user)
		{
			PrintFormat("Iron_ViewBackpackInventoryUserAction: No User");
			return false;
		}            
         
        // Ensure target is a player and alive
        SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
        if (!charController || charController.IsUnconscious() || charController.GetLifeState() != ECharacterLifeState.ALIVE)
		{
			PrintFormat("Iron_ViewBackpackInventoryUserAction: charController Error");
			return false;
		}            

        return true;
    }
    
	//------------------------------------------------------------------------------------------------
    // Perform the action
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        // Get the inventory storage component of the target
        SCR_UniversalInventoryStorageComponent targetStorage = SCR_UniversalInventoryStorageComponent.Cast(pOwnerEntity.FindComponent(SCR_UniversalInventoryStorageComponent));
        if (!targetStorage)
		{
			PrintFormat("Iron_ViewBackpackInventoryUserAction: No Target Storage");
			return;
		}

        // Get the player's inventory UI manager
//        SCR_InventoryMenuUI inventoryMenu = SCR_InventoryMenuUI.GetInventoryMenu(); //SCR_InventoryMenuUI.Cast(GetGame().GetMenuManager().OpenMenu(SCR_InventoryMenuUI));
//        if (inventoryMenu)
//        {
//            // Open the target's inventory
//			inventoryMenu.GetOpenedStorage(targetStorage);
//        }
		
		SCR_InventoryStorageManagerComponent manager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (manager)
		{
			manager.SetStorageToOpen(pOwnerEntity);
			manager.OpenInventory();
			PrintFormat("Iron_ViewBackpackInventoryUserAction: Opened!");
		}
		else
		{
			PrintFormat("Iron_ViewBackpackInventoryUserAction: No Manager");
		}
    }
}

