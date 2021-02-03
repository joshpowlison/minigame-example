'use strict';

///////////////
// CONSTANTS //
///////////////

const CANVAS				= document.getElementById('canvas');
const CTX					= CANVAS.getContext('2d');
const SPRITES				= new Image();

///////////////
// VARIABLES //
///////////////

var WASM					= null;
var lastFrameTimestamp;

var entities				= {};
var settings				= [];
var settingsFloat			= [];

var entityDataTypes = {
	active		:	'Int32Array'
	,type		:	'Int32Array'
	,x			:	'Float32Array'
	,y			:	'Float32Array'
	,speedX		:	'Float32Array'
	,speedY		:	'Float32Array'
	,maxSpeed	:	'Float32Array'
	,rot		:	'Float32Array'
	,rotSpeed	:	'Float32Array'
	,graphic	:	'Int32Array'
	,frame		:	'Float32Array'
	,flip		:	'Int32Array'
	,timer		:	'Float32Array'
};

var sfxValues = [];

///////////////
// FUNCTIONS //
///////////////

async function loadFiles(initialLoad = true){
	// Load sprites
	SPRITES.src =  'game/sprites.png';
	
	// Load C File to get all constants and put them into JS
	await fetch('game/script.c')
	.then(response => response.text())
	.then(text => {
		var regex = /[\n\r]const\s+([^\s]+)\s*=\s*([^;]+)\s*;/gi;
		var results;
		
		while((results = regex.exec(text)) !== null){
			window[results[1]] = parseInt(results[2]);
		}
	});
	
	// Set canvas size from C file
	CANVAS.height				= CANVAS_WIDTH;
	CANVAS.width				= CANVAS_HEIGHT;
	CTX.imageSmoothingEnabled	= false; // Our pixel art will look trash without this
	
	// Create SFX based on C file names
	sfxValues[SFX_JUMP]			= [['e2'],.25];
	sfxValues[SFX_COLLECT]		= [['e4'],.1];
	sfxValues[SFX_LAND]			= [['d2'],.25];
	sfxValues[SFX_MOVE_1]		= [['e2'],.05,2];
	sfxValues[SFX_MOVE_2]		= [['d2'],.05,2];
	sfxValues[SFX_GAME_START]	= [['a2','b2','c2','c3'],.15];
	sfxValues[SFX_GAME_END]		= [['c3','a2','c3','c4'],.15];

	// Load WASM
	await fetch('game/script.wasm',{headers:{'Content-Type':'application/wasm'}})
	.then(response => response.arrayBuffer())
	.then(bits => WebAssembly.instantiate(bits))
	.then(async function(obj){
		WASM = obj.instance;
		if(initialLoad) WASM.exports.main();
		
		var wasmBufferLength = WASM.exports.getBufferLength();
		
		// On initial load, instantiate the values
		if(initialLoad){
			// Set up entity vars that need set up with WASM
			var keys = Object.keys(entityDataTypes);
			for(var i = 0, l = keys.length; i < l; i ++){
				entities[keys[i]] = new window[entityDataTypes[keys[i]]](
					WASM.exports.memory.buffer
					,WASM.exports.getDataPointer(i)
					,wasmBufferLength
				);
			}
			
			// Save and share the collection data
			settings			= new Int32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(13),wasmBufferLength);
			settingsFloat		= new Float32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(14),wasmBufferLength);
			
			// Set the lastFrameTimestamp to now
			lastFrameTimestamp = 0;
		} else {
			// Copy values (don't just reference them)
			var settingsValues		= [...settings];
			var settingsFloatValues	= [...settingsFloat];
			var entitiesValues		= {...entities};
			
			// Set up entity vars that need set up with WASM
			var keys = Object.keys(entityDataTypes);
			for(var i = 0, l = keys.length; i < l; i ++){
				entities[keys[i]] = new window[entityDataTypes[keys[i]]](
					WASM.exports.memory.buffer
					,WASM.exports.getDataPointer(i)
					,wasmBufferLength
				);
			}
			
			// Save and share the collection data
			settings			= new Int32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(13),wasmBufferLength);
			settingsFloat		= new Float32Array(WASM.exports.memory.buffer,WASM.exports.getDataPointer(14),wasmBufferLength);
			
			// Update shared to match old data
			// var values
			for(var i = 0; i < ENTITY_MAX; i ++){
				settings[i]			= settingsValues[i];
				settingsFloat[i]	= settingsFloatValues[i];
				
				// Update all entity values
				for(var keyI = 0, keyL = keys.length; keyI < keyL; keyI ++){
					entities[keys[keyI]][i] = entitiesValues[keys[keyI]][i];
				}
			}
			
			console.log('Files Refreshed!');
		}
		
		window.requestAnimationFrame(onAnimationFrame);
	});
}

