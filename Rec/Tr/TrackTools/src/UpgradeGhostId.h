#ifndef UpgradeGhostId_H
#define UpgradeGhostId_H 1

// Include files
#include "Linker/LinkerTool.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVPExpectation.h"
#include "TrackInterfaces/ITrackManipulator.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"


class IClassifierReader;


/** @class UpgradeGhostId UpgradeGhostId.h
 *
 *  @author Paul Seyfert
 *  @date   30-12-2014
 */
class UpgradeGhostId : public GaudiTool,
                       virtual public ITrackManipulator,
                       virtual public IIncidentListener {


public:
  /// Standard constructor
  UpgradeGhostId( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);



  virtual ~UpgradeGhostId(){}; ///< Destructor

  virtual StatusCode finalize(); ///< Algorithm initialization
  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute(LHCb::Track& aTrack) const; 


  virtual void handle( const Incident& incident);

protected:
  StatusCode countHits();

private:
  bool              m_needrefresh;
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

  std::vector<std::string> veloVars() ;
  std::vector<std::string> upstreamVars() ;
  std::vector<std::string> downstreamVars() ;
  std::vector<std::string> longVars() ;
  std::vector<std::string> ttrackVars() ;

};

#endif // UpgradeGhostId_H
