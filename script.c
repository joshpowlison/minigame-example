///////////////
// CONSTANTS //
///////////////

const int ENTITY_MAX	= 100;
const int CANVAS_WIDTH = 90, CANVAS_HEIGHT = 90;

const int ENTITY_ID_ME	= 0;	// We can trust this number because we're spawning the player character first
const int TYPE_ME = 1, TYPE_COLLECT = 2;

const int GROUND_Y		= 72.0f;

// Setting value ids for settings[]. Lines up with JS.
const int SETTING_INPUT_LEFT = 5, SETTING_INPUT_RIGHT = 6, SETTING_INPUT_ACTION = 7;
const int SETTING_SCORE	= 8;

// Input values. Lines up with JS.
const int JUST_RELEASED = 0, RELEASED = 1, PRESSED = 2, JUST_PRESSED = 3;

///////////////
// VARIABLES //
///////////////

// Entity arrays. Shared with JS.
int		eActive[ENTITY_MAX], eType[ENTITY_MAX], eGraphic[ENTITY_MAX], eFlip[ENTITY_MAX];
float	eX[ENTITY_MAX], eY[ENTITY_MAX], eSpeedX[ENTITY_MAX], eSpeedY[ENTITY_MAX], eFrame[ENTITY_MAX];

// Settings array containing key data. Shared with JS.
int		settings[ENTITY_MAX];

float	collectSpawnDelay		= 0.5f;
unsigned short lfsr				= 1234; // Linear-feedback shift register value, for RNG

///////////////
// FUNCTIONS //
///////////////

// In C, you can't call a function on a line before the function is written!