// The Loop
function onAnimationFrame(frameTimestamp) {
	// Delta time, based on seconds
	var deltaTime = (frameTimestamp - lastFrameTimestamp) / 1000;
	
	/// FUNCTION LOOP ///
	if(
		!settings[SETTING_PAUSE]
		&& (!settings[SETTING_DEBUG] || !settings[SETTING_DEBUG_PAUSE])
	){
		//// CONTROLS ////
		settings[SETTING_INPUT_ACTION]	= playerInput[ACTION];
		settings[SETTING_INPUT_LEFT]	= playerInput[LEFT];
		settings[SETTING_INPUT_RIGHT]	= playerInput[RIGHT];
		
		//// WASM processes the movement calculations and tells us what SFX to play////
		var sfx = WASM.exports.loopEntities(deltaTime);
		if(sfx > 0) playSynth(...sfxValues[sfx])
	}
	
	//// JS draws the entities onto the canvas ////
	
	// Offsets the transformations; we use this to restore settings at the end
	CTX.clearRect(0,0,CANVAS.width,CANVAS.height);
	
	// Draw background
	CTX.drawImage(
		SPRITES
		,0
		,90
		,90
		,90
		,0
		,0
		,90
		,90
	);
	
	var canvasOffsetX	= 0;
	var canvasOffsetY	= 0;
	for(var i = 0; i < ENTITY_MAX; i ++){
		if(entities.active[i] === 0) continue;
		
		// Move the context to center the element for drawing
		CTX.translate(
			entities.x[i] - canvasOffsetX
			,entities.y[i] - canvasOffsetY
		);
		// Rotate the context for drawing
		if(entities.rot[i] !== 0) CTX.rotate((entities.rot[i] * Math.PI / 180));
		
		// Save the current canvas offset
		canvasOffsetX = entities.x[i];
		canvasOffsetY = entities.y[i];
		
		switch(entities.type[i]){
			// Me
			case 1:
				var drawFrame = (entities.frame[i] < 30) ? 0 : 1; 
				if(entities.flip[i] === 1) drawFrame += 2;
			
				// Get the position of the image and draw it. Position in image is based on sprite number, frame number, and considers padding that exists between images (to prevent bleeding that is inherent to every number being a float in JS)
				CTX.drawImage(
					SPRITEIMG_MES
					,(drawFrame * SPRITE_SIZE) + drawFrame
					,(entities.graphic[i] * SPRITE_SIZE) + entities.graphic[i]
					,SPRITE_SIZE
					,SPRITE_SIZE
					,-(SPRITE_SIZE * SCALE_ME * .5)
					,-(SPRITE_SIZE * SCALE_ME * .5)
					,SPRITE_SIZE * SCALE_ME
					,SPRITE_SIZE * SCALE_ME
				);
				
				break;
			// Collect
			case 2:
				var drawFrame = entities.graphic[i];
				if(entities.flip[i] === 1) CTX.scale(-1,1);
				
				// Choose other frame based on timer
				if(
					(entities.timer[i] / entities.maxSpeed[i] > .5)
					&& entities.speedY[i] < 0
				) drawFrame ++;
				
				CTX.drawImage(
					SPRITES
					,(drawFrame * SPRITE_SIZE) + drawFrame
					,(0 * SPRITE_SIZE) + 0
					,SPRITE_SIZE
					,SPRITE_SIZE
					,-(SPRITE_SIZE / 2)
					,-(SPRITE_SIZE / 2)
					,SPRITE_SIZE
					,SPRITE_SIZE
				);
				
				// Flip the image if needbe
				if(entities.flip[i] === 1) CTX.scale(-1,1);
				break;
			default:
				break;
		}
		
		// Rotate the canvas back to its correct state
		if(entities.rot[i] !== 0) CTX.rotate(-(entities.rot[i] * Math.PI / 180));
	}
	
	// Recenter the canvas context after performing all of the operations
	CTX.translate(-canvasOffsetX,-canvasOffsetY);
	
	//// Canvas HUD ////
	if(settings[SETTING_GAME_STATE] === STATE_START){
		CTX.drawImage(
			SPRITES
			,0
			,19
			,32
			,26
			,(CANVAS.width / 2) - 12
			,30
			,32
			,26
		);
	}
	
	// Collect number
	CTX.drawImage(
		SPRITES
		,0
		,0
		,SPRITE_SIZE
		,SPRITE_SIZE
		,3
		,2
		,SPRITE_SIZE * 2
		,SPRITE_SIZE * 2
	);
	drawNumber(Math.floor(settings[SETTING_SCORE]),17,5);
	
	// Timer
	CTX.drawImage(
		SPRITES
		,7
		,7
		,SPRITE_SIZE
		,SPRITE_SIZE
		,CANVAS.width - 14
		,2
		,SPRITE_SIZE * 2
		,SPRITE_SIZE * 2
	);
	if(settingsFloat[SETTINGF_GAME_TIMER] >= 100)		drawNumber(Math.floor(settingsFloat[SETTINGF_GAME_TIMER]),58,5);
	else if(settingsFloat[SETTINGF_GAME_TIMER] >= 10)	drawNumber(Math.floor(settingsFloat[SETTINGF_GAME_TIMER]),64,5);
	else						drawNumber(Math.floor(settingsFloat[SETTINGF_GAME_TIMER]),70,5);
	
	// Process the button presses
	processPresses();
	
	/// DEBUG ///
	if(settings[SETTING_DEBUG] === 1){
		// CTX.globalCompositeOperation = 'luminosity';
		
		var drawValuePosition = 20;
		
		for(var i = 0; i < ENTITY_MAX; i ++){
			// Get the entities we want to display and show their info
			if(i >= settings[SETTING_DEBUG_LISTPOS] && i < settings[SETTING_DEBUG_LISTPOS] + 10){
				// Draw values on left
				drawNumber(i,1,drawValuePosition);
				drawNumber(entities.active[i],20,drawValuePosition);
				drawNumber(entities.type[i],30,drawValuePosition);
				drawNumber(Math.round(entities.x[i]),40,drawValuePosition);
				drawNumber(Math.round(entities.y[i]),60,drawValuePosition);
				drawValuePosition += 7;
			}
			
			// Draw id over active entities
			if(entities.active[i]){
				drawNumber(i,entities.x[i],entities.y[i]);
			}
		}
		// CTX.globalCompositeOperation = 'source-over';
	}
	
	if(
		settings[SETTING_PAUSE]
		|| (settings[SETTING_DEBUG] && settings[SETTING_DEBUG_PAUSE])
	){
		CTX.fillStyle = '#000000aa';
		CTX.fillRect(0,0,CANVAS.width,CANVAS.height);
	}
	
	// The last frame's timestamp is now the current frame's timestamp (calculating for the future)
	lastFrameTimestamp = frameTimestamp;
	
	window.requestAnimationFrame(onAnimationFrame);
}

