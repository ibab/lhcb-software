// $Id: CollectLumiData.h,v 1.1.1.1 2008-07-17 08:50:25 panmanj Exp $
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
  CollectLumiData( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CollectLumiData( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  std::string m_RZVeloContainerName;
  const LHCb::Tracks* m_RZVelo;
  short m_nRZVelo;
  std::string m_PV2DContainerName;
  const LHCb::RecVertices* m_PV2D;
  short m_nPV2D;

  std::string m_OutputContainerName;
  LHCb::HltLumiSummarys* m_HltLumiSummarys;

  virtual void collect   ();    ///< collect data
  

  // printing
  bool m_printing_verbose;
  bool m_printing_debug;
  bool m_printing_info;

private:

};
#endif // COLLECTLUMIDATA_H
