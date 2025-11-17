//------------------------------------------------------------------------------------------------
// Represents a grid cell with its influencing properties
class GridCell
{
    ref array<ref BaseInfluence> influences = new array<ref BaseInfluence>(); // Bases influencing this cell
    float centerX, centerZ; // Center of the cell in world space
    string factionKey;      // Dominant faction key
	Faction faction;
    float alpha;            // Alpha value for rendering
    int color;              // Precomputed color with alpha
    bool isFrontLine;       // True if cell is on a faction boundary
}

//------------------------------------------------------------------------------------------------
class BaseInfluence
{
    vector position;        // World position of the base
    string factionKey;      // Faction key for coloring
	Faction faction;
    float influenceRadius;  // Influence radius
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class Iron_FrontlineMapModule : SCR_MapModuleBase
{
    protected ref array<ref CanvasWidgetCommand> m_Commands = new array<ref CanvasWidgetCommand>(); // Draw commands
    protected CanvasWidget m_DrawCanvas;        // Canvas for rendering
    protected FactionManager m_FactionManager; // For faction colors
    protected ref array<ref BaseInfluence> m_Bases = new array<ref BaseInfluence>(); // Base data
    protected ref map<int, ref GridCell> m_GridCells = new map<int, ref GridCell>(); // Cached grid cells
	protected ref array<SCR_MilitaryBaseComponent> m_AllBases = {};
	protected SCR_GameModeCampaign m_CampaignInfo;
	protected bool m_bHideOutsideRange;
	protected bool m_bObfuscateOutsideRange;
	protected bool m_bAllowEnemyHQTerritory;
	
	[RplProp(onRplName:"TriggerUpdate")]
    protected bool m_BasesChanged = true;      						// Flag for base changes
	
    protected float m_GridMinX, m_GridMinZ, m_GridMaxX, m_GridMaxZ; // Grid bounds
    protected float m_LastZoom = 0.0;         						// Last known zoom level
    protected vector m_LastMapCenter = "0 0 0"; 					// Last known map center
    protected bool m_IsMapInteracting = false; 						// Flag for map interaction
	protected bool m_ShouldRefresh = false;
    protected float m_InteractionDelay = 0.0; 						// Delay to resume rendering (seconds)
	protected Faction m_PlayerFaction;
	protected bool m_bEnabled;

    // Configuration
    protected const float DEFAULT_INFLUENCE_RADIUS = 2000.0;   							// Influence radius (meters) (1500)
	protected Color m_EnemyColor = Color.FromRGBA(255, 0, 0, 255);
	protected static const int ENEMY_COLOR = Color.FromRGBA(255, 0, 0, 50).PackToInt(); // Default enemy color
    protected const float FACTION_COLOR_ALPHA = 200;           							// Transparency (0-255)
    protected const float GRID_CELL_SIZE = 200.0;              							// Grid cell size (meters) (300)
    protected const float GRID_CELL_HALF = GRID_CELL_SIZE / 2.0;
    protected const float INTERACTION_STOP_DELAY = 0.5;        							// Delay after interaction stops (seconds)
    protected const float GRID_BUFFER = 500.0;                 							// Buffer for grid bounds (meters)

    //------------------------------------------------------------------------------------------------
    override void OnMapOpen(MapConfiguration config)
    {
		if (!m_bEnabled)
		{
			return;
		}
		
        m_DrawCanvas = CanvasWidget.Cast(config.RootWidgetRef.FindAnyWidget(SCR_MapConstants.DRAWING_WIDGET_NAME));
        if (!m_DrawCanvas)
        {
            Print("Iron_FrontlineMapModule: OnMapOpen: Draw canvas not found", LogLevel.ERROR);
            return;
        }

        if (!m_FactionManager)
        {
            Print("Iron_FrontlineMapModule: OnMapOpen: Faction manager not found", LogLevel.ERROR);
            return;
        }

        // Initialize map state
        if (m_MapEntity)
        {
            m_LastZoom = m_MapEntity.GetCurrentZoom();
            m_LastMapCenter = m_MapEntity.GetCurrentPan();
			m_PlayerFaction = SCR_FactionManager.SGetLocalPlayerFaction();
        }

		if (m_BasesChanged)
		{
			CollectBases();
		}
		
		SetShouldRefresh(true);      
    }

    //------------------------------------------------------------------------------------------------
//    override void OnMapClose(MapConfiguration config)
//    {
//        SCR_MilitaryBaseSystem baseSystem = SCR_MilitaryBaseSystem.GetInstance();
//        if (baseSystem)
//            baseSystem.GetOnBaseFactionChanged().Remove(OnBaseCaptured);
//    }

    //------------------------------------------------------------------------------------------------
    private void CollectBases()
    {
		m_Bases.Clear();
		m_Commands.Clear();      

        SCR_GameModeCampaign gameMode = SCR_GameModeCampaign.GetInstance();
        if (!gameMode)
        {
            Print("Iron_FrontlineMapModule: CollectBases: GameModeCampaign not found", LogLevel.ERROR);
            return;
        }
		
		SCR_CampaignMilitaryBaseManager campaignBaseManager = gameMode.GetBaseManager();
        if (!campaignBaseManager)
        {
            Print("Iron_FrontlineMapModule: CollectBases: CampaignMilitaryBaseManager not found", LogLevel.ERROR);
            return;
        }

        if (!m_PlayerFaction)
        {
            Print("Iron_FrontlineMapModule: CollectBases: Player faction not found", LogLevel.ERROR);
            return;
        }

        // Collect bases
        foreach (SCR_MilitaryBaseComponent base : m_AllBases)
        {
            SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
            if (!campaignBase)
			{	
				continue;
			}

			// Relays don't really have influence, only extend range
			if (campaignBase.GetType() == SCR_ECampaignBaseType.RELAY)
			{
		        continue;
			}
			
            Faction faction = campaignBase.GetFaction();
            if (!faction)
			{
				continue;
			}

            IEntity owner = campaignBase.GetOwner();
            if (!owner)
			{
				continue;
			}

			// Too performance intensive to draw cells for the entire map
            bool isInRange = gameMode.GetBaseManager().IsEntityInFactionRadioSignal(owner, m_PlayerFaction);
            if (!isInRange && m_bHideOutsideRange)
			{
				continue;
			}

			if (campaignBase.IsHQ() && faction != m_PlayerFaction && !m_bAllowEnemyHQTerritory)
			{
				continue;
			}
		
            vector pos = owner.GetOrigin();
            if (pos[0] != pos[0] || pos[2] != pos[2])
            {
                Print("Iron_FrontlineMapModule: CollectBases: Invalid base position (x=" + pos[0] + ", z=" + pos[2] + ")", LogLevel.WARNING);
                continue;
            }

//			if (campaignBase.IsHQ() && faction == m_PlayerFaction)
//			{
//				SCR_CampaignMilitaryBaseComponent farthestEnemyBase = campaignBaseManager.FindFarthestEnemyBase(pos, campaignBase, faction);
//				vector farthestEnemyPos = farthestEnemyBase.GetOwner().GetOrigin();
//				
//				SCR_CampaignMilitaryBaseComponent closestEnemyToEnemyBase = campaignBaseManager.FindClosestEnemyBase(farthestEnemyPos, farthestEnemyBase, farthestEnemyBase.GetFaction());
//				vector closestEnemyPos = closestEnemyToEnemyBase.GetOwner().GetOrigin();
//				
//				vector farthestEnemyInfluencePos = closestEnemyPos - farthestEnemyPos;
//				float enemyRange = vector.DistanceXZ(closestEnemyPos, farthestEnemyInfluencePos);
//				
//				ref BaseInfluence influence = new BaseInfluence();
//	            influence.position = farthestEnemyPos;
//	            influence.factionKey = farthestEnemyBase.GetFaction().GetFactionKey();
//				influence.faction = farthestEnemyBase.GetFaction();
//	            influence.influenceRadius = enemyRange; //DEFAULT_INFLUENCE_RADIUS;
//	            m_Bases.Insert(influence);
//			}
			
			SCR_CampaignMilitaryBaseComponent enemyBase = campaignBaseManager.FindClosestEnemyBase(pos, campaignBase, faction);
//		    SCR_CampaignMilitaryBaseComponent friendlyBase = campaignBaseManager.FindClosestFriendlyBase(campaignBase, pos, faction);
			float closestEnemy = vector.DistanceXZ(enemyBase.GetOwner().GetOrigin(), pos);
			
			float range = DEFAULT_INFLUENCE_RADIUS; //campaignBase.GetRadioAntennaServiceRange();
			if (closestEnemy > range)
			{
				range = closestEnemy;
			}
			if (faction == m_PlayerFaction)
			{
				range = range * 0.8;
			}
			
			// Too performance intensive to draw cells for the entire map
//			bool isInRange = gameMode.GetBaseManager().IsEntityInFactionRadioSignal(owner, m_PlayerFaction);
            if (!isInRange && faction != m_PlayerFaction && m_bObfuscateOutsideRange && !m_bHideOutsideRange)
			{
				range = range * Math.RandomFloatInclusive(0.8,1.3);
				
				int rand = Math.RandomIntInclusive(0,2);
				switch (rand)
				{
					case 0:
					{
						pos[0] = pos[0] + (pos[0] * 0.20);
						pos[1] = pos[1] - (pos[1] * 0.20);
						pos[2] = pos[2] + (pos[2] * 0.20);
						break;
					}
					
					case 1:
					{
						pos[1] = pos[1] - (pos[1] * 0.20);
						pos[0] = pos[0] + (pos[0] * 0.20);
						pos[2] = pos[2] - (pos[2] * 0.20);
						break;
					}
					
					case 2:
					{
						pos[2] = pos[2] + (pos[2] * 0.20);
						pos[0] = pos[0] - (pos[0] * 0.20);
						pos[1] = pos[1] + (pos[1] * 0.20);
						break;
					}
				}				
			}
			
            ref BaseInfluence influence = new BaseInfluence();
            influence.position = pos;
            influence.factionKey = faction.GetFactionKey();
			influence.faction = faction;
            influence.influenceRadius = range; //DEFAULT_INFLUENCE_RADIUS;
            m_Bases.Insert(influence);
        }

        ComputeGridCells();
    }

    //------------------------------------------------------------------------------------------------
    private void ComputeGridCells()
    {
        if (m_Bases.IsEmpty())
        {
            Print("Iron_FrontlineMapModule: ComputeGridCells: No bases to compute grid cells from", LogLevel.WARNING);
            return;
        }
		
		if (!m_BasesChanged)
		{
			return;
		}
		
        // Clear existing cells
        m_GridCells.Clear();

        // Determine bounding box with buffer
		float minX = float.MAX, minZ = float.MAX, maxX = -float.MAX, maxZ = -float.MAX;
        foreach (BaseInfluence base : m_Bases)
        {
            float x = base.position[0];
            float z = base.position[2];
            float radius = base.influenceRadius;
            minX = Math.Min(minX, x - radius - GRID_BUFFER);
            minZ = Math.Min(minZ, z - radius - GRID_BUFFER);
            maxX = Math.Max(maxX, x + radius + GRID_BUFFER);
            maxZ = Math.Max(maxZ, z + radius + GRID_BUFFER);
        }

        // Snap to grid
        int minGridX = Math.Floor(minX / GRID_CELL_SIZE);
        int maxGridX = Math.Ceil(maxX / GRID_CELL_SIZE);
        int minGridZ = Math.Floor(minZ / GRID_CELL_SIZE);
        int maxGridZ = Math.Ceil(maxZ / GRID_CELL_SIZE);

        m_GridMinX = minGridX * GRID_CELL_SIZE;
        m_GridMaxX = (maxGridX + 1) * GRID_CELL_SIZE;
        m_GridMinZ = minGridZ * GRID_CELL_SIZE;
        m_GridMaxZ = (maxGridZ + 1) * GRID_CELL_SIZE;

        // Create grid cells
        int cellCount = 0;
        for (int gridX = minGridX; gridX <= maxGridX; gridX++)
        {
            for (int gridZ = minGridZ; gridZ <= maxGridZ; gridZ++)
            {
                int gridKey = gridX * 1000000 + gridZ;
                ref GridCell cell = new GridCell();
                cell.centerX = gridX * GRID_CELL_SIZE + GRID_CELL_HALF;
                cell.centerZ = gridZ * GRID_CELL_SIZE + GRID_CELL_HALF;
                cell.isFrontLine = false;				
                m_GridCells.Insert(gridKey, cell);
                cellCount++;
            }
        }

        // Assign influences
        foreach (BaseInfluence base : m_Bases)
        {			
            float cx = base.position[0];
            float cz = base.position[2];
            float r = base.influenceRadius;

            int baseMinGridX = Math.Floor((cx - r) / GRID_CELL_SIZE);
            int baseMaxGridX = Math.Ceil((cx + r) / GRID_CELL_SIZE);
            int baseMinGridZ = Math.Floor((cz - r) / GRID_CELL_SIZE);
            int baseMaxGridZ = Math.Ceil((cz + r) / GRID_CELL_SIZE);
			
            for (int gridX = baseMinGridX; gridX <= baseMaxGridX; gridX++)
            {
                for (int gridZ = baseMinGridZ; gridZ <= baseMaxGridZ; gridZ++)
                {
                    int gridKey = gridX * 1000000 + gridZ;
                    if (!m_GridCells.Contains(gridKey)) continue;

                    GridCell cell = m_GridCells.Get(gridKey);
                    float dx = cell.centerX - cx;
                    float dz = cell.centerZ - cz;
                    float dist = Math.Sqrt(dx * dx + dz * dz);
                    if (dist <= r)
                        cell.influences.Insert(base);
                }
            }
        }

        // Compute dominant faction
        foreach (GridCell cell : m_GridCells)
        {
            if (cell.influences.IsEmpty())
            {
                cell.factionKey = "";
                continue;
            }

            BaseInfluence closestBase = null;
            float minDist = float.MAX;
            ref map<string, int> factionCount = new map<string, int>();

            foreach (BaseInfluence influence : cell.influences)
            {
                float dx = cell.centerX - influence.position[0];
                float dz = cell.centerZ - influence.position[2];
                float dist = Math.Sqrt(dx * dx + dz * dz);

                string factionKey = influence.factionKey;
                int count = factionCount.Get(factionKey);
                if (!count)
                    count = 0;
                factionCount.Set(factionKey, count + 1);

                if (dist < minDist)
                {
                    minDist = dist;
                    closestBase = influence;
                }
            }

            if (!closestBase)
            {
                cell.factionKey = "";
                continue;
            }

            cell.factionKey = closestBase.factionKey;
			cell.faction = closestBase.faction;
            float alpha = FACTION_COLOR_ALPHA;
            if (factionCount.Count() > 1)
                alpha = FACTION_COLOR_ALPHA; // * 0.7; // Dim for contested areas (0.8)			
			
            cell.alpha = alpha;
            Faction faction = m_FactionManager.GetFactionByKey(cell.factionKey);
            if (!faction)
            {
                Print("Iron_FrontlineMapModule: ComputeGridCells: Faction not found for key=" + cell.factionKey, LogLevel.WARNING);
                cell.factionKey = "";
                continue;
            }
            Color color = Color.FromRGBA(255, 0, 0, 255); //faction.GetFactionColor();
            if (!color)
            {
                Print("Iron_FrontlineMapModule: ComputeGridCells: Invalid faction color for " + cell.factionKey, LogLevel.WARNING);
                cell.factionKey = "";
                continue;
            }
            color.SetA(cell.alpha);
            cell.color = color.PackToInt(); 
        }

        // Mark front line cells
        int frontLineCount = 0;
        foreach (GridCell cell : m_GridCells)
        {
            if (cell.factionKey.IsEmpty()) continue;

            int gridX = Math.Floor(cell.centerX / GRID_CELL_SIZE);
            int gridZ = Math.Floor(cell.centerZ / GRID_CELL_SIZE);
            int gridKey = gridX * 1000000 + gridZ;

            // Check 8 neighbors (including diagonals)
            int neighbors[8] = {
                gridX * 1000000 + (gridZ + 1),         // Up
                gridX * 1000000 + (gridZ - 1),         // Down
                (gridX - 1) * 1000000 + gridZ,         // Left
                (gridX + 1) * 1000000 + gridZ,         // Right
                (gridX - 1) * 1000000 + (gridZ + 1),   // Up-left
                (gridX + 1) * 1000000 + (gridZ + 1),   // Up-right
                (gridX - 1) * 1000000 + (gridZ - 1),   // Down-left
                (gridX + 1) * 1000000 + (gridZ - 1)    // Down-right
            };

            cell.isFrontLine = false;
            foreach (int neighborKey : neighbors)
            {
                GridCell neighbor = m_GridCells.Get(neighborKey);
                if (!neighbor || (neighbor.factionKey != cell.factionKey && !neighbor.factionKey.IsEmpty()) || (neighbor && neighbor.factionKey.IsEmpty()))
                {
                    cell.isFrontLine = true;
                    break;
                }
            }

            // Mark cells with multiple influences as front lines
            if (cell.influences.Count() > 1)
            {
                ref map<string, int> factionCount = new map<string, int>();
                foreach (BaseInfluence influence : cell.influences)
                {
                    int count = factionCount.Get(influence.factionKey);
                    if (!count)
                        count = 0;
                    factionCount.Set(influence.factionKey, count + 1);
                }
				
                if (factionCount.Count() > 1)
                    cell.isFrontLine = true;
            }

            if (cell.isFrontLine)
            {
                frontLineCount++;
            }
        }
		
		m_BasesChanged = false;
    }

    //------------------------------------------------------------------------------------------------
    private array<float> GenerateQuadVertices(float centerX, float centerZ)
    {
        if (!m_MapEntity)
        {
            Print("Iron_FrontlineMapModule: GenerateQuadVertices: Map entity not available", LogLevel.WARNING);
            return new array<float>();
        }

        float zoom = m_MapEntity.GetCurrentZoom();
        vector pan = m_MapEntity.GetCurrentPan();
		
        // Fixed quad size
        float quadSize = GRID_CELL_SIZE;
        float quadHalf = GRID_CELL_HALF;

        // Define quad corners
        float x1 = centerX - quadHalf; // Top-left
        float z1 = centerZ - quadHalf;
        float x2 = centerX + quadHalf; // Top-right
        float z2 = centerZ - quadHalf;
        float x3 = centerX + quadHalf; // Bottom-right
        float z3 = centerZ + quadHalf;
        float x4 = centerX - quadHalf; // Bottom-left
        float z4 = centerZ + quadHalf;

        int sx1, sy1, sx2, sy2, sx3, sy3, sx4, sy4;
        m_MapEntity.WorldToScreen(x1, z1, sx1, sy1, true);
        m_MapEntity.WorldToScreen(x2, z2, sx2, sy2, true);
        m_MapEntity.WorldToScreen(x3, z3, sx3, sy3, true);
        m_MapEntity.WorldToScreen(x4, z4, sx4, sy4, true);

        // Clamp to prevent out-of-bounds rendering
        sx1 = Math.Clamp(sx1, -10000, 10000);
        sy1 = Math.Clamp(sy1, -10000, 10000);
        sx2 = Math.Clamp(sx2, -10000, 10000);
        sy2 = Math.Clamp(sy2, -10000, 10000);
        sx3 = Math.Clamp(sx3, -10000, 10000);
        sy3 = Math.Clamp(sy3, -10000, 10000);
        sx4 = Math.Clamp(sx4, -10000, 10000);
        sy4 = Math.Clamp(sy4, -10000, 10000);

        ref array<float> vertices = new array<float>();
        vertices.Insert(sx1); vertices.Insert(sy1); // v1
        vertices.Insert(sx2); vertices.Insert(sy2); // v2
        vertices.Insert(sx3); vertices.Insert(sy3); // v3
        vertices.Insert(sx1); vertices.Insert(sy1); // v1
        vertices.Insert(sx3); vertices.Insert(sy3); // v3
        vertices.Insert(sx4); vertices.Insert(sy4); // v4

        return vertices;
    }

    //------------------------------------------------------------------------------------------------
    private bool IsMapInteracting(float timeSlice)
    {
        if (!m_MapEntity)
            return false;

        float currentZoom = m_MapEntity.GetCurrentZoom();
        vector currentCenter = m_MapEntity.GetCurrentPan();

        bool isInteracting = false;
        if (currentZoom != m_LastZoom || currentCenter != m_LastMapCenter)
        {
            isInteracting = true;
            m_InteractionDelay = INTERACTION_STOP_DELAY;
        }
        else if (m_InteractionDelay > 0)
        {
            m_InteractionDelay -= timeSlice;
            isInteracting = true;
        }

        m_LastZoom = currentZoom;
        m_LastMapCenter = currentCenter;
        return isInteracting;
    }

	//------------------------------------------------------------------------------------------------
	private void SetShouldRefresh(bool value)
	{
		m_ShouldRefresh = value;
	}
	
    //------------------------------------------------------------------------------------------------
    override void Update(float timeSlice)
    {
		if (!m_bEnabled)
		{
			return;
		}
		
        if (!m_DrawCanvas || m_GridCells.IsEmpty())
        {
            m_DrawCanvas.SetDrawCommands(null);
            return;
        }

        // Hide grid during map interaction
        m_IsMapInteracting = IsMapInteracting(timeSlice);
        if (m_IsMapInteracting)
        {
			m_DrawCanvas.SetDrawCommands(null);
			SetShouldRefresh(true);
			return;
        }
		
		if (!m_ShouldRefresh)
		{
			return;
		}

        // Regenerate draw commands every frame
        m_Commands.Clear();

        // Use grid bounds to avoid culling issues
		vector worldMins, worldMaxs;
		vector worldSize = m_MapEntity.Size();
		m_MapEntity.GetWorldBounds(worldMins, worldMaxs);
		
		float visMinX = 0;
        float visMaxX = worldSize[0];
        float visMinZ = 0;
        float visMaxZ = worldSize[2];

        if (m_MapEntity)
        {
            vector mapCenter = m_MapEntity.GetCurrentPan();
            float zoom = m_MapEntity.GetCurrentZoom();
        }

        int minGridX = Math.Floor(visMinX / GRID_CELL_SIZE);
        int maxGridX = Math.Ceil(visMaxX / GRID_CELL_SIZE);
        int minGridZ = Math.Floor(visMinZ / GRID_CELL_SIZE);
        int maxGridZ = Math.Ceil(visMaxZ / GRID_CELL_SIZE);

        int renderedCells = 0;
        foreach (int gridKey, GridCell cell : m_GridCells)
        {
            // Skip if no faction or if is in players faction
            if (cell.factionKey.IsEmpty() || cell.faction == m_PlayerFaction)
                continue;

            // Check if cell is in visible bounds
            int gridX = Math.Floor(cell.centerX / GRID_CELL_SIZE);
            int gridZ = Math.Floor(cell.centerZ / GRID_CELL_SIZE);
            if (gridX < minGridX || gridX > maxGridX || gridZ < minGridZ || gridZ > maxGridZ)
                continue;

            // Generate vertices for the cell
            array<float> vertices = GenerateQuadVertices(cell.centerX, cell.centerZ);
            if (vertices.IsEmpty())
            {
                continue;
            }
			
            // Create two triangles for the quad
            ref PolygonDrawCommand cmd1 = new PolygonDrawCommand();
            cmd1.m_Vertices = {vertices[0], vertices[1], vertices[2], vertices[3], vertices[4], vertices[5]};
            cmd1.m_iColor = cell.color; // factionColor
            m_Commands.Insert(cmd1);

            ref PolygonDrawCommand cmd2 = new PolygonDrawCommand();
            cmd2.m_Vertices = {vertices[6], vertices[7], vertices[8], vertices[9], vertices[10], vertices[11]};
            cmd2.m_iColor = cell.color; // factionColor
            m_Commands.Insert(cmd2);

            renderedCells++;
        }

        // Apply or clear draw commands
        if (!m_Commands.IsEmpty())
		{
			m_DrawCanvas.SetDrawCommands(m_Commands);
			SetShouldRefresh(false);
		}           
        else
            m_DrawCanvas.SetDrawCommands(null);
    }

    //------------------------------------------------------------------------------------------------
    void OnBaseCaptured(SCR_MilitaryBaseComponent base, Faction faction)
    {
//        m_BasesChanged = true;
//		if (m_MapEntity.IsOpen())
//		{
//			CollectBases();
//		}
//				
//		SetShouldRefresh(true);
				
		TriggerUpdate();
    }
	
	//------------------------------------------------------------------------------------------------
	void OnRadioCoverageChanged()
	{
		TriggerUpdate();
	}
	
	//------------------------------------------------------------------------------------------------
	void TriggerUpdate()
	{
		m_BasesChanged = true;
		if (m_MapEntity.IsOpen())
		{
			CollectBases();
		}
				
		SetShouldRefresh(true);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Init method for cases where all modules and components should be loaded already so constructor cannot be used, called once after creation
	override void Init()
	{
		m_FactionManager = GetGame().GetFactionManager();
		m_CampaignInfo = SCR_GameModeCampaign.GetInstance();
		
		if (m_CampaignInfo)
		{
			m_bEnabled = m_CampaignInfo.GetEnableFrontlineMapModule();
			if (!m_bEnabled)
			{
				return;
			}
			
			m_bAllowEnemyHQTerritory = m_CampaignInfo.GetAllowEnemyHQTerritory();
			m_bHideOutsideRange = m_CampaignInfo.GetHideEnemyTerritoryOutsideRadioRange();
			m_bObfuscateOutsideRange = m_CampaignInfo.GetObfuscateTerritoryOutsideRadioRange();
		}		

		SCR_MilitaryBaseSystem baseSystem = SCR_MilitaryBaseSystem.GetInstance();
        if (baseSystem)
            baseSystem.GetOnBaseFactionChanged().Insert(OnBaseCaptured);
		
		SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
        if (!baseManager)
        {
            Print("Iron_FrontlineMapModule: Init: MilitaryBaseSystem not found", LogLevel.ERROR);
            return;
        }
		        
        if (m_AllBases.IsEmpty())
        {
			baseManager.GetBases(m_AllBases);
			if (!m_AllBases)
			{
				Print("Iron_FrontlineMapModule: Init: No bases found", LogLevel.WARNING);
            	return;
			}            
        }
		
		foreach (SCR_MilitaryBaseComponent base : m_AllBases)
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
			if (!campaignBase)
			{
				continue;
			}
			
			SCR_CoverageRadioComponent radioComponent = SCR_CoverageRadioComponent.Cast(campaignBase.GetOwner().FindComponent(SCR_CoverageRadioComponent));
			if (campaignBase && radioComponent)
			{
				radioComponent.GetOnCoverageChanged().Insert(OnRadioCoverageChanged);
			}
		}
	}
}