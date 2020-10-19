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
    , openChatFunc                      (baseAddr + 0x657E0)
    , switchModeFunc                    (baseAddr + 0x5D7B0)
    , openScoreboardFunc                (baseAddr + 0x6AD30)
    , createPlayerInPoolFunc            (baseAddr + 0x10D50)
    , deletePlayerFromPoolFunc          (baseAddr + 0x10B90)
    , spawnLocalPlayerFunc              (baseAddr + 0x3AD0)
    , drawLabelFunc                     (baseAddr + 0x686C0)
#elif defined(SAMP_R3)
    , rcInitAddr                        (baseAddr + 0xB658)
    , bassInitCallAddr                  (baseAddr + 0x65D2F)
    , bassSetConfigAddr                 (baseAddr + 0x65D5F)
    , sampInitAddr                      (baseAddr + 0xC57E2)
    , sampDestructAddr                  (baseAddr + 0x9510)
    , openChatFunc                      (baseAddr + 0x68D10)
    , switchModeFunc                    (baseAddr + 0x60B50)
    , openScoreboardFunc                (baseAddr + 0x6EC80)
    , createPlayerInPoolFunc            (baseAddr + 0x13E80)
    , deletePlayerFromPoolFunc          (baseAddr + 0x13CB0)
    , spawnLocalPlayerFunc              (baseAddr + 0x3AD0)
    , drawLabelFunc                     (baseAddr + 0x6C630)
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

DWORD AddressesBase::GetOpenChatFunc() const noexcept
{
    return this->openChatFunc;
}

DWORD AddressesBase::GetSwitchModeFunc() const noexcept
{
    return this->switchModeFunc;
}

DWORD AddressesBase::GetOpenScoreboardFunc() const noexcept
{
    return this->openScoreboardFunc;
}

DWORD AddressesBase::GetCreatePlayerInPoolFunc() const noexcept
{
    return this->createPlayerInPoolFunc;
}

DWORD AddressesBase::GetDeletePlayerFromPoolFunc() const noexcept
{
    return this->deletePlayerFromPoolFunc;
}

DWORD AddressesBase::GetSpawnLocalPlayerFunc() const noexcept
{
    return this->spawnLocalPlayerFunc;
}

DWORD AddressesBase::GetDrawLabelFunc() const noexcept
{
    return this->drawLabelFunc;
}
