/** @file VersionedObjectInstantiations.h
 *
 * @brief helper file for class mapping a version object to some kind of
 * payload object
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2013-04-15
 *
 * Never include this file directly
 */

#ifndef VERSIONEDOBJECTINSTANTIATIONS_H
#define VERSIONEDOBJECTINSTANTIATIONS_H

#ifdef INSTANTIATE_VERSIONEDOBJECTS_NOW
// get ROOT predefined types
#include <Rtypes.h>

// Instantiate templates most likely to be useful, those include all the basic
// types provided by the language (char, short, ...)
//
// We'll need some machinery for that:
// - a macro helper macro which instantiates a class: INSTANTIATE
// - a macro helper macro which instantiates the iterators of a class:
//   INSTANTIATE_ITERATORS
// - a macro to put a comma into a macro argument: COMMA (the preprocessor
//   uses a comma as argument separator, so it needs some form of protection
//   from the greedy preprocessor... ;)
// - a macro which does the instantiation for all the basic types:
//   INSTANTIATE_BASICTYPES
// - a macro which does the instantiation for all iterators and value_types
//   and the basic types: INSTANTIATE_ITERATORS_BASICTYPES
//
// FIXME:
// - the code does not play well with the std::vector<bool> specialisation, so
//   that's why we do not generate the bool-based types just yet
// - for some reasons, the reverse iterators are somehow not included
//   correctly in the dictionary (and including them in the forced
//   instantiations in this file produces a compile time error)
//
// The following code is not too readable, but it saves lots of typing
#undef COMMA
#undef INSTANTIATE
#undef INSTANTIATE_ITERATORS
#undef INSTANTIATE_BASICTYPES
#undef INSTANTIATE_ITERATORS_BASICTYPES
#undef JOIN3
#undef JOIN3_HELPER
#define COMMA ,
#define INSTANTIATE(CLASS, ...) template class CLASS <__VA_ARGS__>;
#define INSTANTIATE_BASICTYPES(CLASS, PFX, SFX) \
    INSTANTIATE(CLASS, PFX Char_t SFX) \
    INSTANTIATE(CLASS, PFX UChar_t SFX) \
    INSTANTIATE(CLASS, PFX Short_t SFX) \
    INSTANTIATE(CLASS, PFX UShort_t SFX) \
    INSTANTIATE(CLASS, PFX Int_t SFX) \
    INSTANTIATE(CLASS, PFX UInt_t SFX) \
    INSTANTIATE(CLASS, PFX Long_t SFX) \
    INSTANTIATE(CLASS, PFX ULong_t SFX) \
    INSTANTIATE(CLASS, PFX Long64_t SFX) \
    INSTANTIATE(CLASS, PFX ULong64_t SFX) \
    INSTANTIATE(CLASS, PFX Float_t SFX) \
    INSTANTIATE(CLASS, PFX Double_t SFX) 
//    INSTANTIATE(CLASS, PFX Bool_t SFX)

#define JOIN3_HELPER(x, y, z) x ## y ## z
#define JOIN3(x, y, z) JOIN3_HELPER(x, y, z)
#define INSTANTIATE_ITERATORS(CTR1, CTR2, CLASS, ...) \
    namespace { struct JOIN3(__map_iterator_instantiations_, CTR1, CTR2) { \
    CLASS <__VA_ARGS__>::value_type value; \
    CLASS <__VA_ARGS__>::iterator iterator; \
    CLASS <__VA_ARGS__>::const_iterator const_iterator; \
    CLASS <__VA_ARGS__>::reverse_iterator reverse_iterator; \
    CLASS <__VA_ARGS__>::const_reverse_iterator const_reverse_iterator; }; }

#define INSTANTIATE_ITERATORS_BASICTYPES(CTR, CLASS, PFX, SFX) \
    INSTANTIATE_ITERATORS(CTR, 0, CLASS, PFX Char_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 1, CLASS, PFX UChar_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 2, CLASS, PFX Short_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 3, CLASS, PFX UShort_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 4, CLASS, PFX Int_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 5, CLASS, PFX UInt_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 6, CLASS, PFX Long_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 7, CLASS, PFX ULong_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 8, CLASS, PFX Long64_t SFX) \
    INSTANTIATE_ITERATORS(CTR, 9, CLASS, PFX ULong64_t SFX) \
    INSTANTIATE_ITERATORS(CTR, a, CLASS, PFX Float_t SFX) \
    INSTANTIATE_ITERATORS(CTR, b, CLASS, PFX Double_t SFX) 
