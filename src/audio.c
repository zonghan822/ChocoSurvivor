#include "audio.h"
#include "cprocessing.h"

#pragma region CP_Sounds
#pragma region SFX Group
#pragma region Player Sounds
static CP_Sound _soundPlayerWalk,
	_soundPlayerShoot,
	_soundPlayerHurt,
	_soundPlayerDash,
	_soundPlayerDie;
#pragma endregion

#pragma region Barricade Sounds
static CP_Sound _soundBarricadeHurt,
	_soundBarricadeDie;
#pragma endregion

#pragma region Roach Sounds
static CP_Sound _soundRoachWalk,
	_soundRoachAttack,
	_soundRoachHurt,
	_soundRoachDie;
#pragma endregion

#pragma region Boss Sounds
static CP_Sound _soundBossWalk,
	_soundBossAttack,
	_soundBossAttackWindup,
	_soundBossHurt,
	_soundBossCharge,
	_soundBossChargeWindup;
#pragma endregion

#pragma region Projectile Sounds
static CP_Sound _soundProjectileHit;
#pragma endregion
#pragma endregion

#pragma region UI Group
static CP_Sound _soundUiButtonHover,
	_soundUiButtonConfirmClick,
	_soundUiButtonBackClick;
#pragma endregion

#pragma region Music Group
#pragma endregion
#pragma endregion

static CP_Sound _get_sound_object(enum SoundType sound_type) {
	switch (sound_type) {
		case PLAYER_WALK:
			return _soundPlayerWalk;
		case PLAYER_SHOOT:
			return _soundPlayerShoot;
		case PLAYER_HURT:
			return _soundPlayerHurt;
		case PLAYER_DASH:
			return _soundPlayerDash;
		case PLAYER_DIE:
			return _soundPlayerDie;
		case BARRICADE_HURT:
			return _soundBarricadeHurt;
		case BARRICADE_DIE:
			return _soundBarricadeDie;
		case ROACH_WALK:
			return _soundRoachWalk;
		case ROACH_ATTACK:
			return _soundRoachAttack;
		case ROACH_HURT:
			return _soundRoachHurt;
		case ROACH_DIE:
			return _soundRoachDie;
		case BOSS_WALK:
			return _soundBossWalk;
		case BOSS_ATTACK:
			return _soundBossAttack;
		case BOSS_ATTACK_WINDUP:
			return _soundBossAttackWindup;
		case BOSS_HURT:
			return _soundBossHurt;
		case BOSS_CHARGE:
			return _soundBossCharge;
		case BOSS_CHARGE_WINDUP:
			return _soundBossChargeWindup;
		case PROJECTILE_HIT:
			return _soundProjectileHit;
		case UI_BUTTON_HOVER:
			return _soundUiButtonHover;
		case UI_BUTTON_BACK_CLICK:
			return _soundUiButtonBackClick;
		case UI_BUTTON_CONFIRM_CLICK:
			return _soundUiButtonConfirmClick;
			// Add more cases as needed for UI and Music sounds
		default:
			// Return a default sound or handle the error appropriately
			return NULL;
	}
}

static enum SoundGroup _get_sound_group(enum SoundType sound_type) {
	switch (sound_type) {
		// Player Sounds (SFX Group)
		case PLAYER_WALK:
		case PLAYER_SHOOT:
		case PLAYER_HURT:
		case PLAYER_DASH:
		case PLAYER_DIE:
		case PLAYER_BUILD_BARRICADE:
		case BARRICADE_HURT:
		case BARRICADE_DIE:
		case ROACH_WALK:
		case ROACH_ATTACK:
		case ROACH_HURT:
		case ROACH_DIE:
		case BOSS_WALK:
		case BOSS_ATTACK:
		case BOSS_ATTACK_WINDUP:
		case BOSS_HURT:
		case BOSS_CHARGE:
		case BOSS_CHARGE_WINDUP:
		case PROJECTILE_HIT:
			return SOUNDGROUP_SFX;

		// Music Group
		// case MUSIC_SOMETHING:
		//     return SOUNDGROUP_MUSIC;

		// UI Group
		case UI_BUTTON_HOVER:
		case UI_BUTTON_CONFIRM_CLICK:
		case UI_BUTTON_BACK_CLICK:
		    return SOUNDGROUP_UI;

		default:
			return SOUNDGROUP_NONE;
	}
}

