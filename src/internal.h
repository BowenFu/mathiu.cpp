#ifndef INTERNAL_H
#define INTERNAL_H

#include "mathiu/core.h"
#include "matchit.h"

#define VERBOSE_DEBUG 0
#define DEBUG 1

#if DEBUG
#include <iostream>
#endif // DEBUG

namespace matchit::impl
{
    template <>
    class PatternTraits<mathiu::impl::ExprPtr>
    {
        using Pattern = mathiu::impl::ExprPtr;
    public:
        template <typename Value>
        using AppResultTuple = std::tuple<>;

        constexpr static auto nbIdV = 0;

        template <typename Value, typename ContextT>
        constexpr static auto matchPatternImpl(Value &&value, Pattern const &pattern,
                                               int32_t /* depth */,
                                               ContextT & /*context*/)
        {
            return mathiu::impl::equal(pattern, std::forward<Value>(value));
        }
        constexpr static void processIdImpl(Pattern const &, int32_t /*depth*/,
                                            IdProcess) {}
    };

    template <>
    class IdTraits<mathiu::impl::ExprPtr>
    {
        using Type = mathiu::impl::ExprPtr;
    public:
        static auto
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
            __attribute__((no_sanitize_address))
#endif
#endif
            equal(Type const &lhs, Type const &rhs)
        {
            return mathiu::impl::equal(lhs, rhs);
        }
    };

} // namespace matchit::impl

namespace mathiu
{
    namespace impl
    {
        inline constexpr auto isRational = matchit::or_(matchit::as<int>(matchit::_), matchit::as<Fraction>(matchit::_));


        inline auto const asDouble = matchit::meet([](auto&& e)
        {
            try{
                evald(e);
                return true;
            }
            catch (...)
            {
                return false;
            }
        });

    } // namespace impl
} // namespace mathiu

#endif // INTERNAL_H