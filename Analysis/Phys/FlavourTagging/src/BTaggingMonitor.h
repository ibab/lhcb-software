// $Id: BTaggingMonitor.h,v 1.1 2004-05-03 13:54:09 pkoppenb Exp $
#ifndef BTaggingMonitor_H 
#define BTaggingMonitor_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "Event/EventHeader.h"
#include "Event/Collision.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "Event/FlavourTag.h"
#include "DaVinciMCTools/IDebugTool.h"

// from DaVinci
#include "DaVinciTools/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

/** @class BTaggingMonitor BTaggingMonitor.h
 *  
 *
 *  @author Marco Musy
 *  @date   2004-02-15
 */
class BTaggingMonitor : public DVAlgorithm {
public:
  /// Standard constructor
  BTaggingMonitor( const std::string& , ISvcLocator* );

  virtual ~BTaggingMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  /// Vector of locations of the tags to monitor
  std::vector<std::string> m_tags_locations; 
  Particle2MCLinksAsct::IAsct* m_pAsctLinks; 
  std::string m_nameMCAsct;  ///< Name of tool for Part to MCPart Association
  IDebugTool* m_debug;
  IParticlePropertySvc* ppSvc;

};
#endif // BTaggingMonitor_H
