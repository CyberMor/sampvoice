# **SAMPVOICE**

English | [Русский](https://github.com/CyberMor/sampvoice/blob/master/README.ru.md)

## Description
---------------------------------
**SAMPVOICE** - is Software Development Kit for the implementation of voice systems in the Pawn language for SA:MP servers. Currently the plugin supports only version **SA:MP 0.3.7-R1**.

## Features
---------------------------------
* Controlled voice stream
* Play sound files
* Record voice stream in the buffer and broadcast it
* Player microphone control
* Creating sound effects
* Binding voice stream to game objects
* And many more minor features...

## Installation
---------------------------------
In order for the plugin to work, it is necessary for the players to install the plugin locally as well as the plugin must be loaded on the server. For this there are client and server version of the plugin.

#### For players
---------------------------------
For players there are two variants for installation: automatic (through an installer) and manually (using the archive)

##### Automatically
---------------------------------
1. In order to download the installer, head over to [the `releases` page](https://github.com/CyberMor/sampvoice/releases) and choose the desired version of the plugin.
2. After downloading, launch the installer and choose the desired language for your installation, afterwards the installer will automatically find your GTA San Andreas folder.
3. Press `OK` and wait until the installation finishes.

##### Manually
---------------------------------
1. Head over [the `releases` page](https://github.com/CyberMor/sampvoice/releases) and download the archive with the desired client version.
2. Extract the archive to your GTA San Andreas folder.

#### For developers
---------------------------------
1. Download from [the `releases` page](https://github.com/CyberMor/sampvoice/releases) the desired version of the plugin for your platform
2. Extract the files to the root of your server directory.
3. Add to the server configuration file (`server.cfg`) the line *"plugins sampvoice"* for Win32 and *"plugins sampvoice.so"* for Linux x86 (If you're using the Pawn.RakNet plugin, make sure this [sampvoice] plugin loads *after* Pawn.RakNet) 
3. Add to the `plugins` line the plugin's name as seen below
Windows 32 bit
```
plugins sampvoice
```
Linux x86
```
plugins sampvoice.so
```
**If your server uses Pawn.RakNet, then put `sampvoice` after it.**

## Usage
---------------------------------
In order to start using the plugin, read the documentation that goes with the server kit. For this, open `sampvoice.chm` file using `Help Explorer Viewer`.
Its important to know, that the plugin uses its own system of types and constants. Even though that this this is only a cover over the basic types of Pawn, it helps in the navigate through the internal types of the plugin and not confuse between the pointers.

In order to start using the functions of the plugin, include it in your gamemode:
```php
#include <sampvoice>
```

In order for your script to accept voice packets, add to it the callback `OnPlayerVoice`. Packet cannot be sent directly to the player, because of this inside SV the are audio streams. "audio streams" are abstractive entities that you can connnect players to and send voice packets over. moreover, it is possible to create in them audio effects and play audio files. This is pretty much universal thing.
Lets view some of the plugin's possibilities on a practical example. Afterwards we will create a server, that will accept packets and send it back to the player, that way the player will hear itself.
```php
#include <sampvoice>

new SV_GSTREAM:player_stream[MAX_PLAYERS];  // Global variable that will hold the stream for each player

public OnGameModeInit() {
    sv_init(6000, SV_FREQUENCY_VERY_HIGH, SV_VOICE_RATE_100MS);  // Initializing plugin's settings
    return 1;
}

public OnPlayerConnect(playerid) {
    // Checking availability and relevance of the client plugin
    if(sv_get_version(playerid) == SV_NULL) {
        SendClientMessage(playerid, -1, "You don't have SampVoice installed");
    } else if(sv_get_version(playerid) < SV_VERSION) {
        SendClientMessage(playerid, -1, "You're holding an old version of the plugin, possibly incompatiable. update it.");
    }
    // Checking microphone availability
    if(!sv_has_micro(playerid)) {
        SendClientMessage(playerid, -1, "You don't have a microphone connected. You can head, but not talk.");
    }
    // Creating a global stream for the player and connecting him to it
    player_stream[playerid] = sv_gstream_create();
    sv_stream_player_attach(player_stream[playerid], playerid);
    // Installation of player microphone activation keys
    sv_set_key(playerid, 0x32); // 0x32 - this is the code for the key '2'
    return 1;
}

public OnPlayerDisconnect(playerid) {
    sv_stream_delete(player_stream[playerid]);  // Deleting global player stream
    return 1;
}

public SV_BOOL:OnPlayerVoice(
    SV_UINT:playerid,   // ID of the sender
    SV_PACKET:packet,   // Packet descriptor
    SV_UINT:volume      // Volume level [0-32768]
) {
    sv_send_packet(packet, player_stream[playerid]);    // Sending back to the player
	return SV_TRUE;     // Delete the packet? (SV_TRUE - yes, SV_FALSE - no)
}
```
In order for one player to hear another, it is necessary to connect him to a stream of another player using the function `sv_stream_player_attach(stream, playerid)`

## Compiling
---------------------------------
Plugin compiles using the Win32 platform and Linux x86.
Below are further instructions:

Clone the repository and navigate to the directory
```sh
git clone https://github.com/CyberMor/sampvoice.git
cd sampvoice
```

### Windows
---------------------------------
In order to compile the client version of the plugin you will need DirectX SDK. In order to build client and server version of the plugin under Win32 you will need to open the project *sampvoice.sln* inside MS Visual Studio 2017 and compile it
> Build -> Build Solution

### Linux (Server)
---------------------------------
In order to build the server version of the plugin, follow the next instructions:
```sh
cd ./server
make
```


## Donation
---------------------------------
You can motivate the project developer by donating a small amount of money

[![PayPal](https://cdn1.savepice.ru/uploads/2019/1/13/63100462276ba15752b3a1f7f1a9a8b9-full.png)](https://paypal.me/sampvoice)