// Keys for debugging tools
function debugKeys(event){
	if(event.key === 'F10'){
		settings[SETTING_DEBUG] = (settings[SETTING_DEBUG]) ? 0 : 1;
		
		if(settings[SETTING_DEBUG]) document.title = 'DEBUG MINIGAME';
		event.preventDefault();
	}
	
	if(settings[SETTING_DEBUG] === 0) return;
	
	switch(event.key){
		// Scroll up debug entity list
		case 'PageUp':
			settings[SETTING_DEBUG_LISTPOS] -= 10;
			if(settings[SETTING_DEBUG_LISTPOS] < 0) settings[SETTING_DEBUG_LISTPOS] = ENTITY_MAX - 10;
			event.preventDefault();
			break;
		// Scroll down debug entity list
		case 'PageDown':
			settings[SETTING_DEBUG_LISTPOS] += 10;
			if(settings[SETTING_DEBUG_LISTPOS] > ENTITY_MAX - 10) settings[SETTING_DEBUG_LISTPOS] = 0;
			event.preventDefault();
			break;
		// Pause/unpause
		case 'F11':
			settings[SETTING_DEBUG_PAUSE] = settings[SETTING_DEBUG_PAUSE] ? 0 : 1;
			event.preventDefault();
			break;
	}
}

///////////////
// LISTENERS //
///////////////

document.addEventListener('keydown',debugKeys);

window.addEventListener('focus',function(){
	settings[SETTING_PAUSE] = 0;
	
	// If we're on localhost, see if we need to rebuild the C file
	if(location.hostname === 'localhost'){
		fetch('game/compile.php')
		.then(response => response.text())
		.then(text => {
			if(text == '1'){
				loadFiles(false);
				console.log('C updated.');
			}
		});
	}
});

window.addEventListener('blur',function(){
	settings[SETTING_PAUSE] = 1;
});

///////////////
// START/RUN //
///////////////

document.getElementById('canvas').style.backgroundImage = 'url("game/background.png")';

loadFiles();