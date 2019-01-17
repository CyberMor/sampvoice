#if defined _sampvoice_included
	#endinput
#endif
#define _sampvoice_included

#define SV_VERSION			2

enum SV_FREQUENCY {
	SV_FREQUENCY_VERY_LOW	= 8000,
	SV_FREQUENCY_LOW		= 12000,
	SV_FREQUENCY_MEDIUM		= 16000,
	SV_FREQUENCY_HIGH		= 24000,
	SV_FREQUENCY_VERY_HIGH	= 48000
}
enum SV_VOICE_RATE {
	SV_VOICE_RATE_40MS		= 40,
	SV_VOICE_RATE_60MS		= 60,
	SV_VOICE_RATE_80MS		= 80,
	SV_VOICE_RATE_100MS		= 100,
	SV_VOICE_RATE_120MS		= 120
}
enum SV_PARAMETER {
	SV_PARAMETER_FREQUENCY	= 1,
	SV_PARAMETER_VOLUME		= 2,
	SV_PARAMETER_PANNING	= 3,
	SV_PARAMETER_EAXMIX		= 4,
	SV_PARAMETER_SRC		= 8
}

#define SV_NULL				0
#define SV_FALSE			false
#define SV_TRUE				true

#define SV_PTR:
#define SV_INT:
#define SV_UINT:
#define SV_FLOAT:			Float:
#define SV_BOOL:			bool:
#define SV_VOID:			void:

#define SV_RECEIVER:		SV_PTR:
#define SV_GROUP:			SV_PTR:
#define SV_BUFFER:			SV_PTR:
#define SV_STREAMABLE:		SV_PTR:
#define SV_EFFECTABLE:		SV_PTR:
#define SV_SGROUP:			SV_PTR:
#define SV_EGROUP:			SV_PTR:
#define SV_EFFECT:			SV_PTR:
#define SV_STREAM:			SV_PTR:
#define SV_SGSTREAM:		SV_PTR:
#define SV_SLSTREAM:		SV_PTR:
#define SV_DLSTREAM:		SV_PTR:
#define SV_PACKET:			SV_PTR:
#define SV_SOUND:			SV_UINT:

native SV_BOOL:sv_init(SV_UINT:bitrate, SV_FREQUENCY:frequency, SV_VOICE_RATE:voice_rate, SV_FLOAT:factor_distance, SV_FLOAT:factor_rolloff, SV_FLOAT:factor_doppler);
native SV_UINT:sv_get_version(SV_UINT:player_id);
native SV_VOID:sv_set_key(SV_UINT:player_id, SV_UINT:key_id);
native SV_VOID:sv_send_packet(SV_PACKET:packet, SV_RECEIVER:target);

native SV_BOOL:sv_has_micro(SV_UINT:player_id);
native SV_VOID:sv_record_start(SV_UINT:player_id);
native SV_VOID:sv_record_stop(SV_UINT:player_id);
native SV_VOID:sv_record_volume(SV_UINT:player_id, SV_FLOAT:volume);

native SV_EFFECT:sv_effect_create_chorus(SV_INT:priority, SV_FLOAT:wetdrymix, SV_FLOAT:depth, SV_FLOAT:feedback, SV_FLOAT:frequency, SV_UINT:waveform, SV_FLOAT:delay, SV_UINT:phase);
native SV_EFFECT:sv_effect_create_compressor(SV_INT:priority, SV_FLOAT:gain, SV_FLOAT:attack, SV_FLOAT:release, SV_FLOAT:threshold, SV_FLOAT:ratio, SV_FLOAT:predelay);
native SV_EFFECT:sv_effect_create_distortion(SV_INT:priority, SV_FLOAT:gain, SV_FLOAT:edge, SV_FLOAT:posteqcenterfrequency, SV_FLOAT:posteqbandwidth, SV_FLOAT:prelowpasscutoff);
native SV_EFFECT:sv_effect_create_echo(SV_INT:priority, SV_FLOAT:wetdrymix, SV_FLOAT:feedback, SV_FLOAT:leftdelay, SV_FLOAT:rightdelay, SV_BOOL:pandelay);
native SV_EFFECT:sv_effect_create_flanger(SV_INT:priority, SV_FLOAT:wetdrymix, SV_FLOAT:depth, SV_FLOAT:feedback, SV_FLOAT:frequency, SV_UINT:waveform, SV_FLOAT:delay, SV_UINT:phase);
native SV_EFFECT:sv_effect_create_gargle(SV_INT:priority, SV_UINT:ratehz, SV_UINT:waveshape);
native SV_EFFECT:sv_effect_create_i3dl2reverb(SV_INT:priority, SV_INT:room, SV_INT:roomhf, SV_FLOAT:roomrollofffactor, SV_FLOAT:decaytime, SV_FLOAT:decayhfratio, SV_INT:reflections, SV_FLOAT:reflectionsdelay, SV_INT:reverb, SV_FLOAT:reverbdelay, SV_FLOAT:diffusion, SV_FLOAT:density, SV_FLOAT:hfreference);
native SV_EFFECT:sv_effect_create_parameq(SV_INT:priority, SV_FLOAT:center, SV_FLOAT:bandwidth, SV_FLOAT:gain);
native SV_EFFECT:sv_effect_create_reverb(SV_INT:priority, SV_FLOAT:ingain, SV_FLOAT:reverbmix, SV_FLOAT:reverbtime, SV_FLOAT:highfreqrtratio);
native SV_VOID:sv_effect_delete(SV_EFFECT:effect);

