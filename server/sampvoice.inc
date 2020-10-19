#if defined _sampvoice_included
    #endinput
#endif
#define _sampvoice_included

#define SV_VERSION      11

#define SV_NULL         0
#define SV_INFINITY     -1
#define SV_FALSE        false
#define SV_TRUE         true

enum SV_PARAMETER
{
    SV_PARAMETER_FREQUENCY = 1,
    SV_PARAMETER_VOLUME    = 2,
    SV_PARAMETER_PANNING   = 3,
    SV_PARAMETER_EAXMIX    = 4,
    SV_PARAMETER_SRC       = 8
}

#define SV_PTR:
#define SV_STR:
#define SV_INT:
#define SV_UINT:
#define SV_FLOAT:       Float:
#define SV_BOOL:        bool:
#define SV_VOID:        void:

#define SV_STREAM:      SV_PTR:
#define SV_GSTREAM:     SV_PTR:
#define SV_LSTREAM:     SV_PTR:
#define SV_LPSTREAM:    SV_PTR:
#define SV_SLSTREAM:    SV_PTR:
#define SV_DLSTREAM:    SV_PTR:
#define SV_EFFECT:      SV_PTR:

native SV_VOID:SvDebug(SV_BOOL:mode);
native SV_VOID:SvInit(SV_UINT:bitrate);
native SV_UINT:SvGetVersion(SV_UINT:playerid);

native SV_BOOL:SvHasMicro(SV_UINT:playerid);
native SV_BOOL:SvStartRecord(SV_UINT:playerid);
native SV_BOOL:SvStopRecord(SV_UINT:playerid);
native SV_BOOL:SvAddKey(SV_UINT:playerid, SV_UINT:keyid);
native SV_BOOL:SvHasKey(SV_UINT:playerid, SV_UINT:keyid);
native SV_BOOL:SvRemoveKey(SV_UINT:playerid, SV_UINT:keyid);
native SV_VOID:SvRemoveAllKeys(SV_UINT:playerid);
native SV_BOOL:SvMutePlayerStatus(SV_UINT:playerid);
native SV_VOID:SvMutePlayerEnable(SV_UINT:playerid);
native SV_VOID:SvMutePlayerDisable(SV_UINT:playerid);

