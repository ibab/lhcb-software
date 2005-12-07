// $Id: FixedLuminosity.h,v 1.1 2005-12-07 23:03:55 robbep Exp $
#ifndef GENERATORS_FIXEDLUMINOSITY_H 
#define GENERATORS_FIXEDLUMINOSITY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IPileUpTool.h"

class IRndmGenSvc ;

/** @class FixedLuminosity FixedLuminosity.h 
 *  
 *  Utility tool to compute variable number of pile up events
 *  depending on beam parameters
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */
class FixedLuminosity : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  FixedLuminosity( const std::string& type, const std::string& name,
                   const IInterface* parent) ;

  virtual ~FixedLuminosity( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /// Finalize method
  virtual StatusCode finalize( ) ;

  virtual unsigned int numberOfPileUp( double & currentLuminosity ) ;

  virtual void printPileUpCounters( ) ;

protected:

private:
  double m_luminosity ;
  double m_crossingRate ;
  double m_totalXSection ;

  int    m_numberOfZeroInteraction ;
  int    m_nEvents ;

  IRndmGenSvc * m_randSvc ;
};
#endif // GENERATORS_FIXEDLUMINOSITY_H
