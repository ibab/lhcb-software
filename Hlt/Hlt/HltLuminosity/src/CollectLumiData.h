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
  CollectLumiData( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CollectLumiData( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  std::string m_RZVeloContainerName;
  const LHCb::Tracks* m_RZVelo;
  int m_nRZVelo;
  int m_iRZVelo;
  std::string m_PV3DContainerName;
  const LHCb::RecVertices* m_PV3D;
  int m_nPV3D;
  int m_iPV3D;

  std::string m_OutputContainerName;
  LHCb::HltLumiSummary* m_HltLumiSummary;

  virtual void collect   ();    ///< collect data
  

  // printing
  bool m_printing_verbose;
  bool m_printing_debug;
  bool m_printing_info;

private:

};
#endif // COLLECTLUMIDATA_H
