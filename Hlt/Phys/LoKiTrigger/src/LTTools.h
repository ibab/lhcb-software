// $Id: LTTools.h,v 1.2 2008-11-14 12:56:03 ibelyaev Exp $
// ============================================================================
#ifndef LTTOOLS_H 
#define LTTOOLS_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IANNSvc.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltSelection.h"
#include "HltBase/IHltDataSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHltUnit.h"
// ============================================================================
// forward decalrations 
// ============================================================================
class GaudiAlgorithm ;
namespace LoKi { class AuxFunBase ; }
// ============================================================================
namespace LoKi
{
  // =========================================================================
  namespace Hlt1
  {
    // ========================================================================
    namespace Utils
    {
      // ======================================================================
      /// get gaudi algorithm from the context 
      GaudiAlgorithm* getGaudiAlg ( const LoKi::AuxFunBase& base ) ;
      // ======================================================================
      /// get the algorithm from context service 
      IAlgorithm*     getAlg      ( const LoKi::AuxFunBase& base ) ;
      // ======================================================================
      /// get the "unit" from the algorithm 
      inline SmartIF<LoKi::IHltUnit> getUnit ( const LoKi::AuxFunBase& base )
      { return SmartIF<LoKi::IHltUnit>( getAlg ( base )  ) ; }
      // ======================================================================
      /// get ANN-Service 
      inline SmartIF<IANNSvc> annSvc( const LoKi::AuxFunBase& base ) 
      {
        IInterface* iif = getUnit( base ) ;
        SmartIF<IANNSvc> ann ( iif ) ;
        base.Assert ( !(!iif) , "IANNSvc* points to NULL" ) ;
        return ann ;
      }
      // ======================================================================
      /// get Hlt Data Service 
      inline SmartIF<IHltDataSvc> hltSvc ( const LoKi::AuxFunBase& base ) 
      {
        IInterface* iif = getUnit( base ) ;
        SmartIF<IHltDataSvc> ann ( iif ) ;
        base.Assert ( !(!iif) , "IHltDataSvc* points to NULL" ) ;
        return ann ;
      }
      // ======================================================================
      /// get the selection by key 
      Hlt::Selection* getSelection
      ( const stringKey&        key  ,
        const LoKi::AuxFunBase& base ) ;
      // ======================================================================
      /// cast the seelction
      template <class TYPE>
      inline Hlt::TSelection<TYPE>* cast ( Hlt::Selection* sel ) 
      {
        if ( 0 == sel ) { return 0 ; }
        return dynamic_cast<Hlt::TSelection<TYPE>*>( sel ) ;
      } 
      // ======================================================================
      /// cast the seelction
      template <class TYPE>
      inline const Hlt::TSelection<TYPE>* cast ( const Hlt::Selection* sel ) 
      {
        if ( 0 == sel ) { return 0 ; }
        return cast<TYPE>( const_cast<Hlt::Selection*>( sel ) ) ;
      }  
      // ======================================================================
      /// get the concrete selection by key 
      template <class TYPE>
      Hlt::TSelection<TYPE>* getTSelection
      ( const stringKey&        key  ,
        const LoKi::AuxFunBase& base )
      {
        Hlt::Selection* sel = getSelection ( key , base ) ;
        base.Assert ( 0 !=  sel , "Hlt::Selection  points to NULL!" ) ;
        typedef Hlt::TSelection<TYPE> TSel ;
        TSel* tsel = cast<TYPE> ( sel ) ;
        base.Assert ( 0 != tsel , "Hlt::TSelection points to NULL!" ) ;    
        return tsel ;
      }
      // ======================================================================
      /** @struct CmpTrack
       *  comparison criteria to eliminate double count 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-11-14
       */
      struct CmpTrack 
        : public std::binary_function<const LHCb::Track*,const LHCb::Track*,bool>
      {
        // ====================================================================
        /// compare tracks 
        inline bool operator() 
          ( const LHCb::Track* trk1 , 
            const LHCb::Track* trk2 ) const 
        {
          return 
            trk1 == trk2 ? false :
            0    == trk1 ? true  :
            0    == trk2 ? false : compare ( trk1 , trk2 ) ;
        }
        // ====================================================================
      private:
        // ====================================================================
        /// compare valid pointers 
        bool compare
        ( const LHCb::Track* trk1 , 
          const LHCb::Track* trk2 ) const ;
        // ====================================================================
      };
      // ======================================================================
    } // end of namespace LoKi::Hlt1::Utils
    // ========================================================================
  } // end of namespace LoKi::Hlt1
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The end 
// ============================================================================
#endif // LTTOOLS_H
// ============================================================================
