#include <array>
#include <iterator>
#include <string>
#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "gsl_util.hpp"

#include "span.hpp"

TEMPLATE_TEST_CASE_SIG(
    "span constructed from various sources have correct data and size",
    "[span][ctor][data][size]",
    ( ( typename T , std::size_t Extent , typename TestType          ), T, Extent, TestType ),
    (   int        ,                   8, int[8]                     ),
    (   double     ,                  16, double[16]                 ),
    (   std::string,                  12, std::string[12]            ),
    (   int        ,                   4, std::array<int,4>          ),
    (   double     ,                   7, std::array<double,7>       ),
    (   std::string,                  12, std::array<std::string,12> ),
    (   int        , gsl::dynamic_extent, std::vector<int>           ),
    (   double     , gsl::dynamic_extent, std::vector<double>        ),
    (   std::string, gsl::dynamic_extent, std::vector<std::string>   )
) {
    SECTION( "default-constructed span" ) {
        gsl::span<TestType> dyn_ext_span;
        CHECK( dyn_ext_span.data() == nullptr );
        CHECK( dyn_ext_span.size() == 0 );

        gsl::span<TestType,0> stat_ext_span;
        CHECK( stat_ext_span.data() == nullptr );
        CHECK( stat_ext_span.size() == 0 );
    }

    SECTION( "pointer-count-constructed span" ) {
        TestType test_data;
        if constexpr ( Extent == gsl::dynamic_extent ) {
            const auto num_elems = GENERATE( random<gsl::index>( 0, 32 ) );

            for ( gsl::index i = 0; i < num_elems; ++i ) {
                test_data.push_back( T{} );
            }
        }

        gsl::span<T> dyn_ext_span( std::data( test_data ), std::size( test_data ) );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );

        gsl::span<T,Extent> stat_ext_span( std::data( test_data ), std::size( test_data ) );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );
    }

    SECTION( "pointer-pair-constructed span" ) {
        TestType test_data;
        if constexpr ( Extent == gsl::dynamic_extent ) {
            auto num_elems = GENERATE( random<gsl::index>( 0, 32 ) );

            for ( gsl::index i = 0; i < num_elems; ++i ) {
                test_data.push_back( T{} );
            }
        }

        gsl::span<T> dyn_ext_span(
            std::data( test_data ),
            std::data( test_data ) + std::size( test_data )
        );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );

        gsl::span<T,Extent> stat_ext_span(
            std::data( test_data ),
            std::data( test_data ) + std::size( test_data )
        );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );
    }

    SECTION( "container-constructed span" ) {
        TestType test_data;
        if constexpr ( Extent == gsl::dynamic_extent ) {
            auto num_elems = GENERATE( random<gsl::index>( 0, 32 ) );

            for ( gsl::index i = 0; i < num_elems; ++i ) {
                test_data.push_back( T{} );
            }
        }

        gsl::span<T> dyn_ext_span( test_data );
        CHECK( dyn_ext_span.data() == std::data( test_data ) );
        CHECK( dyn_ext_span.size() == std::size( test_data ) );

        gsl::span<T,Extent> stat_ext_span( test_data );
        CHECK( stat_ext_span.data() == std::data( test_data ) );
        CHECK( stat_ext_span.size() == std::size( test_data ) );
    }
}
