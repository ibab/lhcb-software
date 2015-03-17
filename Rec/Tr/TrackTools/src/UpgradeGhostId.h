#ifndef UpgradeGhostId_H
#define UpgradeGhostId_H 1

// Include files
#include "Linker/LinkerTool.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVPExpectation.h"
#include "TrackInterfaces/IGhostProbability.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"


class IClassifierReader;
namespace Rich{
  class TabulatedFunction1D;
}


/** @class UpgradeGhostId UpgradeGhostId.h
 *
 *  @author Paul Seyfert
 *  @date   30-12-2014
 */
class UpgradeGhostId : public GaudiTool,
                       virtual public IGhostProbability {


public:
  /// Standard constructor
  UpgradeGhostId( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);



  virtual ~UpgradeGhostId(){}; ///< Destructor

  virtual StatusCode finalize(); ///< Algorithm initialization
  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute(LHCb::Track& aTrack) const; 
  virtual StatusCode beginEvent() {return countHits();}; 
  /** reveal the variable names for a track type */
  virtual std::vector<std::string> variableNames(LHCb::Track::Types type) const {
    if (LHCb::Track::Velo==type) return veloVars();
    if (LHCb::Track::Long==type) return longVars();
    if (LHCb::Track::Upstream==type) return upstreamVars();
    if (LHCb::Track::Downstream==type) return downstreamVars();
    if (LHCb::Track::Ttrack==type) return ttrackVars();
    return std::vector<std::string>();
  }

  /** reveal the variable values for a track */
  virtual std::vector<float> netInputs(LHCb::Track& ) const {
    fatal() << "UpgradeGhostId::netInputs is NOT IMPLEMENTED" << endmsg;
    return std::vector<float>();
  }

protected:
  StatusCode countHits();

private:
  IVPExpectation   *m_vpExpectation;
  std::vector<IClassifierReader*> m_readers;


  std::vector<IHitExpectation*> m_Expectations;
  std::vector<std::string> m_inNames;
  std::vector<std::string> m_expectorNames;
  std::vector<int>* m_expectedHits;


  int m_veloHits;
  //int m_ttHits;
  int m_utHits;
  //int m_ftHits;
  //int m_itHits;
  //int m_otHits;

  std::vector<std::string> veloVars() const ;
  std::vector<std::string> upstreamVars() const ;
  std::vector<std::string> downstreamVars() const ;
  std::vector<std::string> longVars() const ;
  std::vector<std::string> ttrackVars() const ;

};

#endif // UpgradeGhostId_H
