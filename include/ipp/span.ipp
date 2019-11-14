#include "gsl_util.hpp"

namespace gsl {

    template<typename T, std::size_t Extent>
    template<typename>
    constexpr span<T,Extent>::span() noexcept :
        begin_pos( nullptr ),
        end_pos( nullptr )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    constexpr span<T,Extent>::span( pointer ptr, index_type count ) :
        begin_pos( ptr ),
        end_pos( ptr + count )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    constexpr span<T,Extent>::span( pointer first, pointer last ) :
        begin_pos( first ),
        end_pos( last + 1 )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    template<std::size_t N, typename>
    constexpr span<T,Extent>::span( element_type(&arr)[N] ) noexcept :
        begin_pos( std::data( arr ) ),
        end_pos( &( *std::end( arr ) ) )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    template<std::size_t N, typename>
    constexpr span<T,Extent>::span( std::array<value_type,N>& arr ) noexcept :
        begin_pos( std::data( arr ) ),
        end_pos( &( *std::end( arr ) ) )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    template<std::size_t N, typename>
    constexpr span<T,Extent>::span( const std::array<value_type,N>& arr ) noexcept :
        begin_pos( std::data( arr ) ),
        end_pos( &( *std::end( arr ) ) )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    template<typename Container, typename>
    constexpr span<T,Extent>::span( Container& cont ) :
        begin_pos( std::data( cont ) ),
        end_pos( &( *std::end( cont ) ) )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    template<typename Container, typename>
    constexpr span<T,Extent>::span( const Container& cont ) :
        begin_pos( std::data( cont ) ),
        end_pos( &( *std::end( cont ) ) )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    template<typename U, std::size_t N, typename>
    constexpr span<T,Extent>::span( const span<U,N>& s ) noexcept :
        begin_pos( std::data( s ) ),
        end_pos( &( *std::end( s ) ) )
    {
        return;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::iterator
    span<T,Extent>::begin() const noexcept {
        return begin_pos;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::const_iterator
    span<T,Extent>::cbegin() const noexcept {
        return begin_pos;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::iterator
    span<T,Extent>::end() const noexcept {
        return end_pos;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::const_iterator
    span<T,Extent>::cend() const noexcept {
        return end_pos;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::reverse_iterator
    span<T,Extent>::rbegin() const noexcept {
        return end_pos - 1;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::const_reverse_iterator
    span<T,Extent>::crbegin() const noexcept {
        return end_pos - 1;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::reverse_iterator
    span<T,Extent>::rend() const noexcept {
        return begin_pos - 1;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::const_reverse_iterator
    span<T,Extent>::crend() const noexcept {
        return begin_pos - 1;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::reference
    span<T,Extent>::front() const {
        return *begin_pos;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::reference
    span<T,Extent>::back() const {
        return *( end_pos - 1 );
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::reference
    span<T,Extent>::operator[]( index_type idx ) const {
        return *( begin_pos + idx );
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::pointer
    span<T,Extent>::data() const noexcept {
        return begin_pos;
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::index_type
    span<T,Extent>::size() const noexcept {
        return std::distance( begin_pos, end_pos );
    }

    template<typename T, std::size_t Extent>
    constexpr typename span<T,Extent>::index_type
    span<T,Extent>::size_bytes() const noexcept {
        return narrow_cast<std::uintptr_t>( end_pos ) - narrow_cast<std::uintptr_t>( begin_pos );
    }

    template<typename T, std::size_t Extent>
    [[nodiscard]]
    constexpr bool span<T,Extent>::empty() const noexcept {
        return begin_pos == end_pos;
    }

    template<typename T, std::size_t Extent>
    template<std::size_t Count>
    constexpr span<typename span<T,Extent>::element_type,Count>
    span<T,Extent>::first() const {
        return span( begin_pos, Count );
    }

    template<typename T, std::size_t Extent>
    constexpr span<typename span<T,Extent>::element_type,dynamic_extent>
    span<T,Extent>::first( std::size_t Count ) const {
        return span( begin_pos, Count );
    }

    template<typename T, std::size_t Extent>
    template<std::size_t Count>
    constexpr span<typename span<T,Extent>::element_type,Count>
    span<T,Extent>::last() const {
        return span( end_pos - Count, Count );
    }

    template<typename T, std::size_t Extent>
    constexpr span<typename span<T,Extent>::element_type,dynamic_extent>
    span<T,Extent>::last( std::size_t Count ) {
        return span( end_pos - Count, Count );
    }

    template<typename T, std::size_t Extent>
    template<std::size_t Offset, std::size_t Count>
    constexpr span<
        typename span<T,Extent>::element_type,
        details::subspan_extent_v<Extent,Offset,Count>
    > span<T,Extent>::subspan() const {
        return span( begin_pos + Offset, Count );
    }

    template<typename T, std::size_t Extent>
    constexpr span<typename span<T,Extent>::element_type,dynamic_extent>
    span<T,Extent>::subspan( std::size_t Offset, std::size_t Count ) const {
        return span( begin_pos + Offset, Count );
    }

}
