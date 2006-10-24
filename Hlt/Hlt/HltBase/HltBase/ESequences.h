#ifndef HLT_ESEQUENCES_H 
#define HLT_ESEQUENCES_H 1


/** Operations over sequences with begin, and end. To apply, functions,
 *  filters, etc
 *
 *  @author Jose Angel Hernando Morata
 *          based on LoKi ideas by I. Belyaev
 *  @date   2006-07-18
 */

namespace Estd {

  //   has key
  
  template <class MAPCONTAINER, class KEY>
  inline bool has_key(const MAPCONTAINER& cont, KEY key)
  {return (cont.find(key) != cont.end());}
  
  template <class MAPITERATOR, class KEY>
  inline bool has_key(MAPITERATOR begin, 
                      MAPITERATOR end,
                      KEY key) {
    for(; begin != end; ++begin) {if( begin->first == key) return true;}
    return false;
  }

  template <class INPUT, class OBJECT>
  inline bool exist(INPUT begin, const INPUT& end,const OBJECT& obj ) {
    for ( ; begin!= end; ++begin) 
      if ( (int) (*begin) == (int) &obj) return true;
    return false;
  }

  // loops in uni functions

  template<class INPUT , class OUTPUT , class FUNCTOR>
  inline OUTPUT map( INPUT begin , 
                     const INPUT& end,
                     FUNCTOR& funct,
                     OUTPUT out   )
  {
    for( ; begin != end ; ++begin ) 
    { *out = funct(**begin) ; ++out ;  }
    return out ;
  };


  template <class INPUT, class FUNCTION>
  inline void map(INPUT begin, 
                  const INPUT& end,
                  const FUNCTION& fun, 
                  int key) { 
    for ( ;begin != end; ++begin) 
    {(**begin).addInfo(key,fun(**begin));}
  }
  
  template<class INPUT , class OUTPUT , class FILTER>
  inline OUTPUT select( INPUT begin , 
                        const INPUT& end   ,
                        const FILTER& filter,
                        OUTPUT out )
  {
    for( ; begin != end ; ++begin ) 
    {if( filter(**begin) ) { *out = *begin ; ++out ; } }
    return out ;
  };

  // loops in reduced bifunction to one argument functions

  template <class ARG, class INPUT, class FUNCTION, class COMPARATOR> 
  inline double map_compare_value(const ARG& arg, 
                                  INPUT begin, 
                                  const INPUT& end,
                                  const FUNCTION& fun, 
                                  const COMPARATOR& com,
                                  size_t& index ) {
    double d0 = fun(arg,**begin);++begin; index = 0; size_t k = 1;
    for (; begin != end; ++begin, k++)
    {double d = fun(arg,**begin); if (com(d,d0)) {d0 = d; index = k;}; }
    return d0;
  }

  template <class INPUT, class INPUT2, class FUNCTION, class COMPARATOR> 
  inline  void map_compare_value(INPUT begin, 
                                 const INPUT& end, 
                                 const INPUT2& begin2, 
                                 const INPUT2& end2,
                                 const FUNCTION& fun, 
                                 const COMPARATOR& com,
                                 int key1, 
                                 int key2 = -1) {
    for (; begin!= end; ++begin) {
      INPUT2 _begin2 = begin2; size_t index = 0;
      double d = Estd::map_compare_value(**begin,_begin2,end2,fun,com,index);
      (**begin).addInfo(key1,d);
      if (key2>=0) (**begin).addInfo(key2,index);
    }
  }

  template <class INPUT, class FUNCTION, class ARG> 
  inline void map_first(const ARG& o, 
                        INPUT begin, 
                        const INPUT& end, 
                        const FUNCTION& fun, 
                        int key) {
    for ( ; begin != end; ++begin) 
      (**begin).addInfo(key, fun( (**begin), o) );
  }      
  
  template <class INPUT, class FUNCTION, class ARG> 
  inline  void map_second(const ARG& o, 
                          INPUT begin, 
                          const INPUT& end, 
                          const FUNCTION& fun, 
                          int key) {
    for ( ; begin != end; ++begin) 
      (**begin).addInfo(key, fun( o, (**begin) ) );
  }      

  template <class INPUT, class OUTPUT, class FUNCTION, class ARG> 
  inline OUTPUT select_first(INPUT begin,
                             const INPUT& end,
                             const ARG& o2, 
                             const FUNCTION& fun, 
                             OUTPUT out) {
    for (; begin != end; ++begin) 
      if (fun(**begin,o2)) {*out = *begin; ++out;}
    return out;
  }      

