#ifndef GSL_UTIL_HPP
#define GSL_UTIL_HPP

#include <cstddef>

namespace gsl {

    /**
     * Index type to avoid common errors and help the optimizer.
     * 
     * \note C++ Core Guidelines ES.107
     */
    using index = std::ptrdiff_t;

    /**
     * Explicitly lossy or undefined-behavior cast.
     * 
     * \note C++ Core Guidelines ES.46
     * 
     * \tparam N The narrow type to cast to.
     * \tparam W The wide type to cast from.
     * 
     * \param val The value to perform a lossy cast on.
     * 
     * \return Returns the result of the narrowing cast.
     */
    template <class N, class W>
    constexpr N narrow_cast( W&& val ) noexcept {
        return static_cast<N>( std::forward<W>( val ) );
    }

}

#endif // #ifndef GSL_UTIL_HPP
