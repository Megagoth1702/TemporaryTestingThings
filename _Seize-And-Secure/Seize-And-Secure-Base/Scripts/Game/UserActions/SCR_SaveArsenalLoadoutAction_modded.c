//------------------------------------------------------------------------------------------------
modded class SCR_SaveArsenalLoadout : SCR_BaseFactionCheckUserAction
{
//	[Attribute("0", desc: "Is Rank Required for saving loadout?")]
//	protected bool m_bRankRequired;
//
//	bool IsRankRequired()
//	{
//	return m_bRankRequired;
//	}
	
	int inventoryCost;
	SCR_GameModeCampaign CampaignInfo;
	
	//------------------------------------------------------------------------------------------------
	override protected bool CanBePerformedScript(IEntity user)
	{
		UpdateActionDisplayName(user);
		
		if (m_ArsenalComponent.GetArsenalSaveType() == SCR_EArsenalSaveType.SAVING_DISABLED)
			return false;
		
		if (!super.CanBePerformedScript(user))
			{
				m_sSaveTypeDisplayName = string.Empty;
				return false;
			}
		
		if (m_ArsenalComponent.GetArsenalSaveType() == SCR_EArsenalSaveType.FACTION_ITEMS_ONLY)
			{
				Faction arsenalFaction = m_ArsenalComponent.GetAssignedFaction();
				
				if (arsenalFaction)
				{
					FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
					if (factionAffiliation && factionAffiliation.GetAffiliatedFaction() != arsenalFaction)
					{
						m_sSaveTypeDisplayName = string.Empty;
						SetCannotPerformReason(WidgetManager.Translate(SPECIFIC_FACTION_CANNOT_PERFORM, arsenalFaction.GetFactionName()));
						return false;
					}
				}
			}
		
		if (CampaignInfo.IsRankAllowanceEnabled())
		{
				SCR_ECharacterRank userRank = SCR_CharacterRankComponent.GetCharacterRank(user);
				
				string userRankName = GetUserRankName(user);
				inventoryCost = GetInventoryCost(user);
			
//				Print("Inventory Cost: " + inventoryCost);
				
				int loadoutBudget = GetLoadoutAllowance(user);
			
				if (inventoryCost <= loadoutBudget) 
					{
						return true;
					}
			
				else
					{
						SetCannotPerformReason("Allowance for " + userRankName + " [" + inventoryCost + "/" + loadoutBudget + "]");
						return false;
					}
				
//				if (userRank >= SCR_ECharacterRank.LIEUTENANT) // && loadoutCost <= loadoutBudget)
//					{
//				
//						return true;
//					}
//			
//				else
//					{
//						SetCannotPerformReason("Lieutenant");
//						return false;
//					}
		}

		return true;
		
	}

	//------------------------------------------------------------------------------------------------
	int GetInventoryCost(IEntity user)
	{			
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
		InventoryStorageManagerComponent InventoryManager = characterController.GetInventoryStorageManager();
		
		ref array<IEntity> inventoryItems = {};
		InventoryManager.GetItems(inventoryItems);
		
		int loadoutCost;
		int inventoryItemCost;
		
		SCR_ArsenalManagerComponent arsenalManager;
		SCR_ArsenalManagerComponent.GetArsenalManager(arsenalManager);
		SCR_EntityCatalogManagerComponent entityCatalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
	
		foreach (IEntity item : inventoryItems)
		{

			SCR_ArsenalComponent arsenalComponent = SCR_ArsenalComponent.FindArsenalComponent(item);
			ResourceName resourceNameItem = item.GetPrefabData().GetPrefabName();
			SCR_EntityCatalogEntry entry = entityCatalogManager.GetEntryWithPrefabFromAnyCatalog(EEntityCatalogType.ITEM, resourceNameItem);	
			float resourceCost;
				
			if (entry)
			{
				SCR_ArsenalItem itemData = SCR_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
					
				if (itemData)
				{
					resourceCost += itemData.GetSupplyCost(SCR_EArsenalSupplyCostType.DEFAULT);
				}
			}
				
			inventoryItemCost += resourceCost;
		}
	
		loadoutCost = Math.Clamp(inventoryItemCost - 60, 0, inventoryItemCost); // no idea where these 60 supplies are coming from
	
		return loadoutCost; 
	}
	
	//------------------------------------------------------------------------------------------------
	int GetLoadoutAllowance(IEntity user)
	{	
		SCR_ECharacterRank userRank = SCR_CharacterRankComponent.GetCharacterRank(user);
		return CampaignInfo.GetRankAllowance(userRank);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetUserRankName(IEntity user)
	{	
		SCR_ECharacterRank userRank = SCR_CharacterRankComponent.GetCharacterRank(user);
		string userRankName;
	
		if (userRank == SCR_ECharacterRank.RENEGADE)
			userRankName = "Renegade";
		
		if (userRank == SCR_ECharacterRank.PRIVATE)
			userRankName = "Private";

		if (userRank == SCR_ECharacterRank.CORPORAL)
			userRankName = "Corporal";
		
		if (userRank == SCR_ECharacterRank.SERGEANT)
			userRankName = "Sergeant";
									
		if (userRank == SCR_ECharacterRank.LIEUTENANT)
			userRankName = "Lieutenant";
				
		if (userRank == SCR_ECharacterRank.CAPTAIN)
			userRankName = "Captain";

		if (userRank == SCR_ECharacterRank.MAJOR)
			userRankName = "Major";
	
		return userRankName;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		if (SCR_Global.IsEditMode())
			return;
		
		super.Init(pOwnerEntity, pManagerComponent);
		
		SCR_ArsenalManagerComponent.GetArsenalManager(m_ArsenalManager);
		if (!m_ArsenalManager)
			return;
		
		CampaignInfo = SCR_GameModeCampaign.GetInstance();
		
		//~ Get Arsenal component
		m_ArsenalComponent = SCR_ArsenalComponent.Cast(GetOwner().FindComponent(SCR_ArsenalComponent));
		if (!m_ArsenalComponent && m_bAllowGetArsenalFromParent && GetOwner().GetParent())
			m_ArsenalComponent = SCR_ArsenalComponent.Cast(GetOwner().GetParent().FindComponent(SCR_ArsenalComponent));
		
		//~ Could not get arsenal
		if (!m_ArsenalComponent)
		{
			Print("SCR_SaveArsenalLoadout is attached to an entity that does not have an arsenal on self or parent (if m_bAllowGetArsenalFromParent is true)!", LogLevel.WARNING);
			return;
		}	
		
		//~ Allows editor to set the set the allowing of arsenal saving
		m_ArsenalComponent.SetHasSaveArsenalAction(true);
	}
}