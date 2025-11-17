enum PainEffectState
{
    INACTIVE,
    PULSING,
    RESTING
}

class IRON_SUP_ScreenEffects : SCR_BaseScreenEffect
{	
	[Attribute(defvalue: "0.111", uiwidget: UIWidgets.EditBox, desc: "Blur intensity multiplier")] // 0.111
	protected float m_fSuppressionBlurMultiplier;
	
	[Attribute(defvalue: "0.03", uiwidget: UIWidgets.EditBox, desc: "Blur size min value")] // 0.03
	protected float m_fSuppressionBlurSizeMin;
	
	[Attribute(defvalue: "0.320", uiwidget: UIWidgets.EditBox, desc: "Blur size max value")] // 0.420
	protected float m_fSuppressionBlurSizeMax;
	
	[Attribute(defvalue: "0.400", uiwidget: UIWidgets.EditBox)] // 0.400
	protected float m_fSuppressionVignetteMultiplier;
	
	[Attribute(defvalue: "0.350", uiwidget: UIWidgets.EditBox)]
	protected float m_fSuppressionContrastOffset;
	
	[Attribute(defvalue: "0.886", uiwidget: UIWidgets.EditBox)]
	protected float m_fSuppressionSaturation; // 0.886
	
	[Attribute("0 0 1 1", UIWidgets.GraphDialog, params: "1 1 0 0")]
	protected ref Curve m_cSuppressionCurve;
	
	[Attribute(defvalue: "6", uiwidget: UIWidgets.EditBox, desc: "Blur size max value")]
	protected float m_fSuppressionSmoothness;
	
	IEntity m_PlayerEntity;
	
	protected float m_fSuppressionMultiplier = 1;
	protected float m_fTargetBlurX;
	protected float m_fBlurSmoothSpeed;
	protected float m_fSuppressionTarget = 0;
	protected float m_fSuppression = 0;
	
	protected static bool s_bEnableRadialBlur = false;
	protected static float s_fBlurriness = 0;
	protected static float s_fBlurrinessSize = 0;
	
	protected static bool s_bEnableColorEffect = false;
	protected static float s_fContrast = 1;
	protected static float s_fColorContrast = 1;
	protected static float s_fColorBrightness = 1;
	protected static float s_fFOVSaturation = 1;
	protected float s_fFOVEffect;
	
	protected CameraManager m_cCameraManager;
	protected BaseContainer m_cFovSettings;
	protected float m_fDefaultCameraFOV;
	protected float m_fFov;
	protected float m_fSuppressionFOV;
	protected float m_fSuppressionFOVMultiplier = 1;
	protected static bool s_bPainEnabled;
	protected float m_fPainLevel;
	protected static float m_fPainIntensity;
	protected float m_fPainEffectDelay;
	private static float s_fChromAberPower;
	
	protected ChimeraCharacter m_cCharacterEntity;
	protected SCR_PlayerController m_pPlayerController;
	
	protected const string m_sRadialBlurMaterialName 		= "{875B2E77E09A1453}UI/Materials/Suppression_ScreenEffects_BlurPP.emat";
	protected static const int m_sRadialBlurPriority 		= 18; // 18
	
	protected const string m_sColorMaterialName 			= "{12959ADAFFD69CF2}UI/Materials/Suppression_ScreenEffects_ColorPP.emat";
	protected static const int m_sColorPriority 			= 10; // 10
	
	protected const string m_sChromeAberrationMaterialName 	= "{B8FEA02AA6D7EA51}UI/Materials/Suppression_ScreenEffects_ChromAberrPP.emat";
	protected static const int m_sChromeAberrationPriority 	= 7; // 15
	
	protected const string m_sPainBlurMaterialName 			= "{E94A3633C7236BF6}UI/Materials/Suppression_Pain_ScreenEffects_GaussBlurPP.emat";
	protected static const int m_sPainBlurPriority 			= 11; // 15
	
	// Light Flinch Timings
	protected const float FLINCHEFFECT_INITIAL_PROGRESSION_TARGET 			= 0.625;
	protected const float FLINCHEFFECT_FADEIN_OPACITY_TARGET 				= 0.725; //0.725
	protected const float FLINCHEFFECT_FADEOUT_OPACITY_SPEED 				= 8;
	protected const float FLINCHEFFECT_CLEAREFFECT_DELAY 					= 120;
	
