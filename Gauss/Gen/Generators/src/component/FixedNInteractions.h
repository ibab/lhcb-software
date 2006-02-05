// $Id: FixedNInteractions.h,v 1.3 2006-02-05 21:02:45 robbep Exp $
#ifndef GENERATORS_FIXEDNINTERACTIONS_H 
#define GENERATORS_FIXEDNINTERACTIONS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IPileUpTool.h"

/** @class FixedNInteractions FixedNInteractions.h "FixedNInteractions.h"
 *  
 *  Tool to generate fixed number of pile-up interactions. Concrete
 *  implementation of IPileUpTool. 
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */
class FixedNInteractions : public GaudiTool, virtual public IPileUpTool {
 public:
  /// Standard constructor
  FixedNInteractions( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  virtual ~FixedNInteractions( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /** Returns a constant number of pile-up interactions.
   *  Implementation of IPileUpTool::numberOfPileUp. 
   *  Returns a luminosity equal to 0.
   */
  virtual unsigned int numberOfPileUp( double & currentLuminosity ) ;
  
  /// Implementation of IPileUpTool::printPileUpCounters
  virtual void printPileUpCounters( ) ;
  
 private:
  /// Number of interactions to generate (set by job options)
  int m_nInteractions ; 
  /// Luminosity to use for spill over (set by job options)
  double m_luminosity ;
};
#endif // GENERATORS_FIXEDNINTERACTIONS_H
