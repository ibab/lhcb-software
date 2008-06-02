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

  template<class INPUT , class OUTPUT , class FILTER>
  inline OUTPUT select( INPUT begin , 
                        const INPUT& end   ,
                        const FILTER& filter,
                        OUTPUT out )
  {
    for( ; begin != end ; ++begin ) 
    {if( filter(**begin) ) *(out++) = *begin ;  }
    return out ;
  };
  
  template <class INPUT, class OUTPUT>
  inline void copy(const INPUT& c1, OUTPUT& c2) 
  {std::copy(c1.begin(),c1.end(),std::back_inserter(c2));}

  template <class INPUT, class OBJECT>
  inline bool find(INPUT begin, INPUT end,const OBJECT& obj ) {
    return std::find(begin,end,obj)!=end;
  }
  
  template <class INPUT> 
  size_t count(INPUT b1, const INPUT& e1, const INPUT& b2, const INPUT& e2) {
    size_t n = 0;
    for (;b1 != e1; ++b1) {if (zen::find(b2,e2,*b1)) ++n;}
    return n;
  }

  //--- sequences in containers ---------------------------
  
  template <class INPUT,class OBJECT>
  inline bool extend(INPUT& cont, OBJECT& obj) {
    if (std::find(cont.begin(),cont.end(),obj) != cont.end()) return false;
    cont.push_back(obj);
    return true;
  }
  
  template <class CONTAINER, class OBJECT>
  inline bool find(const CONTAINER& cont, const OBJECT& obj ) {
    return zen::find(cont.begin(),cont.end(),obj);
  }
  
  template <class INPUT>
  inline size_t count(const INPUT& c1, const INPUT& c2) 
  {return zen::count(c1.begin(),c1.end(),c2.begin(),c2.end());}
  

};


#endif