	// Flyby Flinch Timings
	protected const float FLYBYEFFECT_INITIAL_PROGRESSION_TARGET 			= 0.425; //0.625
	protected const float FLYBYEFFECT_FADEIN_OPACITY_TARGET 				= 0.725; //0.325 0.725
	protected const float FLYBYEFFECT_FADEOUT_OPACITY_SPEED 				= 8;
	protected const float FLYBYEFFECT_CLEAREFFECT_DELAY 					= 120;
	
	// Heavy Flinch Timings
	protected const float SUPPRESSIONEFFECT_INITIAL_OPACITY_TARGET			= 0.90; // 0.75
	protected const float SUPPRESSIONEFFECT_FADEIN_PROGRESSION_TARGET		= 0.35; // 0.35
	protected const float SUPPRESSIONEFFECT_FADEIN_PROGRESSION_DURATION		= 0.5; // 0.5
	protected const float SUPPRESSION_CLEAREFFECT_DELAY 					= 3000; // 2000
	
	// Flyby blur effects
	protected bool m_bFlybyEffectEnabled;
	protected string m_sFlybyEffectName;
	
	protected bool m_bFlybyEffectFullEnabled;
	protected bool m_bFlybyEffectAboveCenterEnabled;
	protected bool m_bFlybyEffectAboveLeftEnabled;
	protected bool m_bFlybyEffectAboveRightEnabled;
	protected bool m_bFlybyEffectLevelCenterEnabled;
	protected bool m_bFlybyEffectLevelLeftEnabled;
	protected bool m_bFlybyEffectLevelRightEnabled;
	protected bool m_bFlybyEffectBelowCenterEnabled;
	protected bool m_bFlybyEffectBelowLeftEnabled;
	protected bool m_bFlybyEffectBelowRightEnabled;
	
	protected ImageWidget m_wSuppressionVignette;
	protected ImageWidget m_wSuppressionVignetteHeavy;
	protected ImageWidget m_wSuppressionFlinch;
	protected ImageWidget m_wSuppressionFlinchHeavy;
	
	protected ImageWidget m_wSuppressionFlyby;
	protected ImageWidget m_wIron_Suppression_Flyby_AboveCenter;
	protected ImageWidget m_wIron_Suppression_Flyby_AboveLeft;
	protected ImageWidget m_wIron_Suppression_Flyby_AboveRight;
	protected ImageWidget m_wIron_Suppression_Flyby_LevelCenter;
	protected ImageWidget m_wIron_Suppression_Flyby_LevelLeft;
	protected ImageWidget m_wIron_Suppression_Flyby_LevelRight;
	protected ImageWidget m_wIron_Suppression_Flyby_BottomCenter;
	protected ImageWidget m_wIron_Suppression_Flyby_BottomLeft;
	protected ImageWidget m_wIron_Suppression_Flyby_BottomRight;
	
	//------------------------------------------------------------------------------------------------
	private float GetSuppressionAmount()
	{
		return Iron_SuppressionManager.GetSuppressionLevel();
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		m_wSuppressionFlinch = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_SuppressionFlinch"));
		m_wSuppressionFlinchHeavy = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_SuppressionFlinchHeavy"));
		m_wSuppressionVignette = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_SuppressionVignette"));
		m_wSuppressionVignetteHeavy = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_SuppressionVignetteHeavy"));
		
		m_wIron_Suppression_Flyby_AboveCenter = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_AboveCenter"));
		m_wIron_Suppression_Flyby_AboveLeft = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_AboveLeft"));
		m_wIron_Suppression_Flyby_AboveRight = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_AboveRight"));
		m_wIron_Suppression_Flyby_LevelCenter = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_LevelCenter"));
		m_wIron_Suppression_Flyby_LevelLeft = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_LevelLeft"));
		m_wIron_Suppression_Flyby_LevelRight = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_LevelRight"));
		m_wIron_Suppression_Flyby_BottomCenter = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_BottomCenter"));
		m_wIron_Suppression_Flyby_BottomLeft = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_BottomLeft"));
		m_wIron_Suppression_Flyby_BottomRight = ImageWidget.Cast(m_wRoot.FindAnyWidget("Iron_Suppression_Flyby_BottomRight"));
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		m_cCharacterEntity = ChimeraCharacter.Cast(to);
		if (!m_cCharacterEntity)
		{
			Print("Iron_SuppressionScreenEffect: DisplayControlledEntityChanged: No Character Entity!", LogLevel.ERROR);
			return;
		}

		m_pPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		m_PlayerEntity = to;
		
		int cameraId = m_cCharacterEntity.GetWorld().GetCurrentCameraId();
		m_cCharacterEntity.GetWorld().SetCameraPostProcessEffect(cameraId, m_sRadialBlurPriority, PostProcessEffectType.RadialBlur, m_sRadialBlurMaterialName);
		m_cCharacterEntity.GetWorld().SetCameraPostProcessEffect(cameraId, m_sColorPriority, PostProcessEffectType.Colors, m_sColorMaterialName);
		m_cCharacterEntity.GetWorld().SetCameraPostProcessEffect(cameraId, m_sChromeAberrationPriority, PostProcessEffectType.ChromAber, m_sChromeAberrationMaterialName);
		m_cCharacterEntity.GetWorld().SetCameraPostProcessEffect(cameraId, m_sPainBlurPriority, PostProcessEffectType.GaussFilter, m_sPainBlurMaterialName);
		
		m_cCameraManager = GetGame().GetCameraManager();
		m_fDefaultCameraFOV = m_cCameraManager.GetFirstPersonFOV();
		m_cFovSettings = GetGame().GetGameUserSettings().GetModule("SCR_FieldOfViewSettings");
	}

