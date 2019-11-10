#ifndef GSL_SPAN_HPP
#define GSL_SPAN_HPP

#include <cstddef>

#include <array>
#include <iterator>
#include <limits>
#include <tuple>
#include <type_traits>

namespace gsl {

    inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

    namespace details {

        template<std::size_t Extent, std::size_t Offset, std::size_t Count, typename = void>
        struct subspan_extent : std::integral_constant<std::size_t,dynamic_extent> {};

        template<std::size_t Extent, std::size_t Offset, std::size_t Count>
        struct subspan_extent<
            Extent,
            Offset,
            Count,
            std::enable_if_t<
                Count == dynamic_extent &&
                Extent != dynamic_extent
            >
        > :
            std::integral_constant<std::size_t,Extent-Offset>
        {};

        template<std::size_t Extent, std::size_t Offset, std::size_t Count>
        struct subspan_extent<
            Extent,
            Offset,
            Count,
            std::enable_if_t<Count != dynamic_extent>
        > :
            std::integral_constant<std::size_t,Count>
        {};

        template<std::size_t Extent, std::size_t Offset, std::size_t Count>
        inline constexpr std::size_t subspan_extent_v = subspan_extent<Extent,Offset,Count>::value;

        template<typename T, std::size_t N, typename = void>
        struct bytes_span_extent : std::integral_constant<std::size_t,dynamic_extent> {};

        template<typename T, std::size_t N>
        struct bytes_span_extent<T,N,std::enable_if_t<N != dynamic_extent>> :
            std::integral_constant<std::size_t,sizeof(T)*N>
        {};

        template<typename T, std::size_t N>
        inline constexpr std::size_t bytes_span_extent_v = bytes_span_extent<T,N>::value;

    }

    template<typename T, std::size_t Extent = dynamic_extent>
    class span {
        public:
            using element_type = T;
            using value_type = std::remove_cv_t<T>;
            using index_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;
            using const_reference = const T&;
            using iterator = pointer;
            using const_iterator = const_pointer;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

            static constexpr std::size_t extent = Extent;

            constexpr span() noexcept;

            constexpr span( pointer ptr, index_type count );

            constexpr span( pointer first, pointer last );

            template<std::size_t N>
            constexpr span( element_type(&arr)[N] ) noexcept;

            template<std::size_t N>
            constexpr span( std::array<value_type,N>& arr ) noexcept;

            template<std::size_t N>
            constexpr span( const std::array<value_type,N>& arr ) noexcept;

            template<typename Container>
            constexpr span( Container& cont );

            template<typename Container>
            constexpr span( const Container& cont );

            template<typename U, std::size_t N>
            constexpr span( const span<U,N>& s ) noexcept;

            constexpr span( const span& other ) noexcept = default;

            constexpr span& operator=( const span& other ) noexcept = default;

            constexpr iterator begin() const noexcept;

            constexpr const_iterator cbegin() const noexcept;

            constexpr iterator end() const noexcept;

            constexpr const_iterator cend() const noexcept;

            constexpr reverse_iterator rbegin() const noexcept;

            constexpr const_reverse_iterator crbegin() const noexcept;

            constexpr reverse_iterator rend() const noexcept;

            constexpr const_reverse_iterator crend() const noexcept;

            constexpr reference front() const;

            constexpr reference back() const;

            constexpr reference operator[]( index_type idx ) const;

            constexpr pointer data() const noexcept;

            constexpr index_type size() const noexcept;

            constexpr index_type size_bytes() const noexcept;

            [[nodiscard]]
            constexpr bool empty() const noexcept;

            template<std::size_t Count>
            constexpr span<element_type,Count> first() const;

            constexpr span<element_type,std::dynamic_extent> first( std::size_t Count ) const;

            template<std::size_t Count>
            constexpr span<element_type,Count> last() const;

            constexpr span<element_type,dynamic_extent> last( std::size_t Count );

            template<std::size_t Offset, std::size_t Count = dynamic_extent>
            constexpr span<
                element_type,
                details::subspan_extent_v<Extent,Offset,Count>
            > subspan() const;

            constexpr span<element_type,dynamic_extent>
            subspan( std::size_t Offset, std::size_t Count = dynamic_extent ) const;
    };

}

namespace std {

    template<typename T, std::size_t Extent>
    constexpr typename gsl::span<T,Extent>::iterator
    begin( gsl::span<T,Extent> s ) noexcept;

    template<typename T, std::size_t Extent>
    constexpr typename gsl::span<T,Extent>::iterator
    end( gsl::span<T,Extent> s ) noexcept;

    template<typename T, std::size_t N>
    gsl::span<const std::byte, gsl::details::bytes_span_extent_v<T,N>>
    as_bytes( gsl::span<T,N> s ) noexcept;

    template<typename T, std::size_t N>
    gsl::span<std::byte, gsl::details::bytes_span_extent_v<T,N>>
    as_writable_bytes( gsl::span<T,N> s ) noexcept;

    template<std::size_t I, typename T, std::size_t N>
    constexpr T& get(
        std::enable_if_t<
            (
                N != gsl::dynamic_extent &&
                I >= 0 &&
                I < N
            ),
            gsl::span<T,N>
        > s
    ) noexcept;

    template<typename T, std::size_t N>
    class tuple_size<gsl::span<T,N>> : public std::integral_constant<std::size_t,N> {};

    template<typename T>
    class tuple_size<gsl::span<T,gsl::dynamic_extent>>;

    template<std::size_t I, typename T, std::size_t N>
    struct tuple_element<I,gsl::span<T,N>> {
        static_assert( I < N, "tuple element index out of bounds" );
        static_assert( N != gsl::dynamic_extent, "cannot take the element type of a dynamic span" );
        using type = T;
    };

}

#endif // #ifndef GSL_SPAN_HPP