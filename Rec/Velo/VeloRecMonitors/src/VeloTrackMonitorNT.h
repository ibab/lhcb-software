// $Id: VeloTrackMonitorNT.h,v 1.1 2009-08-20 11:05:14 siborghi Exp $
#ifndef VELOTRACKMONITORNT_H 
#define VELOTRACKMONITORNT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"



/** @class VeloTrackMonitorNT
 * 
 * Class for Velo track monitoring Ntuple
 *  @author S. Borghi
 *  @date   28-07-2009
 */                 
                                                           
namespace Velo
{
  class VeloTrackMonitorNT : public GaudiTupleAlg {
                                                                             
  public:
                                                                             
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

  };

}


#endif // VELOTRACKMONITORNT_H