  // bifunctions
  template <class INPUT, class INPUT2, class FUNCT, class OPER, class OUTPUT> 
  void select(INPUT begin, 
              const INPUT& end, 
              const INPUT2& Begin2, 
              const INPUT2& end2,
              const FUNCT& fun, 
              const OPER& oper, 
              OUTPUT& out) {
    for ( ; begin != end; ++begin) {
      for (INPUT2 begin2 = Begin2;  begin2 != end2; ++begin2) {
        if (fun( (**begin), (**begin2) )) 
        {*out = oper( (**begin),(**begin2) ); ++out;}
      }
    }
  }      	
  
  template <class INPUT,  class FUNCT, class OPER, class OUTPUT> 
  void nexted_select(INPUT begin, 
                     const INPUT& end,
                     const FUNCT& fun, 
                     const OPER& oper, 
                     OUTPUT out) {
    for ( ; begin != (end-1); ++begin) {
      for (INPUT begin2 = (begin+1);  begin2 != end; ++begin2) {
        if (fun( (**begin), (**begin2) )) 
        {*out = (oper( (**begin),(**begin2) )); ++out;}
      }
    }
  }     

}; // Estd namespace


// CONTAINER
//-------------------------------

namespace ELoop {


  //--- uni loops ---------
  template <class INPUT, class FUNCTION, class OUTPUT> 
  inline  void map(const INPUT& ci, 
                   const FUNCTION& fun, 
                   OUTPUT& co) { 
    Estd::map(ci.begin(),ci.end(),fun,std::back_inserter(co));
  }

  template <class INPUT, class FUNCTION> 
  inline  void map(const INPUT& ci, 
                   const FUNCTION& fun, 
                   int key) { 
    Estd::map(ci.begin(),ci.end(),fun,key);
  }
  
  template <class INPUT, class FUNCTION, class OUTPUT> 
    void select(const INPUT& ci, 
                const FUNCTION& fun, 
                OUTPUT& co) {
    Estd::select(ci.begin(),ci.end(),fun,std::back_inserter(co));
  }
  
  //--- bi loops : reduced ---------

  template <class INPUT, class OBJECT>
  inline bool exist(const INPUT& cont, const OBJECT& obj ) {
    return Estd::exist(cont.begin(),cont.end(),obj);
  }
  

  template <class INPUT, class INPUT2, class FUNCTION, class COMPARATOR> 
  inline  void map_compare_value(INPUT& ci, 
                                 const INPUT2& ci2,
                                 const FUNCTION& fun, 
                                 const COMPARATOR& com,
                                 int key1, 
                                 int key2 = -2) {
    Estd::map_compare_value(ci.begin(),ci.end(),
                            ci2.begin(),ci2.end(),
                            fun,com,key1,key2);
  }

  template <class INPUT, class FUNCTION, class ARG> 
  inline  void map_first(INPUT& ci1, 
                         const ARG& o2, 
                         const FUNCTION& fun, 
                         int key) {
    Estd::map_first(o2,ci1.begin(),ci1.end(),fun,key);
  }      

  template <class INPUT, class FUNCTION, class ARG> 
  inline void map_second(const ARG& o1, 
                         INPUT& ci2,
                         const FUNCTION& fun, 
                         int key) {
    Estd::map_second(o1,ci2.begin(),ci2.end(),fun,key);
  }      

  template <class INPUT,  class FUNCTION, class ARG> 
  inline  void select_first(INPUT& ci, 
                            const ARG& o2, 
                            const FUNCTION& fun, 
                            INPUT& co) {
    Estd::select_first(ci.begin(),ci.end(),o2,fun,std::back_inserter(co));
  }      

  //--- bi loops ---
  
  template <class INPUT, class INPUT2, class FUNCT, class OPER, class OUTPUT> 
  inline  void select(const INPUT& ci1, 
                      const INPUT2& ci2, 
                      const FUNCT& fun,
                      const OPER& oper, 
                      OUTPUT& co) {
    Estd::select(ci1.begin(),ci1.end(),
                 ci2.begin(),ci2.end(),
                 fun,oper,
                 std::back_inserter(co));
  }      
  
  template <class INPUT, class FUNCT, class OPER, class OUTPUT> 
  inline void nexted_select(const INPUT& ci,
                            const FUNCT& fun,
                            const OPER& oper, 
                            OUTPUT& co) {
    Estd::nexted_select(ci.begin(),ci.end(),
                        fun,oper,
                        std::back_inserter(co));
  }            
  
};


#endif
