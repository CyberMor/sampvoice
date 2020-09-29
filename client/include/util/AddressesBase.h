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
public:

    AddressesBase() noexcept = default;
    AddressesBase(const AddressesBase&) noexcept = default;
    AddressesBase(AddressesBase&&) noexcept = default;
    AddressesBase& operator=(const AddressesBase&) noexcept = default;
    AddressesBase& operator=(AddressesBase&&) noexcept = default;

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
    DWORD GetSampOpenChatFunc() const noexcept;
    DWORD GetSampSwitchModeFunc() const noexcept;
    DWORD GetSampOpenScoreboardFunc() const noexcept;
    DWORD GetSampCreatePlayerInPoolFunc() const noexcept;
    DWORD GetSampDeletePlayerFromPoolFunc() const noexcept;
    DWORD GetSampSpawnLocalPlayerFunc() const noexcept;
    DWORD GetSampDrawLabelFunc() const noexcept;

private:

    DWORD baseAddr { NULL };
    DWORD rcInitAddr { NULL };
    DWORD bassInitCallAddr { NULL };
    DWORD bassSetConfigAddr { NULL };
    DWORD sampInitAddr { NULL };
    DWORD sampDestructAddr { NULL };
    DWORD sampOpenChatFunc { NULL };
    DWORD sampSwitchModeFunc { NULL };
    DWORD sampOpenScoreboardFunc { NULL };
    DWORD sampCreatePlayerInPoolFunc { NULL };
    DWORD sampDeletePlayerFromPoolFunc { NULL };
    DWORD sampSpawnLocalPlayerFunc { NULL };
    DWORD sampDrawLabelFunc { NULL };

};

using AddressesBasePtr = std::shared_ptr<AddressesBase>;
#define MakeAddressesBase std::make_shared<AddressesBase>
