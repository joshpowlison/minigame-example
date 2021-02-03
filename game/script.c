///////////////
// CONSTANTS //
///////////////

const SPRITE_SIZE						= 6;
const DRAW_SCALE						= 1;

const ENTITY_ID_ME						= 0;
const SCALE_ME							= 1;

const ENTITY_MAX						= 100;

const CANVAS_WIDTH						= 90;
const CANVAS_HEIGHT						= 90;

// Types
const TYPE_EMPTY						= 0;
const TYPE_ME							= 1;
const TYPE_COLLECT						= 2;

// Game States
const STATE_START						= 0;
const STATE_MAIN						= 1;
const STATE_END							= 2;

const ME_JUMP_SPEED						= -120.0f;
const ME_MOVE_SPEED						= 50.0f;
const ME_GRAVITY						= 300.0f;
const GROUND_Y							= 72.0f;
	
// SETTINGS INT POINTERS
const SETTING_DEBUG						= 0;
const SETTING_DEBUG_PAUSE				= 1;
const SETTING_DEBUG_LISTPOS				= 2;
const SETTING_PAUSE						= 3;
const SETTING_GAME_STATE				= 4;
const SETTING_INPUT_LEFT				= 5;
const SETTING_INPUT_RIGHT				= 6;
const SETTING_INPUT_ACTION				= 7;
const SETTING_SCORE						= 8;
const SETTING_PARTICLE_1_TO_SPAWN		= 9;

// SETTINGS FLOAT POINTERS
const SETTINGF_GAME_TIMER				= 1;
const SETTINGF_TO_SPAWN					= 2;
const SETTINGF_PARTICLE_1_DELAY			= 3;
const SETTINGF_PARTICLE_1_DELAY_RATE	= 4;
const SETTINGF_DELAY_RESTART			= 5;

// SFX POINTERS
const SFX_NONE							= 0;
const SFX_JUMP							= 1;
const SFX_COLLECT						= 2;
const SFX_LAND							= 3;
const SFX_MOVE_1						= 4;
const SFX_MOVE_2						= 5;
const SFX_GAME_START					= 6;
const SFX_GAME_END						= 7;

// Input data
const ACTION							= 0;
const LEFT								= 1;
const RIGHT								= 2;

const JUST_RELEASED						= 0;
const RELEASED							= 1;
const PRESSED							= 2;
const JUST_PRESSED						= 3;

///////////////
// VARIABLES //
///////////////

int		eActive[ENTITY_MAX];
int		eType[ENTITY_MAX];
float	eX[ENTITY_MAX];
float	eY[ENTITY_MAX];
float	eSpeedX[ENTITY_MAX];
float	eSpeedY[ENTITY_MAX];
float	eMaxSpeed[ENTITY_MAX];
float	eRot[ENTITY_MAX];
float	eRotSpeed[ENTITY_MAX];
int		eGraphic[ENTITY_MAX];
float	eFrame[ENTITY_MAX];
int		eFlip[ENTITY_MAX];
float	eTimer[ENTITY_MAX];
int		settings[ENTITY_MAX];
float	settingsFloat[ENTITY_MAX];

///////////////
// FUNCTIONS //
///////////////

void entityReset(int e){
	eActive[e]		= 0;
	eType[e]		= 0;
	eX[e]			= 0.0f;
	eY[e]			= 0.0f;
	eSpeedX[e]		= 0.0f;
	eSpeedY[e]		= 0.0f;
	eMaxSpeed[e]	= 0.0f;
	eRot[e]			= 0.0f;
	eRotSpeed[e]	= 0.0f;
	eGraphic[e]		= 0;
	eFrame[e]		= 0.0f;
	eFlip[e]		= 0;
	eTimer[e]		= 0.0f;
}

void entityResetAll(){
	for(int i = 0; i < ENTITY_MAX; i ++) entityReset(i);
}

