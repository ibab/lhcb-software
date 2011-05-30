// $Id: $
// ============================================================================
#ifndef LBONIAPAIRS_PRODUCTION_H 
#define LBONIAPAIRS_PRODUCTION_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// LbPythia 
// ============================================================================
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"
// ============================================================================
// Local 
// ============================================================================
#include "LbOniaPairs/OniaPairs.h"
// ============================================================================
/** @class OniaPairsProduction
 *  The main production tool for Onia-Pairs production
 *
 *  @author Alexey Nososelov Alexey.Novoselov@cern.ch
 *  @date 2011-05-30
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
class OniaPairsProduction : public PythiaProduction 
{
  // ==========================================================================
  friend class ToolFactory<OniaPairsProduction>;
  // ==========================================================================  
public:
  // ==========================================================================
  /// initilize the production tool 
  virtual StatusCode initialize( ) ;  
  /// finalize 
  virtual StatusCode finalize( ) ;
  /// 
  /// generate the event
  virtual StatusCode generateEvent
  ( HepMC::GenEvent*    theEvent     , 
    LHCb::GenCollision* theCollision ) ;
  // ==========================================================================
protected:
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
  /// virtual and protected desctructor 
  virtual ~OniaPairsProduction( ) { ; }
  // ==========================================================================
private:
  // ==========================================================================
  /// default constructor is disabled 
  OniaPairsProduction () ; // default constructor is disabled 
  /// copy constructor is disabled 
  OniaPairsProduction ( const OniaPairsProduction& ) ; // no copy
  /// assignement operator is disabled 
  OniaPairsProduction& operator=( const OniaPairsProduction& );
  // ==========================================================================
private:
  // ==========================================================================
  /// my Ecm
  double m_ecm ;                                                     // my Ecm
  // ==========================================================================
} ;
// ============================================================================
// The END 
// ============================================================================
#endif // LBONIAPAIRS_PRODUCTION_H 
// ============================================================================

