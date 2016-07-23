#pragma once
#include <array>

namespace OSSIA
{
class Value;
template<typename T, int N>
class Vec
{
    public:
        static const constexpr int size_value = N;

        std::array<T, N> value;

        Vec() = default;
        Vec(std::array<T, N> v):
            value(std::move(v))
        {
        }

        virtual ~Vec() = default;
        Vec(const Vec&) = default;
        Vec(Vec&&) = default;
        Vec& operator=(const Vec&) = default;
        Vec& operator=(Vec&&) = default;


        /*! equal operator */
        bool operator== (const Value&) const;

        /*! different operator */
        bool operator!= (const Value&) const;

        /*! greater than operator */
        bool operator> (const Value&) const;

        /*! greater than and equal operator */
        bool operator>= (const Value&) const;

        /*! less than operator */
        bool operator< (const Value&) const;

        /*! less than and equal operator */
        bool operator<= (const Value&) const;
};

using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;

}
