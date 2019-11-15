#ifndef GSL_DETAIL_HPP
#define GSL_DETAIL_HPP

#include <cstddef>

#include <type_traits>

namespace gsl::detail {

    template<typename C>
    constexpr auto ssize( const C& c )
        -> std::common_type_t<
            std::ptrdiff_t,
            std::make_signed_t<decltype( c.size() )>
        >
    {
        using R = std::common_type_t<
            std::ptrdiff_t,
            std::make_signed_t<decltype( c.size() )>
        >;
        return static_cast<R>( c.size() );
    }

    template<typename T, std::ptrdiff_t N>
    constexpr std::ptrdiff_t ssize( const T (&array)[N] ) noexcept {
        return N;
    }

}

#endif // #ifndef GSL_DETAIL_HPP
