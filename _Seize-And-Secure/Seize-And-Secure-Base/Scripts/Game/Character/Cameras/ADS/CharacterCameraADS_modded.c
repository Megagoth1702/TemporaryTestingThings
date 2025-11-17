// *************************************************************************************
// ! CharacterCameraADS - Aim down sights camera
// *************************************************************************************
modded class CharacterCameraADS extends CharacterCameraBase
{
	// Sway parameters for each stance
    private const float SWAY_AMP_X_STANDING = 0.0045;		// 0.0008 // Horizontal sway range
    private const float SWAY_AMP_X_CROUCHING = 0.004;		// 0.0005
    private const float SWAY_AMP_X_PRONE = 0.003;			// 0.0001
	
    private const float SWAY_AMP_Y_STANDING = 0.0045;		// 0.001   // Vertical sway range
    private const float SWAY_AMP_Y_CROUCHING = 0.0035;		// 0.0008
    private const float SWAY_AMP_Y_PRONE = 0.003;			// 0.0001
	
	private const float SWAY_LERP_SPEED_MIN_STANDING = 0.7;		// 1.5 - 4.0  // Min transition speed
    private const float SWAY_LERP_SPEED_MAX_STANDING = 1.7;		// 2.5 - 6.0  // Max transition speed
    private const float SWAY_LERP_SPEED_MIN_CROUCHING = 0.5;	// 1.0 - 2.5
    private const float SWAY_LERP_SPEED_MAX_CROUCHING = 1.5;	// 2.0 - 3.5
    private const float SWAY_LERP_SPEED_MIN_PRONE = 0.5;		// 0.1 - 0.8
    private const float SWAY_LERP_SPEED_MAX_PRONE = 1.0;		// 0.5 - 1.2

	// Pause duration ranges at targets (in seconds)
    private const float PAUSE_DURATION_MIN_STANDING = 0;  		// 0 	// Min pause time
    private const float PAUSE_DURATION_MAX_STANDING = 0.02;  	// 0.1 // Max pause time
    private const float PAUSE_DURATION_MIN_CROUCHING = 0;
    private const float PAUSE_DURATION_MAX_CROUCHING = 0.02;
    private const float PAUSE_DURATION_MIN_PRONE = 0;
    private const float PAUSE_DURATION_MAX_PRONE = 0.02;
	
	private float currentPauseDurationMin = PAUSE_DURATION_MIN_STANDING;
    private float currentPauseDurationMax = PAUSE_DURATION_MAX_STANDING;
    private float targetPauseDurationMin = PAUSE_DURATION_MIN_STANDING;
    private float targetPauseDurationMax = PAUSE_DURATION_MAX_STANDING;
	
	// Pause parameters
    private float pauseTimer = 0.0;  	// Time remaining for pause
    private float pauseDuration = 0.0;  // Randomized pause duration for current target
	private float pauseDurationX = 0.0;
	private float pauseDurationY = 0.0;
	private float pauseTimerX = 0.0;
	private float pauseTimerY = 0.0;
	
    // Current and target sway values
	private float currentSwayAmpX = SWAY_AMP_X_STANDING;
    private float currentSwayAmpY = SWAY_AMP_Y_STANDING;
    private float targetSwayAmpX = SWAY_AMP_X_STANDING;
    private float targetSwayAmpY = SWAY_AMP_Y_STANDING;
    private float currentSwayLerpSpeedMin = SWAY_LERP_SPEED_MIN_STANDING;
    private float currentSwayLerpSpeedMax = SWAY_LERP_SPEED_MAX_STANDING;
    private float targetSwayLerpSpeedMin = SWAY_LERP_SPEED_MIN_STANDING;
    private float targetSwayLerpSpeedMax = SWAY_LERP_SPEED_MAX_STANDING;
    private float currentRandomLerpSpeed = SWAY_LERP_SPEED_MIN_STANDING; // Current randomized speed
    private float currentRandomLerpSpeedX = SWAY_LERP_SPEED_MIN_STANDING;
	private float currentRandomLerpSpeedY = SWAY_LERP_SPEED_MIN_STANDING;

	private float swayOffsetX = 0.0;
    private float swayOffsetY = 0.0;
    private float targetSwayX = 0.0;
    private float targetSwayY = 0.0;

    // Constants
    private const float SWAY_THRESHOLD = 0.0008; 	// 0.0001 - 0.0008 // Distance to pick new target
    private float SWAY_THRESHOLD_MIN = 0.0001;
	private float SWAY_THRESHOLD_MAX = 0.0008;
	private const float BASE_SWAY_FACTOR = 0.4;  	// 0.25 min - 0.5	// Minimum sway amplitude at full stamina
    private const float INTERP_SPEED = 2;      		// 5.0	// Transition speed between stances
    private const float STAMINA_SPEED_FACTOR = 1.5; // 1.5	// Max speed multiplier at low stamina

	float interpSpeedMin = 2;
	float interpSpeedMax = 10;
	
    // Cached state
    private ECharacterStance m_CachedStance;
    private float m_CachedStamina;
    private bool m_CachedIsWeaponRested;
    private bool m_CachedIsBipodDeployed;
    private ECharacterStance m_LastStance = ECharacterStance.STAND;

	//------------------------------------------------------------------------------------------------
    override void OnUpdate(float pDt, out ScriptedCameraItemResult pOutResult)
    {
        super.OnUpdate(pDt, pOutResult);

        // Update cached values (stance, stamina, etc.)
//		DebugMeBro();
        UpdateCachedValues();

        // Detect stance change and update sway targets
        if (m_CachedStance != m_LastStance)
        {
            UpdateSwayTargets(m_CachedStance);
            m_LastStance = m_CachedStance;
			currentRandomLerpSpeedX = Math.RandomFloatInclusive(currentSwayLerpSpeedMin, currentSwayLerpSpeedMax);
			currentRandomLerpSpeedY = Math.RandomFloatInclusive(currentSwayLerpSpeedMin, currentSwayLerpSpeedMax);
        }

        // Interpolate sway parameters
		float interpSpeed = Math.RandomFloatInclusive(interpSpeedMin, interpSpeedMax);
        currentSwayAmpX = Math.Lerp(currentSwayAmpX, targetSwayAmpX, pDt * interpSpeed);
        currentSwayAmpY = Math.Lerp(currentSwayAmpY, targetSwayAmpY, pDt * interpSpeed);
        currentSwayLerpSpeedMin = Math.Lerp(currentSwayLerpSpeedMin, targetSwayLerpSpeedMin, pDt * interpSpeed);
        currentSwayLerpSpeedMax = Math.Lerp(currentSwayLerpSpeedMax, targetSwayLerpSpeedMax, pDt * interpSpeed);
		currentPauseDurationMin = Math.Lerp(currentPauseDurationMin, targetPauseDurationMin, pDt * interpSpeed);
      	currentPauseDurationMax = Math.Lerp(currentPauseDurationMax, targetPauseDurationMax, pDt * interpSpeed);

        // Calculate stamina factors
        float staminaAmpFactor = Math.Clamp(BASE_SWAY_FACTOR + (1.0 - m_CachedStamina), 0, 0.75); // 0.5 to 1.5 for amplitude
        float staminaSpeedFactor = 1.0 + (1.0 - m_CachedStamina) * (STAMINA_SPEED_FACTOR - 1.0); // 1.0 to 1.5 for speed

        // Apply rest modifiers
        float restModifier = 1.0;
        if (m_CachedIsWeaponRested) restModifier *= 0.5;
        if (m_CachedIsBipodDeployed) restModifier *= 0.1;

        // Set effective sway range
        float maxSwayX = currentSwayAmpX * staminaAmpFactor * restModifier;
        float maxSwayY = currentSwayAmpY * staminaAmpFactor * restModifier;

        // Check if sway is close to target
		float swayThreshold = Math.RandomFloatInclusive(SWAY_THRESHOLD_MIN, SWAY_THRESHOLD_MAX);
		
		// Update pause timer if active
        if (pauseTimerX >= 0.0)
        {
            pauseTimerX -= pDt;
            if (pauseTimerX <= 0.0)
            {
                // Pause ended, pick new target and speed
                targetSwayX = Math.RandomFloatInclusive(-maxSwayX, maxSwayX);
                currentRandomLerpSpeedX = Math.RandomFloatInclusive(currentSwayLerpSpeedMin, currentSwayLerpSpeedMax);
                pauseDurationX = Math.RandomFloatInclusive(currentPauseDurationMin, currentPauseDurationMax);
            }
        }
        else
        {
            // Check if sway is close to target
            float dx = targetSwayX - swayOffsetX;
            if (dx * dx < swayThreshold * swayThreshold)
            {
                // Start pause before picking new target
                pauseTimerX = pauseDurationX;
            }
            else
            {
                // Interpolate towards target, faster when stamina is low
                float effectiveLerpSpeed = currentRandomLerpSpeedX * staminaSpeedFactor;
                float lerpFactorX = Math.Clamp(pDt * effectiveLerpSpeed, 0.0, 1.0); // Safe lerp factor
                swayOffsetX = Math.Lerp(swayOffsetX, targetSwayX, lerpFactorX);
            }
        }
		
		if (pauseTimerY >= 0.0)
        {
            pauseTimerY -= pDt;
            if (pauseTimerY <= 0.0)
            {
                // Pause ended, pick new target and speed
                targetSwayY = Math.RandomFloatInclusive(-maxSwayX, maxSwayX);
                currentRandomLerpSpeedY = Math.RandomFloatInclusive(currentSwayLerpSpeedMin, currentSwayLerpSpeedMax);
                pauseDurationY = Math.RandomFloatInclusive(currentPauseDurationMin, currentPauseDurationMax);
            }
        }
        else
        {
            // Check if sway is close to target
            float dy = targetSwayY - swayOffsetY;
            if (dy * dy < swayThreshold * swayThreshold)
            {
                // Start pause before picking new target
                pauseTimerY = pauseDurationY;
            }
            else
            {
                // Interpolate towards target, faster when stamina is low
                float effectiveLerpSpeed = currentRandomLerpSpeedY * staminaSpeedFactor;
                float lerpFactorY = Math.Clamp(pDt * effectiveLerpSpeed, 0.0, 1.0); // Safe lerp factor
                swayOffsetY = Math.Lerp(swayOffsetY, targetSwayY, lerpFactorY);
            }
        }

        // Apply sway to camera position
        vector originalCameraPosition = pOutResult.m_CameraTM[3];
        vector cameraTransform = originalCameraPosition;
        cameraTransform[0] = cameraTransform[0] + swayOffsetX; // Horizontal sway
        cameraTransform[1] = cameraTransform[1] + swayOffsetY; // Vertical sway
        pOutResult.m_CameraTM[3] = cameraTransform;
    }

	//------------------------------------------------------------------------------------------------
    private void UpdateCachedValues()
    {
        IEntity entity = SCR_PlayerController.GetLocalControlledEntity();
        if (!entity) return;

        ChimeraCharacter character = ChimeraCharacter.Cast(entity);
        if (!character) return;

        CharacterControllerComponent controller = character.GetCharacterController();
        if (!controller) return;

        m_CachedStance = controller.GetStance();
        m_CachedIsBipodDeployed = controller.GetIsWeaponDeployedBipod();
        m_CachedIsWeaponRested = controller.GetIsWeaponDeployed();
        m_CachedStamina = Math.Clamp(controller.GetStamina(), 0.01, 1.0);
    }
	
    //------------------------------------------------------------------------------------------------
    private void UpdateSwayTargets(ECharacterStance stance)
    {
        switch (stance)
        {
            case ECharacterStance.STAND:
                targetSwayAmpX = SWAY_AMP_X_STANDING * RankFactor();
                targetSwayAmpY = SWAY_AMP_Y_STANDING * RankFactor();
                targetSwayLerpSpeedMin = SWAY_LERP_SPEED_MIN_STANDING;
                targetSwayLerpSpeedMax = SWAY_LERP_SPEED_MAX_STANDING;
				SWAY_THRESHOLD_MIN = 0.0001;
				SWAY_THRESHOLD_MAX = 0.0008;
				interpSpeedMin = 0.1;
				interpSpeedMax = 1;
				targetPauseDurationMin = PAUSE_DURATION_MIN_STANDING;
              	targetPauseDurationMax = PAUSE_DURATION_MAX_STANDING;
                break;
            case ECharacterStance.CROUCH:
                targetSwayAmpX = SWAY_AMP_X_CROUCHING * RankFactor();
                targetSwayAmpY = SWAY_AMP_Y_CROUCHING * RankFactor();
                targetSwayLerpSpeedMin = SWAY_LERP_SPEED_MIN_CROUCHING;
                targetSwayLerpSpeedMax = SWAY_LERP_SPEED_MAX_CROUCHING;
				SWAY_THRESHOLD_MIN = 0.0006;
				SWAY_THRESHOLD_MAX = 0.001;
				interpSpeedMin = 2;
				interpSpeedMax = 5;
				targetPauseDurationMin = PAUSE_DURATION_MIN_CROUCHING;
              	targetPauseDurationMax = PAUSE_DURATION_MAX_CROUCHING;
                break;
            case ECharacterStance.PRONE:
                targetSwayAmpX = SWAY_AMP_X_PRONE * RankFactor();
                targetSwayAmpY = SWAY_AMP_Y_PRONE * RankFactor();
                targetSwayLerpSpeedMin = SWAY_LERP_SPEED_MIN_PRONE;
                targetSwayLerpSpeedMax = SWAY_LERP_SPEED_MAX_PRONE;
				SWAY_THRESHOLD_MIN = 0.0008;
				SWAY_THRESHOLD_MAX = 0.001;
				interpSpeedMin = 1;
				interpSpeedMax = 2.5;
				targetPauseDurationMin = PAUSE_DURATION_MIN_PRONE;
              	targetPauseDurationMax = PAUSE_DURATION_MAX_PRONE;
                break;
        }
    }
	
	//------------------------------------------------------------------------------------------------
	private float RankFactor()
	{
		float rankFactor;
		IEntity entity = SCR_PlayerController.GetLocalControlledEntity();
		if (!entity)
			return 1;
		
		SCR_ECharacterRank rank = SCR_CharacterRankComponent.GetCharacterRank(entity);
		if (!rank)
			return 1;
		
		switch (rank)
		{
			case SCR_ECharacterRank.RENEGADE:
			{
				rankFactor = 1.0;
				break;
			}
			case SCR_ECharacterRank.PRIVATE:
			{
				rankFactor = 1.0;
				break;
			}
			case SCR_ECharacterRank.CORPORAL:
			{
				rankFactor = 0.8;
				break;
			}
			case SCR_ECharacterRank.SERGEANT:
			{
				rankFactor = 0.7;
				break;
			}
			case SCR_ECharacterRank.LIEUTENANT:
			{
				rankFactor = 0.6;
				break;
			}
			case SCR_ECharacterRank.CAPTAIN:
			{
				rankFactor = 0.5;
				break;
			}
			case SCR_ECharacterRank.MAJOR:
			{
				rankFactor = 0.4;
				break;
			}
			default:
			{
				rankFactor = 0.1;
				break;
			}
		}
		
		return rankFactor;
	}
	
	//------------------------------------------------------------------------------------------------
	void DebugMeBro()
	{
		float ampX = targetSwayX - swayOffsetX;
		float ampY = targetSwayY - swayOffsetY;
//		DbgUI.Begin("Bearded UI Window");		
		DbgUI.Text("Rank Factor: " + RankFactor().ToString());
		DbgUI.Text("Stamina: " + m_CachedStamina.ToString());
		DbgUI.Text("Lerp Speed: " + currentRandomLerpSpeed.ToString());
		DbgUI.Text("Amp X: " + ampX.ToString());
		DbgUI.Text("Amp Y: " + ampY.ToString());
		DbgUI.Text("Pause X: " + pauseTimerX.ToString());
		DbgUI.Text("Pause Y: " + pauseTimerY.ToString());
//		DbgUI.SliderFloat("Blur Slider", m_fBlurX, 0, 1000);
	}
};
