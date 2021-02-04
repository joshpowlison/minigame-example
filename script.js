'use strict';

///////////////
// CONSTANTS //
///////////////

const CANVAS				= document.getElementById('canvas');
const CTX					= CANVAS.getContext('2d');
CTX.imageSmoothingEnabled	= false; // Our pixel art will look blurry without this

const SPRITE_PIXEL_SIZE		= 6;
const SPRITE_SHEET			= new Image();
SPRITE_SHEET.src			= 'sprite-sheet.png';

// Constants shared with C
const ENTITY_MAX			= 100;
const SETTING_SCORE			= 8; // The position of this value in the settings array
const SETTING_INPUT_LEFT = 5, SETTING_INPUT_RIGHT = 6, SETTING_INPUT_ACTION = 7; // The position of these values in the settings array
const JUST_RELEASED = 0, RELEASED = 1, PRESSED = 2, JUST_PRESSED = 3; // Basically, an enum

///////////////
// VARIABLES //
///////////////

var WASM				= null;	// The WASM object gets put in here
var entities			= {};	// Becomes an object that shares all the entity data between JS and C
var settings			= [];	// Holds all other settings- score, timers, etc
var lastFrameTimestamp	= 0;	// Needed for calculating time between frames in the loop

///////////////
// FUNCTIONS //
///////////////

// The Loop
function onAnimationFrame(frameTimestamp) {
	// Run the WASM loop, passing in a deltaTime based on seconds
	WASM.exports.loop((frameTimestamp - lastFrameTimestamp) / 1000);
	
	// Updates the state of the button inputs, transitioning them from "just pressed" to "pressed" and "just released" to "released" if necessary
	updateInputState(SETTING_INPUT_ACTION);
	updateInputState(SETTING_INPUT_LEFT);
	updateInputState(SETTING_INPUT_RIGHT);

	// Draw the background. If you remove this, you'll have to run clearRect on the canvas.
	CTX.drawImage(SPRITE_SHEET,0,0);
	
	// Loop through and draw all active entities
	for(var i = 0; i < ENTITY_MAX; i ++){
		if(entities.active[i] === 0) continue; // Skip over inactive entities
		
		var sheetStartX, sheetStartY = 0; // Position of sprite in SPRITE_SHEET
		
		// Player Character, with type === 1
		if(entities.type[i] === 1){
			var drawFrame = (entities.frame[i] < 30) ? 0 : 1; // Set the draw frame based on the current animation frame of movement
			if(entities.flip[i] === 1) drawFrame += 2; // If the player is flipped, get the flipped frames
			
			sheetStartX = (drawFrame * SPRITE_PIXEL_SIZE) + drawFrame;
			sheetStartY = 91 + (entities.graphic[i] * SPRITE_PIXEL_SIZE);
		}
		// Balloon, with type === 2
		else if(entities.type[i] === 2){
			sheetStartX = (entities.graphic[i] * SPRITE_PIXEL_SIZE) + entities.graphic[i];
			sheetStartY = 98 + (0 * SPRITE_PIXEL_SIZE);
		}
		
		// Draw sprite onto canvas
		CTX.drawImage(
			SPRITE_SHEET
			,sheetStartX
			,sheetStartY
			,SPRITE_PIXEL_SIZE
			,SPRITE_PIXEL_SIZE
			,entities.x[i] - (SPRITE_PIXEL_SIZE * .5)
			,entities.y[i] - (SPRITE_PIXEL_SIZE * .5)
			,SPRITE_PIXEL_SIZE
			,SPRITE_PIXEL_SIZE
		);
	}
	
	// Draw score using drawn numbers from SPRITE_SHEET (getting a font to look good at this low of a resolution is rough)
	var numberString = String(settings[SETTING_SCORE]);
	for(var i = 0, l = numberString.length; i < l; i ++){
		CTX.drawImage(
			SPRITE_SHEET
			,7 + (parseInt(numberString[i]) * 4) + parseInt(numberString[i])
			,105
			,4
			,6
			,6 + (i * 6)
			,5
			,4
			,6
		);
	}
	
	lastFrameTimestamp = frameTimestamp; // The last frame's timestamp is now the current frame's timestamp (calculating for the future)
	window.requestAnimationFrame(onAnimationFrame); // Continue the loop on the next draw frame
}

async function keyInput(event){
	if(event.altKey) return; // Ignore all key presses if Alt is held
	if(event.repeat) return event.preventDefault(); // Ignore repeat keystrokes
	
	// Read key presses
	var down = (event.type === 'keydown');
	if(event.key === 'ArrowLeft')		settings[SETTING_INPUT_LEFT] = down ? JUST_PRESSED : JUST_RELEASED;
	else if(event.key === 'ArrowRight')	settings[SETTING_INPUT_RIGHT] = down ? JUST_PRESSED : JUST_RELEASED;
	else if(event.key === ' ')			settings[SETTING_INPUT_ACTION] = down ? JUST_PRESSED : JUST_RELEASED;
	else return true; // If no registered key, just exit and continue as normal
	
	return event.preventDefault(); // If we found the key in the if/elses above, prevent default actions from the key
}

// Changes a button's state from just pressed or released to continuously held or released
function updateInputState(id){
	if(settings[id] === JUST_PRESSED) settings[id] = PRESSED;
	else if(settings[id] === JUST_RELEASED) settings[id] = RELEASED;
}

// Listen for button inputs
document.addEventListener('keydown',keyInput);
document.addEventListener('keyup',keyInput);

// Load WASM
fetch('script.wasm',{headers:{'Content-Type':'application/wasm'}})
.then(response => response.arrayBuffer())
.then(bits => WebAssembly.instantiate(bits))
.then(async function(obj){
	WASM = obj.instance;	// Put the WASM instance into a global variable
	WASM.exports.main();	// Run the WASM's "main" function
	
	// Share entity data
	entities.active		= new Int32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(0),ENTITY_MAX);
	entities.type		= new Int32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(1),ENTITY_MAX);
	entities.x			= new Float32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(2),ENTITY_MAX);
	entities.y			= new Float32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(3),ENTITY_MAX);
	entities.speedX		= new Float32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(4),ENTITY_MAX);
	entities.speedY		= new Float32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(5),ENTITY_MAX);
	entities.graphic	= new Int32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(6),ENTITY_MAX);
	entities.frame		= new Float32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(7),ENTITY_MAX);
	entities.flip		= new Int32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(8),ENTITY_MAX);
	
	// Share settings data
	settings			= new Int32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(9),ENTITY_MAX);
	
	// Start the game loop, which happens on every animation frame
	window.requestAnimationFrame(onAnimationFrame);
});