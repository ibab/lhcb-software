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
 *  The productiuon tool tfor Onia-pairs 
 *
 *  @author Alexey NONOSELOV Alexey.Novoselov@cern.ch
 *
 *                     $Revision$
 *  Last modiffication $Date$
 *                  by $Author$
 */
class OniaPairsProduction : public PythiaProduction 
{
public:
  // ==========================================================================
  /** standard constructor 
   *  @param type   tool type (?)
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
public:
  // ==========================================================================
  /// initilize the production tool 
  virtual StatusCode initialize( ) ;
  /// finalize
  virtual StatusCode finalize( ) ;
  // ==========================================================================  
public:
  // ==========================================================================  
  /// generate the event
  virtual StatusCode generateEvent
  ( HepMC::GenEvent*    theEvent     , 
      LHCb::GenCollision* theCollision ) ;
  // ==========================================================================
private:  
  // ==========================================================================  
  /// my Ecm
  double m_ecm ;
  double m_psi1S1S, m_psi1S2S, m_psi2S2S ;
  double m_ups1S1S, m_ups1S2S, m_ups1S3S, m_ups2S2S, m_ups2S3S, m_ups3S3S ; 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // LBONIAPAIRS_PRODUCTION_H 
// ============================================================================