native SV_GSTREAM:SvCreateGStream(SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_LPSTREAM:SvCreateSLStreamAtPoint(SV_FLOAT:distance, SV_FLOAT:posx, SV_FLOAT:posy, SV_FLOAT:posz, SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_SLSTREAM:SvCreateSLStreamAtVehicle(SV_FLOAT:distance, SV_UINT:vehicleid, SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_SLSTREAM:SvCreateSLStreamAtPlayer(SV_FLOAT:distance, SV_UINT:playerid, SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_SLSTREAM:SvCreateSLStreamAtObject(SV_FLOAT:distance, SV_UINT:objectid, SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_LPSTREAM:SvCreateDLStreamAtPoint(SV_FLOAT:distance, SV_UINT:maxplayers, SV_FLOAT:posx, SV_FLOAT:posy, SV_FLOAT:posz, SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_DLSTREAM:SvCreateDLStreamAtVehicle(SV_FLOAT:distance, SV_UINT:maxplayers, SV_UINT:vehicleid, SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_DLSTREAM:SvCreateDLStreamAtPlayer(SV_FLOAT:distance, SV_UINT:maxplayers, SV_UINT:playerid, SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_DLSTREAM:SvCreateDLStreamAtObject(SV_FLOAT:distance, SV_UINT:maxplayers, SV_UINT:objectid, SV_UINT:color = SV_NULL, SV_STR:name[] = "");
native SV_VOID:SvUpdateDistanceForLStream(SV_LSTREAM:lstream, SV_FLOAT:distance);
native SV_VOID:SvUpdatePositionForLPStream(SV_LPSTREAM:lpstream, SV_FLOAT:posx, SV_FLOAT:posy, SV_FLOAT:posz);
native SV_BOOL:SvAttachListenerToStream(SV_STREAM:stream, SV_UINT:playerid);
native SV_BOOL:SvHasListenerInStream(SV_STREAM:stream, SV_UINT:playerid);
native SV_BOOL:SvDetachListenerFromStream(SV_STREAM:stream, SV_UINT:playerid);
native SV_VOID:SvDetachAllListenersFromStream(SV_STREAM:stream);
native SV_BOOL:SvAttachSpeakerToStream(SV_STREAM:stream, SV_UINT:playerid);
native SV_BOOL:SvHasSpeakerInStream(SV_STREAM:stream, SV_UINT:playerid);
native SV_BOOL:SvDetachSpeakerFromStream(SV_STREAM:stream, SV_UINT:playerid);
native SV_VOID:SvDetachAllSpeakersFromStream(SV_STREAM:stream);
native SV_VOID:SvStreamParameterSet(SV_STREAM:stream, SV_PARAMETER:parameter, SV_FLOAT:value);
native SV_VOID:SvStreamParameterReset(SV_STREAM:stream, SV_PARAMETER:parameter);
native SV_BOOL:SvStreamParameterHas(SV_STREAM:stream, SV_PARAMETER:parameter);
native SV_FLOAT:SvStreamParameterGet(SV_STREAM:stream, SV_PARAMETER:parameter);
native SV_VOID:SvStreamParameterSlideFromTo(SV_STREAM:stream, SV_PARAMETER:parameter, SV_FLOAT:startvalue, SV_FLOAT:endvalue, SV_UINT:time);
native SV_VOID:SvStreamParameterSlideTo(SV_STREAM:stream, SV_PARAMETER:parameter, SV_FLOAT:endvalue, SV_UINT:time);
native SV_VOID:SvStreamParameterSlide(SV_STREAM:stream, SV_PARAMETER:parameter, SV_FLOAT:deltavalue, SV_UINT:time);
native SV_VOID:SvDeleteStream(SV_STREAM:stream);

native SV_EFFECT:SvEffectCreateChorus(SV_INT:priority, SV_FLOAT:wetdrymix, SV_FLOAT:depth, SV_FLOAT:feedback, SV_FLOAT:frequency, SV_UINT:waveform, SV_FLOAT:delay, SV_UINT:phase);
native SV_EFFECT:SvEffectCreateCompressor(SV_INT:priority, SV_FLOAT:gain, SV_FLOAT:attack, SV_FLOAT:release, SV_FLOAT:threshold, SV_FLOAT:ratio, SV_FLOAT:predelay);
native SV_EFFECT:SvEffectCreateDistortion(SV_INT:priority, SV_FLOAT:gain, SV_FLOAT:edge, SV_FLOAT:posteqcenterfrequency, SV_FLOAT:posteqbandwidth, SV_FLOAT:prelowpasscutoff);
native SV_EFFECT:SvEffectCreateEcho(SV_INT:priority, SV_FLOAT:wetdrymix, SV_FLOAT:feedback, SV_FLOAT:leftdelay, SV_FLOAT:rightdelay, SV_BOOL:pandelay);
native SV_EFFECT:SvEffectCreateFlanger(SV_INT:priority, SV_FLOAT:wetdrymix, SV_FLOAT:depth, SV_FLOAT:feedback, SV_FLOAT:frequency, SV_UINT:waveform, SV_FLOAT:delay, SV_UINT:phase);
native SV_EFFECT:SvEffectCreateGargle(SV_INT:priority, SV_UINT:ratehz, SV_UINT:waveshape);
native SV_EFFECT:SvEffectCreateI3dl2reverb(SV_INT:priority, SV_INT:room, SV_INT:roomhf, SV_FLOAT:roomrollofffactor, SV_FLOAT:decaytime, SV_FLOAT:decayhfratio, SV_INT:reflections, SV_FLOAT:reflectionsdelay, SV_INT:reverb, SV_FLOAT:reverbdelay, SV_FLOAT:diffusion, SV_FLOAT:density, SV_FLOAT:hfreference);
native SV_EFFECT:SvEffectCreateParameq(SV_INT:priority, SV_FLOAT:center, SV_FLOAT:bandwidth, SV_FLOAT:gain);
native SV_EFFECT:SvEffectCreateReverb(SV_INT:priority, SV_FLOAT:ingain, SV_FLOAT:reverbmix, SV_FLOAT:reverbtime, SV_FLOAT:highfreqrtratio);
native SV_VOID:SvEffectAttachStream(SV_EFFECT:effect, SV_STREAM:stream);
native SV_VOID:SvEffectDetachStream(SV_EFFECT:effect, SV_STREAM:stream);
native SV_VOID:SvEffectDelete(SV_EFFECT:effect);

forward SV_VOID:OnPlayerActivationKeyPress(SV_UINT:playerid, SV_UINT:keyid);
forward SV_VOID:OnPlayerActivationKeyRelease(SV_UINT:playerid, SV_UINT:keyid);
