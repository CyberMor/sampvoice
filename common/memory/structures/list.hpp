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

#define ListRemove(iterator, list, condition, action)        \
    for (auto iterator  = (list).Begin();                    \
              iterator != (list).End();)                     \
    {                                                        \
        const auto next = (list).Next(iterator);             \
        if (condition) { action; (list).Destroy(iterator); } \
        iterator = next;                                     \
    }

template <class Type, class Allocator = SystemAllocator>
struct List {

    List() noexcept = default;

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
        Node* prev = nullptr;
        Node* next = nullptr;

    };

private:

    static_assert(offsetof(Node, data) == 0);

public:

    ~List() noexcept
    {
        while (_head != nullptr)
        {
            Node* const next = _head->next;
            _allocator.Destroy(_head);
            _head = next;
        }
    }

    List(const List&) = delete;
    List(List&&) noexcept = default;
    List& operator=(const List&) = delete;
    List& operator=(List&&) noexcept = default;

public:

    template <class... Arguments>
    List(Arguments&&... arguments) noexcept
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
        if (pointer != nullptr)
        {
            const auto prev = reinterpret_cast<const Node*>(pointer)->prev;
            const auto next = reinterpret_cast<const Node*>(pointer)->next;

            if (prev != nullptr) prev->next = next;
            if (next != nullptr) next->prev = prev;

            if (_head == reinterpret_cast<Node*>(pointer)) _head = next;
            if (_tail == reinterpret_cast<Node*>(pointer)) _tail = prev;
        }

        _allocator.Destroy(reinterpret_cast<Node*>(pointer));
    }

public:

    const Type* First() const noexcept
    {
        return reinterpret_cast<const Type*>(_head);
    }

    Type* First() noexcept
    {
        return reinterpret_cast<Type*>(_head);
    }

    const Type* Last() const noexcept
    {
        return reinterpret_cast<const Type*>(_tail);
    }

    Type* Last() noexcept
    {
        return reinterpret_cast<Type*>(_tail);
    }

public:

    const Type* Begin() const noexcept
    {
        return reinterpret_cast<const Type*>(_head);
    }

    Type* Begin() noexcept
    {
        return reinterpret_cast<Type*>(_head);
    }

    const Type* End() const noexcept
    {
        return nullptr;
    }

    Type* End() noexcept
    {
        return nullptr;
    }

public:

    const Type* Previous(const Type* const iterator) const noexcept
    {
        return iterator != nullptr ?
            reinterpret_cast<const Type*>(reinterpret_cast<const Node*>(iterator)->prev) :
            reinterpret_cast<const Type*>(_tail);
    }

    Type* Previous(Type* const iterator) noexcept
    {
        return iterator != nullptr ?
            reinterpret_cast<Type*>(reinterpret_cast<const Node*>(iterator)->prev) :
            reinterpret_cast<Type*>(_tail);
    }

    const Type* Next(const Type* const iterator) const noexcept
    {
        return iterator != nullptr ?
            reinterpret_cast<const Type*>(reinterpret_cast<const Node*>(iterator)->next) :
            reinterpret_cast<const Type*>(_head);
    }

    Type* Next(Type* const iterator) noexcept
    {
        return iterator != nullptr ?
            reinterpret_cast<Type*>(reinterpret_cast<const Node*>(iterator)->next) :
            reinterpret_cast<Type*>(_head);
    }

public:

    void PushBefore(Type* const iterator, Type* const pointer) noexcept
    {
        assert(iterator != nullptr && pointer != nullptr);

        const auto prev = reinterpret_cast<const Node*>(iterator)->prev;
        const auto next = reinterpret_cast<Node*>(iterator);

        if (prev != nullptr) prev->next = reinterpret_cast<Node*>(pointer);
        next->prev = reinterpret_cast<Node*>(pointer);

        reinterpret_cast<Node*>(pointer)->prev = prev;
        reinterpret_cast<Node*>(pointer)->next = next;

        if (_head == reinterpret_cast<Node*>(iterator))
            _head = reinterpret_cast<Node*>(pointer);
    }

    void PushAfter(Type* const iterator, Type* const pointer) noexcept
    {
        assert(iterator != nullptr && pointer != nullptr);

        const auto prev = reinterpret_cast<Node*>(iterator);
        const auto next = reinterpret_cast<const Node*>(iterator)->next;

        prev->next = reinterpret_cast<Node*>(pointer);
        if (next != nullptr) next->prev = reinterpret_cast<Node*>(pointer);

        reinterpret_cast<Node*>(pointer)->prev = prev;
        reinterpret_cast<Node*>(pointer)->next = next;

        if (_tail == reinterpret_cast<Node*>(iterator))
            _tail = reinterpret_cast<Node*>(pointer);
    }

public:

    void PushFront(Type* const pointer) noexcept
    {
        assert(pointer != nullptr);

        if (_tail == nullptr) _tail = reinterpret_cast<Node*>(pointer);
        else _head->prev = reinterpret_cast<Node*>(pointer);

        _head = reinterpret_cast<Node*>(pointer);
    }

    void PushBack(Type* const pointer) noexcept
    {
        assert(pointer != nullptr);

        if (_head == nullptr) _head = reinterpret_cast<Node*>(pointer);
        else _tail->next = reinterpret_cast<Node*>(pointer);

        _tail = reinterpret_cast<Node*>(pointer);
    }

public:

    void PopFront() noexcept
    {
        if (_head != nullptr)
        {
            Node* const next = _head->next;
            if (next != nullptr) next->prev = nullptr;
            _allocator.Destroy(_head);
            if (_tail != _head) _head = next;
            else _tail = _head = next;
        }
    }

    void PopBack() noexcept
    {
        if (_tail != nullptr)
        {
            Node* const prev = _tail->prev;
            if (prev != nullptr) prev->next = nullptr;
            _allocator.Destroy(_tail);
            if (_head != _tail) _tail = prev;
            else _head = _tail = prev;
        }
    }

public:

    bool ShiftLeft(Type* const iterator) noexcept
    {
        assert(iterator != nullptr);

        const auto left = reinterpret_cast<const Node*>(iterator)->prev;
        const auto right = reinterpret_cast<Node*>(iterator);

        if (left == nullptr) return false;

        left->next = right->next;
        right->prev = left->prev;

        left->prev = right;
        right->next = left;

        if (_head == left) _head = right;
        if (_tail == right) _tail = left;

        return true;
    }

    bool ShiftRight(Type* const iterator) noexcept
    {
        assert(iterator != nullptr);

        const auto left = reinterpret_cast<Node*>(iterator);
        const auto right = reinterpret_cast<const Node*>(iterator)->next;

        if (right == nullptr) return false;

        left->next = right->next;
        right->prev = left->prev;

        left->prev = right;
        right->next = left;

        if (_head == left) _head = right;
        if (_tail == right) _tail = left;

        return true;
    }

public:

    void Exclude(Type* const iterator) noexcept
    {
        assert(iterator != nullptr);

        const auto prev = reinterpret_cast<const Node*>(iterator)->prev;
        const auto next = reinterpret_cast<const Node*>(iterator)->next;

        if (prev != nullptr) prev->next = next;
        if (next != nullptr) next->prev = prev;

        if (_head == reinterpret_cast<Node*>(iterator)) _head = next;
        if (_tail == reinterpret_cast<Node*>(iterator)) _tail = prev;

        reinterpret_cast<Node*>(iterator)->prev = nullptr;
        reinterpret_cast<Node*>(iterator)->next = nullptr;
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
