/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>

#include <Windows.h>

struct AddressesBase {

    AddressesBase() = delete;
    AddressesBase(const AddressesBase&) noexcept = default;
    AddressesBase(AddressesBase&&) = delete;
    AddressesBase& operator=(const AddressesBase&) noexcept = default;
    AddressesBase& operator=(AddressesBase&&) = delete;

public:

    explicit AddressesBase(DWORD baseAddr) noexcept;

    ~AddressesBase() noexcept = default;

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

    DWORD baseAddr { NULL };
    DWORD rcInitAddr { NULL };
    DWORD bassInitCallAddr { NULL };
    DWORD bassSetConfigAddr { NULL };
    DWORD sampInitAddr { NULL };
    DWORD sampDestructAddr { NULL };
    DWORD openChatFunc { NULL };
    DWORD switchModeFunc { NULL };
    DWORD openScoreboardFunc { NULL };
    DWORD createPlayerInPoolFunc { NULL };
    DWORD deletePlayerFromPoolFunc { NULL };
    DWORD spawnLocalPlayerFunc { NULL };
    DWORD drawLabelFunc { NULL };

};

using AddressesBasePtr = std::unique_ptr<AddressesBase>;
#define MakeAddressesBase std::make_unique<AddressesBase>
