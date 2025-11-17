//------------------------------------------------------------------------------------------------
//! Created in SCR_GameModeCampaign
modded class SCR_CampaignMilitaryBaseManager
{
	protected static const float CP_AVG_DISTANCE_TOLERANCE = 0.12; // 0.25 0.12
	protected int m_iAdditionalRange;
	protected SCR_GameModeCampaign m_CampaignInfo;
	protected ref array<SCR_CampaignFaction> m_aPlayableFactions = {};
	
	//------------------------------------------------------------------------------------------------
	//! If there are only two candidates for main HQ or the main process fails, HQs are selected simply and cheaply
	protected override void SelectHQsSimple(notnull array<SCR_CampaignMilitaryBaseComponent> candidates, out notnull array<SCR_CampaignMilitaryBaseComponent> selectedHQs)
	{
		// If AAD, make sure attacker and defender bases are selected
		if (m_Campaign && m_Campaign.IsAADEnabled())
		{
			array<SCR_CampaignMilitaryBaseComponent> attackers = {};
			array<SCR_CampaignMilitaryBaseComponent> defenders = {};

			foreach(SCR_CampaignMilitaryBaseComponent candidate : candidates)
			{
				AADBaseType baseType = candidate.GetAADBaseType();
				switch (baseType)
				{
					case AADBaseType.Defender:
					{
						defenders.Insert(candidate);
						break;
					}
					
					case AADBaseType.Attacker:
					{
						attackers.Insert(candidate);
						break;
					}
					
					default:
					{
						PrintFormat("SelectHQsSimple: Base: %1 is missing AADBaseType Assignment... skipping", candidate.GetBaseName());
						break;
					}
				}
			}
			
			int randomAttackerIdx = GetRandomInt(0, attackers.Count() - 1);
			SCR_CampaignMilitaryBaseComponent attacker = attackers[randomAttackerIdx];
			
			int randomDefenderIdx = GetRandomInt(0, defenders.Count() - 1);
			SCR_CampaignMilitaryBaseComponent defender = defenders[randomDefenderIdx];
			
			if (Math.RandomFloat01() >= 0.5)
				selectedHQs = {attacker, defender};
			else
				selectedHQs = {defender, attacker};
			
			return;
		}
		
		super.SelectHQsSimple(candidates, selectedHQs);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Picks Main Operating Bases from a list of candidates by checking average distance to active control points
	override void SelectHQs(notnull array<SCR_CampaignMilitaryBaseComponent> candidates, notnull array<SCR_CampaignMilitaryBaseComponent> controlPoints, out notnull array<SCR_CampaignMilitaryBaseComponent> selectedHQs)
	{
		if (m_Campaign && m_Campaign.IsAADEnabled())
		{
			SelectHQsSimple(candidates, selectedHQs);
			return;
		}
		
		super.SelectHQs(candidates, controlPoints, selectedHQs);
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetHQFactions(notnull array<SCR_CampaignMilitaryBaseComponent> selectedHQs)
	{
		if (m_Campaign && m_Campaign.IsAADEnabled())
		{		
		    SCR_CampaignFactionManager factionManager = SCR_CampaignFactionManager.Cast(GetGame().GetFactionManager());
		    if (!factionManager)
		    {
		        Print("SetHQFactions: Faction manager not found");
		        return;
		    }
		
		    // Get defender and attacker factions
		    SCR_CampaignFaction defenderFaction = GetOrSetFaction(AADBaseType.Defender, factionManager);
		    SCR_CampaignFaction attackerFaction = GetOrSetFaction(AADBaseType.Attacker, factionManager, defenderFaction);
		
		    if (!defenderFaction || !attackerFaction)
		    {
		        Print("SetHQFactions: Failed to set defender or attacker faction");
		        return;
		    }
#ifdef WORKBENCH		
			PrintFormat("SetHQFactions: Assigning Factions: Defender: %1 | Attacker: %2", defenderFaction.GetFactionName(), attackerFaction.GetFactionName());
			PrintFormat("SetHQFactions: HQ Count: %1 | HQ[0]: %2 | HQ[1]: %3", selectedHQs.Count(), selectedHQs[0].GetBaseName(), selectedHQs[1].GetBaseName());
#endif
			
			foreach (SCR_CampaignMilitaryBaseComponent selectedBase : selectedHQs)
			{
				AADBaseType baseType = selectedBase.GetAADBaseType();
				switch (baseType)
				{
					case AADBaseType.Defender:
					{
						selectedBase.SetFaction(defenderFaction);
						PrintFormat("SetHQFactions: %1 has been set to Faction: %2", selectedBase.GetBaseName(), defenderFaction.GetFactionName());
						break;
					}
					
					case AADBaseType.Attacker:
					{
						selectedBase.SetFaction(attackerFaction);
						PrintFormat("SetHQFactions: %1 has been set to Faction: %2", selectedBase.GetBaseName(), attackerFaction.GetFactionName());
						break;
					}
					
					default:
					{
						Faction randomFaction;
						if (Math.RandomFloat01() >= 0.5)
						{
							randomFaction = attackerFaction;
						}							
						else
						{
							randomFaction = defenderFaction;
						}							
						
						selectedBase.SetFaction(randomFaction);
						PrintFormat("SetHQFactions: Base: %1 is somehow missing AADBaseType Assignment... setting random faction", selectedBase.GetBaseName());
						break;
					}
				} 
			}
			
			return;
		}
		
		super.SetHQFactions(selectedHQs);
	}
	
	//------------------------------------------------------------------------------------------------
	private SCR_CampaignFaction GetOrSetFaction(AADBaseType baseType, SCR_CampaignFactionManager factionManager, SCR_CampaignFaction excludeFaction = null)
	{
	    // Determine the faction key based on base type
	   	string key;
		if (baseType == AADBaseType.Defender) 
		{
		    key = m_Campaign.GetAADDefendingFaction();
		} 
		else 
		{
		    key = m_Campaign.GetAADAttackingFaction();
		}
		
	    SCR_CampaignFaction faction;	
	    // Try to use existing faction key
	    if (!key.IsEmpty())
	    {
	        faction = SCR_CampaignFaction.Cast(factionManager.GetFactionByKey(key));
	        if (faction)
	        {
	            PrintFormat("GetOrSetFaction: Using existing faction for %1: %2", baseType.ToString(), faction.GetFactionName());
	            return faction;
	        }
	    }
	
	    // Faction not found or key empty, select a random playable faction
	    array<SCR_CampaignFaction> playableFactions = GetPlayableFactions(factionManager);
	    if (playableFactions.IsEmpty())
	    {
	        Print("GetOrSetFaction: No playable factions available");
	        return null;
	    }
	
	    int randomIdx = GetRandomInt(0, playableFactions.Count() - 1);
	    faction = playableFactions[randomIdx];
	
	    // Ensure faction differs from excludeFaction
	    while (faction == excludeFaction && playableFactions.Count() > 1)
	    {
	        randomIdx = GetRandomInt(0, playableFactions.Count() - 1);
	        faction = playableFactions[randomIdx];
	    }
	
	    // Set the faction in campaign info
	    if (faction)
	    {
	        string factionKey = faction.GetFactionKey();
	        if (baseType == AADBaseType.Defender)
	            m_Campaign.SetAADDefendingFaction(factionKey);
	        else
	            m_Campaign.SetAADAttackingFaction(factionKey);
	        PrintFormat("GetOrSetFaction: Set random faction for %1: %2", baseType.ToString(), faction.GetFactionName());
	    }
	
	    return faction;
	}
	
	//------------------------------------------------------------------------------------------------
	private array<SCR_CampaignFaction> GetPlayableFactions(SCR_CampaignFactionManager factionManager)
	{
	    array<Faction> factions = {};
	    factionManager.GetFactionsList(factions);
	    array<SCR_CampaignFaction> playableFactions = {};
	
	    foreach (Faction faction : factions)
	    {
	        SCR_CampaignFaction cFaction = SCR_CampaignFaction.Cast(faction);
	        if (cFaction && cFaction.IsPlayable())
	        {
	            playableFactions.Insert(cFaction);
	            PrintFormat("GetPlayableFactions: Added playable faction: %1", cFaction.GetFactionName());
	        }
	    }
	
	    PrintFormat("GetPlayableFactions: Total playable factions: %1", playableFactions.Count());
	    return playableFactions;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetRandomInt(int min, int max)
	{
		return Math.RandomIntInclusive(min, max);
	}
	
	//------------------------------------------------------------------------------------------------
//	override void OnAllBasesInitialized()
//	{
//		super.OnAllBasesInitialized();
//		
//		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
//		if (!m_CampaignInfo)
//			return;
//		
//		if (m_CampaignInfo.GetAreBasesRandomized() && !m_Campaign.GetEstablishingBasesEnabled())
//		{
////			ProcessFringeBases();
//			ProcessMST(); // <-- this seems to work most reliably
//			ProcessFringeBases();
////			ProcessConnectingBases();
////			ProcessControlPoints();
//			ProcessHQBases();
////			EstablishDynamicRadioRanges();
//		}
//		
//		DeletedUnusedStarterPositions();
//	}
	
	//------------------------------------------------------------------------------------------------
	void ProcessRandomizedBaseState()
	{
		if (m_Campaign && m_Campaign.GetAreBasesRandomized() && !m_Campaign.GetEstablishingBasesEnabled())
		{
			ProcessMST();
			ProcessFringeBases();
			ProcessHQBases();
		}
		
		DeletedUnusedStarterPositions();
		SetRegisteredState(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetRegisteredState(bool value)
	{
		foreach (SCR_MilitaryBaseComponent base : m_aBases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
			
			if (campaignBase)
			{
				campaignBase.SetIsRegistered(value);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void DeletedUnusedStarterPositions()
	{
		SCR_MilitaryBaseSystem baseSystem = SCR_MilitaryBaseSystem.GetInstance();
		foreach (SCR_MilitaryBaseComponent base : m_aBases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
			
			if (campaignBase && campaignBase.CanBeHQ() && !campaignBase.IsHQ())
			{
				baseSystem.DeleteUnusedBase(base);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	void ProcessHQBases()
	{
		float customRadioRange;
		array<SCR_CampaignMilitaryBaseComponent> HQBasesAtRange = {};
		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
		GetHQBasesAtRange(HQBasesAtRange);
		GetBasesAtRange(basesAtRange);
		
		// Process all HQ bases and set ranges
		foreach (SCR_CampaignMilitaryBaseComponent base : HQBasesAtRange)
		{
			customRadioRange = base.GetRadioAntennaServiceRange();
			
			array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
			excludeBases.Insert(base);
			
			vector basePosition = base.GetOwner().GetOrigin();
			SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
			excludeBases.Insert(closestBase);
			
			float distance = GetDistanceToBase(basePosition, closestBase);
			if (distance > customRadioRange)
			{
//				PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 (%3) | Next Closest: %2 (%4)", closestBase.GetBaseName(), nextClosestBase.GetBaseName(), distance, distanceToNextClosestBase);

//				base.SetIsFringeBase(true);
//				closestBase.SetIsFringeBase(true);

				m_iAdditionalRange = m_CampaignInfo.GetAdditionalAntennaRange();
				
				if (base.GetRadioAntennaServiceRange() < distance)
				{
					base.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
				}
				
				if (closestBase.GetRadioAntennaServiceRange() < distance)
				{
					closestBase.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
				}					
			}
		}
		
		SCR_RadioCoverageSystem.UpdateAll(true);
	}
	
	//------------------------------------------------------------------------------------------------ v1
	//!
//	void ProcessFringeBases()
//	{
//		float customRadioRange;
//		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
//		GetBasesAtRange(basesAtRange);
//		
//		// Process all bases and set Fringe status
//		foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
//		{
//			customRadioRange = base.GetRadioAntennaServiceRange();
//			
//			array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
//			excludeBases.Insert(base);
//			
//			vector basePosition = base.GetOwner().GetOrigin();
//			SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
//			excludeBases.Insert(closestBase);
//			
//			float distance = GetDistanceToBase(basePosition, closestBase);
//			
//			if (distance > customRadioRange)
//			{
////				PrintFormat("GetCustomRadioAntennaRange: Closest Base: %1 (%3) | Next Closest: %2 (%4)", closestBase.GetBaseName(), nextClosestBase.GetBaseName(), distance, distanceToNextClosestBase);
//
//				base.SetIsFringeBase(true);
//				closestBase.SetIsFringeBase(true);
//				
//				m_iAdditionalRange = m_CampaignInfo.GetAdditionalAntennaRange();
//				
//				if (base.GetRadioAntennaServiceRange() < distance)
//				{
//					base.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
//				}
//				
//				if (closestBase.GetRadioAntennaServiceRange() < distance)
//				{
//					closestBase.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
//				}		
//			}
//		}
//	}
	
	//------------------------------------------------------------------------------------------------ v2
	//!
	void ProcessFringeBases()
	{
		float customRadioRange;
		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
		array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
		GetBasesAtRange(basesAtRange);
		
		// Process all bases and set Fringe status
		foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
		{
			customRadioRange = base.GetRadioAntennaServiceRange();
			
//			array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
			excludeBases.Insert(base);
			
			vector basePosition = base.GetOwner().GetOrigin();
			SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
			excludeBases.Insert(closestBase);
			
			float distance = GetDistanceToBase(basePosition, closestBase);
			
			m_iAdditionalRange = m_CampaignInfo.GetAdditionalAntennaRange();
					
			if (base.GetRadioAntennaServiceRange() < distance)
			{
				base.SetIsFringeBase(true);
				base.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
				
				closestBase.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
				closestBase.SetIsConnectingBase(true);
				
				base.RecalculateRadioRange();
				closestBase.RecalculateRadioRange();					
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------ v1
	//!
//	void ProcessConnectingBases()
//	{
//		float customRadioRange;
//		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
//		array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
//		array<SCR_CampaignMilitaryBaseComponent> connectingBases = {};
//		array<SCR_CampaignMilitaryBaseComponent> groupBases = {};
//		GetBasesAtRange(basesAtRange);
//		
//		foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
//		{
//			if (base.GetIsFringeBase())
//			{
//				excludeBases.Insert(base);
//			}
//			
//			if (base.GetIsConnectingBase())
//			{
//				connectingBases.Insert(base);
//			}
//		}
//		
//		// Process all bases and set Fringe status
//		foreach (SCR_CampaignMilitaryBaseComponent base : connectingBases)
//		{						
//			customRadioRange = base.GetRadioAntennaServiceRange();		
//			excludeBases.Insert(base);
//			
//			vector basePosition = base.GetOwner().GetOrigin();
//			SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBases(basesAtRange, excludeBases, basePosition);
////			excludeBases.Insert(closestBase);
//			
//			float distance = GetDistanceToBase(basePosition, closestBase);
//			
//			m_iAdditionalRange = m_CampaignInfo.GetAdditionalAntennaRange();
//					
//			if (base.GetRadioAntennaServiceRange() < distance)
//			{
//				base.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
//				
//				closestBase.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
//				closestBase.SetIsConnectingBase(true);
//				
//				base.RecalculateRadioRange();
//				closestBase.RecalculateRadioRange();					
//			}
//		}
//	}
	
	//------------------------------------------------------------------------------------------------ v2
	//!
	void ProcessConnectingBases()
	{
		float customRadioRange;
		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
		array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
		array<SCR_CampaignMilitaryBaseComponent> connectingBases = {};
		array<SCR_CampaignMilitaryBaseComponent> clusterBases = {};
		array<SCR_CampaignMilitaryBaseComponent> anomalyBases = {};
		GetBasesAtRange(basesAtRange);
		
		foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
		{
			if (!base.IsControlPoint())
			{
				continue;
			}
			
			array<SCR_CampaignMilitaryBaseComponent> groupBases = {};
			FindNextNClosestBases(6, basesAtRange, base, base.GetOwner().GetOrigin(), groupBases);
			
			foreach (SCR_CampaignMilitaryBaseComponent foundBase : groupBases)
			{
				if (!clusterBases.Contains(foundBase))
				{
					clusterBases.Insert(foundBase);
				}
				else
				{
					connectingBases.Insert(foundBase);
				}
			}
		}
		
		// Process all bases and set Fringe status
		foreach (SCR_CampaignMilitaryBaseComponent base : connectingBases)
		{		
			int count;		
//			PrintFormat("ProcessConnectingBases: Connecting Base: %1", base.GetBaseName());
			for (int i = 0; i < connectingBases.Count(); i++)
			{
				SCR_CampaignMilitaryBaseComponent baseInArray = connectingBases[i];
				if (base == baseInArray)
				{
					count++;
				}
			}
			
			if (count >= 3 && !anomalyBases.Contains(base))
			{
				PrintFormat("ProcessConnectingBases: Connecting Base: %1", base.GetBaseName());
				anomalyBases.Insert(base);
			}
		}
		
		foreach (SCR_CampaignMilitaryBaseComponent base : anomalyBases)
		{
			float distance = 999999.9;
			
			for (int i = 0; i < anomalyBases.Count() - 1; i++)
			{
				float checkedDistance =	GetDistanceToBase(base.GetOwner().GetOrigin(), anomalyBases[i+1]);
				
				if (checkedDistance > base.GetRadioAntennaServiceRange())
				{
					distance = checkedDistance;
					PrintFormat("ProcessConnectingBases: Closest Base To: %1 is %2 at %3", base.GetBaseName(), anomalyBases[i+1].GetBaseName(), checkedDistance);
					
					if (base.GetRadioAntennaServiceRange() < checkedDistance)
					{
//						base.SetRadioAntennaServiceRange(checkedDistance);
//						PrintFormat("ProcessConnectingBases: Set Base: %1 to %2", base.GetBaseName(), checkedDistance);
//						
//						SCR_CampaignMilitaryBaseComponent anomalyBase = anomalyBases[i+1];
//						anomalyBase.SetRadioAntennaServiceRange(checkedDistance);
//						PrintFormat("ProcessConnectingBases: Set Base: %1 to %2", anomalyBase.GetBaseName(), checkedDistance);
//						break;
					}					
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------ v1
	//!
	void ProcessMST()
	{
		array<SCR_CampaignMilitaryBaseComponent> allBases = {};
		GetBasesAtRange(allBases);
		
		// Track bases in the MST
		map<SCR_CampaignMilitaryBaseComponent, bool> inTree = new map<SCR_CampaignMilitaryBaseComponent, bool>;
		map<SCR_CampaignMilitaryBaseComponent, SCR_CampaignMilitaryBaseComponent> parent = new map<SCR_CampaignMilitaryBaseComponent, SCR_CampaignMilitaryBaseComponent>;
		
		// Start with the first base
		inTree[allBases[0]] = true;
		parent[allBases[0]] = null;
		int numInTree = 1;
		int n = allBases.Count();
		
		// Build the MST
		while (numInTree < n) 
		{
		    float minDist = float.MAX;
		    SCR_CampaignMilitaryBaseComponent nextBase = null;
		    SCR_CampaignMilitaryBaseComponent fromBase = null;
		
		    foreach (SCR_CampaignMilitaryBaseComponent base1 : allBases) 
			{
		        if (inTree[base1])
				{
//					PrintFormat("ProcessMST: Checking Tree: Base 1: %1", base1.GetBaseName());
		            foreach (SCR_CampaignMilitaryBaseComponent base2 : allBases) 
					{
		                if (!inTree[base2]) 
						{						
		                    float dist = GetDistanceToBase(base1.GetOwner().GetOrigin(), base2);
//							PrintFormat("ProcessMST: Base 2: %1 is NOT in Tree. Getting distance between Base 1: %2 and Base 2: %1 | Distance: %3", base2.GetBaseName(), base1.GetBaseName(), dist);
		                    if (dist < minDist) 
							{
		                        minDist = dist;
		                        nextBase = base2;
		                        fromBase = base1;
		                    }
		                }
		            }
		        }
		    }

		    if (nextBase == null) 
			{
		        Print("ProcessMST: Error: Disconnected components detected");
		        break;
		    }
		
		    inTree[nextBase] = true;
		    parent[nextBase] = fromBase;
		    numInTree++;
			
//			PrintFormat("ProcessMST: Updating Tree: Parent Base: %1 | Next Base: %2 | Distance: %3", fromBase.GetBaseName(), nextBase.GetBaseName(), minDist);

		}
		
		// Set radio ranges
		foreach (SCR_CampaignMilitaryBaseComponent base : allBases) 
		{
		    float maxDist = 0;
			SCR_CampaignMilitaryBaseComponent parentBase;
			string parentBaseName = "Null";
		    
			// Distance to parent
		    if (parent[base] != null) 
			{
				parentBase = parent[base];
		        float distanceToParent = GetDistanceToBase(base.GetOwner().GetOrigin(), parent[base]);
				
				PrintFormat("ProcessMST: Base: %1 | Parent: %2 | Distance To Parent: %3", base.GetBaseName(), parentBase.GetBaseName(), distanceToParent);
		        if (distanceToParent > maxDist) 
				{					
					maxDist = distanceToParent;
					PrintFormat("ProcessMST: Max Distance Set To: %1", maxDist);
				}
		    }
			
		    // Distance to children
		    array<SCR_CampaignMilitaryBaseComponent> children = {};
		    foreach (SCR_CampaignMilitaryBaseComponent otherBase : allBases) 
			{
		        if (parent[otherBase] == base)
				{
					children.Insert(otherBase);
				}	
		    }
			
		    foreach (SCR_CampaignMilitaryBaseComponent child : children) 
			{
		        float distanceToChild = GetDistanceToBase(base.GetOwner().GetOrigin(), child);
		        if (distanceToChild > maxDist)
				{
					maxDist = distanceToChild;
					PrintFormat("ProcessMST: Max Distance Set For Child Base (%2) To: %1", maxDist, child.GetBaseName());
				}
		    }
			
			m_iAdditionalRange = m_CampaignInfo.GetAdditionalAntennaRange();
		    float range = Math.Ceil(maxDist + m_iAdditionalRange); // Add buffer
			if (range > base.GetRadioAntennaServiceRange())
			{
				base.SetRadioAntennaServiceRange(range);
				base.RecalculateRadioRange();
				
				if (parentBase)
				{
					parentBaseName = parentBase.GetBaseName();
				}
				
				PrintFormat("ProcessMST: Set Base: %1 to %2 | Parent: %3", base.GetBaseName(), range, parentBaseName);
			}
		    
//			float baseRange = base.GetRadioAntennaServiceRange();
//			if (parentBase && baseRange > parentBase.GetRadioAntennaServiceRange())
//			{
//				parentBase.SetRadioAntennaServiceRange(baseRange);
//				parentBase.RecalculateRadioRange();
//				
//				parentBaseName = parentBase.GetBaseName();
//				
//				PrintFormat("ProcessMST: Parent Base: %1 set to %2", parentBaseName, baseRange);
//			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//!
	void ProcessControlPoints()
	{
		float customRadioRange;
		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
		GetBasesAtRange(basesAtRange);
		
		// Process all bases and set Fringe status
		foreach (SCR_CampaignMilitaryBaseComponent base : basesAtRange)
		{
			if (!base.IsControlPoint())
				continue;
			
			customRadioRange = base.GetRadioAntennaServiceRange();
			
			array<SCR_CampaignMilitaryBaseComponent> excludeBases = {};
			array<SCR_CampaignMilitaryBaseComponent> closestBases = {};
			excludeBases.Insert(base);
			
			vector basePosition = base.GetOwner().GetOrigin();
			SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBase(basesAtRange, base, basePosition);
			int closestBasesN = m_CampaignInfo.GetClosestBasesCheck();
			
			FindNextNClosestBases(closestBasesN, basesAtRange, base, basePosition, closestBases);
			excludeBases.Insert(closestBase);
			
			foreach (SCR_CampaignMilitaryBaseComponent closeBase : closestBases)
			{	
				float distance = GetDistanceToBase(basePosition, closeBase);
				
				PrintFormat("ProcessControlPoints: Closest Base To %1 is %2. Distance: %3 | Base Range: %4", base.GetBaseName(), closeBase.GetBaseName(), distance, base.GetRadioAntennaServiceRange());
				
				m_iAdditionalRange = m_CampaignInfo.GetAdditionalAntennaRange();
					
				if (base.GetRadioAntennaServiceRange() < distance)
				{
					PrintFormat("--ProcessControlPoints: Set %1 to %2", base.GetBaseName(), distance + m_iAdditionalRange);

					base.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
					base.RecalculateRadioRange();
				}
				
				if (closeBase.GetRadioAntennaServiceRange() < distance)
				{
					PrintFormat("--ProcessControlPoints: Set %1 to %2", closeBase.GetBaseName(), distance + m_iAdditionalRange);
					closeBase.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
					closeBase.RecalculateRadioRange();
				}
				
//				if (distance > customRadioRange)
//				{	
//					m_iAdditionalRange = m_CampaignInfo.GetAdditionalAntennaRange();
//					
//					if (base.GetRadioAntennaServiceRange() < distance)
//					{
//						PrintFormat("--ProcessControlPoints: Set %1 to %2", base.GetBaseName(), distance + m_iAdditionalRange);
//
//						base.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
//					}
//					
//					if (closeBase.GetRadioAntennaServiceRange() < distance)
//					{
//						PrintFormat("--ProcessControlPoints: Set %1 to %2", closeBase.GetBaseName(), distance + m_iAdditionalRange);
//						closeBase.SetRadioAntennaServiceRange(distance + m_iAdditionalRange);
//					}		
//				}
			}						
		}
	}
	
	//------------------------------------------------------------------------------------------------ v4
	//!
	void EstablishDynamicRadioRanges()
	{
		array<SCR_CampaignMilitaryBaseComponent> basesAtRange = {};
		array<SCR_CampaignMilitaryBaseComponent> excludedBases = {};
		array<SCR_CampaignMilitaryBaseComponent> stringBases = {};
		GetBasesAtRange(basesAtRange);
				
		for (int i=0; i < basesAtRange.Count(); i++)
		{
			vector basePosition = basesAtRange[0].GetOwner().GetOrigin();
			Faction thisBaseFaction = basesAtRange[0].GetFaction();
			SCR_CampaignMilitaryBaseComponent closestFriendlyBase = FindClosestFriendlyBaseV2(basesAtRange, basesAtRange[0], excludedBases, basePosition, thisBaseFaction);			
			float distance;
			
			if (closestFriendlyBase)
			{
				if (!stringBases.Contains(basesAtRange[0]))
				{
					stringBases.Insert(basesAtRange[0]);
				}
				
				if (!stringBases.Contains(closestFriendlyBase))// && !closestFriendlyBase.IsControlPoint()) // don't exclude control points?
				{
					stringBases.Insert(closestFriendlyBase);
				}
				
				excludedBases.Insert(closestFriendlyBase);
				distance = GetDistanceToBase(basePosition, closestFriendlyBase);
#ifdef WORKBENCH				
				PrintFormat("Closest Base to %1 is %2 | Distance: %3", basesAtRange[0].GetBaseName(), closestFriendlyBase.GetBaseName(), distance);
#endif
			}
		}
			
		SCR_CampaignMilitaryBaseComponent previousBase;
		vector previousBasePosition;
		SCR_CampaignMilitaryBaseComponent nextBase;
		vector nextBasePosition;
		
		array<SCR_CampaignMilitaryBaseComponent>excludedStringBases = {};
		array<SCR_CampaignMilitaryBaseComponent>anomalyBases = {};
		
		for (int i = 0; i < stringBases.Count() - 1; i++)
		{
			int next = i + 1;
			SCR_CampaignMilitaryBaseComponent currentBase = stringBases[i];
			nextBase = stringBases[next];
			
			vector currentBasePosition = currentBase.GetOwner().GetOrigin();
			nextBasePosition = nextBase.GetOwner().GetOrigin();
			
			float distance;
			distance = GetDistanceToBase(currentBasePosition, nextBase);
			
#ifdef WORKBENCH			
			PrintFormat("EstablishDynamicRadioRanges: %1 to %2 | Distance: %3", currentBase.GetBaseName(), nextBase.GetBaseName(), distance);
#endif
			float currentBaseRange = currentBase.GetRadioAntennaServiceRange();
			float nextBaseRange = nextBase.GetRadioAntennaServiceRange();
			
			if (distance > currentBaseRange)
			{
				// Ignore Fringe Bases
				if (currentBase.GetIsFringeBase())
				{
					continue;
				}
		
				if (!excludedStringBases.Contains(previousBase))
				{
					excludedStringBases.Insert(previousBase);
				}
				
				if (!excludedStringBases.Contains(currentBase))
				{
					excludedStringBases.Insert(currentBase);
				}
				
				if (!excludedStringBases.Contains(nextBase))
				{
					excludedStringBases.Insert(nextBase);
				}
				
				// Does previous base have a closest base that isn't the previous base?
				SCR_CampaignMilitaryBaseComponent closestBase = FindNextClosestBases(stringBases, excludedStringBases, currentBasePosition);
				float distanceToClosestBase = GetDistanceToBase(currentBasePosition, closestBase);

				if (closestBase != nextBase)
				{
					// if (!anomalyBases.Contains(nextBase) && !nextBase.GetIsFringeBase())// && !nextBase.GetIsFringeBase())
					if (!anomalyBases.Contains(nextBase))// && !nextBase.GetIsFringeBase())// && !nextBase.GetIsFringeBase())
					{
						anomalyBases.Insert(nextBase);
					}
#ifdef WORKBENCH					
					PrintFormat("EstablishDynamicRadioRanges: Closest Base to %1 is actually %2 | Distance: %3", currentBase.GetBaseName(), closestBase.GetBaseName(), distanceToClosestBase);
#endif
				}
				else
				{
#ifdef WORKBENCH
//					PrintFormat("EstablishDynamicRadioRanges: Setting distance for %1 and %2 to %3", currentBase.GetBaseName(), closestBase.GetBaseName(), distanceToClosestBase);
#endif
				}
				
			}
			
			previousBase = currentBase;
			previousBasePosition = currentBasePosition;
		}

		foreach (SCR_CampaignMilitaryBaseComponent base : anomalyBases)
		{
#ifdef WORKBENCH
			PrintFormat("EstablishDynamicRadioRanges: Checking Anomaly Base for: %1", base.GetBaseName());
#endif
			vector basePosition = base.GetOwner().GetOrigin();
			array<SCR_CampaignMilitaryBaseComponent> closestBases;
			int closestBasesN = m_CampaignInfo.GetClosestBasesCheck();
			FindNextNClosestBases(closestBasesN, stringBases, base, basePosition, closestBases);

			float currentRange = base.GetRadioAntennaServiceRange();
			float minOutOfRangeDistance = 9999999.0; // A large initial value
			SCR_CampaignMilitaryBaseComponent targetClosestBase;
			
			// Step 1: Find the smallest distance greater than the current range
			foreach (SCR_CampaignMilitaryBaseComponent closestBase : closestBases)
			{
			    float currentDistance = GetDistanceToBase(basePosition, closestBase);
			    if (currentDistance > currentRange && currentDistance < minOutOfRangeDistance)
			    {
			        minOutOfRangeDistance = currentDistance;
			        targetClosestBase = closestBase; // Keep track of this base
			    }
			}
			
			// Step 2: Set radio ranges if a qualifying distance is found
			if (targetClosestBase)
			{
				m_iAdditionalRange = m_CampaignInfo.GetAdditionalAntennaRange();
			    float newRange = minOutOfRangeDistance + m_iAdditionalRange;
			    
				// Update the base's radio range
				if (newRange > base.GetRadioAntennaServiceRange())
				{
					base.SetRadioAntennaServiceRange(newRange);
#ifdef WORKBENCH
			    	PrintFormat("--EstablishDynamicRadioRanges: Set %1 range to %2", base.GetBaseName(), newRange);
#endif
				}
   
			    // Update the corresponding closestBase's radio range (if needed)
			    if (targetClosestBase.GetRadioAntennaServiceRange() < minOutOfRangeDistance)
			    {
			        targetClosestBase.SetRadioAntennaServiceRange(newRange);
#ifdef WORKBENCH
			        PrintFormat("--EstablishDynamicRadioRanges: Set %1 range to %2", targetClosestBase.GetBaseName(), newRange);
#endif
			    }				
			}					
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void GetHQBasesAtRange(out array<SCR_CampaignMilitaryBaseComponent> outBases)
	{
		SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
		if (!baseManager)
			return;
		
		array<SCR_MilitaryBaseComponent>bases = {};
		array<SCR_CampaignMilitaryBaseComponent>basesAtRange = {};
		baseManager.GetBases(bases);
	
		foreach (SCR_MilitaryBaseComponent base : bases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
			if (campaignBase && campaignBase.IsInitialized() && campaignBase.IsHQ() && campaignBase.GetType() != SCR_ECampaignBaseType.RELAY)
			{
				basesAtRange.Insert(campaignBase);
			}
		}
		
		outBases = basesAtRange;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetBasesAtRange(out array<SCR_CampaignMilitaryBaseComponent> outBases)
	{
		SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
		if (!baseManager)
			return 0;
		
		array<SCR_MilitaryBaseComponent>bases = {};
		array<SCR_CampaignMilitaryBaseComponent>basesAtRange = {};
		baseManager.GetBases(bases);
	
		foreach (SCR_MilitaryBaseComponent base : bases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base.GetOwner().FindComponent(SCR_CampaignMilitaryBaseComponent));
			if (campaignBase && campaignBase.IsInitialized() && !campaignBase.IsHQ() && campaignBase.GetType() != SCR_ECampaignBaseType.RELAY)
			{
				basesAtRange.Insert(campaignBase);
			}
		}
		
		outBases = basesAtRange;
		return basesAtRange.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDistanceToBase(vector currentPosition, SCR_MilitaryBaseComponent targetBase)
	{
		float distance;
		
		vector basePosition = targetBase.GetOwner().GetOrigin();
		distance = vector.DistanceXZ(currentPosition, basePosition);
		
		return distance;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent FindNextClosestBase(array<SCR_CampaignMilitaryBaseComponent>bases, SCR_CampaignMilitaryBaseComponent excludedBase, vector position)
	{
		SCR_CampaignMilitaryBaseComponent closestBase;
		SCR_CampaignMilitaryBaseComponent nextClosestBase;
		float closestBaseDistance = float.MAX;

		foreach (SCR_CampaignMilitaryBaseComponent base : bases)
		{
			if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY)
				continue;

			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);

			if (distance < closestBaseDistance)
			{
				closestBaseDistance = distance;
				closestBase = base;
			}
		}

		return closestBase;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent FindClosestEnemyBase(vector position, SCR_CampaignMilitaryBaseComponent excludedBase, Faction friendlyBaseFaction)
	{
		SCR_CampaignMilitaryBaseComponent closestBase;
		float closestBaseDistance = float.MAX;

		foreach (SCR_CampaignMilitaryBaseComponent base : m_aBases)
		{
			if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY || base.GetFaction() == friendlyBaseFaction)
				continue;

			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);

			if (distance < closestBaseDistance)
			{
				closestBaseDistance = distance;
				closestBase = base;
			}
		}

		return closestBase;
	}
	
	//------------------------------------------------------------------------------------------------
//	SCR_CampaignMilitaryBaseComponent FindClosestFriendlyBase(array<SCR_CampaignMilitaryBaseComponent>bases, SCR_CampaignMilitaryBaseComponent excludedBase, vector position, Faction baseFaction)
//	{
//		SCR_CampaignMilitaryBaseComponent closestBase;
//		SCR_CampaignMilitaryBaseComponent nextClosestBase;
//		float closestBaseDistance = float.MAX;
//
//		foreach (SCR_CampaignMilitaryBaseComponent base : bases)
//		{
//			if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY || base.GetFaction() != baseFaction)
//				continue;
//
//			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);
//
//			if (distance < closestBaseDistance)
//			{
//				closestBaseDistance = distance;
//				closestBase = base;
//			}
//		}
//
//		return closestBase;
//	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent FindClosestFriendlyBase(SCR_CampaignMilitaryBaseComponent excludedBase, vector position, Faction baseFaction)
	{
		SCR_CampaignMilitaryBaseComponent closestBase;
		SCR_CampaignMilitaryBaseComponent nextClosestBase;
		float closestBaseDistance = float.MAX;

		foreach (SCR_CampaignMilitaryBaseComponent base : m_aBases)
		{
			if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY || base.GetFaction() != baseFaction)
				continue;

			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);

			if (distance < closestBaseDistance)
			{
				closestBaseDistance = distance;
				closestBase = base;
			}
		}

		return closestBase;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent FindClosestFriendlyBaseV2(array<SCR_CampaignMilitaryBaseComponent>bases, SCR_CampaignMilitaryBaseComponent excludedBase, array<SCR_CampaignMilitaryBaseComponent>excludedBases, vector position, Faction baseFaction)
	{
		SCR_CampaignMilitaryBaseComponent closestBase;
		SCR_CampaignMilitaryBaseComponent nextClosestBase;
		float closestBaseDistance = float.MAX;

		foreach (SCR_CampaignMilitaryBaseComponent base : bases)
		{
			if (!base.IsInitialized() || base == excludedBase || excludedBases.Contains(base) || base.GetType() == SCR_ECampaignBaseType.RELAY || base.GetFaction() != baseFaction)
				continue;

			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);

			if (distance < closestBaseDistance)
			{
				closestBaseDistance = distance;
				closestBase = base;
			}
		}

		return closestBase;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_CampaignMilitaryBaseComponent FindNextClosestBases(array<SCR_CampaignMilitaryBaseComponent>bases, array<SCR_CampaignMilitaryBaseComponent> excludedBases, vector position)
	{
		SCR_CampaignMilitaryBaseComponent closestBase;
		SCR_CampaignMilitaryBaseComponent nextClosestBase;
		float closestBaseDistance = float.MAX;

		foreach (SCR_CampaignMilitaryBaseComponent base : bases)
		{
			if (!base.IsInitialized() || excludedBases.Contains(base) || base.GetType() == SCR_ECampaignBaseType.RELAY)
				continue;

			float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);

			if (distance < closestBaseDistance)
			{
				closestBaseDistance = distance;
				closestBase = base;
			}
		}

		return closestBase;
	}
	
	//------------------------------------------------------------------------------------------------
	void FindNextNClosestBases(int count, array<SCR_CampaignMilitaryBaseComponent>bases, SCR_CampaignMilitaryBaseComponent excludedBase, vector position, out array<SCR_CampaignMilitaryBaseComponent> outBases)
	{
		array<SCR_CampaignMilitaryBaseComponent> closestBases = {};
	    array<float> closestDistances = {};
	
	    foreach (SCR_CampaignMilitaryBaseComponent base : bases)
	    {
	        // Skip invalid, excluded, or relay bases
	        if (!base.IsInitialized() || base == excludedBase || base.GetType() == SCR_ECampaignBaseType.RELAY)
	            continue;
	
	        float distance = vector.DistanceSq(base.GetOwner().GetOrigin(), position);
	
	        if (closestBases.Count() < count)
	        {
	            closestBases.Insert(base);
	            closestDistances.Insert(distance);
	        }
	        else
	        {
	            // Find the index of the farthest base in the current set
	            int maxIndex = 0;
	            for (int i = 1; i < closestDistances.Count(); i++)
	            {
	                if (closestDistances[i] > closestDistances[maxIndex])
	                    maxIndex = i;
	            }
	
	            // If the new base is closer than the farthest, replace it
	            if (distance < closestDistances[maxIndex])
	            {
	                closestBases[maxIndex] = base;
	                closestDistances[maxIndex] = distance;
	            }
	        }
	    }
	
	    outBases = closestBases;
	}
}
