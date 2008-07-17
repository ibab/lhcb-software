// $Id: TsaConfirmTool.h,v 1.8 2008-07-17 08:07:35 albrecht Exp $
#ifndef TSACONFIRMTOOL_H 
#define TSACONFIRMTOOL_H 1

// Include files
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/ITrackConfirmTool.h"            // Interface
#include "TsaKernel/TsaTStationHitManager.h"
#include "TsaKernel/ITsaSeedTrackCnvTool.h"

#include "L0ConfDataStore.h"
#include "HltBase/IL0ConfExtrapolator.h"
/** @class TsaConfirmTool TsaConfirmTool.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2007-01-26
 *
 *  2007-11-16 Johannes Albrecht
 *  Bring TsaConfirmTool to new Tf framework
 *
 */

namespace Tf{
  namespace Tsa{
    class ITsaSeedStep;
    class ITsaStubFind;
    class ITsaStubLinker;
    class ITsaStubExtender;
  }
}

namespace LHCb{
  class Track;
}

class TsaConfirmTool : public GaudiTool, virtual public ITrackConfirmTool {
public: 
  /// Standard constructor
  TsaConfirmTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~TsaConfirmTool( ); ///< Destructor

  // Initialize method
  virtual StatusCode initialize();

  StatusCode tracks(const LHCb::State& seedState, std::vector<LHCb::Track*>& outputTracks );

  //@@JA fix!!!
  ParabolaHypothesis prepareT( const LHCb::State& seedState ,  std::vector<LHCb::LHCbID> ids){
    return ParabolaHypothesis(0,0,0,0,0,0,0,0,0);
  };     
protected:

private:
  //variables declared in job options
  double m_nsigma;
  bool m_debugMode;
  bool m_restrictTx;
  bool m_restrictTy;

  std::string m_seedTrackLocation;
  std::string m_seedHitLocation;
  std::string m_seedStubLocation;

  std::string m_selectorType;
  bool m_calcLikelihood; 

  IL0ConfExtrapolator* m_l0ConfExtrapolator;

  //TStationHitManager does decoding on demand
  Tf::Tsa::TStationHitManager* m_hitMan;
  
  std::vector<Tf::Tsa::ITsaSeedStep*> m_xSearchStep;
  std::vector<Tf::Tsa::ITsaSeedStep*> m_stereoStep;
  Tf::Tsa::ITsaSeedStep* m_xSelection;
  Tf::Tsa::ITsaSeedStep* m_finalSelection;
  Tf::Tsa::ITsaSeedStep* m_likelihood;
  Tf::Tsa::ITsaStubFind* m_stubFind;
  Tf::Tsa::ITsaStubLinker* m_stubLinker;
  Tf::Tsa::ITsaStubExtender* m_extendStubs;
  
  //for conversion from SeedTracks to LHCb::Tracks
  double m_likCut;
  Tf::Tsa::ITsaSeedTrackCnvTool* m_Tsa2TrackCnv;
  
  //debug information
  L0ConfDataStore* m_DataStore;

  unsigned m_minHitsInOT;
};
#endif // TSACONFIRMTOOL_H
