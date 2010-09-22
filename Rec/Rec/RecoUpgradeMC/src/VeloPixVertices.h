// $Id: VeloPixV0s.h,v 1.1.1.1 2007/10/09 18:41:19 smenzeme Exp $
#ifndef VELOPIXVERTICES_H 
#define VELOPIXVERTICES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "VeloPixDet/DeVeloPix.h"
#include "TrackInterfaces/IVeloPixClusterPosition.h"
#include "VeloPixDet/DeVeloPix.h"
#include "Event/VeloPixLiteCluster.h"
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

/** @class VeloPixVertices VeloPixVertices.h
 *  
 *   Algorithm to produce an NTuple to check Pat tracks.
 *  @author Olivier Callot
 *  @date   2006-05-11
 *  @update for A-Team framework 2007-08-20
 */


  class VeloPixVertices : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    VeloPixVertices( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloPixVertices( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    ILHCbIDsToMCHits* linkTool() const;
    
  protected:

  private:
    std::string m_tupleName;
    std::string m_v0ContainerName;
    DeVeloPix* m_veloPix;
    IVeloPixClusterPosition* m_positiontool ;
    ILHCbIDsToMCHits* m_linkTool;
    LHCb::VeloPixLiteCluster::VeloPixLiteClusters* m_clusters;
    
  };


inline ILHCbIDsToMCHits* VeloPixVertices::linkTool() const{
  return m_linkTool;
}

#endif // VELOPIXVERTICES_H