	//------------------------------------------------------------------------------------------------
	override void UpdateEffect(float timeSlice)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(m_cCharacterEntity);
		CharacterControllerComponent charControl = character.GetCharacterController();
		if (!charControl || charControl.IsDead())
		{
			m_fSuppression = 0;
			return;
		}
		
		m_fSuppressionTarget = Math3D.Curve(ECurveType.CatmullRom, GetSuppressionAmount(), m_cSuppressionCurve)[1];		
		m_fSuppression = Math.Lerp(m_fSuppression, m_fSuppressionTarget, timeSlice * m_fSuppressionSmoothness);

		s_bEnableRadialBlur = m_fSuppression > 0;
		s_fBlurriness = m_fSuppression * m_fSuppressionBlurMultiplier;
		s_fBlurrinessSize = m_fSuppressionBlurSizeMin + (1 - m_fSuppression) * (m_fSuppressionBlurSizeMax - m_fSuppressionBlurSizeMin);
		
		s_bEnableColorEffect = m_fSuppression > 0;	
		
		s_fFOVSaturation = 1 - (m_fSuppression * (1 - m_fSuppressionSaturation)); //m_fSuppressionSaturation));
		s_fContrast = 1 + m_fSuppression * Math.Clamp(m_fSuppressionContrastOffset, 0, 1);	
		s_fColorContrast = Math.Lerp(s_fColorContrast, 1 + (m_fSuppression * 0.6), timeSlice * 2); //Math.Lerp(s_fColorContrast, 1 + (m_fSuppression * 1.1), timeSlice * 2);
//		PrintFormat("FOV Saturation: %1", s_fFOVSaturation);
		m_wSuppressionVignette.SetMaskProgress(m_fSuppression * m_fSuppressionVignetteMultiplier * m_fSuppressionMultiplier); //  m_fSuppression * m_fSuppressionVignetteMultiplier * 1.3 * m_fSuppressionMultiplier
		m_wSuppressionVignette.SetOpacity(0.75);
		
		if (m_cFovSettings)
		{
			if (m_cFovSettings.Get("m_fFirstPersonFOV", m_fFov))
			m_fDefaultCameraFOV = m_fFov;
		}
		
//		float fovTarget = Math.Clamp(m_fDefaultCameraFOV * s_fFOVSaturation, (m_fDefaultCameraFOV - 15)/m_fSuppressionFOVMultiplier, m_fDefaultCameraFOV); 
		m_fSuppressionFOV = Math.Clamp(m_fDefaultCameraFOV * s_fFOVSaturation, (m_fDefaultCameraFOV - 15)/m_fSuppressionFOVMultiplier, m_fDefaultCameraFOV); //Math.Lerp(m_fSuppressionFOV, fovTarget, timeSlice * 2);
		m_cCameraManager.SetFirstPersonFOV(m_fSuppressionFOV);
		
		s_bPainEnabled = GetPainState(m_cCharacterEntity);
		m_fPainIntensity = s_fChromAberPower * 35; // 35
		