void init_sounds(void) {
	// Audio files should be placed in /sounds/
	// Player
	//_soundPlayerWalk = CP_Sound_Load(filePath);
	//...

	// UI
	_soundUiButtonHover = CP_Sound_Load("Assets/Sounds/bong_001.ogg");
	_soundUiButtonConfirmClick = CP_Sound_Load("Assets/Sounds/drop_001.ogg");
	_soundUiButtonBackClick = CP_Sound_Load("Assets/Sounds/error_005.ogg");
}

void play_sound(enum SoundType sound_type) {
	CP_Sound snd = _get_sound_object(sound_type);
	enum SoundGroup grp = _get_sound_group(sound_type);

	if (snd == NULL || grp == SOUNDGROUP_NONE) return;
	float vol = CP_Sound_GetGroupVolume(grp);

	if (grp != SOUNDGROUP_MUSIC)
		CP_Sound_PlayAdvanced(snd, vol, 1.f, 0, grp);
	else
		CP_Sound_PlayMusic(snd);
}

void play_sound_advanced(enum SoundType sound_type, float volume_scale, int loop) {
	CP_Sound snd = _get_sound_object(sound_type);
	enum SoundGroup grp = _get_sound_group(sound_type);

	if (snd == NULL || grp == SOUNDGROUP_NONE) return;

	float vol = CP_Sound_GetGroupVolume(grp) * volume_scale;
	CP_Sound_PlayAdvanced(snd, vol, 1.f, (unsigned int)loop, grp);
}

void stop_sounds(void) {
	CP_Sound_StopAll();
}

void stop_sound_group(enum SoundGroup sound_group) {
	CP_Sound_StopGroup(sound_group);
}

void set_volume(enum SoundType sound_type, float volume) {
	enum SoundGroup grp = _get_sound_group(sound_type);

	if (grp == SOUNDGROUP_NONE) return;

	set_volume_group(grp, volume);
}

void set_volume_group(enum SoundGroup sound_group, float volume) {
	volume = CP_Math_ClampFloat(volume, 0.f, 1.f);
	CP_Sound_SetGroupVolume(sound_group, volume);
}

float get_volume_group(enum SoundGroup sound_group) {
	return CP_Sound_GetGroupVolume(sound_group);
}

void dispose_sounds(void) {
	stop_sounds();

	// Player Sounds
	CP_Sound_Free(&_soundPlayerWalk);
	CP_Sound_Free(&_soundPlayerShoot);
	CP_Sound_Free(&_soundPlayerHurt);
	CP_Sound_Free(&_soundPlayerDash);
	CP_Sound_Free(&_soundPlayerDie);

	// Barricade Sounds
	// CP_Sound_Free(&_soundBarricadeHurt);
	// CP_Sound_Free(&_soundBarricadeDie);

	// Roach Sounds
	// CP_Sound_Free(&_soundRoachWalk);
	// CP_Sound_Free(&_soundRoachAttack);
	// CP_Sound_Free(&_soundRoachHurt);
	// CP_Sound_Free(&_soundRoachDie);

	// Boss Sounds
	// CP_Sound_Free(&_soundBossWalk);
	// CP_Sound_Free(&_soundBossAttack);
	// CP_Sound_Free(&_soundBossAttackWindup);
	// CP_Sound_Free(&_soundBossHurt);
	// CP_Sound_Free(&_soundBossCharge);
	// CP_Sound_Free(&_soundBossChargeWindup);

	// Projectile Sounds
	// CP_Sound_Free(&_soundProjectileHit);

	// Music

	// UI
	CP_Sound_Free(&_soundUiButtonHover);
	CP_Sound_Free(&_soundUiButtonConfirmClick);
	CP_Sound_Free(&_soundUiButtonBackClick);
}