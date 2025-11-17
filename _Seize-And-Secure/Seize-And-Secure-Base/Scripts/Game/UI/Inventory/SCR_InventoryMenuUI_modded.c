modded class SCR_InventoryMenuUI : ChimeraMenuBase
{	
	protected TextWidget m_wTotalWalletBalanceText;

    protected string walletBalanceString;
    protected string walletThresholdString;
	
	//------------------------------------------------------------------------------------------------
	//! returns true if the init procedure was success
	override protected bool Init()
	{
		m_widget = GetRootWidget();

		if ( m_widget == null )
			return false;

		Widget loading = m_widget.FindAnyWidget("LoadingOverlay");
		if (loading)
		{
			loading.SetVisible(true);
			m_LoadingOverlay = SCR_LoadingOverlay.Cast(loading.FindHandler(SCR_LoadingOverlay));
		}

		//Get player
		PlayerController playerController = GetGame().GetPlayerController();
		if ( playerController != null )
		{
			m_Player = ChimeraCharacter.Cast(playerController.GetControlledEntity());
			if ( m_Player != null )
			{
				ChimeraWorld world = ChimeraWorld.CastFrom(m_Player.GetWorld());
				if (!world)
					return false;
				
				m_pPreviewManager = world.GetItemPreviewManager();
				if (!m_pPreviewManager)
				{
					Resource rsc = Resource.Load(m_ItemPreviewManagerPrefab);
					if (rsc.IsValid())
						GetGame().SpawnEntityPrefabLocal(rsc, world);
					
					m_pPreviewManager = world.GetItemPreviewManager();
				}

				m_CharController = SCR_CharacterControllerComponent.Cast(m_Player.GetCharacterController());
				if (m_CharController)
				{
					m_CharController.m_OnLifeStateChanged.Insert(LifeStateChanged);
					if (m_CharController.GetLifeState() != ECharacterLifeState.ALIVE)
						return false;
				}
				
				SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
				if (gameMode)
					gameMode.GetOnControllableDeleted().Insert(OnControllableDeleted);
				
				//Inventory Manager
				m_InventoryManager = SCR_InventoryStorageManagerComponent.Cast( m_Player.FindComponent( SCR_InventoryStorageManagerComponent ) );
				m_StorageManager = SCR_CharacterInventoryStorageComponent.Cast( m_Player.FindComponent( SCR_CharacterInventoryStorageComponent ) );
				if( !m_StorageManager )
					return false;
				m_pWeaponStorageComp = EquipedWeaponStorageComponent.Cast(m_StorageManager.GetWeaponStorage());
				if( !m_pWeaponStorageComp )
					return false;
				if ( m_InventoryManager )
				{
					m_InventoryManager.m_OnItemAddedInvoker.Insert(OnItemAddedListener);
					m_InventoryManager.m_OnItemRemovedInvoker.Insert(OnItemRemovedListener);
				}

				Widget wNaviBar = m_widget.FindAnyWidget( "Footer" );
				if( wNaviBar )
					m_pNavigationBar = SCR_NavigationBarUI.Cast( wNaviBar.FindHandler( SCR_NavigationBarUI ) );
				m_pVicinity = SCR_CharacterVicinityComponent.Cast(m_Player.FindComponent(SCR_CharacterVicinityComponent));
				if (m_pVicinity)
				{
					m_pVicinity.OnVicinityUpdateInvoker.Insert(RefreshLootUIListener);
					m_iVicinityDiscoveryRadius = m_pVicinity.GetDiscoveryRadius();
					IEntity itemOfInterest = m_pVicinity.GetItemOfInterest();
					if (itemOfInterest && itemOfInterest.IsInherited(SCR_ChimeraCharacter))
					{
						SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(itemOfInterest);
						SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
						if (charCtrl)
							charCtrl.m_OnLifeStateChanged.Insert(OnLootedCharacterLifeStateChanged);
				}
				}

				m_wStoragesContainer = GridLayoutWidget.Cast( m_widget.FindAnyWidget( "StorageGrid" ) );
				m_wTotalWeightText = TextWidget.Cast( m_widget.FindAnyWidget("TotalWeightText") );
				m_wTotalWalletBalanceText = TextWidget.Cast( m_widget.FindAnyWidget("TotalWalletText") );
			}
			else
			{
				return false;
			}

		}
		else
		{
			return false;
		}

		m_wDragDropContainer = FrameWidget.Cast( m_widget.FindAnyWidget( "DragDropContainer" ) );
		if ( m_wDragDropContainer )
		{
			m_pDragDropFrameSlotUI = SCR_SlotUIComponent.Cast( m_wDragDropContainer.FindHandler( SCR_SlotUIComponent ) );
			m_pDragDropPreviewImage = ItemPreviewWidget.Cast( m_wDragDropContainer.FindAnyWidget( "item" ) );
		}

		m_wAttachmentPointsContainer = m_widget.FindAnyWidget("AttachmentPoints");

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void MoveItemToStorageSlot()
	{
		if (MoveItemToStorageSlot_VirtualArsenal())
			return;
		
		if (MoveItemToStorageSlot_ResourceContainer())
		{
			if (m_pSelectedSlotUI)
			{
				m_pSelectedSlotUI.SetSelected(false);
				m_pSelectedSlotUI = null;
			}
			
			return;
		}

		if (!m_pSelectedSlotUI)
			return;
		
		InventoryItemComponent pComp = m_pSelectedSlotUI.GetInventoryItemComponent();
		if (!pComp)
			return;

		IEntity pItem = pComp.GetOwner();
		if (!m_InventoryManager.CanMoveItem(pItem) || !m_pFocusedSlotUI.IsCompatible())
			return;
		
		m_pCallBack.m_pItem = pItem;
		m_pCallBack.m_pMenu = this;
		m_pCallBack.m_pStorageFrom = m_pSelectedSlotUI.GetStorageUI();

		// If magazine is placed on another magazine, repack it then return
		if (m_pSelectedSlotUI.GetSlotedItemFunction() == ESlotFunction.TYPE_MAGAZINE ||	m_pFocusedSlotUI.GetSlotedItemFunction() == ESlotFunction.TYPE_MAGAZINE)
		{
			if (RepackMagazine())
			{
				return;	
			}		
		}
		
		SCR_InventoryOpenedStorageUI open = GetOpenedStorage(m_pFocusedSlotUI.GetAsStorage());
		if (open)
			m_pCallBack.m_pStorageTo = open;
		else
			m_pCallBack.m_pStorageTo = m_pFocusedSlotUI.GetStorageUI();

		BaseInventoryStorageComponent pStorageFromComponent = m_pCallBack.m_pStorageFrom.GetCurrentNavigationStorage();
		BaseInventoryStorageComponent pStorageToComponent = m_pFocusedSlotUI.GetAsStorage();
		
		if (!pStorageToComponent)
			pStorageToComponent = m_pCallBack.m_pStorageTo.GetStorage();

		if (!pStorageToComponent || pStorageToComponent == pStorageFromComponent)
			return;

		if (IsStorageArsenal(pStorageToComponent))
		{
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_INV_DROP_ERROR);
			return;
		}
		
		bool shouldEquip = m_pCallBack.m_pStorageTo == m_pStorageListUI;
		bool equip = shouldEquip && m_InventoryManager.EquipAny(m_StorageManager , pItem, 0, m_pCallBack);
		
		if (!equip)
			m_InventoryManager.InsertItem( pItem, pStorageToComponent, pStorageFromComponent, m_pCallBack );
		else
			m_InventoryManager.PlayItemSound(pItem, SCR_SoundEvent.SOUND_EQUIP);
		/*
		if ( pItem.FindComponent( SCR_GadgetComponent ) )
			m_InventoryManager.EquipGadget( pItem, m_pCallBack );
		else
		*/
	}

	//------------------------------------------------------------------------------------------------
	bool RepackMagazine()
	{
		InventoryItemComponent fromComponent = m_pSelectedSlotUI.GetInventoryItemComponent();
		if (!fromComponent)
			return false;
	
		IEntity fromEntity = fromComponent.GetOwner();
		if (!fromEntity)
			return false;
	
		InventoryItemComponent toComponent = m_pFocusedSlotUI.GetInventoryItemComponent();
		if (!toComponent)
			return false;
	
		IEntity toEntity = toComponent.GetOwner();	
		if (!toEntity)
			return false;

		// Slot is stacked
		if (m_pSelectedSlotUI == m_pFocusedSlotUI)																				
		{
			if (m_pFocusedSlotUI.IsStacked())
			{	
				array<IEntity> items = {};
				GetInventoryStorageManager().GetItems(items);
								
//				array<MagazineComponent> magazines = {};
				int i = 0;
				int magStackPosition;
				foreach (IEntity item : items)
				{
					if (item.FindComponent(MagazineComponent))
					{
						MagazineComponent magStack = MagazineComponent.Cast(fromEntity.FindComponent(MagazineComponent));
						MagazineComponent mag = MagazineComponent.Cast(item.FindComponent(MagazineComponent));
						
						if (mag.GetMagazineWell().Type() == magStack.GetMagazineWell().Type())
						{
#ifdef WORKBENCH
							Print("Position: " + i + " Ammo Count: " + mag.GetAmmoCount());
#endif
//							magazines.Insert(mag);
							
							// Stop when position is found
							magStackPosition = i;
							break;
						}
					}
						
					i++;
				}

			// Take from next magazine in stack
			toEntity = items[magStackPosition];	
//			magazines.Clear();												
			}
						
			else
			{
#ifdef WORKBENCH
				Print("RepackMagazine: Same magazine. Repacking canceled", LogLevel.NORMAL);
#endif
				return false;																										
			}																									
		}
		
		// Cancel if trying to repack same magazine on purpose
		if (fromEntity.GetID() == toEntity.GetID())
		{
#ifdef WORKBENCH
			Print("RepackMagazine: Same magazine. Repacking canceled", LogLevel.NORMAL);
#endif
			return false;
		}																		
			
		MagazineComponent fromMag = MagazineComponent.Cast(fromEntity.FindComponent(MagazineComponent));
		if (!fromMag)
		{
#ifdef WORKBENCH
			Print("RepackMagazine: No fromMag", LogLevel.NORMAL);
#endif
			return false;
		}
		
		MagazineComponent toMag = MagazineComponent.Cast(toEntity.FindComponent(MagazineComponent));
		if (!toMag)
		{
#ifdef WORKBENCH
			Print("RepackMagazine: No toMag", LogLevel.NORMAL);
#endif			
			return false;
		}
		
		// Compatibility check
		if (fromMag.GetMagazineWell().Type() != toMag.GetMagazineWell().Type())							
		{
#ifdef WORKBENCH
			Print("RepackMagazine: Incompatible magazines...", LogLevel.NORMAL);
#endif
			return false;
		}
 
		// Quantity check
		if (toMag.GetAmmoCount() == toMag.GetMaxAmmoCount() && fromMag.GetAmmoCount() == fromMag.GetMaxAmmoCount() || toMag.GetAmmoCount() == toMag.GetMaxAmmoCount())
		{				
#ifdef WORKBENCH
			Print("RepackMagazine: Magazines are full...", LogLevel.NORMAL);
#endif
			return false;
		}
		
		// Send to server for replication nightmare
		return AskRpc_RepackMagazine(toEntity, fromMag, toMag);	
	}
	
	//------------------------------------------------------------------------------------------------
	bool AskRpc_RepackMagazine(IEntity toEntity, MagazineComponent fromMag, MagazineComponent toMag)
	{
		IEntity owner = m_InventoryManager.GetOwner();
		if (!owner)
		{
			Print("AskRpc_RepackMagazine: No Owner!", LogLevel.ERROR);
			return false;
		}
		
		if (!toMag)
		{
			Print("AskRpc_RepackMagazine: No toMag!", LogLevel.ERROR);
			return false;
		}
		
		if (!fromMag)
		{
			Print("AskRpc_RepackMagazine: No fromMag!", LogLevel.ERROR);
			return false;
		}
				
		m_pCallBack.m_pStorageFrom = m_pSelectedSlotUI.GetStorageUI();
		m_pCallBack.m_pStorageTo = m_pFocusedSlotUI.GetStorageUI();

		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
		{
			Print("AskRpc_RepackMagazine: No Player Controller", LogLevel.ERROR);
			return false;
		}
		
		RplComponent replicationComp = RplComponent.Cast(owner.FindComponent(RplComponent)); //fromMag.GetOwner().FindComponent(RplComponent));
		if (!replicationComp)
		{
			Print("AskRpc_RepackMagazine: No Replication Component", LogLevel.ERROR);
			return false;
		}
		
		BaseInventoryStorageComponent fromStorageComponent = m_pCallBack.m_pStorageFrom.GetStorage();
		BaseInventoryStorageComponent toStorageComponent = m_pCallBack.m_pStorageTo.GetStorage();
		if (!fromStorageComponent || !toStorageComponent)
		{
			Print("AskRpc_RepackMagazine: Storage Components Missing!", LogLevel.ERROR);
			return false;
		}
	
		RplId rplInvManager = Replication.FindId(m_InventoryManager);
		RplId rplFromMag = Replication.FindId(fromMag);
		RplId rplToMag = Replication.FindId(toMag);
		RplId rplToStorage = Replication.FindId(toStorageComponent);
		RplId rplFromStorage = Replication.FindId(fromStorageComponent);
		
		playerController.RpcAsk_RepackMagazine(playerController.GetPlayerId(), rplInvManager, rplToStorage, rplFromStorage, rplFromMag, rplToMag);
		
		m_pCallBack.m_pStorageTo.Refresh();
		m_pCallBack.m_pStorageFrom.Refresh();

		//Play sound. TODO: Repacking animation
		m_InventoryManager.PlayItemSound(toEntity, SCR_SoundEvent.SOUND_EQUIP); // SOUND_EQUIP
	
		return true;
	}
	
}
