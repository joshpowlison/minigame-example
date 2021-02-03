/*

IDEAS FOR ENGINE DEMO:
	- Make engine.js and script.js into one file
	- Combine all sprites into 1 image (including !me character and numbers)
	-  Make into a Git repo and put on GitHub

Basically, the goal is to condense things as much as possible so that people are looking at less stuff.

Simple demo of game engine in C and JS.

*/

'use strict';

///////////////
// CONSTANTS //
///////////////

const SPRITEIMG_MES		= new Image();
SPRITEIMG_MES.src		= 'game/mes.png';

const SPRITEIMG_NUMBERS	= new Image();
SPRITEIMG_NUMBERS.src	= 'game/numbers.png';

const ACTION			= 0;
const LEFT				= 1;
const RIGHT				= 2;

const JUST_RELEASED		= 0;
const RELEASED			= 1;
const PRESSED			= 2;
const JUST_PRESSED		= 3;

const CONTROL_LEFT		= document.getElementById('control-left');
const CONTROL_RIGHT		= document.getElementById('control-right');
const CONTROL_ACTION	= document.getElementById('control-action');

///////////////
// VARIABLES //
///////////////

var inputType		= 'keyboard';
var playerInput		= [1,1,1];
var gamepadInUse	= false;

var noteLength		= .25;
var context			= new AudioContext(), oscillator;

var masterGainNode				= context.createGain();
masterGainNode.connect(context.destination);
masterGainNode.gain.value		= 1;

var volumeNodes = [
	context.createGain()
	,context.createGain()
	,context.createGain()
];

volumeNodes[0].connect(context.destination);
volumeNodes[1].connect(context.destination);
volumeNodes[2].connect(context.destination);

// These are loud, so we're limiting them hard
const VOLUME_LOW			= .1 * .1;
const VOLUME_MEDIUM			= .5 * .1;
const VOLUME_HIGH			= 1 * .1;

volumeNodes[0].gain.value = VOLUME_LOW;
volumeNodes[1].gain.value = VOLUME_MEDIUM;
volumeNodes[2].gain.value = VOLUME_HIGH;

///////////////
// FUNCTIONS //
///////////////

async function keyInput(event){
	// Ignore all key presses if Alt is held
	if(event.altKey) return;
	
	// Ignore repeat keystrokes
	if(event.repeat) return event.preventDefault();
	
	// Start ignoring the gamepad on keyboard input
	gamepadInUse = false;
	
	// Read key presses
	var down = (event.type === 'keydown');
	switch(event.key){
		case 'a':
		case 'A':
		case 'ArrowLeft':
			playerInput[LEFT] = down ? JUST_PRESSED : JUST_RELEASED;
			CONTROL_LEFT.classList[down ? 'add' : 'remove']('control-pressed');
			break;
		case 'd':
		case 'D':
		case 'ArrowRight':
			playerInput[RIGHT] = down ? JUST_PRESSED : JUST_RELEASED;
			CONTROL_RIGHT.classList[down ? 'add' : 'remove']('control-pressed');
			break;
		case ' ':
		case 'Enter':
		case 'z':
		case 'Z':
			playerInput[ACTION] = down ? JUST_PRESSED : JUST_RELEASED;
			CONTROL_ACTION.classList[down ? 'add' : 'remove']('control-pressed');
			break;
		// Perform default functions
		default:
			return true;
			break;
	}
	
	event.preventDefault();
}

