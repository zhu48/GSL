#ifndef GSL_UTIL_HPP
#define GSL_UTIL_HPP

#include <cstddef>

#include <initializer_list>

#include "gsl_assert.hpp"
#include "gsl_detail.hpp"

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

    /**
     * Checked indexed built-in array access.
     * 
     * \tparam T The type of the array elements.
     * \tparam N The number of elements in the array.
     * 
     * \param arr The array to access an element of.
     * 
     * \param i The index of the array element to access.
     * 
     * \return Returns the element to be accessed.
     */
    template<typename T, std::size_t N>
    constexpr T& at( T ( &arr )[N], index i ) {
        Expects( i >= 0 && i < narrow_cast<index>( N ) );
        return arr[i];
    }

    /**
     * Checked indexed container access.
     * 
     * \tparam Container The type of container to access.
     * 
     * \param cont The container to access an element of.
     * \param i    The index of the element to access.
     * 
     * \return Returns the element to be accessed.
     */
    template<typename Container>
    constexpr auto at( Container& cont, index i ) -> decltype( cont[cont.size()] ) {
        Expects( i >= 0 && i < detail::ssize( cont ) );
        return cont[i];
    }

    /**
     * Checked indexed initializer list access.
     * 
     * \tparam T The type of object the initializer list contains.
     * 
     * \param cont The initializer list to access an element of.
     * \param i    The index of the initializer list element to access.
     * 
     * \return Returns the element to be accessed.
     */
    template<typename T>
    constexpr T at( const std::initializer_list<T> cont, index i ) {
        Expects( i >= 0 && i < detail::ssize( cont ) );
        return *( std::begin( cont ) + i );
    }

}

#endif // #ifndef GSL_UTIL_HPP
