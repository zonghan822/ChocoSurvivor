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
	PROJECTILE_HIT
#pragma endregion

#pragma region Music
	// Music
	// MUSIC_SOMETHING
#pragma endregion

#pragma region UI
	// UI
	// UI_SOMETHING
#pragma endregion
};

enum SoundGroup {
	NONE = -1,
	SFX = 0,
	MUSIC = 1,
	UI
};

void init_sounds(void);
void play_sound(enum SoundType);
void play_sound_advanced(enum SoundType, float volume_scale, int loop);
void stop_sounds(void);
void stop_sound_group(enum SoundGroup);
void set_volume(enum SoundType, float volume);
void dispose_sounds(void);