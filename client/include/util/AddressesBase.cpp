/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "AddressesBase.h"

#include <samp/commonSA.h>

AddressesBase::AddressesBase(const DWORD baseAddr) noexcept

    : baseAddr                          (baseAddr)
#if defined(SAMP_R1)
    , rcInitAddr                        (baseAddr + 0x2401D3)
    , bassInitCallAddr                  (baseAddr + 0x628DF)
    , bassSetConfigAddr                 (baseAddr + 0x6290F)
    , sampInitAddr                      (baseAddr + 0x2565E2)
    , sampDestructAddr                  (baseAddr + 0x9380)
    , sampOpenChatFunc                  (baseAddr + 0x657E0)
    , sampSwitchModeFunc                (baseAddr + 0x5D7B0)
    , sampOpenScoreboardFunc            (baseAddr + 0x6AD30)
    , sampCreatePlayerInPoolFunc        (baseAddr + 0x10D50)
    , sampDeletePlayerFromPoolFunc      (baseAddr + 0x10B90)
    , sampSpawnLocalPlayerFunc          (baseAddr + 0x3AD0)
    , sampDrawLabelFunc                 (baseAddr + 0x686C0)
#elif defined(SAMP_R3)
    , rcInitAddr                        (baseAddr + 0xB658)
    , bassInitCallAddr                  (baseAddr + 0x65D2F)
    , bassSetConfigAddr                 (baseAddr + 0x65D5F)
    , sampInitAddr                      (baseAddr + 0xC57E2)
    , sampDestructAddr                  (baseAddr + 0x9510)
    , sampOpenChatFunc                  (baseAddr + 0x68D10)
    , sampSwitchModeFunc                (baseAddr + 0x60B50)
    , sampOpenScoreboardFunc            (baseAddr + 0x6EC80)
    , sampCreatePlayerInPoolFunc        (baseAddr + 0x13E80)
    , sampDeletePlayerFromPoolFunc      (baseAddr + 0x13CB0)
    , sampSpawnLocalPlayerFunc          (baseAddr + 0x3AD0)
    , sampDrawLabelFunc                 (baseAddr + 0x6C630)
#endif

{}

DWORD AddressesBase::GetBaseAddr() const noexcept
{
    return this->baseAddr;
}

DWORD AddressesBase::GetRcInitAddr() const noexcept
{
    return this->rcInitAddr;
}

DWORD AddressesBase::GetBassInitCallAddr() const noexcept
{
    return this->bassInitCallAddr;
}

DWORD AddressesBase::GetBassSetConfigAddr() const noexcept
{
    return this->bassSetConfigAddr;
}

DWORD AddressesBase::GetSampInitAddr() const noexcept
{
    return this->sampInitAddr;
}

DWORD AddressesBase::GetSampDestructAddr() const noexcept
{
    return this->sampDestructAddr;
}

DWORD AddressesBase::GetSampOpenChatFunc() const noexcept
{
    return this->sampOpenChatFunc;
}

DWORD AddressesBase::GetSampSwitchModeFunc() const noexcept
{
    return this->sampSwitchModeFunc;
}

DWORD AddressesBase::GetSampOpenScoreboardFunc() const noexcept
{
    return this->sampOpenScoreboardFunc;
}

DWORD AddressesBase::GetSampCreatePlayerInPoolFunc() const noexcept
{
    return this->sampCreatePlayerInPoolFunc;
}

DWORD AddressesBase::GetSampDeletePlayerFromPoolFunc() const noexcept
{
    return this->sampDeletePlayerFromPoolFunc;
}

DWORD AddressesBase::GetSampSpawnLocalPlayerFunc() const noexcept
{
    return this->sampSpawnLocalPlayerFunc;
}

DWORD AddressesBase::GetSampDrawLabelFunc() const noexcept
{
    return this->sampDrawLabelFunc;
}
