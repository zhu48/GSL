#ifndef GSL_SPAN_HPP
#define GSL_SPAN_HPP

#include <cstddef>

#include <array>
#include <iterator>
#include <limits>
#include <tuple>
#include <type_traits>

namespace gsl {

    /**
     * Sentinel span Extent value indicating the span has a runtime-determined extent.
     */
    inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

    /**
     * A non-owning view over a portion of a contiguous range.
     * 
     * \tparam T      The type of elements the contiguous range contains.
     * \tparam Extent The static number of elements this span grants a view over.
     */
    template<typename T, std::size_t Extent = dynamic_extent>
    class span;

    namespace details {

        template<typename T, std::size_t Extent, std::size_t N, typename ArrayType, typename = void>
        struct array_is_valid : std::false_type {};

        template<typename T, std::size_t Extent, std::size_t N, typename ArrayType>
        struct array_is_valid<
            T,
            Extent,
            N,
            ArrayType,
            std::enable_if_t<
                ( Extent == dynamic_extent || Extent == N ) &&
                std::is_convertible_v<
                    std::remove_pointer_t<
                        decltype(
                            std::data( std::declval<std::add_lvalue_reference_t<ArrayType>>() )
                        )
                    >(*)[],
                    T(*)[]
                >
            >
        > :
            std::true_type
        {};

        template<typename T, std::size_t Extent, std::size_t N, typename ArrayType>
        inline constexpr bool array_is_valid_v = array_is_valid<T,Extent,N,ArrayType>::value;

        template<typename T>
        struct is_std_array : std::false_type {};

        template<typename T, std::size_t N>
        struct is_std_array<std::array<T,N>> : std::true_type {};

        template<typename T>
        inline constexpr bool is_std_array_v = is_std_array<T>::value;

        template<typename T>
        struct is_gsl_span : std::false_type {};

        template<typename T, std::size_t N>
        struct is_gsl_span<span<T,N>> : std::true_type {};

        template<typename T>
        inline constexpr bool is_gsl_span_v = is_gsl_span<T>::value;

        template<typename T, typename = void>
        struct has_size : std::false_type {};

        template<typename T>
        struct has_size<
            T,
            std::void_t<decltype( std::size( std::declval<T>() ) )>
        > :
            std::true_type
        {};

        template<typename T>
        inline constexpr bool has_size_v = has_size<T>::value;

        template<typename T, typename = void>
        struct has_data : std::false_type {};

        template<typename T>
        struct has_data<
            T,
            std::void_t<decltype( std::data( std::declval<T>() ) )>
        > :
            std::true_type
        {};

        template<typename T>
        inline constexpr bool has_data_v = has_data<T>::value;

        template<typename T, typename = void>
        struct has_size_and_data : std::false_type {};

        template<typename T>
        struct has_size_and_data<T, std::enable_if_t<has_size_v<T> && has_data_v<T>>> :
            std::true_type
        {};

        template<typename T>
        inline constexpr bool has_size_and_data_v = has_size_and_data<T>::value;

        template<typename T, std::size_t Extent, typename ContainerType, typename = void>
        struct container_is_valid : std::false_type {};

        template<typename T, std::size_t Extent, typename ContainerType>
        struct container_is_valid<
            T,
            Extent,
            ContainerType,
            std::enable_if_t<
                !std::is_array_v<ContainerType> &&
                !is_std_array_v<ContainerType> &&
                !is_gsl_span_v<ContainerType> &&
                has_size_and_data_v<ContainerType> &&
                std::is_convertible_v<
                    std::remove_pointer_t<
                        decltype(
                            std::data( std::declval<std::add_lvalue_reference_t<ContainerType>>() )
                        )
                    >(*)[],
                    T(*)[]
                >
            >
        > :
            std::true_type
        {};

        template<typename T, std::size_t Extent, typename ContainerType>
        inline constexpr bool container_is_valid_v = container_is_valid<
            T,
            Extent,
            ContainerType
        >::value;

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

    template<typename T, std::size_t Extent>
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

            /**
             * The static number of elements in the sequence.
             */
            static constexpr std::size_t extent = Extent;

            /**
             * Construct a span that does not grant a view over anything.
             * 
             * This overload only participates in overload resolution if the extent is zero or
             * dynamic.
             */
            template<
                bool Enable = Extent == 0 || Extent == dynamic_extent,
                typename = std::enable_if_t<Enable>
            >
            constexpr span() noexcept;

            /**
             * Construct a span given a pointer to the underlying range and a number of elements.
             * 
             * \param ptr   Pointer to the first element in the range to span over.
             * \param count The number of elements to span over.
             */
            constexpr span( pointer ptr, index_type count );

            /**
             * Construct a span given pointers to the limits of the range.
             * 
             * \param first Pointer to the first elements in the range to span over.
             * \param last  Pointer to one-past-the-last element to span over.
             */
            constexpr span( pointer first, pointer last );

