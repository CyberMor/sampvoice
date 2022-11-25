/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "AddressesBase.h"

#include <cassert>

#include <samp/commonSA.h>

AddressesBase::AddressesBase(const DWORD base_addr) noexcept
    : _base_addr                    { base_addr }
#if defined(SAMP_R1)
    , _rc_init_addr                 { base_addr + 0x2401D3 }
    , _bass_init_call_addr          { base_addr + 0x628DF }
    , _bass_set_config_addr         { base_addr + 0x6290F }
    , _samp_init_addr               { base_addr + 0x2565E2 }
    , _samp_destruct_addr           { base_addr + 0x9380 }
    , _open_chat_func               { base_addr + 0x657E0 }
    , _switch_mode_func             { base_addr + 0x5D7B0 }
    , _open_scoreboard_func         { base_addr + 0x6AD30 }
    , _create_player_in_pool_func   { base_addr + 0x10D50 }
    , _delete_player_from_pool_func { base_addr + 0x10B90 }
    , _spawn_local_player_func      { base_addr + 0x3AD0 }
    , _draw_label_func              { base_addr + 0x686C0 }
#elif defined(SAMP_R3)
    , _rc_init_addr                 { base_addr + 0xB658 }
    , _bass_init_call_addr          { base_addr + 0x65D2F }
    , _bass_set_config_addr         { base_addr + 0x65D5F }
    , _samp_init_addr               { base_addr + 0xC57E2 }
    , _samp_destruct_addr           { base_addr + 0x9510 }
    , _open_chat_func               { base_addr + 0x68D10 }
    , _switch_mode_func             { base_addr + 0x60B50 }
    , _open_scoreboard_func         { base_addr + 0x6EC80 }
    , _create_player_in_pool_func   { base_addr + 0x13E80 }
    , _delete_player_from_pool_func { base_addr + 0x13CB0 }
    , _spawn_local_player_func      { base_addr + 0x3AD0 }
    , _draw_label_func              { base_addr + 0x6C630 }
#endif
{
    assert(base_addr != NULL);
}

DWORD AddressesBase::GetBaseAddr() const noexcept
{
    return _base_addr;
}

DWORD AddressesBase::GetRcInitAddr() const noexcept
{
    return _rc_init_addr;
}

DWORD AddressesBase::GetBassInitCallAddr() const noexcept
{
    return _bass_init_call_addr;
}

DWORD AddressesBase::GetBassSetConfigAddr() const noexcept
{
    return _bass_set_config_addr;
}

DWORD AddressesBase::GetSampInitAddr() const noexcept
{
    return _samp_init_addr;
}

DWORD AddressesBase::GetSampDestructAddr() const noexcept
{
    return _samp_destruct_addr;
}

DWORD AddressesBase::GetOpenChatFunc() const noexcept
{
    return _open_chat_func;
}

DWORD AddressesBase::GetSwitchModeFunc() const noexcept
{
    return _switch_mode_func;
}

DWORD AddressesBase::GetOpenScoreboardFunc() const noexcept
{
    return _open_scoreboard_func;
}

DWORD AddressesBase::GetCreatePlayerInPoolFunc() const noexcept
{
    return _create_player_in_pool_func;
}

DWORD AddressesBase::GetDeletePlayerFromPoolFunc() const noexcept
{
    return _delete_player_from_pool_func;
}

DWORD AddressesBase::GetSpawnLocalPlayerFunc() const noexcept
{
    return _spawn_local_player_func;
}

DWORD AddressesBase::GetDrawLabelFunc() const noexcept
{
    return _draw_label_func;
}
