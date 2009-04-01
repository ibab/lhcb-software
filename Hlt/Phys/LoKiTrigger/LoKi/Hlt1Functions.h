// $Id: Hlt1Functions.h,v 1.2 2009-04-01 12:36:09 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLT1FUNCTIONS_H 
#define LOKI_HLT1FUNCTIONS_H 1
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackTypes.h"
#include "LoKi/Interface.h"
// ============================================================================
/** @file LoKi/Hlt1Functions.h
 *  collection of specific functions for Hl1 implementation
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-30
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Tracks 
  {
    // ========================================================================
    /** @class MuonDeltaP 
     *  Simple class, LoKi-image of MuonDeltaPTool by Leandro de Paula.
     *  @see LoKi::Cuts::TrDPMU
     *  @see LoKi::Cuts::TrDPMU_
     *  @see HltDeltaPTool 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    class MuonDeltaP : public LoKi::TrackTypes::TrCuts
    {
    public:
      // ======================================================================
      /// constructor from delta-cut and list of p-cuts
      MuonDeltaP
      ( const double               delta , 
        const std::vector<double>& cuts  ) ;
      /// constructor from delta-cut 
      MuonDeltaP
      ( const double               delta ) ;
      /// constructor from list of p-cuts 
      MuonDeltaP
      ( const std::vector<double>& cuts  ) ;
      /// default constructor 
      MuonDeltaP () ;
      /// MANDATORY: virtual destructor
      virtual ~MuonDeltaP() {}
      /// MANDATOTY: clone method ("virtual constructor")
      virtual  MuonDeltaP* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: the nice printput 
      virtual std::ostream& fillStream  ( std::ostream& s ) const ;
      /// =====================================================================
    private:
      // ======================================================================
      /// the cut on deltaP
      double m_deltaP ;                                    // the cut on deltaP
      /// the list of p-cuts for various regions 
      std::vector<double> m_pCuts ;   // the list of p-cuts for various regions 
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Tracks
  // ==========================================================================
} // end of namespace LoKi/
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef TrDPMU_ 
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU
     *  @see LoKi::Cuts::TrDPMUON
     *  @see LoKi::Cuts::TrDSELTAPMU
     *  @see LoKi::Cuts::TrDSELTAPMUON
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    typedef LoKi::Tracks::MuonDeltaP                                  TrDPMU_ ;
    // ========================================================================
    /** @var TrDPMU 
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    const LoKi::Tracks::MuonDeltaP                                     TrDPMU ;
    // ========================================================================
    /** @var TrDPMUON 
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    const LoKi::Tracks::MuonDeltaP                                   TrDPMUON ;
    // ========================================================================
    /** @var TrDELTAPMU 
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    const LoKi::Tracks::MuonDeltaP                                 TrDELTAPMU ;
    // ========================================================================
    /** @var TrDELTAPMUON
     *  Simple functor to cut on "delta-P" for muons.
     *  Essentially it is LoKified verison of HltDeltaPTool by Leando de Paula
     *  @see HltDeltaPTool
     *  @see LoKi::Tracks::MuonDeltaP
     *  @see LoKi::Cuts::TrDPMU_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-30
     */
    const LoKi::Tracks::MuonDeltaP                               TrDELTAPMUON ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLT1FUNCTIONS_H
// ============================================================================

