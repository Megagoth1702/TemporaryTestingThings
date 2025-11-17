//------------------------------------------------------------------------------------------------
modded class SCR_MilitaryBaseSystem : GameSystem
{	
	protected ref map<ref string, ref array<SCR_CampaignMilitaryBaseComponent>> m_GroupBases = new map<ref string, ref array<SCR_CampaignMilitaryBaseComponent>>;
	SCR_GameModeCampaign m_CampaignInfo;
	protected ref array<SCR_CampaignMilitaryBaseComponent> m_BasesForRemoval = new array<SCR_CampaignMilitaryBaseComponent>();
	protected ref array<SCR_CampaignMilitaryBaseComponent> m_Whitelist = new array<SCR_CampaignMilitaryBaseComponent>();
	
	//------------------------------------------------------------------------------------------------ v1
//	override void RegisterBase(notnull SCR_MilitaryBaseComponent base)
//	{
//		SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (CampaignInfo.GetAreBasesRandomized() && Replication.IsServer())
//		{
//			int randomInt = GetRandomInt(0, 10);
//			int threshold = CampaignInfo.GetRandomizeThreshold();			
//			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//			
//			if (campaignBase && randomInt <= threshold && !campaignBase.IsHQ() && !campaignBase.GetExcludedFromRandomization())
//			{
//	//			PrintFormat("Base rolled a %1 | %2", randomInt, campaignBase);
//				
//				if (campaignBase.IsControlPoint())
//				{
//					int controlPoints = CampaignInfo.GetControlPointTreshold();
//					controlPoints--;
//					
//					CampaignInfo.SetControlPointTreshold(controlPoints);
//				}
//				
//				return;
//			}
//		}
//			
//		super.RegisterBase(base);
//	}
	
	//------------------------------------------------------------------------------------------------ v2
//	override void RegisterBase(notnull SCR_MilitaryBaseComponent base)
//    {     
//		int randomInt = GetRandomInt(0, 10);   
//        SCR_GameModeCampaign CampaignInfo = SCR_GameModeCampaign.GetInstance();
//        if (CampaignInfo.GetAreBasesRandomized() && Replication.IsServer())
//        {
//            SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//            if (campaignBase)
//            {
//                if (campaignBase.IsHQ() || campaignBase.GetExcludedFromRandomization())
//                {
//                    PrintFormat("RegisterBase: Registering HQ or excluded base: %1", campaignBase.GetBaseName());
////                    campaignBase.SetIsActivated(true);
//                    super.RegisterBase(base);
//                    return;
//                }
//
//                // Check if the base belongs to a group
//                string groupName = campaignBase.GetBasesGroupName();
//                if (!groupName.IsEmpty())
//				{
//				    if (!m_GroupBases.Contains(groupName))
//				    {
//				        array<SCR_CampaignMilitaryBaseComponent> newGroupArray = new array<SCR_CampaignMilitaryBaseComponent>();
//				        m_GroupBases.Set(groupName, newGroupArray);
//				    }
//
//				    m_GroupBases.Get(groupName).Insert(campaignBase);
//				}
//				
//                else
//                {
//                    int threshold = CampaignInfo.GetRandomizeThreshold();
//                    if (randomInt >= threshold)
//                    {
//						if (!m_Whitelist.Contains(campaignBase))
//						{
//							m_Whitelist.Insert(campaignBase);
//						}
//						
//                        PrintFormat("RegisterBase: Registering non-grouped base: %1 (randomInt: %2 > threshold: %3)", campaignBase.GetBaseName(), randomInt, threshold);
////                      campaignBase.SetIsActivated(true);
//                        super.RegisterBase(base);
//                    }
//                    else
//                    {
//						InsertBaseToBeRemoved(campaignBase);
//                        PrintFormat("RegisterBase: Skipping non-grouped base: %1 (randomInt: %2 <= threshold: %3)", campaignBase.GetBaseName(), randomInt, threshold);
//						return;
//                    }
//                }
//            }
//        }
//		
//        else
//        {
//            super.RegisterBase(base);
//        }
//    }

	//------------------------------------------------------------------------------------------------v1
//	void FinalizeBaseRegistration()
//    {
//        if (!Replication.IsServer())
//            return;
//
//		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		
//        // Randomly select and register one base per group
//        foreach (string groupName, array<SCR_CampaignMilitaryBaseComponent> bases : m_GroupBases)
//        {
//            if (bases.IsEmpty())
//                continue;
//
//            // Select a random base from the group 
//			int baseCount = bases.Count();
//			int randomInt = GetRandomInt(0, 10);
//			int randomIndex;
//            int threshold = m_CampaignInfo.GetRandomizeThreshold();
//            while (randomInt < threshold)
//            {
//				randomInt = GetRandomInt(0, 10);
//				randomIndex = GetRandomInt(0, baseCount - 1);
//#ifdef WORKBENCH
//				PrintFormat("FinalizeBaseRegistration: While: Random Int: %1 | Random Index: %2", randomInt, randomIndex);
//#endif
//			}
//			
//            SCR_CampaignMilitaryBaseComponent selectedBase = bases[randomIndex];
//			if (!m_Whitelist.Contains(selectedBase))
//			{
//				m_Whitelist.Insert(selectedBase);
//			}
//			
//#ifdef WORKBENCH
//            PrintFormat("FinalizeBaseRegistration: Registering randomly selected base in group %1: %2", groupName, selectedBase.GetBaseName());
//			PrintFormat("FinalizeBaseRegistration: Random Index: %1 | Count: %2 | Range: %3", randomIndex, bases.Count(), bases.Count() - 1);
//#endif
//			foreach (SCR_MilitaryBaseComponent base : m_aBases)
//			{
//				SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
//	            if (campaignBase)
//	            {
//					if (m_Whitelist.Contains(campaignBase))
//					{
////						campaignBase.SetIsProcessed(true);
////						campaignBase.SetIsRegistered(true);
//						continue;
//					}
//					else
//					{
//						InsertBaseToBeRemoved(campaignBase);
//					}
//				}
//			}
//			
////          selectedBase.SetIsActivated(true);
//            super.RegisterBase(selectedBase); //(selectedBase.GetOwner().FindComponent(SCR_MilitaryBaseComponent));
//        }
//
//        ResetGroupBases();
//		DeleteUnusedBases();
//    }
	
	//------------------------------------------------------------------------------------------------v2
	void FinalizeBaseRegistration()
    {
        if (!Replication.IsServer())
            return;

		RandomizeBases();
		RandomizeGroups();
		DeleteUnusedBases();
        //ResetGroupBases();
		SetRandomizedBasesInitialized(true);
    }
	
	//------------------------------------------------------------------------------------------------
	void RandomizeBases()
	{
        m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
		//bool isSavedSession = m_CampaignInfo.GetIsSavedSession();
		//PrintFormat("RandomizeBases: Is Saved Session: %1", isSavedSession,);
		
        if (m_CampaignInfo.GetAreBasesRandomized() && Replication.IsServer())// && !m_CampaignInfo.GetRandomizedBasesInitialized())
        {
			foreach (SCR_MilitaryBaseComponent base : m_aBases)
			{
				SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
	            if (campaignBase)
	            {
	                if (campaignBase.IsHQ() || campaignBase.GetExcludedFromRandomization() || campaignBase.GetBuiltByPlayers() || campaignBase.GetIsRegistered())
	                {
	                    PrintFormat("RandomizeBases: Registering HQ, excluded, or registered base: %1", campaignBase.GetBaseName());
	                    campaignBase.SetIsRegistered(true);
						campaignBase.SetIsProcessed(true);	
	                    continue;
	                }
	
//					if (!campaignBase.GetIsRegistered())// && isSavedSession)
//					{
//	                    PrintFormat("RandomizeBases: Skipping rejected base from previous session: %1", campaignBase.GetBaseName());
//
//						campaignBase.SetIsRegistered(false);
//						campaignBase.SetIsProcessed(true);	
//						InsertBaseToBeRemoved(campaignBase);
//	                    continue;
//					}
					
	                // Check if the base belongs to a group
	                string groupName = campaignBase.GetBasesGroupName();
	                if (!groupName.IsEmpty())
					{
					    if (!m_GroupBases.Contains(groupName))
					    {
					        array<SCR_CampaignMilitaryBaseComponent> newGroupArray = new array<SCR_CampaignMilitaryBaseComponent>();
					        m_GroupBases.Set(groupName, newGroupArray);
					    }
	
					    m_GroupBases.Get(groupName).Insert(campaignBase);
					}
	                else
	                {
						int randomInt = GetRandomInt(0, 10);   
	                    int threshold = m_CampaignInfo.GetRandomizeThreshold();
	                    if (randomInt >= threshold)
	                    {
	                        PrintFormat("RandomizeBases: Registering non-grouped base: %1 (randomInt: %2 > threshold: %3)", campaignBase.GetBaseName(), randomInt, threshold);
	                        campaignBase.SetIsProcessed(true);
							campaignBase.SetIsRegistered(true);
	                    }
	                    else
	                    {
	                        PrintFormat("RandomizeBases: Skipping non-grouped base: %1 (rolled: %2 < threshold: %3)", campaignBase.GetBaseName(), randomInt, threshold);
							campaignBase.SetIsProcessed(true);
							campaignBase.SetIsRegistered(false);				
							//DeleteUnusedBase(base);
							InsertBaseToBeRemoved(campaignBase);
	                    }
	                }
				}
        	}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void RandomizeGroups()
	{
		if (!Replication.IsServer())
            return;

		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
		//bool isSavedSession = m_CampaignInfo.GetIsSavedSession();
        // Randomly select and register one base per group
        foreach (string groupName, array<SCR_CampaignMilitaryBaseComponent> bases : m_GroupBases)
        {
            if (bases.IsEmpty())
                continue;

            // Select a random base from the group
			int baseCount = bases.Count();
			int randomInt = GetRandomInt(0, 10);
			int randomIndex;
            int threshold = m_CampaignInfo.GetRandomizeThreshold();
            while (randomInt < threshold)
            {
				randomInt = GetRandomInt(0, 10);
				randomIndex = GetRandomInt(0, baseCount - 1);
#ifdef WORKBENCH
				PrintFormat("RandomizeGroups: While: Random Int: %1 | Random Index: %2", randomInt, randomIndex);
#endif
			}
			
            SCR_CampaignMilitaryBaseComponent selectedBase = bases[randomIndex];
			if (!m_Whitelist.Contains(selectedBase))
			{
				m_Whitelist.Insert(selectedBase);
			}
			
#ifdef WORKBENCH
            PrintFormat("RandomizeGroups: Registering randomly selected base in group %1: %2", groupName, selectedBase.GetBaseName());
			PrintFormat("RandomizeGroups: Random Index: %1 | Count: %2 | Range: %3", randomIndex, bases.Count(), bases.Count() - 1);
#endif
            //super.RegisterBase(selectedBase); //(selectedBase.GetOwner().FindComponent(SCR_MilitaryBaseComponent));
        }
		
		foreach (SCR_MilitaryBaseComponent base : m_aBases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
            if (campaignBase)
            {
				if (m_Whitelist.Contains(campaignBase) || campaignBase.GetIsRegistered()) // && isSavedSession)
				{
					campaignBase.SetIsProcessed(true);
					campaignBase.SetIsRegistered(true);
				}
				else
				{
					campaignBase.SetIsProcessed(true);
					campaignBase.SetIsRegistered(false);
					//DeleteUnusedBase(base);
					InsertBaseToBeRemoved(campaignBase);
				}
			}
		}

        //ResetGroupBases();
		SetRandomizedBasesInitialized(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetGroupBases()
    {
		foreach (string group, array<SCR_CampaignMilitaryBaseComponent> bases : m_GroupBases)
		{
			foreach(SCR_CampaignMilitaryBaseComponent base : bases)
			{
				if (!m_Whitelist.Contains(base))
				{	
					UnregisterBase(base);
					PrintFormat("ResetGroupBases: Deleting Base And Children for: %1", base);
					//SCR_EntityHelper.DeleteEntityAndChildren(base.GetOwner());
					base.Deactivate(base.GetOwner());
				}
			}
		}
		
        m_GroupBases.Clear();
    }
	
	//------------------------------------------------------------------------------------------------
	void SetRandomizedBasesInitialized(bool value)
    {
		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
        if (m_CampaignInfo)
		{
			m_CampaignInfo.SetRandomizedBasesInitialized(value);
		}	
    }
	
	//------------------------------------------------------------------------------------------------
	int GetRandomInt(int min, int max)
	{
		return Math.RandomIntInclusive(min, max);
	}
	
	//------------------------------------------------------------------------------------------------
	void InsertBaseToBeRemoved(SCR_CampaignMilitaryBaseComponent base)
	{
		if (!m_BasesForRemoval.Contains(base))
		{
			m_BasesForRemoval.Insert(base);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void DeleteUnusedBases()
	{
		foreach(SCR_CampaignMilitaryBaseComponent base : m_BasesForRemoval)
		{
			UnregisterBase(base);
			PrintFormat("DeleteUnusedBases: Deleting Base And Children for: %1", base);
			SCR_EntityHelper.DeleteEntityAndChildren(base.GetOwner());
			//base.Deactivate(base.GetOwner());
		}
		
//		foreach (SCR_MilitaryBaseComponent base : m_aBases)
//		{
//			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
//			if (campaignBase && campaignBase.IsHQ())
//			{
//				PrintFormat("DeleteUnusedBases: Base : %1 | Faction: %2", campaignBase, campaignBase.GetFaction());
//			}
//		}
	}
	
	//------------------------------------------------------------------------------------------------
	void DeleteUnusedBase(SCR_MilitaryBaseComponent base)
	{
		UnregisterBase(base);
		PrintFormat("DeleteUnusedBase: Deleting Base And Children for: %1", base);
		SCR_EntityHelper.DeleteEntityAndChildren(base.GetOwner());
		//base.Deactivate(base.GetOwner());
	}
}
