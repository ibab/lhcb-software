// $Id: $
// ============================================================================
#ifndef LBONIAPAIRS_PRODUCTION_H 
#define LBONIAPAIRS_PRODUCTION_H 1
// ============================================================================
// Include files
// PythiaProduction
#include "LbPythia/PythiaProduction.h"
// ============================================================================
/** @class OniaPairsProduction
 *
 *  The productiuon tool for Onia-pairs 
 *
 *  @author Alexey NOVOSELOV Alexey.Novoselov@cern.ch
 *
 *                    $Revision$
 *  Last modification mid/10/2013
 *                 by alexn
 */
class OniaPairsProduction : public PythiaProduction 
{
 public:
  // ==========================================================================
  /** standard constructor 
   *  @param type   tool type 
   *  @param name   tool name 
   *  @param parent parent component 
   */
  OniaPairsProduction 
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent ) ;
  /// virtual  desctructor 
  virtual ~OniaPairsProduction( ) { ; }
  // ==========================================================================
  /// initilize the production tool 
  virtual StatusCode initialize( ) ;
  /// finalize
  virtual StatusCode finalize( ) ;
  // ==========================================================================
  /// generate the event
  virtual StatusCode generateEvent
    ( HepMC::GenEvent*    theEvent     , 
      LHCb::GenCollision* theCollision ) ;
  // ==========================================================================
 protected:
  // ==========================================================================
 private:  
  // ==========================================================================
  double m_ecm ;
  double m_psi1S1S, m_psi1S2S, m_psi2S2S ;
  double m_ups1S1S, m_ups1S2S, m_ups1S3S, m_ups2S2S, m_ups2S3S, m_ups3S3S ; 
  double m_ScfAlpS, m_ScfPDF, m_ScfShwr, m_MaxWghtMult ;
  CommandVector m_PyDefComm, m_PyCommVec ;

  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // LBONIAPAIRS_PRODUCTION_H 
// ============================================================================
