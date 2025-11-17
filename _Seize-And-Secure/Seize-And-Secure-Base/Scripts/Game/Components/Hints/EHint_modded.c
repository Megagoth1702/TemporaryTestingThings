//! Hint types.
//! When a hint is shown, its type, when defined, is stored persistently.
//! After that the hint will not be shown anymore.
//!
//! Assign specific value to each entry!
//! Otherwise it may be shifted when new entries are added in front of it,
//! resulting in unintended type change in existing hints.
//!
//! This is because values are saved as numbers in SCR_HintSettings.
modded enum EHint
{
	CONFLICT_NEAR_TEAM,
}
