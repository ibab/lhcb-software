#pragma GCC optimize "O3"
#include "LHCbMath/Similarity.h"
#include  <stdexcept>
#include "instrset.h"

namespace {
template <typename Vtbl, typename Trampoline, typename... Args>
void dispatch( const Vtbl& vtbl, Trampoline& t, Args&&... args ) {

    // Get supported instruction set
    int level = instrset_detect();

    // find pointer to the appropriate version 
    auto i =  std::find_if( std::begin(vtbl), std::end(vtbl), 
                            [&](typename Vtbl::const_reference j) {
        return level >= j.first;
    });

    if (i==std::end(vtbl)) {
        throw std::runtime_error{"no implementation for instruction set level " + std::to_string(level) + " ???"};
    }

    t = i->second;
    (*t)(std::forward<Args>(args)...);
}
}

namespace LHCb {
namespace Math {

// TODO: replace by (static?) functor (instance)s... 
//       (maybe templated, with multiple explicit specializations & tag dispatch???)
//
//       or can we replace the 'explicit' vtbl with inheritance? After all,
//       this is 'just' runtime-polymorphism... but then the 'global'
//       instance in the trampoline functions should be updated somehow.
//       That way, they get all shunted simultaneously on the first invocation
//       of any individual one...
//
namespace similarity_5_avx {
    extern void similarity_5_1(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_5(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_7(const double* Ci, const double* Fi, double* Ti);
}
namespace similarity_5_sse3 {
    extern void similarity_5_1(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_5(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_7(const double* Ci, const double* Fi, double* Ti);
}
namespace similarity_5_generic {
    extern void similarity_5_1(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_5(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_7(const double* Ci, const double* Fi, double* Ti);
}

namespace similarity_5_dispatch {

void similarity_5_1(const double* Ci, const double* Fi, double* ti) {
    auto vtbl = { std::make_pair( 7, similarity_5_avx::similarity_5_1 ),
                  std::make_pair( 3, similarity_5_sse3::similarity_5_1 ),
                  std::make_pair( 0, similarity_5_generic::similarity_5_1 ) };
    dispatch( vtbl, LHCb::Math::similarity_5_1, Ci, Fi, ti );
}

void similarity_5_5(const double* Ci, const double* Fi, double* ti) {
    auto vtbl = { std::make_pair( 7, similarity_5_avx::similarity_5_5 ),
                  std::make_pair( 3, similarity_5_sse3::similarity_5_5  ),
                  std::make_pair( 0, similarity_5_generic::similarity_5_5 ) };
    dispatch( vtbl, LHCb::Math::similarity_5_5, Ci, Fi, ti );
}

void similarity_5_7(const double* Ci, const double* Fi, double* ti) {
    auto vtbl = { std::make_pair( 7, similarity_5_avx::similarity_5_7 ),
                  std::make_pair( 3, similarity_5_sse3::similarity_5_7 ),
                  std::make_pair( 0, similarity_5_generic::similarity_5_7 ) };
    dispatch( vtbl, LHCb::Math::similarity_5_7, Ci, Fi, ti );
}

}

similarity_t similarity_5_1 = &similarity_5_dispatch::similarity_5_1;
similarity_t similarity_5_5 = &similarity_5_dispatch::similarity_5_5;
similarity_t similarity_5_7 = &similarity_5_dispatch::similarity_5_7;

} }
