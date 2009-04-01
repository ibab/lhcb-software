// $Id: HltBindFilters.h,v 1.1 2009-04-01 12:36:09 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLTBINDFILTERS_H 
#define LOKI_HLTBINDFILTERS_H 1
// ============================================================================
// Include files
// ============================================================================

namespace LoKi 
{
  // ==========================================================================
  namespace HltBinders 
  {
    // ========================================================================
    /** the binding algorithm
     *
     *  - BindMin     : bind using std::less 
     *  - BindAbsMin  : bind using LHCb::Math::abs_less
     *  - BindMax     : bind using std::greater 
     *  - BindAbsMin : bind using LHCb::Math::abs_greater
     *
     *  @see LHCb::Math::abs_less 
     *  @see LHCb::Math::abs_greater 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-17
     */
    enum Alg {
      BindMin    ,  // bind using std::less 
      BindAbsMin ,  // bind using LHCb::Math::abs_less
      BindMax    ,  // bind using std::greater 
      BindAbsMin    // bind using LHCb::Math::abs_greater
    };
    // ========================================================================
    /** @class BindFilter LoKi/HltBindFilters.h
     *  
     *
     *  @author Vanya BELYAEV
     *  @date   2009-04-01
     */
    // ========================================================================
    template <class TYPE,class TYPE2=double, class BINOP>
    class BindFilter : public LoKi::BasicFunctors<TYPE*>::Pipe 
    {
    private:
      // ======================================================================
      typedef typename LoKi::BasicFunctors<TYPE*>                     Source_ ;
      typedef typename LoKi::Assignable<Source_>::Type                Source2 ;
      typedef typename LoKi::Holder<TYPE,TYPE>                        Holder_ ;
      typedef typename LoKi::Functor<LoKi::Holder<TYPE,TYPE>,TYPE2>   Fun_    ;
      typedef typename LoKi::Assignable<Fun_>::Type                   Fun2    ;
      typedef typename LoKi::BasicFunctors<TYPE2>::Predicate          Cut_    ;
      typedef typename LoKi::Assignable<Cut_>::Type                   Cut2    ;      
      // ======================================================================
    private:
      // ======================================================================
      typedef typename LoKi::BasicFunctors<TYPE*>::Pipe::result_type      Res ;
      typedef typename LoKi::BasicFunctors<TYPE*>::Pipe::result_type      Arg ;
      typedef BinderValue<TYPE,TYPE,TYPE2,BINOP >                      Binder ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor 
       *  @param  fun2    the function of two (scalar) arguments 
       *  @param  source  the source of the second sequence 
       *  @param  cut     the criteria/decision 
       *  @param  input   filter input or source? 
       */ 
      BindFilter
      ( const Fun_&    fun2   , // the function 
        const Source_& source , // the source 
        const Cut_&    cut    , // the criteria/decision 
        const bool     input  ) // filter input or source?
        : public LoKi::BasicFunctors<TYPE*>::Pipe ()
        //
        , m_source ( source )
        , m_binder ( fun2   , 
                     ( const typename Binder::Binder::Container*) 0 , 
                     "_binder_" , ",internal" ) 
        , m_cut2   ( cut    )
        , m_first  ( first  )  
        //
      {}
      /// MANDATORY: virtual destrcutor 
      virtual ~BindFilter() {}
      /// MANDATORY: clone mehtod ("virtual constructor")
      virtual  BindFilter* clone() const { return new BindFilter(*this) ; }
      /// MANDATORY: the only one essential method
      virtual typename LoKi::BasicFunctors<TYPE*>::Pipe::result_type 
      operator() 
        ( typename LoKi::BasicFunctors<TYPE*>::Pipe::argument a1 ) const ;
      {
        // re-set the binded container 
        m_binder.binder().setContainer ( 0 ) ;
        
        /// nothing to be filtered? 
        if (  m_input && a1.empty() ) { return a1 ; } // RETURN
        
        /// use the source to get the second data:
        typename Source_::result_type a2 = m_source() ;
        
        // nothing to be filtered?
        if ( !m_input && a2.empty() ) { return a2 ; } // RETURN
        
        Res result ;        
        
        const Arg* first  = &a1 ;
        const Arg* second = &a2 ;
        
        if ( !m_input ) 
        {
          first  = &a2 ;
          second = &a1 ;
        }
        
        // nothing to be filtered? 
        if ( first  -> empty () ) { return result ; }                 // RETURN 
        
        // "binded" container is empty
        if ( second -> empty () ) 
        {
          // the exceptional value  
          static const TYPE2 s_value = BinOps<BINOP>::value() ;
          // test the exceptional value:
          return m_cut ( s_value ) ? *first : result ;                 // RETURN 
        }
        
        // set the binded container 
        m_binder.binder().setContainer ( second ) ;
        
        // the loop over the input objects 
        for ( typename Arg::const_iterator iarg1 = first->begin() ; 
              first->end() != iarg1 ; ++iarg ) 
        {
          // for the given input, construct the actual binder:
          if ( !*iarg1 ) { continue ; } // skip NULLS          
          LoKi::Apply<TYPE,TYPE2> applicator ( m_binder ) ;
          TYPE2 res = applicator ( *iarg1 ) ;
          // "good" object? add it to the output!
          if ( m_cut ( res ) ) { result.push_back ( *iarg1 ) ; }
        }

        // re-set the binded container 
        m_binder.binder().setContainer ( 0 ) ;
        
        return result ;                                               // RETURN
      }
      // =======================================================================
    public:
      // =======================================================================
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << "bind" << BinOps<BINOP>::name() << "(" 
                 << this -> m_binder.... << ","    FIX ME HERE!!!!
                 << this -> m_source << "," 
                 << this -> m_cut2   << "," 
                 << this -> m_input ? "True" << "False" << ")" ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      BindFilter() ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the source
      Source2 m_source ; // the source
      /// the consructe binder 
      BinderValue<TYPE,TYPE,TYPE2,BINOP > m_binder ;       // the actual binder 
      /// critera, decision 
      Cut2    m_cut2   ; // critera, decision 
      /// filter input or source? 
      bool    m_input  ; // filter input or source? 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::HltBinders 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_HLTBINDFILTERS_H
// ============================================================================
