// $Id: CollectLumiData.h,v 1.6 2010-05-23 19:36:57 gligorov Exp $
#ifndef COLLECTLUMIDATA_H 
#define COLLECTLUMIDATA_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiAlg/GaudiTool.h"
// from LHCB
#include "Event/Track.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"


/** @class CollectLumiData CollectLumiData.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2008-07-11
 */
class CollectLumiData : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CollectLumiData( std::string name, ISvcLocator* pSvcLocator );

  virtual ~CollectLumiData( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::string m_RZVeloContainerName;
  std::string m_PV3DContainerName;
  std::string m_OutputContainerName;

  int m_iRZVelo;
  int m_iPV3D;
};
#endif // COLLECTLUMIDATA_H
