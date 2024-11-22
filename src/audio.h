#pragma once

enum SoundType {
#pragma region SFX
	// Player
	PLAYER_WALK,
	PLAYER_SHOOT,
	PLAYER_HURT,
	PLAYER_DASH,
	PLAYER_DIE,
	PLAYER_BUILD_BARRICADE,
	// buff section
	//PLAYER_BUFF_SOMETHING
	// end of buff section

	// Barricade
	BARRICADE_HURT,
	BARRICADE_DIE,

	// Common enemies
	ROACH_WALK,
	ROACH_ATTACK,
	ROACH_HURT,
	ROACH_DIE,

	// Boss
	BOSS_WALK,
	BOSS_ATTACK,
	BOSS_ATTACK_WINDUP,
	BOSS_HURT,
	BOSS_CHARGE,
	BOSS_CHARGE_WINDUP,

	// Projectiles
	PROJECTILE_HIT,
#pragma endregion

#pragma region Music
	// Music
	// MUSIC_SOMETHING
#pragma endregion

#pragma region UI
	// UI
	UI_BUTTON_HOVER,
	UI_BUTTON_CONFIRM_CLICK,
	UI_BUTTON_BACK_CLICK,
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