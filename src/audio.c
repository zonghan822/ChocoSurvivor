/******************************************************************************
* @filename		audio.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file manages the game's audio, loading sound effects,
				music, and UI sounds, and providing functions to play, stop,
				adjust volume, and dispose. It includes support for random
				sound variations (e.g., footsteps, player hurt sounds) and
				advanced playback options like volume scaling and looping.
******************************************************************************/

#include "audio.h"
#include "cprocessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#pragma region CP_Sounds
#pragma region SFX Group
#pragma region Player Sounds
static CP_Sound _soundPlayerWalk1, _soundPlayerWalk2, _soundPlayerWalk3, _soundPlayerWalk4, _soundPlayerWalk5,
	_soundPlayerShoot,
	_soundPlayerHurt1, _soundPlayerHurt2, _soundPlayerHurt3,
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
static CP_Sound _soundMusicGameOver,
	_soundMusicLevelUp,
	_soundMusicNewWaveStart,
	_soundMusicNewMapLevel;
#pragma endregion
#pragma endregion

static CP_Sound _get_sound_object(enum SoundType sound_type) {
	switch (sound_type) {
		case SOUND_PLAYER_SHOOT:
			return _soundPlayerShoot;
		case SOUND_PLAYER_DASH:
			return _soundPlayerDash;
		case SOUND_PLAYER_DIE:
			return _soundPlayerDie;
		case SOUND_BARRICADE_HURT:
			return _soundBarricadeHurt;
		case SOUND_BARRICADE_DIE:
			return _soundBarricadeDie;
		case SOUND_ROACH_WALK:
			return _soundRoachWalk;
		case SOUND_ROACH_ATTACK:
			return _soundRoachAttack;
		case SOUND_ROACH_HURT:
			return _soundRoachHurt;
		case SOUND_ROACH_DIE:
			return _soundRoachDie;
		case SOUND_BOSS_WALK:
			return _soundBossWalk;
		case SOUND_BOSS_ATTACK:
			return _soundBossAttack;
		case SOUND_BOSS_ATTACK_WINDUP:
			return _soundBossAttackWindup;
		case SOUND_BOSS_HURT:
			return _soundBossHurt;
		case SOUND_BOSS_CHARGE:
			return _soundBossCharge;
		case SOUND_BOSS_CHARGE_WINDUP:
			return _soundBossChargeWindup;
		case SOUND_PROJECTILE_HIT:
			return _soundProjectileHit;
		case SOUND_MUSIC_GAME_OVER:
			return _soundMusicGameOver;
		case SOUND_MUSIC_LEVEL_UP:
			return _soundMusicLevelUp;
		case SOUND_MUSIC_NEW_WAVE_START:
			return _soundMusicNewWaveStart;
		case SOUND_MUSIC_NEW_MAP_LEVEL:
			return _soundMusicNewMapLevel;
		case SOUND_UI_BUTTON_HOVER:
			return _soundUiButtonHover;
		case SOUND_UI_BUTTON_BACK_CLICK:
			return _soundUiButtonBackClick;
		case SOUND_UI_BUTTON_CONFIRM_CLICK:
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
		case SOUND_PLAYER_WALK:
		case SOUND_PLAYER_SHOOT:
		case SOUND_PLAYER_HURT:
		case SOUND_PLAYER_DASH:
		case SOUND_PLAYER_DIE:
		case SOUND_PLAYER_BUILD_BARRICADE:
		case SOUND_BARRICADE_HURT:
		case SOUND_BARRICADE_DIE:
		case SOUND_ROACH_WALK:
		case SOUND_ROACH_ATTACK:
		case SOUND_ROACH_HURT:
		case SOUND_ROACH_DIE:
		case SOUND_BOSS_WALK:
		case SOUND_BOSS_ATTACK:
		case SOUND_BOSS_ATTACK_WINDUP:
		case SOUND_BOSS_HURT:
		case SOUND_BOSS_CHARGE:
		case SOUND_BOSS_CHARGE_WINDUP:
		case SOUND_PROJECTILE_HIT:
			return SOUNDGROUP_SFX;

		// Music Group
		case SOUND_MUSIC_GAME_OVER:
		case SOUND_MUSIC_LEVEL_UP:
		case SOUND_MUSIC_NEW_WAVE_START:
		case SOUND_MUSIC_NEW_MAP_LEVEL:
		    return SOUNDGROUP_MUSIC;

		// UI Group
		case SOUND_UI_BUTTON_HOVER:
		case SOUND_UI_BUTTON_CONFIRM_CLICK:
		case SOUND_UI_BUTTON_BACK_CLICK:
		    return SOUNDGROUP_UI;

		default:
			return SOUNDGROUP_NONE;
	}
}

