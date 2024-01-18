/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include <vector>
#include <string>

#include <system/types.hpp>
#include <system/file.hpp>

struct Config {

    Config() noexcept = default;
    ~Config() noexcept = default;
    Config(const Config&) = default;
    Config(Config&&) noexcept = default;
    Config& operator=(const Config&) = default;
    Config& operator=(Config&&) noexcept = default;

public:

    template <class Number, typename = std::enable_if_t<std::is_integral_v<Number> == true>>
    const std::string& operator[](const Number index) const noexcept
    {
        return GetValueByIndex(index);
    }

    template <class String, typename = std::enable_if_t<std::is_integral_v<String> == false>>
    const std::string& operator[](String&& key) const noexcept
    {
        return GetValueByIndex(GetIndexByKey(std::forward<String>(key)));
    }

public:

    void Reserve(const size_t parameters) noexcept
    {
        _parameters.resize(parameters);
    }

    template <class String>
    void Register(const size_t index, String&& key) noexcept
    {
        if (_parameters.size() <= index)
            _parameters.resize(index + 1);

        _parameters[index].first = std::forward<String>(key);
    }

public:

    int Load(const cstr_t path) noexcept
    {
        assert(path != nullptr && *path != '\0');

        File file { path, "r" };
        if (file.Invalid())
            return 0;

        for (char line[512];;)
        {
            const int code = file.GetLine(line);
            if (code < 0) return code;

            if (*line != '#' && *line != '\0')
            {
                str_t iterator = line;

                while (*iterator == ' ' || *iterator == '\t')
                    ++iterator;

                const str_t key_begin = iterator;

                while (*iterator != ' ' && *iterator != '=' && *iterator != '\0' && *iterator != '\t')
                    ++iterator;

                const str_t key_end = iterator;

                while (*iterator == ' ' || *iterator == '\t')
                    ++iterator;

                if (*iterator++ != '=')
                    return -2;

                while (*iterator == ' ' || *iterator == '\t')
                    ++iterator;

                const str_t value_begin = iterator;

                while (*iterator != ' ' && *iterator != '\0' && *iterator != '\t')
                    ++iterator;

                const str_t value_end = iterator;

                *key_end = *value_end = '\0';

                size_t index = None<size_t>;

                if (index == None<size_t>) index = GetIndexByKey(key_begin);
                if (index == None<size_t>) index = FindFreeIndex();
                if (index == None<size_t>) index = (_parameters.emplace_back(), _parameters.size() - 1);

                if (_parameters[index].first.empty())
                    _parameters[index].first = { key_begin, static_cast<size_t>(key_end - key_begin) };

                _parameters[index].second = { value_begin, static_cast<size_t>(value_end - value_begin) };
            }

            if (code == 0) break;
        }

        return 1;
    }

public:

    bool HasParameter(const size_t index) const noexcept
    {
        assert(index < _parameters.size());

        return !_parameters[index].first.empty() &&
               !_parameters[index].second.empty();
    }

    template <class String>
    bool SetParameter(const size_t index, String&& value) noexcept
    {
        assert(index < _parameters.size());

        if (_parameters[index].first.empty())
            return false;

        _parameters[index].second = std::forward<String>(value);

        return true;
    }

public:

    const std::string& GetKeyByIndex(const size_t index) const noexcept
    {
        assert(index < _parameters.size());

        return _parameters[index].first;
    }

    const std::string& GetValueByIndex(const size_t index) const noexcept
    {
        assert(index < _parameters.size());

        return _parameters[index].second;
    }

public:

    template <class String>
    size_t GetIndexByKey(String&& key) const noexcept
    {
        for (size_t i = 0; i != _parameters.size(); ++i)
        {
            if (_parameters[i].first == std::forward<String>(key))
                return i;
        }

        return None<size_t>;
    }

    template <class String>
    size_t GetIndexByValue(String&& value) const noexcept
    {
        for (size_t i = 0; i != _parameters.size(); ++i)
        {
            if (_parameters[i].second == std::forward<String>(value))
                return i;
        }

        return None<size_t>;
    }

public:

    void Clear() noexcept
    {
        _parameters.clear();
    }

private:

    size_t FindFreeIndex() const noexcept
    {
        for (size_t i = 0; i != _parameters.size(); ++i)
        {
            if (_parameters[i].first.empty())
                return i;
        }

        return None<size_t>;
    }

private:

    std::vector<std::pair<std::string, std::string>>
        _parameters;

};
