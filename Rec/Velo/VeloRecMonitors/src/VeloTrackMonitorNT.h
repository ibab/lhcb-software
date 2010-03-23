// $Id: VeloTrackMonitorNT.h,v 1.4 2010-03-23 13:19:00 szumlat Exp $
#ifndef VELOTRACKMONITORNT_H 
#define VELOTRACKMONITORNT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/IVeloClusterPosition.h"
#include "Event/MCHit.h"

// Linker
#include "Linker/LinkerTool.h"

// Kernel
#include "Kernel/VeloChannelID.h"

/** @class VeloTrackMonitorNT
 * 
 * Class for Velo track monitoring Ntuple
 *  @author S. Borghi
 *  @date   28-07-2009
 */

class DeVeloRType;
class DeVeloPhiType;
                                                           
namespace Velo
{
  class VeloTrackMonitorNT : public GaudiTupleAlg {
                                                                             
  public:

    typedef IVeloClusterPosition::Direction Direction;
                                                                             
    /** Standard construtor */
    VeloTrackMonitorNT( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
    /** Destructor */
    virtual ~VeloTrackMonitorNT();

    /** Algorithm execute */
    virtual StatusCode execute();

    /** Algorithm initialize */
    virtual StatusCode initialize();

    // associators
    typedef LinkerTool<LHCb::VeloCluster, LHCb::MCHit> AsctTool;
    typedef AsctTool::DirectType Table;
    typedef Table::Range Range;
    typedef Table::iterator iterator;

  private:

    void FillVeloEvNtuple(LHCb::Tracks* tracks);
    StatusCode FillVeloClNtuple(const LHCb::Track& track);
    void FillVeloTrNtuple(const LHCb::Track& track);
  
  private:

    std::string m_tracksInContainer;    ///< Input Tracks container location
    bool m_clntuple;
    bool m_trntuple;
    bool m_evntuple;
    bool m_runWithMC;
    std::string m_allString; 
    std::string m_clusterLoc;
    std::string m_asctLocation; 
    LHCb::VeloClusters* m_rawClusters;
    const DeVelo* m_veloDet ;
    int m_runodin;
    long unsigned int m_eventodin;
    int m_bunchid;
    IVeloExpectation* m_expectTool;
    IVeloClusterPosition* m_clusterTool;
    const Table* m_asctTable;

  protected:

    Gaudi::XYZPoint extrapolateToZ(const LHCb::Track *track, double toZ);
    double projAngleR(const Direction& locDirection, const Gaudi::XYZPoint& aLocPoint);
    double projAnglePhi(const Direction& locDirection,
                        const DeVeloPhiType* phiSensor,
                        unsigned int centreStrip);
    Direction localTrackDirection(const Gaudi::XYZVector& globalTrackDir,
                                  const DeVeloSensor* sensor) const;
    double angleOfInsertion(const Direction& localSlopes,
                            Gaudi::XYZVector& parallel2Track) const;
    const LHCb::MCHit* getAssocMCHit(const LHCb::VeloCluster* clus) const;
    LHCb::VeloChannelID weightedMean(const LHCb::VeloCluster* cluster, double& isp);

  };

}


#endif // VELOTRACKMONITORNT_H