native SV_VOID:sv_stream_set_parameter(SV_STREAMABLE:target, SV_PARAMETER:param_id, SV_FLOAT:value);
native SV_VOID:sv_stream_slide_parameter(SV_STREAMABLE:target, SV_PARAMETER:param_id, SV_FLOAT:start_value, SV_FLOAT:end_value, SV_UINT:time);
native SV_VOID:sv_stream_effect_attach(SV_STREAMABLE:target, SV_EFFECTABLE:effect);
native SV_VOID:sv_stream_effect_detach(SV_STREAMABLE:target, SV_EFFECTABLE:effect);
native SV_BOOL:sv_stream_play_sound_internal(SV_STREAMABLE:target, SV_SOUND:index, SV_BOOL:loop);
native SV_BOOL:sv_stream_play_sound_external(SV_STREAMABLE:target, url[], SV_BOOL:loop);
native SV_VOID:sv_stream_stop_sound(SV_STREAMABLE:target);
native SV_VOID:sv_stream_player_attach(SV_STREAMABLE:target, SV_UINT:player_id);
native SV_VOID:sv_stream_player_detach(SV_STREAMABLE:target, SV_UINT:player_id);
native SV_VOID:sv_stream_delete(SV_STREAM:stream);

native SV_SGSTREAM:sv_sgstream_create();
native SV_VOID:sv_slstream_update(SV_SLSTREAM:slstream, SV_FLOAT:position_x, SV_FLOAT:position_y, SV_FLOAT:position_z, SV_FLOAT:orientation_x, SV_FLOAT:orientation_y, SV_FLOAT:orientation_z, SV_FLOAT:velocity_x, SV_FLOAT:velocity_y, SV_FLOAT:velocity_z);
native SV_SLSTREAM:sv_slstream_create_at_point(SV_FLOAT:position_x, SV_FLOAT:position_y, SV_FLOAT:position_z, SV_FLOAT:orientation_x, SV_FLOAT:orientation_y, SV_FLOAT:orientation_z, SV_FLOAT:velocity_x, SV_FLOAT:velocity_y, SV_FLOAT:velocity_z);
native SV_SLSTREAM:sv_slstream_create_at_vehicle(SV_UINT:vehicle_id);
native SV_SLSTREAM:sv_slstream_create_at_player(SV_UINT:player_id);
native SV_SLSTREAM:sv_slstream_create_at_object(SV_UINT:object_id);

native SV_DLSTREAM:sv_dlstream_create_at_point(SV_FLOAT:position_x, SV_FLOAT:position_y, SV_FLOAT:position_z, SV_FLOAT:orientation_x, SV_FLOAT:orientation_y, SV_FLOAT:orientation_z, SV_FLOAT:velocity_x, SV_FLOAT:velocity_y, SV_FLOAT:velocity_z);
native SV_DLSTREAM:sv_dlstream_create_at_vehicle(SV_UINT:vehicle_id);
native SV_DLSTREAM:sv_dlstream_create_at_player(SV_UINT:player_id);
native SV_DLSTREAM:sv_dlstream_create_at_object(SV_UINT:object_id);

native SV_VOID:sv_group_add(SV_GROUP:group, SV_PTR:element);
native SV_VOID:sv_group_remove(SV_GROUP:group, SV_PTR:element);
native SV_VOID:sv_group_delete(SV_GROUP:group);

native SV_SGROUP:sv_sgroup_create();

native SV_EGROUP:sv_egroup_create();

native SV_BUFFER:sv_buffer_create();
native SV_VOID:sv_buffer_clear(SV_BUFFER:buffer);
native SV_BOOL:sv_buffer_save(SV_BUFFER:buffer, filename[]);
native SV_BOOL:sv_buffer_load(SV_BUFFER:buffer, filename[]);
native SV_VOID:sv_buffer_attach(SV_BUFFER:buffer, SV_STREAMABLE:target);
native SV_VOID:sv_buffer_detach(SV_BUFFER:buffer, SV_STREAMABLE:target);
native SV_VOID:sv_buffer_detach_all(SV_BUFFER:buffer);
native SV_VOID:sv_buffer_play(SV_BUFFER:buffer, SV_BOOL:loop);
native SV_VOID:sv_buffer_stop(SV_BUFFER:buffer);
native SV_VOID:sv_buffer_delete(SV_BUFFER:buffer);

native SV_SOUND:sv_sound_register(url[]);

forward SV_BOOL:OnPlayerVoice(SV_UINT:playerid, SV_PACKET:packet, SV_UINT:volume);



forward SV_VOID:OnStreamEnd(SV_STREAM:stream);
forward SV_VOID:OnBufferEnd(SV_BUFFER:buffer);