function touchInput(event){
	console.log(event);
	
	// Start ignoring the gamepad on touch input
	gamepadInUse = false;
	
	// Register if any touches are on the buttons; if so, they're pressed!
	var touchLeft	= false;
	var touchRight	= false;
	var touchAction	= false;
	for(var i = 0, l = event.touches.length; i < l; i ++){
		if(checkCollision(event.touches[i].clientX,event.touches[i].clientY,CONTROL_LEFT)) touchLeft = true;
		else if(checkCollision(event.touches[i].clientX,event.touches[i].clientY,CONTROL_RIGHT)) touchRight = true;
		else if(checkCollision(event.touches[i].clientX,event.touches[i].clientY,CONTROL_ACTION)) touchAction = true;
	}
	
	// Read left button
	if(touchLeft && playerInput[LEFT] < PRESSED){
		playerInput[LEFT] = JUST_PRESSED;
		CONTROL_LEFT.classList.add('control-pressed');
	}
	
	if(!touchLeft && playerInput[LEFT] >= PRESSED){
		playerInput[LEFT] = JUST_RELEASED;
		CONTROL_LEFT.classList.remove('control-pressed');
	}
	
	// Read right button
	if(touchRight && playerInput[RIGHT] < PRESSED){
		playerInput[RIGHT] = JUST_PRESSED;
		CONTROL_RIGHT.classList.add('control-pressed');
	}
	
	if(!touchRight && playerInput[RIGHT] >= PRESSED){
		playerInput[RIGHT] = JUST_RELEASED;
		CONTROL_RIGHT.classList.remove('control-pressed');
	}
	
	// Read action button
	if(touchAction && playerInput[ACTION] < PRESSED){
		playerInput[ACTION] = JUST_PRESSED;
		CONTROL_ACTION.classList.add('control-pressed');
	}
	
	if(!touchAction && playerInput[ACTION] >= PRESSED){
		playerInput[ACTION] = JUST_RELEASED;
		CONTROL_ACTION.classList.remove('control-pressed');
	}
	
	// If we can cancel the event, and we are pressing inputs, ignore the default actions
	if(
		event.cancelable
		&& (touchLeft || touchRight || touchAction)
	){
		event.preventDefault();
		event.stopPropagation();
		return true;
	}
}

function checkCollision(x = 0,y = 0,element){
	var bounds = element.getBoundingClientRect();
	
	if(bounds.width == 0 || bounds.height == 0
		|| x < bounds.left || x > (bounds.left + bounds.width)
		|| y < bounds.top || y > (bounds.top + bounds.height)
	) return false;
	
	return true;
}

function gamepadControls(){
	var gamepad = navigator.getGamepads()[0];
	
	// Exit out if this if there's no gamepad in use or detected
	if(!gamepadInUse && !gamepad) return;
	
	// Get inputs for a gamepad if it does exist
	if(gamepad){
		// gamepad.id tells us what type of gamepad it is
		var inputLeft	= gamepad.buttons[14].pressed || gamepad.axes[0] <= -.5;	// Dpad Right or Left Analog Left
		var inputRight	= gamepad.buttons[15].pressed || gamepad.axes[0] >= .5;	// Dpad Right or Left Analog Right
		var inputAction	= gamepad.buttons[0].pressed || gamepad.buttons[1].pressed || gamepad.buttons[2].pressed || gamepad.buttons[3].pressed;	// A / B / X / Y
	}
	
	// If no gamepad is registered as connected, and no buttons are being pressed, ignore it
	if(!gamepadInUse && !inputLeft && !inputRight && !inputAction) return;
	// If a button is being pressed on the gamepad
	else gamepadInUse = true;
	
	// Read left button
	if(inputLeft && playerInput[LEFT] < PRESSED){
		playerInput[LEFT] = JUST_PRESSED;
		CONTROL_LEFT.classList.add('control-pressed');
	}
	
	if(!inputLeft && playerInput[LEFT] >= PRESSED){
		playerInput[LEFT] = JUST_RELEASED;
		CONTROL_LEFT.classList.remove('control-pressed');
	}
	
	// Read right button
	if(inputRight && playerInput[RIGHT] < PRESSED){
		playerInput[RIGHT] = JUST_PRESSED;
		CONTROL_RIGHT.classList.add('control-pressed');
	}
	
	if(!inputRight && playerInput[RIGHT] >= PRESSED){
		playerInput[RIGHT] = JUST_RELEASED;
		CONTROL_RIGHT.classList.remove('control-pressed');
	}
	
	// Read action button
	if(inputAction && playerInput[ACTION] < PRESSED){
		playerInput[ACTION] = JUST_PRESSED;
		CONTROL_ACTION.classList.add('control-pressed');
	}
	
	if(!inputAction && playerInput[ACTION] >= PRESSED){
		playerInput[ACTION] = JUST_RELEASED;
		CONTROL_ACTION.classList.remove('control-pressed');
	}
}