            /**
             * Construct a span over a built-in array.
             * 
             * This overload only participates in overload resolution if:
             * - the span extent is equal to N or is dynamic and
             * - a pointer to the array's element type is convertible to a pointer to the span's
             *   element type.
             * 
             * \tparam N The number of elements the array has.
             * 
             * \param arr The array to span over.
             */
            template<
                std::size_t N,
                typename = std::enable_if_t<
                    details::array_is_valid_v<element_type,Extent,N,element_type[N]>
                >
            >
            constexpr span( element_type(&arr)[N] ) noexcept;

            /**
             * Construct a span over a mutable STL array.
             * 
             * This overload only participates in overload resolution if:
             * - the span extent is equal to N or is dynamic and
             * - a pointer to the array's element type is convertible to a pointer to the span's
             *   element type.
             * 
             * \tparam N The number of elements the array has.
             * 
             * \param arr The array to span over.
             */
            template<
                std::size_t N,
                typename = std::enable_if_t<
                    details::array_is_valid_v<element_type,Extent,N,std::array<value_type,N>>
                >
            >
            constexpr span( std::array<value_type,N>& arr ) noexcept;

            /**
             * Construct a span over a const-qualified STL array.
             * 
             * This overload only participates in overload resolution if:
             * - the span extent is equal to N or is dynamic and
             * - a pointer to the array's element type is convertible to a pointer to the span's
             *   element type.
             * 
             * \tparam N The number of elements the array has.
             * 
             * \param arr The array to span over.
             */
            template<
                std::size_t N,
                typename = std::enable_if_t<
                    details::array_is_valid_v<element_type,Extent,N,const std::array<value_type,N>>
                >
            >
            constexpr span( const std::array<value_type,N>& arr ) noexcept;

            /**
             * Construct a span over a mutable contiguous container.
             * 
             * This overload only participates in overload resolution if:
             * - the container is not a span, an STL array, or a built-in array, and
             * - the container provides access to its underlying data pointer and its size, and
             * - a pointer to the container's element type is convertible to a pointer to the span's
             *   element type.
             * 
             * \tparam Container The type of container to span over.
             * 
             * \param cont The container to span over.
             */
            template<
                typename Container,
                typename = std::enable_if_t<
                    details::container_is_valid_v<element_type,Extent,Container>
                >
            >
            constexpr span( Container& cont );

            /**
             * Construct a span over a const-qualified contiguous container.
             * 
             * This overload only participates in overload resolution if:
             * - the container is not a span, an STL array, or a built-in array, and
             * - the container provides access to its underlying data pointer and its size, and
             * - a pointer to the container's element type is convertible to a pointer to the span's
             *   element type.
             * 
             * \tparam Container The type of container to span over.
             * 
             * \param cont The container to span over.
             */
            template<
                typename Container,
                typename = std::enable_if_t<
                    details::container_is_valid_v<element_type,Extent,const Container>
                >
            >
            constexpr span( const Container& cont );

            /**
             * Construct a span from a span of a different type.
             * 
             * This overload only participates in overload resolution if:
             * - the constructed span has extent that is dynamic or equal the source span's, and
             * - a pointer to the source span's element type is convertible to a pointer to the
             *   constructed span's element type.
             * 
             * \tparam U The element type of the source span.
             * \tparam N The extent of the source span.
             * 
             * \param s The source span to construct a new span from.
             */
            template<
                typename    U,
                std::size_t N,
                typename = std::enable_if_t<
                    ( extent == dynamic_extent || extent == N ) &&
                    std::is_convertible_v<U(*)[],element_type(*)[]>
                >
            >
            constexpr span( const span<U,N>& s ) noexcept;

            /**
             * Copy constructor.
             * 
             * Construct a span that spans over the same range as the source span.
             * 
             * \param other The span to make a copy of.
             */
            constexpr span( const span& other ) noexcept = default;

            /**
             * Copy assignment operator.
             * 
             * Make this span span over the same range as the source span.
             * 
             * \param other The span to make a copy of.
             */
            constexpr span& operator=( const span& other ) noexcept = default;

            /**
             * Obtain a mutable iterator to the beginning of the spanned range.
             * 
             * \return Returns a mutable iterator to the beginning of the spanned range.
             */
            constexpr iterator begin() const noexcept;

            /**
             * Obtain a const-qualified iterator to the beginning of the spanned range.
             * 
             * \return Returns a const-qualified iterator to the beginning of the spanned range.
             */
            constexpr const_iterator cbegin() const noexcept;

            /**
             * Obtain a mutable iterator to one-past-the-end of the spanned range.
             * 
             * \return Returns a mutable iterator to one-past-the-end of the spanned range.
             */
            constexpr iterator end() const noexcept;

            /**
             * Obtain a const-qualified iterator to one-past-the-end of the spanned range.
             * 
             * \return Returns a const-qualified iterator to one-past-the-end of the spanned range.
             */
            constexpr const_iterator cend() const noexcept;

            /**
             * Obtain a mutable iterator to the reverse-beginning of the spanned range.
             * 
             * \return Returns a mutable iterator to the reverse-beginning of the spanned range.
             */
            constexpr reverse_iterator rbegin() const noexcept;

            /**
             * Obtain a const-qualified iterator to the reverse-beginning of the spanned range.
             * 
             * \return Returns a const-qualified iterator to the reverse-beginning of the spanned
             *         range.
             */
            constexpr const_reverse_iterator crbegin() const noexcept;

