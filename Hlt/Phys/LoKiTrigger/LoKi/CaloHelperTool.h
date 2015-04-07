// $Id: CaloHelperTool.h 184094 2015-02-19 16:34:28Z mkenzie $
// ============================================================================
#ifndef LOKI_CALOHELPERTOOL_H
#define LOKI_CALOHELPERTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/ObjectVector.h"
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/L0DUBase.h"
#include "Event/L0CaloCandidate.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
#include "CaloUtils/CaloParticle.h"
#include "CaloDAQ/ICaloDataProvider.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/Listener.h"
#include "LoKi/HelperTool.h"
// ============================================================================
class GaudiAlgorithm ;
// ============================================================================
/** @file
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *   @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2010-10-30
 *
 *  @author Albert Puig (albert.puig@epfl.ch)
 *  @date   2015-03-31
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class CaloHelperTool
     *  Helper class for implementation of calo tools
     *  @author Albert Puig (albert.puig@epfl.ch)
     *  @date   2015-03-31
     */
    class GAUDI_API CaloHelperTool : public LoKi::Hlt1::HelperTool
    {
    public:
      // ======================================================================
      /// constructor with the fake argument
      CaloHelperTool ( const int dummy ) ;
      // ======================================================================
      /// non-trivial copy constructor
      CaloHelperTool ( const CaloHelperTool& right ) ;
      // ======================================================================
      /// virtual destructor
      virtual ~CaloHelperTool() ;                             // virtual destructor
      // ======================================================================
    private:
      // ======================================================================
      /// common setup
      StatusCode setup() ;
      // ======================================================================
      /// the default constructor is disabled
      CaloHelperTool () ;                    // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /** handle the incidents:
       *  clear the involved pointers
       */
      virtual void handle ( const Incident& /* incident */ ) ;
      // ======================================================================
    protected:
      // ======================================================================
      /// get new CaloCluster
      inline LHCb::CaloCluster*  newCluster     () const
      {
        if ( !m_calo_clusters ) { m_calo_clusters = _createClusters() ; }
        return _create<LHCb::CaloCluster> ( m_calo_clusters ) ;
      }
      // ======================================================================
      /// get new CaloDigit
      inline LHCb::CaloDigit*  newDigit  (const LHCb::CaloCellID& id) const
      {
        //return new LHCb::CaloDigit(id, 0.0) ;
        if ( !m_calo_digits ) { m_calo_digits = _createDigits() ; }
        LHCb::CaloDigit* _new = m_calo_digits->object(id) ;
        if ( !_new )
        {
            _new = new LHCb::CaloDigit(id, 0.0) ;
            _add_ ( _new , m_calo_digits ) ;
        }
        return _new ;
      }
      // ======================================================================
      /// get new CaloDigit
      inline LHCb::CaloHypo*  newHypo     () const
      {
        if ( !m_calo_hypos ) { m_calo_hypos = _createHypos() ; }
        return _create<LHCb::CaloHypo> ( m_calo_hypos ) ;
      }
      // ======================================================================
      /// get new CaloPosition
      // inline LHCb::CaloPosition*  newCaloPosition     () const
      // {
        // if ( !m_calo_positions ) { m_calo_positions = _createCaloPositions() ; }
        // return _create<LHCb::CaloPosition> ( m_calo_positions ) ;
      // }
      // ======================================================================
      LHCb::Particle* caloCandidateToParticle( const LHCb::L0CaloCandidate* cand,
                                               LHCb::ParticleID pid ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// get the storage for objects
      LHCb::CaloCluster::Container*  _createClusters      () const ;
      //LHCb::CaloDigit::Container*    _createDigits        () const ;
      LHCb::CaloDigits*    _createDigits        () const ;
      LHCb::CaloHypo::Container*     _createHypos         () const ;

      // ======================================================================
    private:
      // ======================================================================
      /// Calo parameters
      const double m_z ;
    protected:
      ICaloDataProvider*  m_calo ;
      /// container of Hlt-candidates
      mutable LHCb::CaloCluster::Container*                   m_calo_clusters  ;
      //mutable LHCb::CaloDigit::Container*                     m_calo_digits  ;
      mutable LHCb::CaloDigits*                     m_calo_digits  ;
      mutable LHCb::CaloHypo::Container*                      m_calo_hypos  ;
      // mutable CaloPositions*     m_calo_positions  ;
      // ======================================================================
      mutable std::string                 m_myname          ;
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
} // ==========================================================================
//                                                        end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_CALOHELPERTOOL_H
// ============================================================================
