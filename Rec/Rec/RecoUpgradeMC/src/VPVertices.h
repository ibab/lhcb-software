// $Id: VPV0s.h,v 1.1.1.1 2007/10/09 18:41:19 smenzeme Exp $
#ifndef VPVERTICES_H 
#define VPVERTICES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "VPDet/DeVP.h"
#include "TrackInterfaces/IVPClusterPosition.h"
#include "VPDet/DeVP.h"
#include "Event/VPLiteCluster.h"
#include "MCInterfaces/ILHCbIDsToMCHits.h"
#include "GaudiKernel/ToolHandle.h"

class ITrajPoca ;
class IMagneticFieldSvc ;
class ITrackVertexer ;
class ITrackExtrapolator ;
class ITrackInterpolator ;

#include "Event/TwoProngVertex.h"
namespace LHCb {
  class ParticleProperty ;
}

/** @class VPVertices VPVertices.h
 *  
 *   Algorithm to produce an NTuple to check Pat tracks.
 *  @author Olivier Callot
 *  @date   2006-05-11
 *  @update for A-Team framework 2007-08-20
 */


  class VPVertices : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    VPVertices( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VPVertices( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    ILHCbIDsToMCHits* linkTool() const;
    
  protected:

  private:
    std::string m_tupleName;
    std::string m_v0ContainerName;
    DeVP* m_vP;
    IVPClusterPosition* m_positiontool ;
    ILHCbIDsToMCHits* m_linkTool;
    LHCb::VPLiteCluster::VPLiteClusters* m_clusters;
    
  };


inline ILHCbIDsToMCHits* VPVertices::linkTool() const{
  return m_linkTool;
}

#endif // VPVERTICES_H
