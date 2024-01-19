/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <vector>
#include <cassert>
#include <string>

#include <system/file.hpp>

#include "plugin_config.hpp"
#include "storage.hpp"

struct Localization {

    enum Strings : size_t
    {
        Language,

        MenuTitle,

        MenuCommon,
        MenuCommonLanguage,
        MenuCommonSound,
        MenuCommonSoundEnable,
        MenuCommonSoundVolume,
        MenuCommonEffects,
        MenuCommonEffectsBalancer,
        MenuCommonEffectsFilter,
        MenuCommonIcon,
        MenuCommonIconScale,
        MenuCommonIconOffset,

        MenuMicrophone,
        MenuMicrophoneDevice,
        MenuMicrophoneDeviceEnable,
        MenuMicrophoneDeviceVolume,
        MenuMicrophoneDeviceSelect,
        MenuMicrophoneDeviceCheck,
        MenuMicrophoneIcon,
        MenuMicrophoneIconScale,
        MenuMicrophoneIconPositionX,
        MenuMicrophoneIconPositionY,
        MenuMicrophoneIconMove,
        MenuMicrophoneNotFound,

        MenuBlacklist,
        MenuBlacklistFilter,
        MenuBlacklistFilterPlaceholder,
        MenuBlacklistOnline,
        MenuBlacklistLocked,

        MenuReset,
        MenuResetButton,

        COUNT
    };

public:

    Localization(const Localization&) = delete;
    Localization(Localization&&) = delete;
    Localization& operator=(const Localization&) = delete;
    Localization& operator=(Localization&&) = delete;

private:

    Localization() noexcept = default;
    ~Localization() noexcept = default;

public:

    static Localization& Instance() noexcept
    {
        static Localization instance;
        return instance;
    }

public:

    bool Initialize() noexcept
    {
        static const std::vector<std::string> kDefaultLanguage
        {
            "English",

            "Voice chat settings",

            "General",
            "Language",
            "Sound",
            "Enable sound",
            "Sound volume",
            "Effects",
            "Volume smoothing",
            "High pass filter",
            "Icon above players",
            "Scale",
            "Offset",

            "Microphone",
            "Device",
            "Enable microphone",
            "Microphone volume",
            "Input device",
            "Check device",
            "Microphone icon",
            "Scale",
            "Position by X",
            "Position by Y",
            "Move",
            "No microphones available",

            "Blacklist",
            "Filter",
            "Enter Player ID or Nickname...",
            "Players online",
            "Blocked players",

            "Reset",
            "Reset all settings"
        };

        _languages.emplace_back(kDefaultLanguage);

        return Storage::Instance().ForEachLanguageFile([this](const cstr_t path) noexcept -> void
        {
            File file { path, "rt" };
            if (file.Invalid())
            {
                Logger::Instance().LogToFile("[sv:err:localization:initialize] failed to open language file (%s)", path);
                return;
            }

            std::vector<std::string> language;

            for (size_t i = 0; i != Strings::COUNT; ++i)
            {
                char buffer[512];

                if (const int result = file.GetLine(buffer, '\n'); result <= 0)
                {
                    switch (result)
                    {
                        case  0: Logger::Instance().LogToFile("[sv:err:localization:initialize] not all strings translated"); break;
                        case -1: Logger::Instance().LogToFile("[sv:err:localization:initialize] file descriptor error"); break;
                        case -2: Logger::Instance().LogToFile("[sv:err:localization:initialize] too long string"); break;
                    }

                    return;
                }

                language.emplace_back(buffer);
            }

            if (PluginConfig::Instance().HasLanguage())
            {
                if (language[Language] == PluginConfig::Instance().GetLanguage())
                    _language = _languages.size();
            }

            _languages.emplace_back(std::move(language));
        });
    }

    void Deinitialize() noexcept
    {
        _languages.clear();
        _language = 0;
    }

public:

    const auto& GetCurrentLanguage() const noexcept
    {
        return _languages[_language];
    }

    const auto& GetLanguages() const noexcept
    {
        return _languages;
    }

public:

    size_t GetLanguage() const noexcept
    {
        return _language;
    }

    void SetLanguage(const size_t language) noexcept
    {
        assert(language < _languages.size());

        if (language == 0) PluginConfig::Instance().ResetLanguage();
        else PluginConfig::Instance().SetLanguage(_languages[language][Language].c_str());

        _language = language;
    }

private:

    std::vector<std::vector<std::string>>
        _languages;

private:

    size_t _language = 0;

};
