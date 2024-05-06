# **SAMPVOICE**
## Description
---------------------------------
**SAMPVOICE** - is a Software Development Kit (SDK) for implementing voice communication systems in the Pawn language for SA:MP servers.

#### Version support
----------------------------------
* Client: SA:MP 0.3.7 (R1, R3)
* Server: SA:MP 0.3.7 (R2)

## Features
---------------------------------
* Controlled voice transmission
* Player microphone control
* Binding a voice stream to game objects
* And many more features...

## Installation
---------------------------------
For the plugin to work, it must be installed by players and on server. There is a client and server part of the plugin for this.

#### For players
---------------------------------
Players have access to 2 installation options: automatic (via the installer) and manual (via the archive).

##### Automatically
---------------------------------
1. In order to download the installer, head over to [the `releases` page](https://github.com/CyberMor/sampvoice/releases) and choose the desired version of the plugin.
2. After downloading, launch the installer. The installer will automatically find your GTA San Andreas folder.
3. If the directory is correct, click "Next" and wait for the installation to complete. After, click "Finish" to close the installer.

##### Manually
---------------------------------
1. Head over [the `releases` page](https://github.com/CyberMor/sampvoice/releases) and download the archive with the desired client version.
2. Extract the archive to your GTA San Andreas folder with the replacement of files.

#### For developers
---------------------------------
1. Download from [the `releases` page](https://github.com/CyberMor/sampvoice/releases) the desired version of the plugin for your platform.
2. Unpack the archive to the root directory of the server.
3. Add to the *server.cfg* server configuration file the line *"plugins sampvoice"* for *Win32* and *"plugins sampvoice.so"* for *Linux x86*. **(If you have a Pawn.RakNet plugin be sure to place SampVoice after it)**

## Usage
---------------------------------
To get started using the plugin, read the documentation that comes with the server side. To do this, open the *sampvoice.chm* file using the Windows reference. **(If the documentation does not open, right-click on the documentation file, then Properties -> Unblock -> OK)**

To start using the plugin functionality, include the header file:
```php
#include <sampvoice>
```

#### Quick reference
---------------------------------
You need to know that the plugin uses its own types and constant system. Despite the fact that this is just a wrapper over the basic types of Pawn, it helps to navigate the types of the plugin itself and not to confuse pointers.

To redirect audio traffic from player A to player B, you need to create an audio stream (using **SvCreateStream**), then attach it to speaker-channel of player A (using **SvAttachStream**), then attach player B to the audio stream (using **SvAttachListener**). Done, now when player A's microphone is activated (for example, with the **SvPlay** function), his audio traffic will be transmitted and then listened to by player B.

Sound streams are pretty handy. They can be visualized using the example of Discord:
* A stream is an analogue of a room (or channel).
* Speakers are participants in the room with speakers off but microphone on.
* Listeners are participants in the room with their microphone off but speakers on.

Players can be both speakers and listeners at the same time. In this case, the player's audio traffic will not be forwarded to him.

#### Example
---------------------------------
Let's take a look at some of the plugin's features with a practical example. Below we will create a server that will bind all connected players to the global stream, and also create a local stream for each player. Thus, players will be able to communicate through the global (heard equally at any point on the map) and local (heard only near the player) chats.
```php
#include <sampvoice>

#define GLOBAL_CHANNEL 0
#define  LOCAL_CHANNEL 1

new SV_UINT:gstream = SV_NONE;
new SV_UINT:lstream[MAX_PLAYERS] = { SV_NONE, ... };

public OnPlayerConnect(playerid)
{
    if (SvGetVersion(playerid) == 0) // Checking for plugin availability
    {
        SendClientMessage(playerid, -1, "Failed to find plugin.");
    }
    else if (SvHasMicro(playerid) == SV_FALSE) // Checking for microphone availability
    {
        SendClientMessage(playerid, -1, "Failed to find microphone.");
    }
    else
    {
        if (gstream != SV_NONE)
        {
            SvSetKey(playerid, 0x5A, GLOBAL_CHANNEL); // Z key
            SvAttachStream(playerid, gstream, GLOBAL_CHANNEL);
            SvAttachListener(gstream, playerid);
            SvSetIcon(gstream, "speaker");

            SendClientMessage(playerid, -1, "Press Z to talk to global chat.");
        }
        if ((lstream[playerid] = SvCreateStream(40.0)) != SV_NONE)
        {
            SvSetKey(playerid, 0x42, LOCAL_CHANNEL); // B key
            SvAttachStream(playerid, lstream[playerid], LOCAL_CHANNEL);
            SvSetTarget(lstream[playerid], SvMakePlayer(playerid));
            SvSetIcon(lstream[playerid], "speaker");

            SendClientMessage(playerid, -1, "Press B to talk to local chat.");
        }
    }
}

public OnPlayerDisconnect(playerid, reason)
{
    // Removing the player's local stream after disconnecting
    if (lstream[playerid] != SV_NONE)
    {
        SvDeleteStream(lstream[playerid]);
        lstream[playerid] = SV_NONE;
    }
}

public OnGameModeInit()
{
    // Uncomment the line to enable debug mode
    // SvEnableDebug();

    gstream = SvCreateStream();
}

public OnGameModeExit()
{
    if (gstream != SV_NONE)
    {
        SvDeleteStream(gstream);
        gstream = SV_NONE;
    }
}
```

## Compiling
---------------------------------
Plugin compiles for *Win32* and *Linux x86* platforms.
Compiled files will be in *sampvoice/binaries* directory.

Below are further instructions:

Clone the repository to your computer and go to the plugin directory:
```sh
git clone https://github.com/CyberMor/sampvoice.git
cd sampvoice
```

### Windows (Client/Server)
---------------------------------
To compile the client side of the plugin, you need the *DirectX 9 SDK*. By default, the client part is compiled for version **SA: MP 0.3.7 (R1)**, but you can also explicitly tell the compiler the version for the build using the **SAMP_R1** and **SAMP_R3** macros. In order to build the client and server parts of the plugin for the *Win32* platform, open the *sampvoice.sln* project in Visual Studio 2019 (or higher) and compile it:
> Build -> Build Solution (F7)

### Linux (Ubuntu 20.04) (Server)
---------------------------------
Make sure you have all dependencies installed:
```sh
sudo apt install build-essential gcc-multilib g++-multilib
```
To build the **control-server** for the *Linux x86* platform, follow these instructions:
```sh
cd control
make
cd ..
```
To build the **voice-server** for the *Linux x86* platform, follow these instructions:
```sh
cd voice
make
cd ..
```