// Process moving from a state of just having been pressed to being perpetually so
function processPresses(){
	for(var i = playerInput.length - 1; i >= 0; i --){
		if(playerInput[i] === JUST_PRESSED) playerInput[i] = PRESSED;
		else if(playerInput[i] === JUST_RELEASED) playerInput[i] = RELEASED;
	}
	
	gamepadControls();
}

function drawNumber(value = '000',x = 0,y = 0){
	var numberString = String(value);
	var scale = 1;
	
	for(var i = 0, l = numberString.length; i < l; i ++){
		CTX.drawImage(
			SPRITEIMG_NUMBERS
			,0
			,(parseInt(numberString[i]) * 6) + parseInt(numberString[i])
			,4
			,6
			,x + (i * 6)
			,y
			,4
			,6
		);
	}
}

function playSynth(notes,noteLength = .25,volume = 2){
	var time = 0;
	var oscillators = [];
	for(var i = 0, l = notes.length; i < l; i ++){
		var halfsteps = 0;
		
		// Calculate the semitone we're moving to
		var data = /(\D)(#?)(\d)/i.exec(notes[i]);
		halfsteps += {
			'A'		: 0
			,'B'	: 2
			,'C'	: 3
			,'D'	: 5
			,'E'	: 7
			,'F'	: 9
			,'G'	: 10
		}[data[1].toUpperCase()];
		if(data[2] === '#') halfsteps ++;
		halfsteps += (parseInt(data[3]) - 4) * 12
		
		oscillators.push(notePlay(halfsteps,0 + time,noteLength,volume));
		time += noteLength;
	}
	
	return oscillators;
}

function notePlay(halfsteps = 0,start = 0,length = .5,volume = 2) {
	// https://pages.mtu.edu/~suits/NoteFreqCalcs.html
	var a4			= 440; // the frequency of A4
	var a			= 1.059463094359; // The 12th root of 2

	// TODO: implement new tone setup from Jeff Neet
	/*
		var octave = 0;
		
		//	f = 440 * 2 ^(n/12)
		//	f = Octave * 110 * 2 ^(n/12)
		
		f = Octave * 110 * Math.pow(2,(n/12))
	*/

	var hz			= a4 * Math.pow(a,halfsteps);

	var oscillator = context.createOscillator();
	oscillator.type = 'square';
	oscillator.frequency.value = hz;
    oscillator.connect(volumeNodes[volume]);
    oscillator.start(context.currentTime + start);
	
	// If length is 0, this could go on forever
    if(length > 0) oscillator.stop(context.currentTime + start + length);
	
	return oscillator;
}

function updateVolume(event = null){
	// If we're updating the control, or don't have a default saved volume, update our saved settings
	if(event || localStorage.getItem('volume') == null){
		localStorage.setItem('volume',document.getElementById('control-volume').value);
	// If we're reading our saved settings, update our control
	} else {
		document.getElementById('control-volume').value = localStorage.getItem('volume');
	}
	
	var volume = localStorage.getItem('volume');
	// Update the volume on each of the gain modules
	volumeNodes[0].gain.value = VOLUME_LOW * volume;
	volumeNodes[1].gain.value = VOLUME_MEDIUM * volume;
	volumeNodes[2].gain.value = VOLUME_HIGH * volume;
}

///////////////
// LISTENERS //
///////////////

document.addEventListener('keydown',keyInput);
document.addEventListener('keyup',keyInput);
document.addEventListener('touchstart',touchInput,{passive:false});
document.addEventListener('touchmove',touchInput,{passive:false});
document.addEventListener('touchend',touchInput,{passive:false});
document.getElementById('control-volume').addEventListener('input',updateVolume);

///////////////
// START/RUN //
///////////////

updateVolume();