		if (s_bPainEnabled)
        {
            // Get pain intensity, which accounts for morphine effects
            float painIntensity = GetPainIntensity(m_cCharacterEntity);
            if (painIntensity > 0)
            {
                SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_cCharacterEntity.GetDamageManager());
                float health_scaled = damageManager.GetHealthScaled();
                health_scaled = Math.Clamp(health_scaled, 0.05, 1.0);

                float minDelay = 0.05;        				// 0.5 - Minimum delay at low health (seconds)
                float maxDelay = 3.0 * health_scaled;       // 2.0 * health_scaled - Maximum delay at high health (seconds)
                float pulseDuration = 3.0 * health_scaled; 	/// 2.0 / health_scaled;   // Fixed duration for pulsing phase (seconds)
                
                // Calculate delay duration: longer delay at higher health
                float delayDuration = minDelay + (maxDelay - minDelay) * health_scaled;
                float totalCycleTime = pulseDuration + delayDuration;
                
                // Update timer
                m_fPainEffectDelay += timeSlice;
                
                // Calculate phase within the cycle
                float phase = Math.Mod(m_fPainEffectDelay, totalCycleTime);
//              m_fPainIntensity = s_fChromAberPower * 35;
				
                if (phase < pulseDuration)
                {
                    // Pulsing phase: Apply cosine-based oscillation
                    float t = phase; 										// Time within pulse
                    float pulsePhase = 2 * Math.PI * (t / pulseDuration);
                    float pulse = (1 - Math.Cos(pulsePhase)); 				// Oscillates between 0 and 1
                    float targetIntensity = painIntensity * pulse;
                    
                    // Smoothly adjust the effect intensity
                    float smoothSpeed = 10; // Fast response for pulsing
                    s_fChromAberPower = Math.Lerp(s_fChromAberPower, targetIntensity, timeSlice * smoothSpeed);
                }
				
                else
                {
                    // Resting phase: Effect is off
                    float targetIntensity = 0.01 * (1 - health_scaled);
                    float smoothSpeed = 10; // Same speed for smooth transition
                    s_fChromAberPower = Math.Lerp(s_fChromAberPower, targetIntensity, timeSlice * smoothSpeed);
                }
            }
			
