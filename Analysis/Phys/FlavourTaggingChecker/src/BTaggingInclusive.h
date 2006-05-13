// $Id: BTaggingInclusive.h
#ifndef DEBUG_BTAGGINGINCLUSIVE_H 
#define DEBUG_BTAGGINGINCLUSIVE_H 1

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

/** @class BTaggingInclusive BTaggingInclusive.h debug/BTaggingInclusive.h
 *  
 *
 *  @author Onuora Awunor
 *  @date   2006-05-11
 */
class BTaggingInclusive: public DVAlgorithm {
public: 
  /// Standard constructor
  BTaggingInclusive( const std::string& , ISvcLocator* ); 

  virtual ~BTaggingInclusive( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


private:
  /// Vector of locations of the tags to monitor
  std::string m_tagslocation; 
  std::string m_ostagslocation;

  Particle2MCLinksAsct::IAsct* m_pAsctLinks; 
  IDebugTool* m_debug;
  int nsele,nrt[50],nwt[50];
 
};
#endif // DEBUG_BTAGGINGINCLUSIVE_H
