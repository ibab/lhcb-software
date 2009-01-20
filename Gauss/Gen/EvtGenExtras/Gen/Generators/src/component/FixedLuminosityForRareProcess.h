// $Id: FixedLuminosityForRareProcess.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
#ifndef GENERATORS_FIXEDLUMINOSITYFORRAREPROCESS_H 
#define GENERATORS_FIXEDLUMINOSITYFORRAREPROCESS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IPileUpTool.h"

// forward declaration
class IRndmGenSvc ;

/** @class FixedLuminosityForRareProcess FixedLuminosityForRareProcess.h "FixedLuminosityForRareProcess.h"
 *  
 *  Tool to compute variable number of pile up events
 *  depending on beam parameters
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */
class FixedLuminosityForRareProcess : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  FixedLuminosityForRareProcess( const std::string& type, const std::string& name,
                   const IInterface* parent) ;

  virtual ~FixedLuminosityForRareProcess( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /// Finalize method
  virtual StatusCode finalize( ) ;

  /** Implements IPileUpTool::numberOfPileUp
   *  Returns the number of pile-up interactions in one event. It follows
   *  a Poisson distribution with 
   *  mean = Luminosity * cross_section / crossing_rate.
   *  The fixed luminosity is returned as the currentLuminosity.
   */
  virtual unsigned int numberOfPileUp( double & currentLuminosity ) ;

  /// Implements IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters( ) ;

protected:

private:
  double m_luminosity ; ///< Fixed luminosity (set by options)

  double m_crossingRate ; ///< Crossing rate (set by options)

  double m_totalXSection ; ///< Cross section (set by options)

  int    m_nEvents ; ///< Counter of events (including empty events)

  double m_mean ; ///< Mean number of events per crossing

  IRndmGenSvc * m_randSvc ; ///< Pointer to random number generator service
};
#endif // GENERATORS_FIXEDLUMINOSITYFORRAREPROCESS_H
