<?php

// This file runs when debug mode is on and we're in localhost
if($_SERVER['REMOTE_ADDR'] != '::1') die('Not localhost');

// The last C file's update time is saved in a session variable
session_start();

// If the c file has been edited since the last check, compile it and re-load it
$lastEditTime = filemtime('script.c');
if($lastEditTime > $_SESSION['last_c_update']){
	$_SESSION['last_c_update'] = $lastEditTime;
	exec('clang --target=wasm32 -Os -flto -nostdlib -std=c99 -Wl,--no-entry -Wl,--export-all -o script.wasm script.c');
	
	die('1');
}

die('0');

?>