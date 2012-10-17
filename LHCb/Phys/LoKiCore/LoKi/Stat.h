// ============================================================================
#ifndef LOKI_STAT_H 
#define LOKI_STAT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatEntity.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Filters.h"
#include "LoKi/Primitives.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Algs 
  {
    // ========================================================================
    /** The trivial algorithm which accumulate the statistics 
     *  of the function over the sequence
     * 
     *  @param first    'begin'-iterator for the sequence
     *  @param last      'end'-iterator for the sequence
     *  @param functor   function to be evaluated 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-03-04
     */
    template <class OBJECT,class FUNCTOR>
    inline 
    StatEntity stat
    ( OBJECT         first     ,
      OBJECT         last      ,
      const FUNCTOR& functor   ) 
    {
      StatEntity _stat ;
      //
      for ( ; first != last ; ++first ) { _stat += functor( *first ) ; }
      //
      return _stat ;
    }
    // ========================================================================
    /** The trivial algorithm which accumulate the statistics 
     *  of the function over the sequence
     * 
     *  @param first    'begin'-iterator for the sequence
     *  @param last      'end'-iterator for the sequence
     *  @param functor   function to be evaluated 
     *  @param predicate predicate to be used 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-03-04
     */
    template <class OBJECT,class FUNCTOR,class PREDICATE>
    inline 
    StatEntity stat
    ( OBJECT           first     ,
      OBJECT           last      ,
      const FUNCTOR&   functor   , 
      const PREDICATE& predicate ) 
    {
      StatEntity _stat ;
      //
      for ( ; first != last ; ++first ) 
      { if ( predicate ( *first ) ) { _stat += functor( *first ) ; } }
      //
      return _stat ;
    }
    // ========================================================================
  } //                                              end of namespace LoKi::Algs 
  // ==========================================================================
  namespace Functors 
  {
    // ========================================================================
    /** @class Stat
     *  get the statistical quantities 
     *  @see LoKi::Algs::accumulate 
     *  @see LoKi::Functors::Sum
     *  @see StatEntity
     *  @author Vanya BELYAEV Ivan.BElyaev@cern.ch
     *  @date 2011-02-27
     */
    template <class TYPE>
    class Stat : public LoKi::Functors::Sum<TYPE>
    {
    public:
      // ======================================================================
      /// the base 
      typedef LoKi::Functor<std::vector<TYPE>,double>  uBase    ; // the base 
      /// pointer to member function 
      typedef double (StatEntity::*PMF)() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the function 
      Stat ( PMF                               pmf , 
             const LoKi::Functor<TYPE,double>& fun , 
             const std::string&                nam ) 
        : LoKi::Functors::Sum <TYPE> ( fun ) 
        , m_pmf ( pmf ) 
        , m_nam ( nam ) 
      {}
      /// constructor from the function and  predicate 
      Stat ( PMF                               pmf , 
             const LoKi::Functor<TYPE,double>& fun , 
             const LoKi::Functor<TYPE,bool>&   cut ,
             const std::string&                nam ) 
        : LoKi::Functors::Sum <TYPE> ( fun , cut ) 
        , m_pmf ( pmf ) 
        , m_nam ( nam ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Stat () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Stat* clone() const { return new Stat ( *this ) ; }
      /// MANDATORY: the only one essential method:
      virtual typename uBase::result_type operator() 
        ( typename uBase::argument a ) const
      {
        const LoKi::Apply<TYPE,double> appFun ( &this->m_fun.func() ) ;
        const LoKi::Apply<TYPE,bool>   appCut ( &this->m_cut.func() ) ;
        //
        StatEntity _stat = this->m_trivCut ? 
          LoKi::Algs::stat
          ( a.begin ()   , 
            a.end   ()   , 
            appFun       ) : 
          LoKi::Algs::stat
          ( a.begin ()   , 
            a.end   ()   ,
            appFun       ,
            appCut       ) ;
        return (_stat.*m_pmf)() ;        
      }
      /// OPTIONAL: the basic printout method 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return this -> _print_      ( s , this->m_nam , 0 ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Stat () ;                              // default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================
      /// member function 
      PMF m_pmf ;                                            // member function 
      /// the function name 
      std::string m_nam ;                                  // the function name 
      // ======================================================================      
    };
    // ========================================================================
  } //                                          end of namespace LoKi::Functors 
  // ==========================================================================
  /** get the mean value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::mean
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  mean ( const LoKi::Functor<TYPE,double>& fun ) 
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::mean , 
                                       fun               , 
                                       "mean"            ) ; }
  // ==========================================================================
  /** get the mean value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::mean
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  mean ( const LoKi::Functor<TYPE,double>& fun , 
         const LoKi::Functor<TYPE,bool>&   cut )
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::mean , 
                                       fun               , 
                                       cut               , 
                                       "mean"            ) ; }
  // ==========================================================================
  /** get the rms value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity:rms
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  rms ( const LoKi::Functor<TYPE,double>& fun ) 
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::rms  , 
                                       fun               , 
                                       "rms"             ) ; }
  // ==========================================================================
  /** get the rms value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::rms
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  rms ( const LoKi::Functor<TYPE,double>& fun , 
        const LoKi::Functor<TYPE,bool>&   cut )
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::rms  , 
                                       fun               , 
                                       cut               , 
                                       "rms"             ) ; }
  // ==========================================================================
  /** get the meanErr value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::meanErr
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  meanErr ( const LoKi::Functor<TYPE,double>& fun ) 
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::meanErr , 
                                       fun                  , 
                                       "meanErr"            ) ; }
  // ==========================================================================
  /** get the mean value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::meanErr
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  meanErr ( const LoKi::Functor<TYPE,double>& fun , 
            const LoKi::Functor<TYPE,bool>&   cut )
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::meanErr , 
                                       fun                  , 
                                       cut                  , 
                                       "meanErr"            ) ; }
  // ==========================================================================
  /** get the 'efficiency' value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::eff
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  eff ( const LoKi::Functor<TYPE,bool>& fun ) 
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::eff , 
                                       LoKi::SimpleSwitch<TYPE> ( fun , 1 , 0 ) , 
                                       "eff"            ) ; }
  // ==========================================================================
  /** get the mean value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::eff
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  eff ( const LoKi::Functor<TYPE,bool>& fun , 
        const LoKi::Functor<TYPE,bool>& cut )
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::eff     , 
                                       LoKi::SimpleSwitch<TYPE> ( fun , 1 , 0 ) , 
                                       cut                  , 
                                       "eff"                ) ; }
  // ==========================================================================
  /** get the 'efficiency' value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::effErr
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  effErr ( const LoKi::Functor<TYPE,bool>& fun ) 
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::effErr , 
                                       LoKi::SimpleSwitch<TYPE> ( fun , 1 , 0 ) , 
                                       "effErr"            ) ; }
  // ==========================================================================
  /** get the mean value for some functor 
   *  @see LoKi::Functors::Stat
   *  @see StatEntity
   *  @see StatEntity::effErr
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-03-04
   */
  template <class TYPE>
  inline LoKi::Functors::Stat<TYPE>
  effErr( const LoKi::Functor<TYPE,bool>& fun , 
          const LoKi::Functor<TYPE,bool>& cut )
  { return LoKi::Functors::Stat<TYPE>( &StatEntity::effErr , 
                                       LoKi::SimpleSwitch<TYPE> ( fun , 1 , 0 ) , 
                                       cut                  , 
                                       "effErr"             ) ; }
  // ==========================================================================
} //                                                      end of namesapce LoKi 
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_STAT_H
// ============================================================================