//    INSTANTIATE_ITERATORS(CTR, c, CLASS, PFX Bool_t SFX)

////////////////////////////////////////////////////////////////////////
// instatiate VersionedObject
////////////////////////////////////////////////////////////////////////
// scalars
INSTANTIATE_ITERATORS_BASICTYPES(0, VersionedObject, , )
INSTANTIATE_BASICTYPES(VersionedObject, , )
// vectors
INSTANTIATE_ITERATORS_BASICTYPES(1, VersionedObject, std::vector<, >)
INSTANTIATE_BASICTYPES(VersionedObject, std::vector<, >)
// vectors of vectors (2D)
INSTANTIATE_ITERATORS_BASICTYPES(2, VersionedObject, std::vector<std::vector<, > >)
INSTANTIATE_BASICTYPES(VersionedObject, std::vector<std::vector<, > >)
// vectors of sparse vectors (2D)
INSTANTIATE_ITERATORS_BASICTYPES(3, VersionedObject, std::vector<std::map<int COMMA, > >)
INSTANTIATE_BASICTYPES(VersionedObject, std::vector<std::map<int COMMA, > >)
// sparse vectors
INSTANTIATE_ITERATORS_BASICTYPES(4, VersionedObject, std::map<int COMMA, >)
INSTANTIATE_BASICTYPES(VersionedObject, std::map<int COMMA, >)
// sparse vectors of vectors (2D)
INSTANTIATE_ITERATORS_BASICTYPES(5, VersionedObject, std::map<int COMMA std::vector<, > >)
INSTANTIATE_BASICTYPES(VersionedObject, std::map<int COMMA std::vector<, > >)
// sparse vectors of sparse vectors (2D)
INSTANTIATE_ITERATORS_BASICTYPES(6, VersionedObject, std::map<int COMMA std::map<int COMMA, > >)
INSTANTIATE_BASICTYPES(VersionedObject, std::map<int COMMA std::map<int COMMA, > >)

namespace {
    struct __map_iterator_instantiations_70 {
	VersionedObject<std::string>::iterator iterator;
	VersionedObject<std::string>::const_iterator const_iterator;
	VersionedObject<std::string>::reverse_iterator reverse_iterator;
	VersionedObject<std::string>::const_reverse_iterator const_reverse_iterator;
    };
}
template class VersionedObject<std::string>;

////////////////////////////////////////////////////////////////////////
// instatiate underlying std::vectors
////////////////////////////////////////////////////////////////////////
INSTANTIATE_ITERATORS_BASICTYPES(8, std::vector, , )
INSTANTIATE_BASICTYPES(std::vector, , )
INSTANTIATE_ITERATORS_BASICTYPES(9, std::vector, std::vector<, >)
INSTANTIATE_BASICTYPES(std::vector, std::vector<, >)
INSTANTIATE_ITERATORS_BASICTYPES(a, std::vector, std::map<int COMMA , >)
INSTANTIATE_BASICTYPES(std::vector, std::map<int COMMA , >)

////////////////////////////////////////////////////////////////////////
// instatiate underlying std::maps
////////////////////////////////////////////////////////////////////////
INSTANTIATE_ITERATORS_BASICTYPES(b, std::map, int COMMA, )
INSTANTIATE_BASICTYPES(std::map, int COMMA, )
INSTANTIATE_ITERATORS_BASICTYPES(c, std::map, int COMMA std::vector<, >)
INSTANTIATE_BASICTYPES(std::map, int COMMA std::vector<, >)
INSTANTIATE_ITERATORS_BASICTYPES(d, std::map, int COMMA std::map<int COMMA , >)
INSTANTIATE_BASICTYPES(std::map, int COMMA std::map<int COMMA , >)

#undef COMMA
#undef INSTANTIATE
#undef INSTANTIATE_ITERATORS
#undef INSTANTIATE_BASICTYPES
#undef INSTANTIATE_ITERATORS_BASICTYPES
#undef JOIN3
#undef JOIN3_HELPER
#else
#error "Never include VersionedObjectInstantiations.h directly."
#endif

#endif // VERSIONEDOBJECTINSTANTIATIONS_H

// vim: sw=4:tw=78:ft=cpp
