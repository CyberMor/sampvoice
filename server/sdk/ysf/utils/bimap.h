#ifndef YSF_BIMAP_H
#define YSF_BIMAP_H

#include <unordered_map>
#include <exception>
#include <functional>
#include "optional.h"

namespace aux
{
	template <class T, class U>
	class bimap
	{
		std::unordered_map<T, U> left;
		std::unordered_map<U, T> right;
	public:
		bool insert(const T &v1, const U &v2)
		{
			bool l = left.insert(std::make_pair(v1, v2)).second;
			bool r = right.insert(std::make_pair(v2, v1)).second;
			if (l != r) throw std::runtime_error("bimap state is inconsistent");
			return l;
		}

		bool erase_l(const T &l)
		{
			auto it = left.find(l);
			if (it == left.end())
			{
				return false;
			} else {
				auto it2 = right.find(it->second);
				if (it2 == right.end() || it2->second != it->first)
				{
					throw std::runtime_error("bimap state is inconsistent");
				}
				left.erase(it);
				right.erase(it2);
				return true;
			}
		}

		bool erase_r(const U &r)
		{
			auto it = right.find(r);
			if (it == right.end())
			{
				return false;
			} else {
				auto it2 = left.find(it->second);
				if (it2 == left.end() || it2->second != it->first)
				{
					throw std::runtime_error("bimap state is inconsistent");
				}
				right.erase(it);
				left.erase(it2);
				return true;
			}
		}

		U &at_l(const T &l)
		{
			return left.at(l);
		}

		const U &at_l(const T &l) const
		{
			return left.at(l);
		}

		T &at_r(const U &r)
		{
			return right.at(r);
		}

		const T &at_r(const U &r) const
		{
			return right.at(r);
		}

		aux::optional<std::reference_wrapper<const U>> find_l(const T &l) const
		{
			auto it = left.find(l);
			if (it == left.end())
			{
				return aux::optional<std::reference_wrapper<const U>>();
			}
			else {
				return aux::optional<std::reference_wrapper<const U>>(std::cref(it->second));
			}
		}

		aux::optional<std::reference_wrapper<const T>> find_r(const U &r) const
		{
			auto it = right.find(r);
			if (it == right.end())
			{
				return aux::optional<std::reference_wrapper<const T>>();
			}
			else {
				return aux::optional<std::reference_wrapper<const T>>(std::cref(it->second));
			}
		}
	};
}

#endif