static void _play_sound_random(enum SoundType sound_type, float vol, enum SoundGroup sound_group) {
	srand((unsigned int)time(NULL));
	int randomNumber = 0;
	
	switch (sound_type) {
		case SOUND_PLAYER_HURT:
			randomNumber = rand() % 3;
			switch (randomNumber) {
				case 0:
					CP_Sound_PlayAdvanced(_soundPlayerHurt1, vol, 1.f, 0, sound_group);
					break;
				case 1: 
					CP_Sound_PlayAdvanced(_soundPlayerHurt2, vol, 1.f, 0, sound_group);
					break;
				default: 
					CP_Sound_PlayAdvanced(_soundPlayerHurt3, vol, 1.f, 0, sound_group);
					break;
			}
		case SOUND_PLAYER_WALK:
			randomNumber = rand() % 5;
			switch (randomNumber) {
			case 0:
				CP_Sound_PlayAdvanced(_soundPlayerWalk1, vol, 1.f, 0, sound_group);
				break;
			case 1:
				CP_Sound_PlayAdvanced(_soundPlayerWalk2, vol, 1.f, 0, sound_group);
				break;
			case 2:
				CP_Sound_PlayAdvanced(_soundPlayerWalk3, vol, 1.f, 0, sound_group);
				break;
			case 3:
				CP_Sound_PlayAdvanced(_soundPlayerWalk4, vol, 1.f, 0, sound_group);
				break;
			default:
				CP_Sound_PlayAdvanced(_soundPlayerWalk5, vol, 1.f, 0, sound_group);
				break;
			}

	}
}

void init_sounds(void) {
	// Audio files should be placed in /sounds/
	// Player
	_soundPlayerWalk1 = CP_Sound_Load("Assets/Sounds/footstep_grass_000.ogg");
	_soundPlayerWalk2 = CP_Sound_Load("Assets/Sounds/footstep_grass_001.ogg");
	_soundPlayerWalk3 = CP_Sound_Load("Assets/Sounds/footstep_grass_002.ogg");
	_soundPlayerWalk4 = CP_Sound_Load("Assets/Sounds/footstep_grass_003.ogg");
	_soundPlayerWalk5 = CP_Sound_Load("Assets/Sounds/footstep_grass_004.ogg");
	_soundPlayerHurt1 = CP_Sound_Load("Assets/Sounds/select_003.ogg");
	_soundPlayerHurt2 = CP_Sound_Load("Assets/Sounds/select_004.ogg");
	_soundPlayerHurt3 = CP_Sound_Load("Assets/Sounds/select_005.ogg");
	//...

	// Music
	_soundMusicGameOver = CP_Sound_Load("Assets/Sounds/jingles_NES00.ogg");
	_soundMusicLevelUp = CP_Sound_Load("Assets/Sounds/jingles_NES03.ogg");
	_soundMusicNewWaveStart = CP_Sound_Load("Assets/Sounds/jingles_NES01.ogg");
	_soundMusicNewMapLevel = CP_Sound_Load("Assets/Sounds/jingles_NES02.ogg");

	// UI
	_soundUiButtonHover = CP_Sound_Load("Assets/Sounds/bong_001.ogg");
	_soundUiButtonConfirmClick = CP_Sound_Load("Assets/Sounds/drop_001.ogg");
	_soundUiButtonBackClick = CP_Sound_Load("Assets/Sounds/error_005.ogg");
}

void play_sound(enum SoundType sound_type) {
	enum SoundGroup grp = _get_sound_group(sound_type);
	float vol = CP_Sound_GetGroupVolume(grp);

	if (grp == SOUNDGROUP_NONE) return;

	// Handle ones with random sounds
	if (sound_type == SOUND_PLAYER_HURT || sound_type == SOUND_PLAYER_WALK)
		_play_sound_random(sound_type, vol, grp);
	else {
		CP_Sound snd = _get_sound_object(sound_type);

		if (snd == NULL) return;
		if (grp != SOUNDGROUP_MUSIC)
			CP_Sound_PlayAdvanced(snd, vol, 1.f, 0, grp);
		else
			CP_Sound_PlayMusic(snd);
	}
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
	CP_Sound_Free(&_soundPlayerWalk1);
	CP_Sound_Free(&_soundPlayerWalk2);
	CP_Sound_Free(&_soundPlayerWalk3);
	CP_Sound_Free(&_soundPlayerWalk4);
	CP_Sound_Free(&_soundPlayerWalk5);
	CP_Sound_Free(&_soundPlayerShoot);
	CP_Sound_Free(&_soundPlayerHurt1);
	CP_Sound_Free(&_soundPlayerHurt2);
	CP_Sound_Free(&_soundPlayerHurt3);
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