int main(){
	// In case we're restarting, reset all the entities
	entityResetAll();
	
	// Initial settings state
	settings[SETTING_GAME_STATE]					= STATE_START;
	settingsFloat[SETTINGF_PARTICLE_1_DELAY_RATE]	= 0.5f;
	
	// Create initial entities (this is just a little more readable than using entityInit)
	eActive[ENTITY_ID_ME]	= 1;
	eX[ENTITY_ID_ME]		= (CANVAS_WIDTH / 2.0f);
	eY[ENTITY_ID_ME]		= GROUND_Y;
	eType[ENTITY_ID_ME]		= 1;
	eGraphic[ENTITY_ID_ME]	= 0;
	
	return 1; // It's just recommended we do this. 1 can show that it succeeded, if we want to check.
}

int getBufferLength(){return ENTITY_MAX;}
float square(float val){return val * val;}

// For RNG, a linear-feedback shift register
unsigned short lfsr = 1234;
float rng(){
	lfsr = (lfsr >> 1) | ((((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1) << 15);
	
	// Cast it to a float and divide by the maximum
	return (float)lfsr / 65535.0f;
}

int entityInit(
	int type
	,float x
	,float y
	,float speedX
	,float speedY
	,float rot
	,float rotSpeed
	,float timer
	,float maxSpeed
	,int graphic
	,int flip
){
	for(int e = 0; e < ENTITY_MAX; e++){
		// Don't overwrite active entities
		if(eActive[e] == 1) continue;
		
		eActive[e]		= 1;
		eType[e]		= type;
		eX[e]			= x;
		eY[e]			= y;
		eFlip[e]		= flip;
		eGraphic[e]		= graphic;
		eFrame[e]		= 0.0f;

		eSpeedX[e]		= speedX;
		eSpeedY[e]		= speedY;
		eMaxSpeed[e]	= maxSpeed;

		eRot[e]			= rot;
		eRotSpeed[e]	= rotSpeed;

		eTimer[e]		= timer;
		
		// We created an entity, break here
		return e;
	}
	
	// If no entity was created, return -1
	return -1;
}

int* getDataPointer(int type){
	switch(type){
		case 0:		return &eActive[0];			break;
		case 1:		return &eType[0];			break;
		case 2:		return &eX[0];				break;
		case 3:		return &eY[0];				break;
		case 4:		return &eSpeedX[0];			break;
		case 5:		return &eSpeedY[0];			break;
		case 6:		return &eMaxSpeed[0];		break;
		case 7:		return &eRot[0];			break;
		case 8:		return &eRotSpeed[0];		break;
		case 9:		return &eGraphic[0];		break;
		case 10:	return &eFrame[0];			break;
		case 11:	return &eFlip[0];			break;
		case 12:	return &eTimer[0];			break;
		case 13:	return &settings[0];		break;
		case 14:	return &settingsFloat[0];	break;
	}
	
	return 0;
}

// Check collision between two entities
int CheckCollision(int e1,int e2,float distance){
	// Get the unsquared distance to check the squared values against each other
	float distanceUnsquared = square(eX[e1] - eX[e2]) + square(eY[e1] - eY[e2]);

	// If within range
	if(square(distanceUnsquared) <= square(distance)) return 1;
	
	// If not within range
	return 0;
}

// Check distance between two points
int IsWithinRange(int x1,int y1,int x2,int y2,float distance){
	// Get the unsquared distance to check the squared values against each other
	float distanceUnsquared = square(x1 - x2) + square(y1 - y2);

	// If within range
	if(square(distanceUnsquared) <= square(distance)) return 1;
	
	// If not within range
	return 0;
}

int particleSystem(float sDeltaTime){
	int spawned = 0;
	if(settings[SETTING_PARTICLE_1_TO_SPAWN] > 0){
		// Remove some of the delay restraint, so we can place so many rain in a second
		settingsFloat[SETTINGF_PARTICLE_1_DELAY] -= sDeltaTime;
		
		for(int i = 3; i < ENTITY_MAX; i ++){
			if(settingsFloat[SETTINGF_PARTICLE_1_DELAY] > 0) break;
			
			int dirRight = (rng() > 0.5f);
			float timerMax = 0.15f + (rng() * 0.1f);
			
			if(entityInit(
				2
				,(dirRight ? 0 : CANVAS_WIDTH)
				,(CANVAS_HEIGHT / 2.0f) + 10.0f
				,(dirRight ? 1.0f : -1.0f) * (20.0f + (rng() * 40.0f))
				,(rng() > .5 ? 1.0f : -1.0f) * (30.0f + (rng() * 40.0f))
				,0.0f
				,0.0f
				,(rng() * timerMax)
				,timerMax
				,(int)(rng() * 3.0f) * 2
				,(dirRight ? 0 : 1)
			) == 0) break;
			
			spawned ++;
			
			// Add the requested amount
			settings[SETTING_PARTICLE_1_TO_SPAWN] --;
			
			// Add to the delay, since we've just spawned confetti
			settingsFloat[SETTINGF_PARTICLE_1_DELAY] += settingsFloat[SETTINGF_PARTICLE_1_DELAY_RATE];
		}
	}
	
	return spawned;
}

// Just based on !mes for now
int loopEntities(float sDeltaTime){
	int sfx = 0;
	rng();						// Update RNG
	particleSystem(sDeltaTime);	// Run the particle system
	
	//// CONTROLS ////
	
	//// TYPE_ME ////
	// Move left and right
	if(settings[SETTING_INPUT_LEFT] >= PRESSED){
		eSpeedX[ENTITY_ID_ME]	= -ME_MOVE_SPEED;
		eFlip[ENTITY_ID_ME]	= 1;
	} else if(settings[SETTING_INPUT_RIGHT] >= PRESSED){
		eSpeedX[ENTITY_ID_ME]	= ME_MOVE_SPEED;
		eFlip[ENTITY_ID_ME]	= 0;
	} else {
		eSpeedX[ENTITY_ID_ME] = 0.0f;
	}
	
	// Jump (can only jump while playing and on the ground)
	if(
		settings[SETTING_GAME_STATE] == STATE_MAIN
		&& settings[SETTING_INPUT_ACTION] == JUST_PRESSED
		&& eY[ENTITY_ID_ME] >= GROUND_Y
	){
		eSpeedY[ENTITY_ID_ME]	= ME_JUMP_SPEED;
		sfx			= SFX_JUMP;
		// playerGrounded = 0;
	}
	
	// If above the ground, fall;
	// Apply gravity
	if(eY[ENTITY_ID_ME] < GROUND_Y){
		// if(eSpeedY != 0){
			eSpeedY[ENTITY_ID_ME] += ME_GRAVITY * sDeltaTime;
			// playerGrounded = 0;
		// }
	// If moving towards the ground and at it
	} else if(eSpeedY[ENTITY_ID_ME] > 0.0f){
		eSpeedY[ENTITY_ID_ME]	= 0;
		eY[ENTITY_ID_ME]		= GROUND_Y + 0.05f;
		sfx			= SFX_LAND;
	}
	
	//// TYPE_COLLECT ////
	for(int e = 1; e < ENTITY_MAX; e++){
		if(!eActive[e]) continue;
		
		if(eType[e] == TYPE_COLLECT){
			if(eTimer[e] <= 0.0f){
				eTimer[e] += eMaxSpeed[e];
				eSpeedY[e] *= -1;
			}
		}
	}
	
	// Loop through data sets one at a time, so we can cache the whole thing in memory and loop through them really quickly
	for(int e = 0; e < ENTITY_MAX; e ++) eRot[e] += eRotSpeed[e] * sDeltaTime;
	for(int e = 0; e < ENTITY_MAX; e ++) eX[e] += eSpeedX[e] * sDeltaTime;
	for(int e = 0; e < ENTITY_MAX; e ++) eY[e] += eSpeedY[e] * sDeltaTime;
	for(int e = 0; e < ENTITY_MAX; e ++){
		if(eTimer[e] > 0.0f) eTimer[e] -= sDeltaTime;
	}
	
	// Entity-specific stuff
	for(int e = 0; e < ENTITY_MAX; e++){
		if(eActive[e] == ENTITY_ID_ME) continue;
		
		// See if collect collided with player
		if(e != 0 && CheckCollision(0,e,60.0f)){
			if(settings[SETTING_GAME_STATE] == STATE_MAIN){
				settings[SETTING_SCORE] ++;
				sfx = 2;
			}
			entityReset(e);
			continue;
		}
		
		// If moving left, off the left side
		if(eX[e] < 0.0f && eSpeedX[e] < 0.0f){
			if(eType[e] == TYPE_ME){
				eX[e] = CANVAS_WIDTH - 0.5f;
			} else {
				entityReset(e);
			}
		}
		// If moving right, off the right side
		else if(eX[e] > CANVAS_WIDTH && eSpeedX[e] > 0.0f){
			// entityReset(e);;
			if(eType[e] == TYPE_ME){
				eX[e] = 0.5f;
			} else {
				entityReset(e);
			}
		}
		
	}
	
	
	/// SPRITE DISPLAY ///
	
	// Player
	// If entity is on the ground, can animate
	if(eSpeedY[ENTITY_ID_ME] == 0){
		float currentFrame = (int)(eFrame[ENTITY_ID_ME] / 30.0f);
		
		// Adjust frame if moving
		if(eSpeedX[ENTITY_ID_ME] != 0){
			eFrame[ENTITY_ID_ME] += 220.0f * sDeltaTime;
		} else {
			eFrame[ENTITY_ID_ME] = 0.0f;
		}
		
		// If just pressed a directional input
		if(
			settings[SETTING_INPUT_LEFT] == JUST_PRESSED
			|| settings[SETTING_INPUT_RIGHT] == JUST_PRESSED
		){
			eFrame[ENTITY_ID_ME] = (eFrame[ENTITY_ID_ME] >= 30.0f) ? 0.0f : 30.0f;
		}
		
		// If we've changed frames, play a tone for running
		if(currentFrame != (int)(eFrame[ENTITY_ID_ME] / 30.0f)){
			if(sfx == 0) sfx = currentFrame ? 4 : 5;
		}
	} else {
		eFrame[ENTITY_ID_ME] = 35.0f;
	}
	
	// Loop back to frame 0 if we've exceeded the max
	if(eFrame[ENTITY_ID_ME] > 60.0f) eFrame[ENTITY_ID_ME] -= 60.0f;
	
	
	
	
	/// MENUING ///
	if(settingsFloat[SETTINGF_DELAY_RESTART] > 0.0f) settingsFloat[SETTINGF_DELAY_RESTART] -= sDeltaTime;
	
	// Make collectibles
	if(settings[SETTING_GAME_STATE] == STATE_MAIN){
		settingsFloat[SETTINGF_GAME_TIMER] -= sDeltaTime;
		settingsFloat[SETTINGF_PARTICLE_1_DELAY_RATE] = (settingsFloat[SETTINGF_GAME_TIMER] + 1.0f) / 70.0f;
		if(settingsFloat[SETTINGF_PARTICLE_1_DELAY_RATE] < 0.1f) settingsFloat[SETTINGF_PARTICLE_1_DELAY_RATE] = 0.1f;
		
		// If the game timer is up, go back to the menu
		if(settingsFloat[SETTINGF_GAME_TIMER] < 0.0f){
			settingsFloat[SETTINGF_GAME_TIMER]		= 0.0f;
			settings[SETTING_PARTICLE_1_TO_SPAWN]	= 0;
			
			settingsFloat[SETTINGF_DELAY_RESTART]	= 1.0f;
			
			settings[SETTING_GAME_STATE]			= STATE_START;
			sfx										= SFX_GAME_END;
		}
	}
	
	// If we haven't started yet, and we're ready, start the game
	if(
		settings[SETTING_INPUT_ACTION] == JUST_PRESSED
		&& settings[SETTING_GAME_STATE] == STATE_START
		&& settingsFloat[SETTINGF_DELAY_RESTART] <= 0
	){
		settings[SETTING_GAME_STATE]			= STATE_MAIN;
		settings[SETTING_SCORE]					= 0;
		settingsFloat[SETTINGF_GAME_TIMER]		= 60.0f;
		
		settings[SETTING_PARTICLE_1_TO_SPAWN]	= 1000000;
		
		// Despawn all collects
		for(int e = 1; e < ENTITY_MAX; e ++) entityReset(e);
		
		sfx = SFX_GAME_START;
	}
	
	return sfx;
}

///////////////
// LISTENERS //
///////////////