            /**
             * Obtain a mutable iterator to one-past-the-reverse-end of the spanned range.
             * 
             * \return Returns a mutable iterator to one-past-the-reverse-end of the spanned range.
             */
            constexpr reverse_iterator rend() const noexcept;

            /**
             * Obtain a const-qualified iterator to one-past-the-reverse-end of the spanned range.
             * 
             * \return Returns a const-qualified iterator to one-past-the-reverse-end of the spanned
             *         range.
             */
            constexpr const_reverse_iterator crend() const noexcept;

            /**
             * Access the first element in the spanned range.
             * 
             * \return Returns the first element in the spanned range.
             */
            constexpr reference front() const;

            /**
             * Access the last element in the spanned range.
             * 
             * \return Returns the last element in the spanned range.
             */
            constexpr reference back() const;

            /**
             * Subscript operator.
             * 
             * Unchecked indexed access into the span.
             * 
             * \param idx The index of the element to access.
             * 
             * \return Returns the element with the given index in the spanned range.
             */
            constexpr reference operator[]( index_type idx ) const;

            /**
             * Access the underlying contiguous data.
             * 
             * \return Returns a pointer to the first element in the spanned range.
             */
            constexpr pointer data() const noexcept;

            /**
             * Obtain the number of elements in the spanned range.
             * 
             * \return Returns the number of elements in the spanned range.
             */
            constexpr index_type size() const noexcept;

            /**
             * Obtain the size of the spanned range in bytes.
             * 
             * \return Returns the number of bytes in the spanned range.
             */
            constexpr index_type size_bytes() const noexcept;

            /**
             * Check whether or not the spanned range has zero size.
             * 
             * \retval true  The spanned range has zero length.
             * \retval false The spanned range is at least one element long.
             */
            [[nodiscard]]
            constexpr bool empty() const noexcept;

            /**
             * Obtain a span over the first given number of elements of the spanned range.
             * 
             * \tparam Count The number of elements to obtain a sub-span over.
             * 
             * \return Returns a new span that spans over the first given number of elements in this
             *         span's underlying range.
             */
            template<std::size_t Count>
            constexpr span<element_type,Count> first() const;

            /**
             * Obtain a span over the first given number of elements of the spanned range.
             * 
             * \param Count The number of elements to obtain a sub-span over.
             * 
             * \return Returns a new span that spans over the first given number of elements in this
             *         span's underlying range.
             */
            constexpr span<element_type,dynamic_extent> first( std::size_t Count ) const;

            /**
             * Obtain a span over the last given number of elements of the spanned range.
             * 
             * \tparam Count The number of elements to obtain a sub-span over.
             * 
             * \return Returns a new span that spans over the last given number of elements in this
             *         span's underlying range.
             */
            template<std::size_t Count>
            constexpr span<element_type,Count> last() const;

            /**
             * Obtain a span over the last given number of elements of the spanned range.
             * 
             * \param Count The number of elements to obtain a sub-span over.
             * 
             * \return Returns a new span that spans over the last given number of elements in this
             *         span's underlying range.
             */
            constexpr span<element_type,dynamic_extent> last( std::size_t Count ) const;

            /**
             * Obtain a span over a sub-section of the spanned range.
             * 
             * \tparam Offset The offset, in number of elements, between the beginning of the new
             *                span and the beginning of this span.
             * \tparam Count  The number of elements to obtain a sub-span over.
             * 
             * \return Returns a new span that spans over the given number of elements in this
             *         span's underlying range, and has the given offset from the beginning of this
             *         span.
             */
            template<std::size_t Offset, std::size_t Count = dynamic_extent>
            constexpr span<
                element_type,
                details::subspan_extent_v<Extent,Offset,Count>
            > subspan() const;

            /**
             * Obtain a span over a sub-section of the spanned range.
             * 
             * \param Offset The offset, in number of elements, between the beginning of the new
             *               span and the beginning of this span.
             * \param Count  The number of elements to obtain a sub-span over.
             * 
             * \return Returns a new span that spans over the given number of elements in this
             *         span's underlying range, and has the given offset from the beginning of this
             *         span.
             */
            constexpr span<element_type,dynamic_extent>
            subspan( std::size_t Offset, std::size_t Count = dynamic_extent ) const;

        private:
            pointer begin_pos; //!< Pointer to the first element in the spanned range.
            pointer end_pos;   //!< Pointer to one-past-the-last element in the spanned range.
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

    template<typename T, std::size_t N, typename = std::enable_if_t<!std::is_const_v<T>>>
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
    class tuple_size<gsl::span<T,gsl::dynamic_extent>> {};

    template<std::size_t I, typename T, std::size_t N>
    struct tuple_element<I,gsl::span<T,N>> {
        static_assert( I < N, "tuple element index out of bounds" );
        static_assert( N != gsl::dynamic_extent, "cannot take the element type of a dynamic span" );
        using type = T;
    };

}

#include "ipp/span.ipp"

#endif // #ifndef GSL_SPAN_HPP
