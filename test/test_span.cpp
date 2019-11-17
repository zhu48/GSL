#include <cstddef>

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "gsl_detail.hpp"

#include "gsl_util.hpp"

#include "span.hpp"

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

TEMPLATE_TEST_CASE_SIG(
    "spans constructed from various sources have correct data, size, and emptiness",
    "[span][ctor][data][size]",
    ( ( typename T , std::size_t Extent ,   typename TestType            ), T, Extent, TestType ),
    (   int        ,                   8,   int[8]                       ),
    (   double     ,                  16,   double[16]                   ),
    (   std::string,                  12,   std::string[12]              ),
    (   int        ,                   4, ( std::array<int,4>          ) ),
    (   double     ,                   7, ( std::array<double,7>       ) ),
    (   std::string,                  12, ( std::array<std::string,12> ) ),
    (   int        , gsl::dynamic_extent,   std::vector<int>             ),
    (   double     , gsl::dynamic_extent,   std::vector<double>          ),
    (   std::string, gsl::dynamic_extent,   std::vector<std::string>     )
) {
    SECTION( "default-constructed span" ) {
        gsl::span<T> dyn_ext_span;
        CHECK( dyn_ext_span.data() == nullptr );
        CHECK( dyn_ext_span.size() == 0 );
        CHECK( dyn_ext_span.size_bytes() == 0 );
        CHECK( dyn_ext_span.empty() );

        gsl::span<T,0> stat_ext_span;
        CHECK( stat_ext_span.data() == nullptr );
        CHECK( stat_ext_span.size() == 0 );
        CHECK( stat_ext_span.size_bytes() == 0 );
        CHECK( stat_ext_span.empty() );
    }

    SECTION( "pointer-count-constructed span" ) {
        TestType test_data;
        if constexpr ( Extent == gsl::dynamic_extent ) {
            const auto num_elems = GENERATE( take( 1, random<gsl::index>( 0, 32 ) ) );

            for ( gsl::index i = 0; i < num_elems; ++i ) {
                test_data.push_back( T{} );
            }
        }

        gsl::span<T> dyn_ext_span( std::data( test_data ), std::size( test_data ) );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );
        CHECK( dyn_ext_span.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( dyn_ext_span.empty() == std::empty( test_data ) );

        gsl::span<T,Extent> stat_ext_span( std::data( test_data ), std::size( test_data ) );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );
        CHECK( dyn_ext_span.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( dyn_ext_span.empty() == std::empty( test_data ) );
    }

    SECTION( "pointer-pair-constructed span" ) {
        TestType test_data;
        if constexpr ( Extent == gsl::dynamic_extent ) {
            auto num_elems = GENERATE( take( 1, random<gsl::index>( 0, 32 ) ) );

            for ( gsl::index i = 0; i < num_elems; ++i ) {
                test_data.push_back( T{} );
            }
        }

        T* const first = std::data( test_data );
        T* const last = std::data( test_data ) + std::size( test_data );
        gsl::span<T> dyn_ext_span( first, last );
        CHECK( dyn_ext_span.data() == first );
        CHECK( dyn_ext_span.size() == last - first );
        CHECK( dyn_ext_span.size_bytes() == ( last - first ) * sizeof( T ) );
        CHECK( dyn_ext_span.empty() == std::empty( test_data ) );

        gsl::span<T,Extent> stat_ext_span( first, last );
        CHECK( dyn_ext_span.data() == first );
        CHECK( dyn_ext_span.size() == last - first );
        CHECK( dyn_ext_span.size_bytes() == ( last - first ) * sizeof( T ) );
        CHECK( dyn_ext_span.empty() == std::empty( test_data ) );
    }

    SECTION( "container-constructed span" ) {
        TestType test_data;
        if constexpr ( Extent == gsl::dynamic_extent ) {
            auto num_elems = GENERATE( take( 1, random<gsl::index>( 0, 32 ) ) );

            for ( gsl::index i = 0; i < num_elems; ++i ) {
                test_data.push_back( T{} );
            }
        }

        gsl::span<T> dyn_ext_span( test_data );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );
        CHECK( dyn_ext_span.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( dyn_ext_span.empty() == std::empty( test_data ) );

        gsl::span<T,Extent> stat_ext_span( test_data );
        CHECK( stat_ext_span.data() == std::data( test_data ) );
        CHECK( stat_ext_span.size() == std::size( test_data ) );
        CHECK( stat_ext_span.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( stat_ext_span.empty() == std::empty( test_data ) );
    }

    SECTION( "span-constructed span" ) {
        TestType test_data;
        if constexpr ( Extent == gsl::dynamic_extent ) {
            auto num_elems = GENERATE( take( 1, random<gsl::index>( 0, 32 ) ) );

            for ( gsl::index i = 0; i < num_elems; ++i ) {
                test_data.push_back( T{} );
            }
        }

        gsl::span<T> original_dyn_span( test_data );
        gsl::span<T,Extent> original_stat_span( test_data );

        gsl::span<T> dyn_from_dyn( original_dyn_span );
        CHECK( dyn_from_dyn.data() == std::data( test_data ) );
        CHECK( dyn_from_dyn.size() == std::size( test_data ) );
        CHECK( dyn_from_dyn.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( dyn_from_dyn.empty() == std::empty( test_data ) );

        gsl::span<T,Extent> stat_from_dyn( original_dyn_span );
        CHECK( stat_from_dyn.data() == std::data( test_data ) );
        CHECK( stat_from_dyn.size() == std::size( test_data ) );
        CHECK( stat_from_dyn.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( stat_from_dyn.empty() == std::empty( test_data ) );

        gsl::span<T> dyn_from_stat( original_stat_span );
        CHECK( dyn_from_stat.data() == std::data( test_data ) );
        CHECK( dyn_from_stat.size() == std::size( test_data ) );
        CHECK( dyn_from_stat.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( dyn_from_stat.empty() == std::empty( test_data ) );

        gsl::span<T,Extent> stat_from_stat( original_stat_span );
        CHECK( stat_from_stat.data() == std::data( test_data ) );
        CHECK( stat_from_stat.size() == std::size( test_data ) );
        CHECK( stat_from_stat.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( stat_from_stat.empty() == std::empty( test_data ) );
    }
}

TEMPLATE_TEST_CASE_SIG(
    "span element access behaves as expected",
    "[span][access]",
    ( ( typename T  , std::size_t Extent ,   typename TestType            ), T, Extent, TestType ),
    (   std::int16_t,                  14,   std::int16_t[14]             ),
    (   std::int32_t,                  14,   std::int32_t[14]             ),
    (   std::int64_t,                  14,   std::int64_t[14]             ),
    (   std::int16_t,                   3, ( std::array<std::int16_t,3> ) ),
    (   std::int32_t,                   3, ( std::array<std::int32_t,3> ) ),
    (   std::int64_t,                   3, ( std::array<std::int64_t,3> ) ),
    (   std::int16_t, gsl::dynamic_extent,   std::vector<std::int16_t>    ),
    (   std::int32_t, gsl::dynamic_extent,   std::vector<std::int32_t>    ),
    (   std::int64_t, gsl::dynamic_extent,   std::vector<std::int64_t>    )
) {
    TestType test_data;
    if constexpr ( Extent == gsl::dynamic_extent ) {
        const auto num_elems = GENERATE( take( 1, random<gsl::index>( 0, 32 ) ) );

        for ( gsl::index i = 0; i < num_elems; ++i ) {
            test_data.push_back( T{} );
        }
    }

    std::vector<T> reference_data;
    for ( gsl::index i = 0; i < ssize( test_data ); ++i ) {
        reference_data.push_back(
            GENERATE(
                take(
                    1,
                    random( std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max() )
                )
            )
        );
    }
    std::copy( reference_data.cbegin(), reference_data.cend(), std::begin( test_data ) );
    gsl::span<const T> const_span( test_data );
    for ( gsl::index i = 0; i < ssize( test_data ); ++i ) {
        CHECK( const_span[i] == gsl::at( reference_data, i ) );
    }

    reference_data.clear();
    for ( gsl::index i = 0; i < ssize( test_data ); ++i ) {
        reference_data.push_back(
            GENERATE(
                take(
                    1,
                    random( std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max() )
                )
            )
        );
    }
    gsl::span<T> mutable_span( test_data );
    std::copy( reference_data.cbegin(), reference_data.cend(), mutable_span.begin() );
    for ( gsl::index i = 0; i < ssize( test_data ); ++i ) {
        CHECK( test_data[i] == reference_data[i] );
    }
}

TEST_CASE(
    "subspans obtained from existing spans have correct data, size, and emptiness",
    "[span][ctor][data][size]"
) {
    int test_arr[15];

    const gsl::span<int> original_dyn_span( test_arr );
    const gsl::span<int,15> original_stat_span( test_arr );
    const gsl::span<const int> original_const_dyn_span( test_arr );
    const gsl::span<const int,15> original_const_stat_span( test_arr );

    SECTION( "subspans created using span::first()" ) {
        const auto test_span_0 = original_dyn_span.first<7>();
        CHECK( test_span_0.data() == std::data( test_arr ) );
        CHECK( test_span_0.size() == 7 );
        CHECK( test_span_0.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_0.empty() );

        const auto test_span_1 = original_dyn_span.first( 7 );
        CHECK( test_span_1.data() == std::data( test_arr ) );
        CHECK( test_span_1.size() == 7 );
        CHECK( test_span_1.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_1.empty() );

        const auto test_span_2 = original_stat_span.first<7>();
        CHECK( test_span_2.data() == std::data( test_arr ) );
        CHECK( test_span_2.size() == 7 );
        CHECK( test_span_2.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_2.empty() );

        const auto test_span_3 = original_stat_span.first( 7 );
        CHECK( test_span_3.data() == std::data( test_arr ) );
        CHECK( test_span_3.size() == 7 );
        CHECK( test_span_3.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_3.empty() );

        const auto test_span_4 = original_const_dyn_span.first<7>();
        CHECK( test_span_4.data() == std::data( test_arr ) );
        CHECK( test_span_4.size() == 7 );
        CHECK( test_span_4.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_4.empty() );

        const auto test_span_5 = original_const_dyn_span.first( 7 );
        CHECK( test_span_5.data() == std::data( test_arr ) );
        CHECK( test_span_5.size() == 7 );
        CHECK( test_span_5.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_5.empty() );

        const auto test_span_6 = original_const_stat_span.first<7>();
        CHECK( test_span_6.data() == std::data( test_arr ) );
        CHECK( test_span_6.size() == 7 );
        CHECK( test_span_6.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_6.empty() );

        const auto test_span_7 = original_const_stat_span.first( 7 );
        CHECK( test_span_7.data() == std::data( test_arr ) );
        CHECK( test_span_7.size() == 7 );
        CHECK( test_span_7.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_7.empty() );
    }

    SECTION( "subspans created using span::last()" ) {
        const auto test_span_0 = original_dyn_span.last<7>();
        CHECK( test_span_0.data() == std::data( test_arr ) + 8 );
        CHECK( test_span_0.size() == 7 );
        CHECK( test_span_0.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_0.empty() );

        const auto test_span_1 = original_dyn_span.last( 7 );
        CHECK( test_span_1.data() == std::data( test_arr ) + 8 );
        CHECK( test_span_1.size() == 7 );
        CHECK( test_span_1.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_1.empty() );

        const auto test_span_2 = original_stat_span.last<7>();
        CHECK( test_span_2.data() == std::data( test_arr ) + 8 );
        CHECK( test_span_2.size() == 7 );
        CHECK( test_span_2.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_2.empty() );

        const auto test_span_3 = original_stat_span.last( 7 );
        CHECK( test_span_3.data() == std::data( test_arr ) + 8 );
        CHECK( test_span_3.size() == 7 );
        CHECK( test_span_3.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_3.empty() );

        const auto test_span_4 = original_const_dyn_span.last<7>();
        CHECK( test_span_4.data() == std::data( test_arr ) + 8 );
        CHECK( test_span_4.size() == 7 );
        CHECK( test_span_4.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_4.empty() );

        const auto test_span_5 = original_const_dyn_span.last( 7 );
        CHECK( test_span_5.data() == std::data( test_arr ) + 8 );
        CHECK( test_span_5.size() == 7 );
        CHECK( test_span_5.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_5.empty() );

        const auto test_span_6 = original_const_stat_span.last<7>();
        CHECK( test_span_6.data() == std::data( test_arr ) + 8 );
        CHECK( test_span_6.size() == 7 );
        CHECK( test_span_6.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_6.empty() );

        const auto test_span_7 = original_const_stat_span.last( 7 );
        CHECK( test_span_7.data() == std::data( test_arr ) + 8 );
        CHECK( test_span_7.size() == 7 );
        CHECK( test_span_7.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_7.empty() );
    }

    SECTION( "subspans created using span::subspan()" ) {
        const auto test_span_0 = original_dyn_span.subspan<2,7>();
        CHECK( test_span_0.data() == std::data( test_arr ) + 2 );
        CHECK( test_span_0.size() == 7 );
        CHECK( test_span_0.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_0.empty() );

        const auto test_span_1 = original_dyn_span.subspan( 2, 7 );
        CHECK( test_span_1.data() == std::data( test_arr ) + 2 );
        CHECK( test_span_1.size() == 7 );
        CHECK( test_span_1.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_1.empty() );

        const auto test_span_2 = original_stat_span.subspan<2,7>();
        CHECK( test_span_2.data() == std::data( test_arr ) + 2 );
        CHECK( test_span_2.size() == 7 );
        CHECK( test_span_2.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_2.empty() );

        const auto test_span_3 = original_stat_span.subspan( 2, 7 );
        CHECK( test_span_3.data() == std::data( test_arr ) + 2 );
        CHECK( test_span_3.size() == 7 );
        CHECK( test_span_3.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_3.empty() );

        const auto test_span_4 = original_const_dyn_span.subspan<2,7>();
        CHECK( test_span_4.data() == std::data( test_arr ) + 2 );
        CHECK( test_span_4.size() == 7 );
        CHECK( test_span_4.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_4.empty() );

        const auto test_span_5 = original_const_dyn_span.subspan( 2, 7 );
        CHECK( test_span_5.data() == std::data( test_arr ) + 2 );
        CHECK( test_span_5.size() == 7 );
        CHECK( test_span_5.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_5.empty() );

        const auto test_span_6 = original_const_stat_span.subspan<2,7>();
        CHECK( test_span_6.data() == std::data( test_arr ) + 2 );
        CHECK( test_span_6.size() == 7 );
        CHECK( test_span_6.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_6.empty() );

        const auto test_span_7 = original_const_stat_span.subspan( 2, 7 );
        CHECK( test_span_7.data() == std::data( test_arr ) + 2 );
        CHECK( test_span_7.size() == 7 );
        CHECK( test_span_7.size_bytes() == 7 * sizeof( int ) );
        CHECK( !test_span_7.empty() );
    }
}
