// $Id: DoubleTagCorrelation.h
#ifndef DOUBLETAGCORRELATION_H 
#define DOUBLETAGCORRELATION_H 1

// Include files
#include <string>
#include <vector>
#include <algorithm>
#include "GaudiKernel/AlgFactory.h"
#include "Event/FlavourTag.h"
#include "DaVinciMCTools/IDebugTool.h"
#include "Event/GenMCLink.h"

// from DaVinci
#include "Kernel/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

/** @class DoubleTagCorrelation DoubleTagCorrelation.h
 *  
 *  Algorithm to evaluate OS/SS tag correlation
 *  @author Chris Barnes
 *  @date   2006-01-28
 */
class DoubleTagCorrelation : public DVAlgorithm {
public:
  /// Standard constructor
  DoubleTagCorrelation( const std::string& , ISvcLocator* );

  virtual ~DoubleTagCorrelation( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  /// Vector of locations of the tags to monitor
  std::string m_tagslocation; 
  std::string m_ostagslocation;

  Particle2MCLinksAsct::IAsct* m_pAsctLinks; 
  IDebugTool* m_debug;

  int** matrix;

};
#endif // DOUBLETAGCORRELATION_H
