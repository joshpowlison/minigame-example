///////////////
// CONSTANTS //
///////////////

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

const ME_JUMP_SPEED						= -180.0f;
const ME_MOVE_SPEED						= 70.0f;
const ME_GRAVITY						= 340.0f;
const GROUND_Y							= 72.0f;

// SETTINGS INT POINTERS
const SETTING_INPUT_LEFT				= 5;
const SETTING_INPUT_RIGHT				= 6;
const SETTING_INPUT_ACTION				= 7;
const SETTING_SCORE						= 8;

// SETTINGS FLOAT POINTERS
const SETTINGF_PARTICLE_1_DELAY			= 3;
const SETTINGF_PARTICLE_1_DELAY_RATE	= 4;

// Input data
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
}

void entityResetAll(){
	for(int i = 0; i < ENTITY_MAX; i ++) entityReset(i);
}

int main(){
	// In case we're restarting, reset all the entities
	entityResetAll();
	
	// Initial settings state
	settingsFloat[SETTINGF_PARTICLE_1_DELAY_RATE]	= 0.5f;
	
	// Create initial entities (this is just a little more readable than using entityInit)
	eActive[ENTITY_ID_ME]	= 1;
	eX[ENTITY_ID_ME]		= (CANVAS_WIDTH / 2.0f);
	eY[ENTITY_ID_ME]		= GROUND_Y;
	eType[ENTITY_ID_ME]		= 1;
	eGraphic[ENTITY_ID_ME]	= 0;
	
	// Make collectibles
	settingsFloat[SETTINGF_PARTICLE_1_DELAY_RATE] = 0.5f;
	settings[SETTING_SCORE]					= 0;
	
	return 1; // It's just recommended we do this. 1 can show that it succeeded, if we want to check.
}

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
		case 6:		return &eGraphic[0];		break;
		case 7:		return &eFrame[0];			break;
		case 8:		return &eFlip[0];			break;
		case 9:		return &settings[0];		break;
		case 10:	return &settingsFloat[0];	break;
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
			,(dirRight ? 1.0f : -1.0f) * (10.0f + (rng() * 60.0f))
			,-10.0f - (rng() * 20.0f)
			,0.0f
			,0.0f
			,0.0f
			,0.0f
			,(int)(rng() * 3.0f)
			,(dirRight ? 0 : 1)
		) == 0) break;
		
		spawned ++;
		
		// Add to the delay, since we've just spawned confetti
		settingsFloat[SETTINGF_PARTICLE_1_DELAY] += settingsFloat[SETTINGF_PARTICLE_1_DELAY_RATE];
	}
	
	return spawned;
}

// The game loop
int loop(float sDeltaTime){
	rng();						// Update RNG
	particleSystem(sDeltaTime);	// Run the particle system
	
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
		settings[SETTING_INPUT_ACTION] == JUST_PRESSED
		&& eY[ENTITY_ID_ME] >= GROUND_Y
	){
		eSpeedY[ENTITY_ID_ME]	= ME_JUMP_SPEED;
	}
	
	// If above the ground, apply gravity
	if(eY[ENTITY_ID_ME] < GROUND_Y){
		eSpeedY[ENTITY_ID_ME] += ME_GRAVITY * sDeltaTime;
	// If moving towards the ground and at it
	} else if(eSpeedY[ENTITY_ID_ME] > 0.0f){
		eSpeedY[ENTITY_ID_ME]	= 0;
		eY[ENTITY_ID_ME]		= GROUND_Y + 0.05f;
	}
	
	//// TYPE_COLLECT ////
	for(int e = 1; e < ENTITY_MAX; e++){
		if(!eActive[e]) continue;
		
		// Slow down balloons' x-speed gradually
		eSpeedX[e] *= 1 - (.95 * sDeltaTime);
	}
	
	// Loop through data sets one at a time, so we can cache the whole thing in memory and loop through them really quickly
	for(int e = 0; e < ENTITY_MAX; e ++) eX[e] += eSpeedX[e] * sDeltaTime;
	for(int e = 0; e < ENTITY_MAX; e ++) eY[e] += eSpeedY[e] * sDeltaTime;
	
	// Entity-specific stuff
	for(int e = 0; e < ENTITY_MAX; e++){
		if(eActive[e] == 0) continue; // Ignore inactive entities
		
		// See if collect collided with player
		if(e != ENTITY_ID_ME && CheckCollision(0,e,60.0f)){
			settings[SETTING_SCORE] ++;
			entityReset(e);
			continue;
		}
		
		// If player moves off the side of the screen
		if(eType[e] == TYPE_ME){
			// Left off the left side
			if(eX[e] < 0.0f && eSpeedX[e] < 0.0f) eX[e] = CANVAS_WIDTH - 0.5f;
			// Right off the right side
			else if(eX[e] > CANVAS_WIDTH && eSpeedX[e] > 0.0f) eX[e] = 0.5f;
		}
		
		// If a collectible is above the top of the screen, remove it
		if(eType[e] == TYPE_COLLECT && eY[e] < 0.0f) entityReset(e);
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
	} else {
		eFrame[ENTITY_ID_ME] = 35.0f;
	}
	
	// Loop back to frame 0 if we've exceeded the max
	if(eFrame[ENTITY_ID_ME] > 60.0f) eFrame[ENTITY_ID_ME] -= 60.0f;
	
	return 1;
}

///////////////
// LISTENERS //
///////////////