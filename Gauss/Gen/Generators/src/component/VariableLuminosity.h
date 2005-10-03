// $Id: VariableLuminosity.h,v 1.1 2005-10-03 10:17:36 robbep Exp $
#ifndef GENERATORS_VARIABLELUMINOSITY_H 
#define GENERATORS_VARIABLELUMINOSITY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IPileUpTool.h"

class IRndmGenSvc ;

/** @class VariableLuminosity VariableLuminosity.h 
 *  
 *  Utility tool to compute variable number of pile up events
 *  depending on beam parameters
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */
class VariableLuminosity : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  VariableLuminosity( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~VariableLuminosity( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /// Finalize method
  virtual StatusCode finalize( ) ;

  virtual unsigned int numberOfPileUp( double & currentLuminosity ) ;

  virtual void printPileUpCounters( ) ;

protected:

private:
  double m_luminosity ;
  double m_fillDuration ;
  double m_beamDecayTime ;
  double m_crossingRate ;
  double m_totalXSection ;

  double m_peakLuminosity ;
  int    m_numberOfZeroInteraction ;
  int    m_nEvents ;

  IRndmGenSvc * m_randSvc ;
  Rndm::Numbers m_flatGenerator ;
};
#endif // GENERATORS_VARIABLELUMINOSITY_H
