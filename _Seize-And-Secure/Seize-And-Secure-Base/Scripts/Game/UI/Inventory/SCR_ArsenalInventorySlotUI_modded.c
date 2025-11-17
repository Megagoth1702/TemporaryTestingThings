//------------------------------------------------------------------------------------------------
modded class SCR_ArsenalInventorySlotUI : SCR_InventorySlotUI
{
	protected ref array<ref LoadoutType> m_eRequiredRoles = {};
	protected ref array<ref LoadoutType> m_eExcludedRoles = {};
	protected ref array<ref LoadoutType> m_ePriviledgedRoles = {};
	protected bool m_bIgnoreRankIfRole;
	
	//------------------------------------------------------------------------------------------------
	override protected void SetItemRank()
	{
		m_eRequiredRank = -1;
		
		if (!m_pItem || !m_pItem.GetOwner())
			return;
		
		SCR_ArsenalManagerComponent arsenalManager;
		if (!SCR_ArsenalManagerComponent.GetArsenalManager(arsenalManager) || !arsenalManager.AreItemsRankLocked())
			return;
		
		//~ TODO: This should be merged with total Resources to only update once!!!
		SCR_EntityCatalogManagerComponent entityCatalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		if (!entityCatalogManager)
			return;
		
		IEntity storageEnt = GetStorageUI().GetCurrentNavigationStorage().GetOwner();
		SCR_ArsenalComponent arsenalComponent = SCR_ArsenalComponent.Cast(storageEnt.FindComponent(SCR_ArsenalComponent));
		
		SCR_Faction faction;
		if (arsenalComponent)
			faction = arsenalComponent.GetAssignedFaction();
		
		Resource resource = Resource.Load(m_pItem.GetOwner().GetPrefabData().GetPrefabName());
		if (!resource.IsValid())
			return;
		
		SCR_EntityCatalogEntry entry;
		if (faction)
			entry = entityCatalogManager.GetEntryWithPrefabFromFactionCatalog(EEntityCatalogType.ITEM, resource.GetResource().GetResourceName(), faction);
		else 
			entry = entityCatalogManager.GetEntryWithPrefabFromAnyCatalog(EEntityCatalogType.ITEM, resource.GetResource().GetResourceName());
		
		if (!entry)
			return;
		
		SCR_ArsenalItem data = SCR_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
		if (!data)
			return;
		
		m_eRequiredRank = data.GetRequiredRank();
		m_eRequiredRoles = data.GetRequiredRoles();
		m_eExcludedRoles = data.GetExcludedRoles();
		m_ePriviledgedRoles = data.GetPriviledgedRoles();
		m_bIgnoreRankIfRole = data.GetIgnoreRankIfRole();
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref LoadoutType> GetRequiredRoles()
	{
		return m_eRequiredRoles;
	}
	
	//------------------------------------------------------------------------------------------------
	LoadoutType GetLoadoutType(IEntity owner)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(owner);
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		SCR_PlayerLoadoutComponent m_PlyLoadoutComp = SCR_PlayerLoadoutComponent.Cast(pc.FindComponent(SCR_PlayerLoadoutComponent));
		SCR_BasePlayerLoadout loadout = m_PlyLoadoutComp.GetLoadout();
		SCR_FactionPlayerLoadout factionLoadout = SCR_FactionPlayerLoadout.Cast(loadout);
		LoadoutType loadoutType = factionLoadout.GetLoadoutType(loadout);
		return loadoutType;
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasCorrectLoadoutType()
	{
//		IEntity localPlayerEntity = SCR_PlayerController.GetLocalControlledEntity();
//		LoadoutType loadoutType = GetLoadoutType(localPlayerEntity);
//		if (!loadoutType)
//		{
//			return true;
//		}
//		
//		if (m_eExcludedRoles.Contains(loadoutType))
//		{
//			return false;
//		}
//		
//		if (m_eRequiredRoles.IsEmpty() || loadoutType == LoadoutType.DEFAULT || loadoutType == LoadoutType.SAVED || m_eRequiredRoles.Contains(loadoutType) || m_ePriviledgedRoles.Contains(loadoutType))
//		{
//			return true;
//		}
//
//		return false;
		
		IEntity localPlayerEntity = SCR_PlayerController.GetLocalMainEntity(); // SCR_PlayerController.GetLocalControlledEntity();
	    LoadoutType loadoutType = GetLoadoutType(localPlayerEntity);
	    
	    // If loadout type is missing, is likely default loadout
	    if (!loadoutType)
	    {
	        return true;
	    }

	    if (m_eExcludedRoles.Contains(loadoutType))
	    {
	        return false;
	    }
	    
	    if (m_eRequiredRoles.IsEmpty() || loadoutType == LoadoutType.DEFAULT || loadoutType == LoadoutType.SAVED)
	    {
	        return true;
	    }
	    
	    // Allow if the loadout type is required or privileged
	    if (m_eRequiredRoles.Contains(loadoutType))
	    {
	        return true;
	    }
	    
	    // Deny by default if none of the above conditions are met
	    return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasPrivilegedLoadoutType()
	{
		IEntity localPlayerEntity = SCR_PlayerController.GetLocalMainEntity();
	    LoadoutType loadoutType = GetLoadoutType(localPlayerEntity);
	    
	    // If loadout type is missing, is likely default loadout
	    if (!loadoutType)
	    {
	        return false;
	    }

	    if (m_ePriviledgedRoles.Contains(loadoutType))
		{
			return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetSlotVisible(bool bVisible)
	{
		super.SetSlotVisible(bVisible);
		
		if (!m_wDimmerEffect)
			m_wDimmerEffect = m_widget.FindAnyWidget("Dimmer");
		
		if (!m_pItem || !m_pItem.GetOwner())
			return;
		
		//~ Never show Stored resources
		Widget resourceStored = m_widget.FindAnyWidget(STORED_RESOURCES_WIDGET_NAME);
		if (resourceStored)
			resourceStored.SetVisible(false);
		
		//~ Never show Available resources
		Widget resourceAvailable = m_widget.FindAnyWidget(AVAILABLE_RESOURCES_WIDGET_NAME);
		if (resourceAvailable)
			resourceAvailable.SetVisible(false);
		
		//~ Get cost widget
		m_CostResourceHolder = m_widget.FindAnyWidget(COST_RESOURCES_WIDGET_NAME);
		if (m_CostResourceHolder)
			m_CostResourceHolderText = TextWidget.Cast(m_CostResourceHolder.FindAnyWidget(RESOURCES_TEXT_WIDGET_NAME));
		
		if (!m_CostResourceHolder || !m_CostResourceHolderText)
			return;
		
		SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.FindResourceComponent(m_pItem.GetOwner());
		SCR_ResourceContainer resourceContainer;
		
		if (resourceComponent)
			resourceContainer = resourceComponent.GetContainer(EResourceType.SUPPLIES);
		
		UpdateTotalResources(GetTotalResources());
		
		// Check if player has enough Military Allocated Supplies
		CheckPersonalResources(GetPersonalResourceCost());
		
		SetItemRank();
			
		//Widget ammoTypeWidget = m_widget.FindAnyWidget(AMMOTYPE_WIDGET_NAME);
		//if (ammoTypeWidget)
		//	ammoTypeWidget.SetVisible(true);
		
		CheckRequiredRank();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void CheckRequiredRank()
	{
		ImageWidget rankIcon = ImageWidget.Cast(m_widget.FindAnyWidget(RANK_ICON_WIDGET_NAME));
		if (!rankIcon)
			return;

		rankIcon.SetVisible(false);
		
		//~ No rank required
		if (m_eRequiredRank < 0 || m_bIgnoreRankIfRole && HasPrivilegedLoadoutType())
			return;
		
		//~ Get the rank icon and set it and disable the slot
		IEntity localPlayerEntity = SCR_PlayerController.GetLocalControlledEntity();
		
		if (!HasCorrectLoadoutType() && !(SCR_CharacterRankComponent.GetCharacterRank(localPlayerEntity) >= SCR_ECharacterRank.LIEUTENANT))
		{
			SetItemAvailability(false);
		}
		
		if (!localPlayerEntity || m_eRequiredRank <= SCR_CharacterRankComponent.GetCharacterRank(localPlayerEntity))
			return;

		SetItemAvailability(false);
		rankIcon.SetVisible(true);
		
		ResourceName rankIconImageSet = SCR_XPInfoDisplay.GetRankIconImageSet();
		if (rankIconImageSet.IsEmpty())
			return;
		
		FactionAffiliationComponent playerFactionAffiliation = FactionAffiliationComponent.Cast(localPlayerEntity.FindComponent(FactionAffiliationComponent));
		if (!playerFactionAffiliation)
			return;
		
		SCR_Faction playerFaction = SCR_Faction.Cast(playerFactionAffiliation.GetAffiliatedFaction());
		if (!playerFaction)
			return;
		
		string rankInsignia = playerFaction.GetRankInsignia(m_eRequiredRank);
		if (rankInsignia.IsEmpty())
			return;
		
		//~ Set rank icon
		rankIcon.LoadImageFromSet(0, rankIconImageSet, rankInsignia);
	}
};