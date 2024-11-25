/******************************************************************************
* @filename		audio.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This interface has functions for managing audio in the game,
				including sound effects, music, and UI sounds. It enumerates
				different sound types and groups them into categories such
				as player actions, enemies, projectiles, and UI interactions. 
				
				Functions are provided for initialising sounds, playing or
				stopping sounds, adjusting their volume, and disposing of
				audio resources. Additionally, advanced controls for sound
				playback, including volume scaling and looping, are included.
******************************************************************************/

#pragma once

enum SoundType {
#pragma region SFX
	// Player
	SOUND_PLAYER_WALK,
	SOUND_PLAYER_SHOOT,
	SOUND_PLAYER_HURT,
	SOUND_PLAYER_DASH,
	SOUND_PLAYER_DIE,
	SOUND_PLAYER_BUILD_BARRICADE,
	// buff section
	//SOUND_PLAYER_BUFF_SOMETHING
	// end of buff section

	// Barricade
	SOUND_BARRICADE_HURT,
	SOUND_BARRICADE_DIE,

	// Common enemies
	SOUND_ROACH_WALK,
	SOUND_ROACH_ATTACK,
	SOUND_ROACH_HURT,
	SOUND_ROACH_DIE,

	// Boss
	SOUND_BOSS_WALK,
	SOUND_BOSS_ATTACK,
	SOUND_BOSS_ATTACK_WINDUP,
	SOUND_BOSS_HURT,
	SOUND_BOSS_CHARGE,
	SOUND_BOSS_CHARGE_WINDUP,

	// Projectiles
	SOUND_PROJECTILE_HIT,
#pragma endregion

#pragma region Music
	// Music
	SOUND_MUSIC_GAME_OVER,
	SOUND_MUSIC_LEVEL_UP,
	SOUND_MUSIC_NEW_WAVE_START,
	SOUND_MUSIC_NEW_MAP_LEVEL,
	// MUSIC_SOMETHING
#pragma endregion

#pragma region UI
	// UI
	SOUND_UI_BUTTON_HOVER,
	SOUND_UI_BUTTON_CONFIRM_CLICK,
	SOUND_UI_BUTTON_BACK_CLICK
	// UI_SOMETHING
#pragma endregion
};

enum SoundGroup {
	SOUNDGROUP_NONE = -1,
	SOUNDGROUP_SFX = 0,
	SOUNDGROUP_MUSIC = 1,
	SOUNDGROUP_UI = 2,
};

void init_sounds(void);
void play_sound(enum SoundType sound_type);
void play_sound_advanced(enum SoundType sound_type, float volume_scale, int loop);
void stop_sounds(void);
void stop_sound_group(enum SoundGroup sound_group);
void set_volume(enum SoundType sound_type, float volume);
void set_volume_group(enum SoundGroup sound_group, float volume);
float get_volume_group(enum SoundGroup sound_group);
void dispose_sounds(void);