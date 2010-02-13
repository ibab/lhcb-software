// $Id: TES.h,v 1.1 2010-02-13 16:31:06 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_TES_H 
#define LOKI_TES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
// Forward decalations 
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace TES
  {
    // ========================================================================
    /** @class Exists 
     *  Simple checker of existence of object in TES 
     *  @see LoKi::Cuts::EXISTS 
     *  @see GaudiCommon<TYPE>::exists 
     *  The concept belong to Gerhard The Great Raven 
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl
     *  @date 2010-02-13
     */
    class Exists : public LoKi::Functor<void,bool>
    {
    public:
      // ======================================================================
      /** constructor from TES location & "rootInTes"-flag 
       *  @see GaudiCommon<TYPE>::exists 
       */
      Exists ( const std::string& location            , 
               const bool         useRootInTes = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Exists() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Exists* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( /* argument v */ ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get TES-location
      const  std::string& location     () const { return m_location     ; }
      inline bool         useRootInTes () const { return m_useRootInTes ; }
      /// get algorithm name 
      const std::string& algName  () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Exists() ;                        // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// TES location to be tested 
      std::string                     m_location ; // TES location to be tested 
      /// Use 'RootInTes' ?
      bool                            m_useRootInTes ; //     Use 'RootInTes' ?
      /// the algorithm to be used 
      mutable LoKi::Interface<GaudiAlgorithm> m_algorithm ;  // the algorithm 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Contains
     *  Simple checker of existence & size of Container  in TES 
     *  @see LoKi::Cuts::CONTAINS
     *  @see GaudiCommon<TYPE>::exists 
     *  @see GaudiCommon<TYPE>::get
     *  @see ObjectContainerBase 
     *  @see ObjectContainerBase::numberOfOjbects
     *  The concept belong to Gerhard The Great Raven 
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl
     *  @date 2010-02-13
     */
    class Contains : public LoKi::Functor<void,double>
    {
    public:
      // ======================================================================
      /** constructor from TES location & "rootInTes"-flag 
       *  @see GaudiCommon<TYPE>::exists 
       *  @see GaudiCommon<TYPE>::get 
       */
      Contains ( const std::string& location            , 
                 const bool         useRootInTes = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Contains () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Contains* clone() const ;
      /** MANDATORY: the only one essential method 
       *  @return numebr of element in continer, -1 for non-existing container
       */
      virtual  result_type operator() ( /* argument v */ ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get TES-location
      const  std::string& location     () const { return m_location     ; }
      inline bool         useRootInTes () const { return m_useRootInTes ; }
      /// get algorithm name 
      const std::string& algName  () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Contains () ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// TES location to be tested 
      std::string                     m_location ; // TES location to be tested 
      /// Use 'RootInTes' ?
      bool                            m_useRootInTes ; //     Use 'RootInTes' ?
      /// the algorithm to be used 
      mutable LoKi::Interface<GaudiAlgorithm> m_algorithm ; // the algorithm
      // ======================================================================
    } ;
    // ========================================================================
  } //                                          end of namespace LoKi::Functors 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef CONTAINS
     *  Trivial checker/function for existence &size of Container in TES 
     *  @see LoKi::TES::Contains
     *  @see GaudiCommon<TYPE>::exists 
     *  @see GaudiCommon<TYPE>::get 
     *  @see ObjectContainerBase 
     *  @see ObjectContainerBase::numberOfOjbects
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl
     *  @date 2010-02-13
     */
    typedef LoKi::TES::Contains                                      CONTAINS ;
    // ========================================================================
    /** @typedef EXISTS 
     *  Trivial checker/predicate for existence of object in TES 
     *  @see LoKi::Cuts::EXISTS 
     *  @see GaudiCommon<TYPE>::exists 
     *  The concept belong to Gerhard The Great Raven 
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl
     *  @date 2010-02-13
     */
    typedef LoKi::TES::Exists                                          EXISTS ;
    // ========================================================================
  } 
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TES_H
// ============================================================================
