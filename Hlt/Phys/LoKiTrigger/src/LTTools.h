// $Id$
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
// ============================================================================
// LoKi
// ============================================================================
#include "HltBase/IHltUnit.h"
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/AlgUtils.h"
// ============================================================================
// forward declarations 
// ============================================================================
class GaudiAlgorithm ;
namespace LoKi { class AuxFunBase ; }
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    namespace Utils
    {
      // ======================================================================
      /// get gaudi algorithm from the context 
      inline GaudiAlgorithm* getGaudiAlg ( const LoKi::AuxFunBase& base ) 
      { return LoKi::AlgUtils::getGaudiAlg ( base ,true ) ; }
      // ======================================================================
      // get the algorithm from context service 
      // ======================================================================
      inline IAlgorithm*     getAlg ( const LoKi::AuxFunBase& base ) 
      { return LoKi::AlgUtils::getAlg ( base , true ) ; }
      // ======================================================================
      inline SmartIF<Hlt::IUnit> getUnit ( const LoKi::AuxFunBase& base )
      { return SmartIF<Hlt::IUnit>( getAlg ( base )  ) ; }
      /// get ANN-Service 
      inline SmartIF<IANNSvc> annSvc( const LoKi::AuxFunBase& base ) 
      {
        IInterface* iif = getUnit( base ) ;
        SmartIF<IANNSvc> ann ( iif ) ;
        base.Assert ( !(!ann) , "IANNSvc* points to NULL" ) ;
        return ann ;
      }
      // ======================================================================
      /// get Hlt Data Service 
      inline SmartIF<Hlt::IData> hltSvc ( const LoKi::AuxFunBase& base ) 
      {
        IInterface* iif = getUnit( base ) ;
        SmartIF<Hlt::IData> hlt ( iif ) ;
        base.Assert ( !(!hlt) , "Hlt::IData* points to NULL" ) ;
        return hlt ;
      }
      /// get Hlt Register Service 
      inline SmartIF<Hlt::IRegister> regSvc ( const LoKi::AuxFunBase& base ) 
      {
        IInterface* iif = getUnit( base ) ;
        SmartIF<Hlt::IRegister> reg ( iif ) ;
        base.Assert ( !(!reg) , "Hlt::IRegister* points to NULL" ) ;
        return reg ;
      }
      // ======================================================================
      /// get the selection by key 
      const Hlt::Selection* getSelection
      ( const Gaudi::StringKey& key  ,
        const LoKi::AuxFunBase& base ) ;
      // ======================================================================
      template <class TYPE>
      inline Hlt::TSelection<TYPE>* cast ( Hlt::Selection* sel ) 
      {
        if ( 0 == sel ) { return 0 ; }
        return dynamic_cast<Hlt::TSelection<TYPE>*>( sel ) ;
      } 
      // ======================================================================
      /// cast the selection
      template <class TYPE>
      inline const Hlt::TSelection<TYPE>* cast ( const Hlt::Selection* sel ) 
      {
        if ( 0 == sel ) { return 0 ; }
        return dynamic_cast<const Hlt::TSelection<TYPE>*>( sel ) ;
      }  
      // ======================================================================
      /// get the concrete selection by key 
      template <class TYPE>
      const Hlt::TSelection<TYPE>* getTSelection
      ( const Gaudi::StringKey& key  ,
        const LoKi::AuxFunBase& base )
      {
        const Hlt::Selection* sel = getSelection ( key , base ) ;
        base.Assert ( 0 !=  sel , "Hlt::Selection  points to NULL!" ) ;
        typedef Hlt::TSelection<TYPE> TSel ;
        const TSel* tsel = cast<TYPE> ( sel ) ;
        base.Assert ( 0 != tsel , "Hlt::TSelection points to NULL!" ) ;    
        return tsel ;
      }
      // ======================================================================
      /** @struct CmpTrack
       *  comparison criteria to eliminate the double count 
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
        /// compare the valid pointers 
        bool compare
        ( const LHCb::Track* trk1 , 
          const LHCb::Track* trk2 ) const ;
        // ====================================================================
      };
      // ======================================================================
    } //                                     end of namespace LoKi::Hlt1::Utils
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The end 
// ============================================================================
#endif // LTTOOLS_H
// ============================================================================
