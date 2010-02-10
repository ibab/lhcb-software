// $Id: VeloTrackMonitorNT.h,v 1.2 2010-02-10 14:26:22 szumlat Exp $
#ifndef VELOTRACKMONITORNT_H 
#define VELOTRACKMONITORNT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/IVeloClusterPosition.h"


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

  private:
    void FillVeloEvNtuple(LHCb::Tracks* tracks);
    void FillVeloClNtuple(const LHCb::Track& track);
    void FillVeloTrNtuple(const LHCb::Track& track);
  
  private:
    std::string m_tracksInContainer;    ///< Input Tracks container location
    bool m_clntuple;
    bool m_trntuple;
    bool m_evntuple;
    std::string m_allString;  
    LHCb::VeloClusters* m_rawClusters;
    const DeVelo* m_veloDet ;
    int m_runodin;
    int m_eventodin;
    int m_bunchid;

    IVeloExpectation* m_expectTool;

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

  };

}


#endif // VELOTRACKMONITORNT_H
