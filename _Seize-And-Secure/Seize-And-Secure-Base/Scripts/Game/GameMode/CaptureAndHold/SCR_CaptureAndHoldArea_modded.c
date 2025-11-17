//------------------------------------------------------------------------------------------------
/*!
	This area awards score to the faction which controls the area periodically.
	Area registers and unregisters from the SCR_CaptureAndHoldManager.
*/
modded class SCR_CaptureAndHoldArea : SCR_CaptureArea
{
	[Attribute("1", UIWidgets.CheckBox, "Does this zone add scoring per tick?")]
	protected bool m_bEnableScoring;
	
	[Attribute("5", UIWidgets.Slider, "Score awarded per tick.", params: "0 1000 1", category: "CaptureAndHold")]
	int m_iScorePerTick;
	
	[Attribute("0", UIWidgets.CheckBox, "Earn rank while in C&H Zone?")]
	protected bool m_bCHZoneRank;

	[Attribute(category: "CaptureAndHold: Naming")]
	protected string m_sAssignedName;
	
	bool maxScoreReached;
	int factionScore = 0;
	SCR_CaptureAndHoldManager m_CHManager;
	bool m_bTicketDrainSystem;
	int m_iTicketDrainScore;
	
	protected IEntity m_AreaEntity;
	protected SCR_CampaignMilitaryBaseComponent m_ClosestBase;
	
	//------------------------------------------------------------------------------------------------
	string GetAssignedName()
	{
		return m_sAssignedName;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Called when point ticks, ie. when score should be awarded
		Authority only.
	*/
	override protected void OnTick()
	{
		
		// Callback is fired, if a faction is owning this point
		Faction owningFaction = GetOwningFaction();
		SCR_CampaignFaction owningCampaignFaction = SCR_CampaignFaction.Cast(owningFaction);

		bool isContested = IsContested();
		
		// If using ticket system, don't go above max tickets 
		m_CHManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
		if (m_CHManager && m_CHManager.TicketScoreSystem() && factionScore >= m_CHManager.GetFactionStartScore())
		{
			maxScoreReached = true;
		}	
		
		else
		{
			maxScoreReached = false;
		}
		
		// Award no score if contesting or max tickets reached
		int awardedScore;
		if (isContested || maxScoreReached || !m_bEnableScoring)
		 	awardedScore = 0;
		else
			awardedScore = GetScorePerTick();
		
		m_OnTickEvent.Invoke(this, owningFaction, awardedScore);

		// No scoring at all
		if (isContested)
			return;

		// Must have scoring system to award points
		SCR_BaseScoringSystemComponent scoringSystem = GetScoringSystemComponent();
		if (!scoringSystem)
			return;
		
		factionScore = scoringSystem.GetFactionScore(owningFaction);
		// Add score to faction, but not FIA (so they don't win the match before players can)
		FactionKey excludedFactionKey = owningFaction.GetFactionKey();
		
//		if (excludedFactionKey != "FIA" && !maxScoreReached)
		//Instead, lets only award if factions are playable
		if (owningCampaignFaction.IsPlayable() && !maxScoreReached)
		{
			scoringSystem.AddFactionObjective(owningFaction, awardedScore);
		}

		// And additionally to all players of given faction in this point

		array<SCR_ChimeraCharacter> occupants = {};
		int count = GetOccupants(owningFaction, occupants);
		for (int i = 0; i < count; ++i)
		{
			int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(occupants[i]);
			// Not a player
			if (playerId <= 0)
				continue;

			// Add rank if CHZones only 
			if (m_bCHZoneRank)
			{
				SCR_XPHandlerComponent compXP = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
				compXP.AwardXP(playerId, SCR_EXPRewards.CUSTOM_1, 0.5, false);
			}
			
			// Award player score, but do not propagate to faction - faction is already awarded
			scoringSystem.AddObjective(playerId, awardedScore, false);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void UpdateMapDescriptor(SCR_MapDescriptorComponent target)
	{
		if (!target)
			return;
	
//		if (Replication.IsClient())
//		{
//			MapItem targetItem = target.Item();
//			if (!targetItem)
//				return;
//			
//			if (!BaseIsReceivingRadioSignal(GetClosestBase(), SCR_FactionManager.SGetLocalPlayerFaction())) //Replication.IsClient()
//			{
//				targetItem.SetVisible(false);
//				return;
//			}
//			else
//			{
//				targetItem.SetVisible(true);
//			}
//		}		
		
		Color color = Color.FromRGBA(255, 0, 255, 255);
		bool friendly = false;
		if (m_pOwnerFaction)
		{
			if (IsContested())
			{
				float val01 = Math.Sin( GetWorld().GetWorldTime() * 0.01 ) * 0.5 + 0.5;
				color.Lerp(m_pOwnerFaction.GetFactionColor(), val01);
			}
			else
			{
				color = m_pOwnerFaction.GetFactionColor();
			}
		}

		MapItem mapItem = target.Item();
		if (!mapItem)
			return;
	
		MapDescriptorProps props = mapItem.GetProps();
		if (!props)
			return;
		
		props.SetFrontColor(color);
		props.SetTextColor(color);

		props.Activate(true);
		mapItem.SetProps(props);
	}
	
	//------------------------------------------------------------------------------------------------
	//! By default queries only for characters of SCR_ChimeraCharacter type
	protected override bool ScriptedEntityFilterForQuery(IEntity ent)
	{
		// Filter for characters only
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(ent);
		if (!character)
			return false;
		
		m_CHManager = SCR_CaptureAndHoldManager.Cast(GetGame().GetGameMode().FindComponent(SCR_CaptureAndHoldManager));
//		if (m_CHManager && m_CHManager.GetRequiresRadioSignal() && !HasRadioSignal(character))
		if (m_CHManager && m_CHManager.GetRequiresRadioSignal() && !BaseIsReceivingRadioSignal(GetClosestBase(), character.GetFaction()))
			return false;

		// That are alive.
		return !character.GetCharacterController().IsDead();
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasRadioSignal(SCR_ChimeraCharacter character)
	{
		IEntity characterEntity = character.GetRootParent();
		Faction characterFaction = character.GetFaction();
		return SCR_GameModeCampaign.GetInstance().GetBaseManager().IsEntityInFactionRadioSignal(characterEntity, characterFaction);
	}
	
	//------------------------------------------------------------------------------------------------
	bool BaseIsReceivingRadioSignal(SCR_CampaignMilitaryBaseComponent base, Faction characterFaction)
	{
		if (!characterFaction || !base)
		{
			PrintFormat("SCR_CaptureAndHoldArea: BaseIsReceivingRadioSignal: Base: %1 | Character Faction: %2", base, characterFaction);
			return false;
		}
		
		return base.IsHQRadioTrafficPossible(SCR_CampaignFaction.Cast(characterFaction));
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent GetClosestBase()
	{
		if (!m_AreaEntity)
		{
			Print("SCR_CaptureAndHoldArea: GetClosestBase: m_AreaEntity is Null");
			return null;
		}
		
		if (!m_ClosestBase)
		{
			m_ClosestBase = SCR_GameModeCampaign.GetInstance().GetBaseManager().FindClosestBase(m_AreaEntity.GetOrigin());
		}
		
		return m_ClosestBase;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnRadioCoverageChanged()
	{
//		Print("SCR_CaptureAndHoldArea: OnRadioCoverageChanged");
//		UpdateMapDescriptor(m_MapDescriptor);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Initialize this area and register it to parent manager.
	protected override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		m_AreaEntity = owner;
		if (!m_AreaEntity)
			return;
		
		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
		if (!CampaignInfo)
			return;
		
		m_ClosestBase = CampaignInfo.GetBaseManager().FindClosestBase(m_AreaEntity.GetOrigin());
		if (!m_ClosestBase)
			return;
		
		SCR_CoverageRadioComponent m_RadioComponent = SCR_CoverageRadioComponent.Cast(m_ClosestBase.GetOwner().FindComponent(SCR_CoverageRadioComponent));

		if (m_RadioComponent)
			m_RadioComponent.GetOnCoverageChanged().Insert(OnRadioCoverageChanged);
	}
}