// Returns a float between 0 and 1. Randomizes using a linear-feedback shift register.
float rng(){
	lfsr = (lfsr >> 1) | ((((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1) << 15);
	return (float)lfsr / 65535.0f; // Cast it to a float and divide by the maximum
}

float square(float val){return val * val;}

int entityInit(
	int type
	,float x
	,float y
	,float speedX
	,float speedY
	,int graphic
	,int flip
){
	for(int e = 0; e < ENTITY_MAX; e++){
		if(eActive[e] == 1) continue; // Don't overwrite active entities
		
		eActive[e]		= 1;
		
		eType[e]		= type;
		eX[e]			= x;
		eY[e]			= y;
		eSpeedX[e]		= speedX;
		eSpeedY[e]		= speedY;
		eGraphic[e]		= graphic;
		eFrame[e]		= 0.0f;
		eFlip[e]		= flip;
		
		return e; // We initiated an entity, break here and return the entity's id
	}
	
	return -1; // If no entity was initiated, return -1
}

int CheckCollision(int e1,int e2,float distance){
	// Get the unsquared distance to check the squared values against each other (square root is slower and unnecessary)
	float distanceUnsquared = square(eX[e1] - eX[e2]) + square(eY[e1] - eY[e2]);

	// Return 1 if within range, and 0 if not
	return (square(distanceUnsquared) <= square(distance));
}

// Runs on start
int main(){
	// Create Player Character
	entityInit(1,(CANVAS_WIDTH / 2.0f),GROUND_Y,0.0f,0.0f,0,0);
	
	return 1; // It's just recommended we do this. 1 can show that it succeeded, if we want to check.
}

// The game loop
void loop(float sDeltaTime){
	rng(); // Update RNG every frame
	
	/// TYPE_ME
	// Left and right movement
	if(settings[SETTING_INPUT_LEFT] >= PRESSED){
		eSpeedX[ENTITY_ID_ME]	= -70.0f;
		eFlip[ENTITY_ID_ME]	= 1;
	} else if(settings[SETTING_INPUT_RIGHT] >= PRESSED){
		eSpeedX[ENTITY_ID_ME]	= 70.0f;
		eFlip[ENTITY_ID_ME]	= 0;
	} else eSpeedX[ENTITY_ID_ME] = 0.0f;
	
	// Jumping
	if(settings[SETTING_INPUT_ACTION] == JUST_PRESSED && eY[ENTITY_ID_ME] >= GROUND_Y) eSpeedY[ENTITY_ID_ME] = -180.0f;
	
	// If above the ground, apply gravity
	if(eY[ENTITY_ID_ME] < GROUND_Y){
		eSpeedY[ENTITY_ID_ME] += 340.0f * sDeltaTime;
	// If moving towards the ground and at it
	} else if(eSpeedY[ENTITY_ID_ME] > 0.0f){
		eSpeedY[ENTITY_ID_ME]	= 0;
		eY[ENTITY_ID_ME]		= GROUND_Y + 0.05f;
	}
	
	/// TYPE_COLLECT
	for(int e = 0; e < ENTITY_MAX; e++){
		if(!eActive[e] || eType[e] == TYPE_ME) continue;
		
		// Slow down balloons' x-speed gradually
		eSpeedX[e] *= 1 - (.95 * sDeltaTime);
	}
	
	// Spawn more collect if we've run out the delay between spawns timer
	collectSpawnDelay -= sDeltaTime;
	while(collectSpawnDelay < 0){
		int dirRight = (rng() > 0.5f);
		
		if(entityInit(
			2
			,(dirRight ? 0 : CANVAS_WIDTH)
			,(CANVAS_HEIGHT / 2.0f) + 10.0f
			,(dirRight ? 1.0f : -1.0f) * (10.0f + (rng() * 60.0f))
			,-10.0f - (rng() * 20.0f)
			,(int)(rng() * 3.0f)
			,(dirRight ? 0 : 1)
		) == -1) break; // If an entity cannot spawn- we've maxed out our entity count- exit the while loop
		
		collectSpawnDelay += 0.5f; // Add to the delay between spawns
	}
	
	// Loop through the x and y values and update them based on speed
	for(int e = 0; e < ENTITY_MAX; e ++) eX[e] += eSpeedX[e] * sDeltaTime;
	for(int e = 0; e < ENTITY_MAX; e ++) eY[e] += eSpeedY[e] * sDeltaTime;
	
	/// Collisions/Triggers
	// Wrap player if moving off the left or right side of canvas
	if(eX[ENTITY_ID_ME] < 0.0f && eSpeedX[ENTITY_ID_ME] < 0.0f) eX[ENTITY_ID_ME] = CANVAS_WIDTH - 0.5f;
	else if(eX[ENTITY_ID_ME] > CANVAS_WIDTH && eSpeedX[ENTITY_ID_ME] > 0.0f) eX[ENTITY_ID_ME] = 0.5f;
	
	for(int e = 0; e < ENTITY_MAX; e++){
		if(eActive[e] == 0) continue; // Ignore inactive entities
		
		if(eType[e] == TYPE_COLLECT){
			// See if collect collided with player
			if(CheckCollision(e,ENTITY_ID_ME,60.0f)){
				settings[SETTING_SCORE] ++;
				eActive[e] = 0;
			}
			
			// If a collectible has moved above the top of the screen, remove it
			if(eY[e] < 0.0f) eActive[e] = 0;
		}
	}
	
	/// Sprite Animation
	// If Player is on the ground, update their animation frames
	if(eSpeedY[ENTITY_ID_ME] == 0){
		// Adjust frame if moving
		if(eSpeedX[ENTITY_ID_ME] != 0){
			eFrame[ENTITY_ID_ME] += 220.0f * sDeltaTime;
			
			// Loop back to frame 0 if we've exceeded the max
			if(eFrame[ENTITY_ID_ME] > 60.0f) eFrame[ENTITY_ID_ME] -= 60.0f;
		}
		// If not moving, hold the still frame
		else eFrame[ENTITY_ID_ME] = 0.0f;
	// If jumping, hold the moving frame
	} else eFrame[ENTITY_ID_ME] = 35.0f;
}

// Used to let JS point to the data in WASM
int* getDataPointer(int type){
	switch(type){
		case 0:		return &eActive[0];
		case 1:		return &eType[0];
		case 2:		return &eX[0];
		case 3:		return &eY[0];
		case 4:		return &eSpeedX[0];
		case 5:		return &eSpeedY[0];
		case 6:		return &eGraphic[0];
		case 7:		return &eFrame[0];
		case 8:		return &eFlip[0];
		case 9:		return &settings[0];
		default:	return 0;
	}
}