            else
            {
                // Morphine is active, clear the pain effect
                float smoothSpeed = 2; // Slower fade-out for morphine suppression
                s_fChromAberPower = Math.Lerp(s_fChromAberPower, 0, timeSlice * smoothSpeed);
//				m_fPainIntensity = 0;
            }
        }
        else
        {
            // Health >= 95%, clear the pain effect
            float smoothSpeed = 2; // Slower fade-out for high health
            s_fChromAberPower = Math.Lerp(s_fChromAberPower, 0, timeSlice * smoothSpeed);
            m_fPainEffectDelay = 0; // Reset timer when pain is disabled
//			m_fPainIntensity = 0;
        }
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetPainState(ChimeraCharacter character)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
		return damageManager.GetHealth() < 95;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPainIntensity(ChimeraCharacter character)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
		array<ref SCR_PersistentDamageEffect> effects = damageManager.GetAllPersistentEffectsOfType(SCR_MorphineDamageEffect);
		float maxIntensity;
		
		if (!effects.IsEmpty())
		{
			maxIntensity = 0;
		}
		
		else
		{
			maxIntensity = Math.Clamp((1 - damageManager.GetHealthScaled()) / 35, 0.005, 0.025);
		}

		return maxIntensity;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPainDelayMultiplier(ChimeraCharacter character)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
		return damageManager.GetHealthScaled();
	}
	
	//------------------------------------------------------------------------------------------------
	void TriggerPainEffect(float target, float timeSlice, float smoothSpeed)
	{		
		s_fChromAberPower = Math.Clamp(s_fChromAberPower, 0, m_fPainLevel);		
		s_fChromAberPower = Math.Lerp(s_fChromAberPower, target, timeSlice * smoothSpeed);	
//		PrintFormat("s_fChromAberPower: %1", s_fChromAberPower);			
	}
	
	//------------------------------------------------------------------------------------------------
	void ClearPainEffect(float target, float timeSlice, float smoothSpeed)
	{
		s_fChromAberPower = Math.Clamp(s_fChromAberPower, 0, 0.025);		
		s_fChromAberPower = Math.Lerp(s_fChromAberPower, target, timeSlice * smoothSpeed);
		
//		PrintFormat("s_fChromAberPower: %1", s_fChromAberPower);
		if (s_fChromAberPower <= 0.001 && m_fPainEffectDelay >= 3 * (1 - GetPainDelayMultiplier(m_cCharacterEntity))) //m_fPainEffectDelay >= 5 * m_fPainLevel
		{
			m_fPainEffectDelay = 0;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnSuppressionFlinch()
	{
		FlinchEffect();
//		ProcessFlyby(screenEffectPos, flinchDirection);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnProcessFlyby(FlinchTotal screenEffectPos, FlinchTotal flinchDirection)
	{
		ProcessFlyby(screenEffectPos, flinchDirection);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void FlinchEffect()
	{
		if (!m_wSuppressionFlinch)
		{	
			m_wSuppressionFlinch = m_wIron_Suppression_Flyby_LevelCenter;
			Print("Iron_SuppressionScreenEffect: FlinchEffect: m_wSuppressionFlinch is NULL!", LogLevel.ERROR);
			return;
		}

		///////////////////////////////////////////////////////////////////////////////
		m_wSuppressionFlinch.SetMaskProgress(FLINCHEFFECT_INITIAL_PROGRESSION_TARGET);
		
		AnimateWidget.StopAllAnimations(m_wSuppressionFlinch);
		AnimateWidget.Opacity(m_wSuppressionFlinch, FLINCHEFFECT_FADEIN_OPACITY_TARGET, FLINCHEFFECT_FADEOUT_OPACITY_SPEED).SetCurve(EAnimationCurve.LINEAR);
		
		GetGame().GetCallqueue().Remove(ClearFlinchEffect);
		GetGame().GetCallqueue().CallLater(ClearFlinchEffect, FLINCHEFFECT_CLEAREFFECT_DELAY, false);
	}
		
	//------------------------------------------------------------------------------------------------
	protected void ProcessFlyby(FlinchTotal screenEffectPos, FlinchTotal flinchDirection)
	{

		switch (screenEffectPos)
	    {
	        case FlinchTotal.AboveCenter:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_AboveCenter;
	            break;
	        case FlinchTotal.AboveLeft:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_AboveLeft;
	            break;
	        case FlinchTotal.AboveRight:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_AboveRight;
	            break;
	        case FlinchTotal.LevelCenter:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_LevelCenter;
	            break;
	        case FlinchTotal.LevelLeft:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_LevelLeft;
	            break;
	        case FlinchTotal.LevelRight:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_LevelRight;
	            break;
	        case FlinchTotal.BelowCenter:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_BottomCenter;
	            break;
	        case FlinchTotal.BelowLeft:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_BottomLeft;
	            break;
	        case FlinchTotal.BelowRight:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_BottomRight;
	            break;
	        default:
	            m_wSuppressionFlyby = m_wIron_Suppression_Flyby_LevelCenter; // Default case if none matches
	            break;
	    }
		
	    if (m_wSuppressionFlyby == null)
	    {
	        m_wSuppressionFlyby = m_wIron_Suppression_Flyby_LevelCenter; // Fallback if not set
	    }

		m_wSuppressionFlyby.SetMaskProgress(FLYBYEFFECT_INITIAL_PROGRESSION_TARGET);
		AnimateWidget.Opacity(m_wSuppressionFlyby, FLYBYEFFECT_FADEIN_OPACITY_TARGET, FLYBYEFFECT_FADEOUT_OPACITY_SPEED).SetCurve(EAnimationCurve.EASE_IN_CUBIC);
	    GetGame().GetCallqueue().CallLater(ClearFlybyEffect, FLINCHEFFECT_CLEAREFFECT_DELAY, false, m_wSuppressionFlyby);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ClearFlybyEffect(ImageWidget effect)
	{
//		if (effect == null)
//		{
//			return;
//		}

		AnimateWidget.AlphaMask(effect, 0, FLYBYEFFECT_FADEOUT_OPACITY_SPEED).SetCurve(EAnimationCurve.EASE_OUT_CUBIC);
		AnimateWidget.Opacity(effect, 0, FLYBYEFFECT_FADEOUT_OPACITY_SPEED).SetCurve(EAnimationCurve.EASE_OUT_CUBIC);	
//		AnimateWidget.StopAllAnimations(effect);
//		GetGame().GetCallqueue().Remove(ClearFlybyEffect);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ClearFlinchEffect()
	{
		if (m_wSuppressionFlinch == null)
		{
			return;
		}
		
//		AnimateWidget.StopAllAnimations(m_wSuppressionFlinch);
		AnimateWidget.AlphaMask(m_wSuppressionFlinch, 0, FLINCHEFFECT_FADEOUT_OPACITY_SPEED).SetCurve(EAnimationCurve.EASE_OUT_CUBIC);
		AnimateWidget.Opacity(m_wSuppressionFlinch, 0, FLINCHEFFECT_FADEOUT_OPACITY_SPEED).SetCurve(EAnimationCurve.EASE_OUT_CUBIC);	
	}
	
	//------------------------------------------------------------------------------------------------
	void HeavyFlinchEffect()
	{ 
//		Print("OnSuppressionFlinch: Heavy Flinched!");
		m_fTargetBlurX = 100;
		m_fBlurSmoothSpeed = 2;

		AnimateWidget.Opacity(m_wSuppressionFlinchHeavy, SUPPRESSIONEFFECT_INITIAL_OPACITY_TARGET, SUPPRESSIONEFFECT_FADEIN_PROGRESSION_DURATION);
		AnimateWidget.AlphaMask(m_wSuppressionFlinchHeavy, 0, SUPPRESSIONEFFECT_FADEIN_PROGRESSION_DURATION);
		
		AnimateWidget.Opacity(m_wSuppressionVignetteHeavy, SUPPRESSIONEFFECT_INITIAL_OPACITY_TARGET, 30); //10
		AnimateWidget.AlphaMask(m_wSuppressionVignetteHeavy, 0, 3); //5
		
		GetGame().GetCallqueue().CallLater(ClearHeavyFlinchEffect, 1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	private void ClearHeavyFlinchEffect()
	{
		AnimateWidget.Opacity(m_wSuppressionFlinchHeavy, 0, 1); //1
		AnimateWidget.AlphaMask(m_wSuppressionFlinchHeavy, 1, 1); //1 Colored blob in the center of screen

		AnimateWidget.Opacity(m_wSuppressionVignetteHeavy, 0, 1); // 0.5
		AnimateWidget.AlphaMask(m_wSuppressionVignetteHeavy, 1, 1); // 0.5
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayOnSuspended()
	{
		s_bEnableRadialBlur = false;
		s_bEnableColorEffect = false;
		s_bPainEnabled = false;
		
		m_bFlybyEffectAboveCenterEnabled = false;
		m_bFlybyEffectAboveLeftEnabled = false;
		m_bFlybyEffectAboveRightEnabled = false;
		m_bFlybyEffectLevelCenterEnabled = false;
		m_bFlybyEffectLevelLeftEnabled = false;
		m_bFlybyEffectLevelRightEnabled = false;
		m_bFlybyEffectBelowCenterEnabled = false;
		m_bFlybyEffectBelowLeftEnabled = false;
		m_bFlybyEffectBelowRightEnabled = false;
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayOnResumed()
	{
		s_bEnableRadialBlur = true;
		s_bEnableColorEffect = true;
		s_bPainEnabled = true;
		
		m_bFlybyEffectAboveCenterEnabled = true;
		m_bFlybyEffectAboveLeftEnabled = true;
		m_bFlybyEffectAboveRightEnabled = true;
		m_bFlybyEffectLevelCenterEnabled = true;
		m_bFlybyEffectLevelLeftEnabled = true;
		m_bFlybyEffectLevelRightEnabled = true;
		m_bFlybyEffectBelowCenterEnabled = true;
		m_bFlybyEffectBelowLeftEnabled = true;
		m_bFlybyEffectBelowRightEnabled = true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffects()
	{

		if (m_wSuppressionVignette != null)
		{
			AnimateWidget.StopAllAnimations(m_wSuppressionVignette);
			m_wSuppressionVignette.SetOpacity(0);
			m_wSuppressionVignette.SetMaskProgress(0);
		}
		
		if (m_wSuppressionFlinch != null)
		{
			AnimateWidget.StopAllAnimations(m_wSuppressionFlinch);
			m_wSuppressionFlinch.SetOpacity(0);
			m_wSuppressionFlinch.SetMaskProgress(0);
		}	
	}
}