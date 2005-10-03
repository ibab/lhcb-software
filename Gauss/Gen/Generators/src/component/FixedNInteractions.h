// $Id: FixedNInteractions.h,v 1.1 2005-10-03 10:18:38 robbep Exp $
#ifndef GENERATORS_FIXEDNINTERACTIONS_H 
#define GENERATORS_FIXEDNINTERACTIONS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IPileUpTool.h"

/** @class FixedNInteractions FixedNInteractions.h 
 *  
 *  Utility tool to generate fixed number of pile-up interactions
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

  virtual unsigned int numberOfPileUp( double & currentLuminosity ) ;

  virtual void printPileUpCounters( ) ;

protected:

private:
  int m_nInteractions ;
};
#endif // GENERATORS_FIXEDNINTERACTIONS_H
