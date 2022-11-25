/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>

struct AddressesBase {

    AddressesBase() noexcept = default;
    ~AddressesBase() noexcept = default;
    AddressesBase(const AddressesBase&) noexcept = default;
    AddressesBase(AddressesBase&&) noexcept = default;
    AddressesBase& operator=(const AddressesBase&) noexcept = default;
    AddressesBase& operator=(AddressesBase&&) noexcept = default;

public:

    AddressesBase(DWORD base_addr) noexcept;

public:

    DWORD GetBaseAddr() const noexcept;
    DWORD GetRcInitAddr() const noexcept;
    DWORD GetBassInitCallAddr() const noexcept;
    DWORD GetBassSetConfigAddr() const noexcept;
    DWORD GetSampInitAddr() const noexcept;
    DWORD GetSampDestructAddr() const noexcept;
    DWORD GetOpenChatFunc() const noexcept;
    DWORD GetSwitchModeFunc() const noexcept;
    DWORD GetOpenScoreboardFunc() const noexcept;
    DWORD GetCreatePlayerInPoolFunc() const noexcept;
    DWORD GetDeletePlayerFromPoolFunc() const noexcept;
    DWORD GetSpawnLocalPlayerFunc() const noexcept;
    DWORD GetDrawLabelFunc() const noexcept;

private:

    DWORD _base_addr                    = NULL;
    DWORD _rc_init_addr                 = NULL;
    DWORD _bass_init_call_addr          = NULL;
    DWORD _bass_set_config_addr         = NULL;
    DWORD _samp_init_addr               = NULL;
    DWORD _samp_destruct_addr           = NULL;
    DWORD _open_chat_func               = NULL;
    DWORD _switch_mode_func             = NULL;
    DWORD _open_scoreboard_func         = NULL;
    DWORD _create_player_in_pool_func   = NULL;
    DWORD _delete_player_from_pool_func = NULL;
    DWORD _spawn_local_player_func      = NULL;
    DWORD _draw_label_func              = NULL;

};
