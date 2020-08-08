# **SAMPVOICE**
English | [Русский](https://github.com/CyberMor/sampvoice/blob/master/README.ru.md)

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
For the plugin to work, it must be installed by the players and on the server. There is a client and server part of the plugin for this.

#### For players
---------------------------------
Players have access to 2 installation options: automatic (via the installer) and manual (via the archive).

##### Automatically
---------------------------------
1. In order to download the installer, head over to [the `releases` page](https://github.com/CyberMor/sampvoice/releases) and choose the desired version of the plugin.
2. After downloading, launch the installer and choose the desired language for your installation, afterwards the installer will automatically find your GTA San Andreas folder.
3. If the directory is correct, click "OK" and wait for the installation to complete. After the installation is complete, the installer will exit.

##### Manually
---------------------------------
1. Head over [the `releases` page](https://github.com/CyberMor/sampvoice/releases) and download the archive with the desired client version.
2. Extract the archive to your GTA San Andreas folder.

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
You need to know that the plugin uses its own type and constant system. Despite the fact that this is just a wrapper over the basic types of Pawn, it helps to navigate the types of the plugin itself and not to confuse pointers.

In order to redirect audio traffic from player A to player B, you need to create an audio stream (for example, a global one, using **SvCreateGStream**), then attach it to the stream of player A as a speaker (using **SvAttachSpeakerToStream**), after which attach to player B's stream as a listener (using **SvAttachListenerToStream**). Done. Now, when player A's microphone is activated (for example, with the **SvStartRecord** function), his audio traffic will be transmitted and then heard by player B.

Sound streams are pretty handy. They can be visualized using the example of Discord:
* A stream is an analogue of a room (or channel).
* Speakers are participants in the room with mute but microphone on.
* Listeners are participants in the room with their microphone mute but mute.

Players can be both speakers and listeners at the same time. In this case, the player's audio traffic will not be forwarded to him.

#### Example
---------------------------------
Let's take a look at some of the plugin's features with a practical example. Below we will create a server that will bind all connected players to the global stream, and also create a local stream for each player. Thus, players will be able to communicate through the global (heard equally at any point on the map) and local (heard only near the player) chats.
```php
#include <sampvoice>

new SV_GSTREAM:gstream = SV_NULL;
new SV_LSTREAM:lstream[MAX_PLAYERS] = { SV_NULL, ... };

/*
    The public OnPlayerActivationKeyPress and OnPlayerActivationKeyRelease
    are needed in order to redirect the player's audio traffic to the
    corresponding streams when the corresponding keys are pressed.
*/

public SV_VOID:OnPlayerActivationKeyPress(SV_UINT:playerid, SV_UINT:keyid) 
{
    // Attach player to local stream as speaker if 'B' key is pressed
    if (keyid == 0x42 && lstream[playerid]) SvAttachSpeakerToStream(lstream[playerid], playerid);
    // Attach the player to the global stream as a speaker if the 'Z' key is pressed
    if (keyid == 0x5A && gstream) SvAttachSpeakerToStream(gstream, playerid);
}

public SV_VOID:OnPlayerActivationKeyRelease(SV_UINT:playerid, SV_UINT:keyid)
{
    // Detach the player from the local stream if the 'B' key is released
    if (keyid == 0x42 && lstream[playerid]) SvDetachSpeakerFromStream(lstream[playerid], playerid);
    // Detach the player from the global stream if the 'Z' key is released
    if (keyid == 0x5A && gstream) SvDetachSpeakerFromStream(gstream, playerid);
}

public OnPlayerConnect(playerid)
{
    // Checking for plugin availability
    if (SvGetVersion(playerid) == SV_NULL)
    {
        SendClientMessage(playerid, -1, "Could not find plugin sampvoice.");
    }
    // Checking for a microphone
    else if (SvHasMicro(playerid) == SV_FALSE)
    {
        SendClientMessage(playerid, -1, "The microphone could not be found.");
    }
    // Create a local stream with an audibility distance of 40.0, an unlimited number of listeners
    // and the name 'Local' (the name 'Local' will be displayed in red in the players' speakerlist)
    else if (lstream[playerid] = SvCreateDLStreamAtPlayer(40.0, SV_INFINITY, playerid, 0xff0000ff, "Local"))
    {
        SendClientMessage(playerid, -1, "Press Z to talk to global chat and B to talk to local chat.");

        // Attach the player to the global stream as a listener
        if (gstream) SvAttachListenerToStream(gstream, playerid);

        // Assign microphone activation keys to the player
        SvAddKey(playerid, 0x42);
        SvAddKey(playerid, 0x5A);
    }
}

public OnPlayerDisconnect(playerid, reason)
{
    // Removing the player's local stream after disconnecting
    if (lstream[playerid])
    {
        SvDeleteStream(lstream[playerid]);
        lstream[playerid] = SV_NULL;
    }
}

public OnGameModeInit()
{
    // Uncomment the line to enable debug mode
    // SvDebug(SV_TRUE);

    gstream = SvCreateGStream(0xffff0000, "Global");
}

public OnGameModeExit()
{
    if (gstream) SvDeleteStream(gstream);
}
```

## Compiling
---------------------------------
Plugin compiles for *Win32* and *Linux x86* platforms.

Below are further instructions:

Clone the repository to your computer and go to the plugin directory:
```sh
git clone https://github.com/CyberMor/sampvoice.git
cd sampvoice
```

### Windows (Client/Server)
---------------------------------
To compile the client side of the plugin, you need the *DirectX SDK*. By default, the client part is compiled for version **SA: MP 0.3.7 (R1)**, but you can also explicitly tell the compiler the version for the build using the **SAMP_R1** and **SAMP_R3** macros. In order to build the client and server parts of the plugin for the *Win32* platform, open the *sampvoice.sln* project in MS Visual Studio 2019 and compile it:
> Build -> Build Solution (F7)

### Linux (Server)
---------------------------------
To build the server part of the plugin for the *Linux x86* platform, follow these instructions:
```sh
cd server
make
```
