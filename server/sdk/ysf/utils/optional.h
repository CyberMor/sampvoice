#ifndef YSF_OPTIONAL_H
#define YSF_OPTIONAL_H

#include <utility>
#include <cstddef>
#include <type_traits>
#include <exception>

namespace aux
{
	template <class T>
	class optional;

	template <class T, class... Args>
	optional<T> make_optional(Args&&... args);

	template <class T>
	class optional
	{
		bool present;
		union {
			char dummy;
			T data;
		};
	public:
		constexpr optional() : present(false), dummy()
		{

		}

		constexpr optional(std::nullptr_t) : present(false), dummy()
		{

		}

		constexpr optional(const T &obj) : present(true), data(obj)
		{

		}

		constexpr optional(T &&obj) : present(true), data(std::move(obj))
		{

		}

		optional(const optional<T> &other) : present(other.present), dummy()
		{
			if (present)
			{
				data = other.data;
			}
		}

		optional(optional<T> &&other) : present(other.present), dummy()
		{
			if (present)
			{
				data = std::move(other.data);
			}
		}

		~optional()
		{
			if (present)
			{
				data.~T();
			}
		}

		void reset()
		{
			if (present)
			{
				data.~T();
				present = false;
			}
		}

		template<class... Args>
		T& emplace(Args&&... args)
		{
			if (present)
			{
				data.~T();
			}
			present = true;
			new (&data) T(std::forward<Args>(args)...);
			return data;
		}

		constexpr T value_or(T other) const
		{
			return present ? data : other;
		}

		constexpr T &value()
		{
			if (!present) throw std::logic_error("Optional object does not have a value.");
			return data;
		}

		constexpr T &operator*()
		{
			return data;
		}

		constexpr bool has_value() const
		{
			return present;
		}

		optional<T>& operator=(std::nullptr_t)
		{
			reset();
			return *this;
		}

		optional<T>& operator=(const optional<T>& other)
		{
			if (this != &other)
			{
				if (!other.present)
				{
					reset();
					return *this;
				}
				else {
					present = true;
					data = other.data;
				}
			}
			return *this;
		}

		optional<T>& operator=(optional<T>&& other)
		{
			if (this != &other)
			{
				if (!other.present)
				{
					return *this = nullptr;
				}
				else {
					present = true;
					data = std::move(other.data);
				}
			}
			return *this;
		}

		constexpr bool operator==(std::nullptr_t) const
		{
			return present;
		}

		constexpr explicit operator bool() const
		{
			return present;
		}

		template <class Function>
		constexpr typename std::conditional<std::is_void<typename std::result_of<Function(T&)>::type>::value, void, aux::optional<typename std::result_of<Function(T&)>::type>>::type map(Function func)
		{
			using ret_t = typename std::conditional<std::is_void<typename std::result_of<Function(T&)>::type>::value, void, aux::optional<typename std::result_of<Function(T&)>::type>>::type;
			return present ? ret_t(func(data)) : ret_t();
		}

		template <class Function>
		constexpr typename std::conditional<std::is_void<typename std::result_of<Function(const T&)>::type>::value, void, aux::optional<typename std::result_of<Function(T&)>::type>>::type map(Function func) const
		{
			using ret_t = typename std::conditional<std::is_void<typename std::result_of<Function(const T&)>::type>::value, void, aux::optional<typename std::result_of<Function(const T&)>::type>>::type;
			return present ? ret_t(func(data)) : ret_t();
		}

		template <class U, class... Args>
		friend optional<U> make_optional(Args&&... args);
	private:
		struct init_args_t {};

		template <class... Args>
		constexpr optional(init_args_t, Args&&... args) : present(true), data(std::forward<Args>(args)...)
		{

		}
	};

	template <class T, class... Args>
	inline optional<T> make_optional(Args&&... args)
	{
		return optional<T>(optional<T>::init_args_t(), std::forward<Args>(args)...);
	}
}

#endif
