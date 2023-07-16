/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstddef>

#include <system/types.hpp>

#include <memory/allocators/system_allocator.hpp>
#include <memory/allocators/object_allocator.hpp>

template <class Type, class Allocator = SystemAllocator>
struct Queue {

    Queue() noexcept = default;

private:

    struct Node {

        Node() = delete;
        ~Node() noexcept = default;
        Node(const Node&) = delete;
        Node(Node&&) = delete;
        Node& operator=(const Node&) = delete;
        Node& operator=(Node&&) = delete;

    public:

        template <class... Arguments>
        Node(Arguments&&... arguments) noexcept
            : data { std::forward<Arguments>(arguments)... }
        {}

    public:

        Type  data;
        Node* next = nullptr;

    };

private:

    static_assert(offsetof(Node, data) == 0);

public:

    ~Queue() noexcept
    {
        while (_head != nullptr)
        {
            Node* const next = _head->next;
            _allocator.Destroy(_head);
            _head = next;
        }
    }

    Queue(const Queue&) = delete;
    Queue(Queue&&) noexcept = default;
    Queue& operator=(const Queue&) = delete;
    Queue& operator=(Queue&&) noexcept = default;

public:

    template <class... Arguments>
    Queue(Arguments&&... arguments) noexcept
        : _allocator { std::forward<Arguments>(arguments)... }
    {}

public:

    bool Valid() const noexcept
    {
        return _allocator.Valid();
    }

    bool Invalid() const noexcept
    {
        return _allocator.Invalid();
    }

public:

    template <class... Arguments>
    bool Initialize(Arguments&&... arguments) noexcept
    {
        _head = nullptr;
        _tail = nullptr;

        return _allocator.Initialize(std::forward<Arguments>(arguments)...);
    }

    void Deinitialize() noexcept
    {
        while (_head != nullptr)
        {
            Node* const next = _head->next;
            _allocator.Destroy(_head);
            _head = next;
        }

        _tail = nullptr;

        _allocator.Deinitialize();
    }

public:

    template <class... Arguments>
    Type* Construct(Arguments&&... arguments) noexcept
    {
        return reinterpret_cast<Type*>(_allocator.template Construct<Node>
            (std::forward<Arguments>(arguments)...));
    }

    void Destroy(Type* const pointer) noexcept
    {
        _allocator.Destroy(reinterpret_cast<Node*>(pointer));
    }

public:

    void Enqueue(Type* const pointer) noexcept
    {
        assert(pointer != nullptr);

        if (_head == nullptr) _head = reinterpret_cast<Node*>(pointer);
        else _tail->next = reinterpret_cast<Node*>(pointer);

        _tail = reinterpret_cast<Node*>(pointer);
    }

    Type* Dequeue() noexcept
    {
        const auto pointer = reinterpret_cast<Type*>(_head);
        if (pointer != nullptr)
        {
            if (_tail != _head) _head = _head->next;
            else _tail = _head = _head->next;
        }

        return pointer;
    }

public:

    void Clear() noexcept
    {
        while (_head != nullptr)
        {
            Node* const next = _head->next;
            _allocator.Destroy(_head);
            _head = next;
        }

        _tail = nullptr;
    }

private:

    ObjectAllocator<Allocator> _allocator;

private:

    Node* _head = nullptr;
    Node* _tail = nullptr;

};
