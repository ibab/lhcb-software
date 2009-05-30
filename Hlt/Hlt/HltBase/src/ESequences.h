#ifndef HLT_ESEQUENCES_H 
#define HLT_ESEQUENCES_H 1


/** @file ESequences.h 
 *
 *  Operations over sequences with begin, and end. To apply, functions,
 *  filters, etc
 *
 *  @author Jose Angel Hernando Morata
 *          based on LoKi ideas by I. Belyaev
 *  @date   2006-07-18
 */

namespace zen {
  
  //-------- sequences in interators ---------------------

  template <class INPUT> 
  size_t count(INPUT b1, const INPUT& e1, const INPUT& b2, const INPUT& e2) {
      // If ranges are sorted, use distance( set_intersection(b1,e1,b2,e2), e1 )
    size_t n = 0;
    for (;b1 != e1; ++b1) {if (std::find(b2,e2,*b1)!=e2) ++n;}
    return n;
  }

  //--- sequences in containers ---------------------------
  
  
  template <class INPUT>
  inline size_t count(const INPUT& c1, const INPUT& c2) 
  {return zen::count(c1.begin(),c1.end(),c2.begin(),c2.end());}
  

};


#endif
