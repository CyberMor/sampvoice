#if defined _sampvoice_included
    #endinput
#endif
#define _sampvoice_included

// Types
// ------------------------------------------------------------

#define SV_STR:
#define SV_INT:
#define SV_UINT:
#define SV_FLOAT: Float:
#define SV_BOOL:  bool:
#define SV_VOID:  void:
#define SV_ANY:   any:

// Constants
// ------------------------------------------------------------

#define SV_TRUE  true
#define SV_FALSE false
#define SV_NONE  -1

// [Macros] Version Maker
// ------------------------------------------------------------

#define SvMakeVersion(%0,%1,%2) (((%0) << 24) | (((%1) & 0xFF) << 16) | ((%2) & 0xFFFF))

// [Macros] Target Makers (for SvSetTarget)
// ------------------------------------------------------------

#define SvMakeVehicle(%0) ((1 << 14) | ((%0) & 0x3FFF))
#define  SvMakePlayer(%0) ((2 << 14) | ((%0) & 0x3FFF))
#define  SvMakeObject(%0) ((3 << 14) | ((%0) & 0x3FFF))

// [Constants] Current Version
// ------------------------------------------------------------

#define SV_VERSION SvMakeVersion(4,0,0)

// [Enumerations] Filters
// ------------------------------------------------------------

enum SV_FILTER
{
    SV_CHORUS,
    SV_COMPRESSOR,
    SV_DISTORTION,
    SV_ECHO,
    SV_FLANGER,
    SV_GARGLE,
    SV_I3DL2REVERB,
    SV_PARAMEQ,
    SV_REVERB
}

// [Functions] Debug
// ------------------------------------------------------------

native SV_VOID:SvEnableDebug();
native SV_VOID:SvDisableDebug();
native SV_BOOL:SvCheckDebug();

// [Functions] Player
// ------------------------------------------------------------

native SV_UINT:SvGetVersion(SV_UINT:player);
native SV_BOOL:SvHasMicro(SV_UINT:player);

native SV_BOOL:SvEnableListener(SV_UINT:player);
native SV_BOOL:SvDisableListener(SV_UINT:player);
native SV_BOOL:SvCheckListener(SV_UINT:player);

native SV_BOOL:SvEnableSpeaker(SV_UINT:player, SV_UINT:/*channels*/...);
native SV_BOOL:SvDisableSpeaker(SV_UINT:player, SV_UINT:/*channels*/...);
native SV_BOOL:SvCheckSpeaker(SV_UINT:player, SV_UINT:/*channels*/...);

native SV_BOOL:SvAttachStream(SV_UINT:player, SV_UINT:stream, SV_UINT:/*channels*/...);
native SV_BOOL:SvDetachStream(SV_UINT:player, SV_UINT:stream, SV_UINT:/*channels*/...);
native SV_BOOL:SvHasStream(SV_UINT:player, SV_UINT:stream, SV_UINT:/*channels*/...);

native SV_BOOL:SvSetKey(SV_UINT:player, SV_UINT:key, SV_UINT:/*channels*/...);
native SV_UINT:SvGetKey(SV_UINT:player, SV_UINT:channel);

native SV_BOOL:SvPlay(SV_UINT:player, SV_UINT:/*channels*/...);
native SV_BOOL:SvStop(SV_UINT:player, SV_UINT:/*channels*/...);

// [Functions] Stream
// ------------------------------------------------------------

native SV_UINT:SvCreateStream(SV_FLOAT:distance = 0.0);

native SV_BOOL:SvEnableTransiter(SV_UINT:stream);
native SV_BOOL:SvDisableTransiter(SV_UINT:stream);
native SV_BOOL:SvCheckTransiter(SV_UINT:stream);

native SV_BOOL:SvAttachListener(SV_UINT:stream, SV_UINT:player);
native SV_BOOL:SvDetachListener(SV_UINT:stream, SV_UINT:player);
native SV_BOOL:SvHasListener(SV_UINT:stream, SV_UINT:player);

native SV_VOID:SvSetVolume(SV_UINT:stream, SV_FLOAT:volume);
native SV_VOID:SvSetPanning(SV_UINT:stream, SV_FLOAT:panning);
native SV_VOID:SvSetDistance(SV_UINT:stream, SV_FLOAT:distance);
native SV_VOID:SvSetPosition(SV_UINT:stream, SV_FLOAT:x, SV_FLOAT:y, SV_FLOAT:z);
native SV_VOID:SvSetTarget(SV_UINT:stream, SV_UINT:target);
native SV_VOID:SvSetEffect(SV_UINT:stream, SV_UINT:effect);
native SV_VOID:SvSetIcon(SV_UINT:stream, const SV_STR:icon[]);

native SV_VOID:SvDeleteStream(SV_UINT:stream);

// [Functions] Effect
// ------------------------------------------------------------

native SV_UINT:SvCreateEffect();

native SV_BOOL:SvAppendFilter(SV_UINT:effect, SV_FILTER:filter, SV_INT:priority, SV_ANY:/*parameters*/...);
native SV_VOID:SvRemoveFilter(SV_UINT:effect, SV_FILTER:filter, SV_INT:priority);

native SV_VOID:SvDeleteEffect(SV_UINT:effect);
