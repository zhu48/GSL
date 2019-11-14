#include <array>
#include <iterator>
#include <string>
#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "gsl_util.hpp"

#include "span.hpp"

TEMPLATE_TEST_CASE_SIG(
    "span constructed from various sources have correct data, size, and emptiness",
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

        gsl::span<T> dyn_ext_span(
            std::data( test_data ),
            std::data( test_data ) + std::size( test_data ) - 1
        );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );
        CHECK( dyn_ext_span.size_bytes() == std::size( test_data ) * sizeof( T ) );
        CHECK( dyn_ext_span.empty() == std::empty( test_data ) );

        gsl::span<T,Extent> stat_ext_span(
            std::data( test_data ),
            std::data( test_data ) + std::size( test_data )
        );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );
        CHECK( dyn_ext_span.size_bytes() == std::size( test_data ) * sizeof( T ) );
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
