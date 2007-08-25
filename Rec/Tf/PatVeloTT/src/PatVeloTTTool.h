// $Id: PatVeloTTTool.h,v 1.3 2007-08-25 14:27:37 jonrob Exp $
#ifndef PATVELOTTTOOL_H
#define PATVELOTTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "TfKernel/TTStationHitManager.h"

// local
#include "PatVTTTrack.h"
#include "PatTTMagnetTool.h"
#include "PatVeloTTHit.h"

static const InterfaceID IID_PatVeloTTTool ( "PatVeloTTTool", 1, 0 );

namespace Tf {

  /** @class PatVeloTTTool PatVeloTTTool.h
   *
   *  PatVeloTT tool
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

  class PatVeloTTTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatVeloTTTool; }

    /// Standard constructor
    PatVeloTTTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    virtual ~PatVeloTTTool( ); ///< Destructor

    StatusCode PatVeloTTTool::initialize ( );


    void recoVeloTT(LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks );

  protected:

    void simpleFit( PatVTTTrack& vttt );
    void simpleFitTracks( std::vector<PatVTTTrack>&);
    void selectBestTracks( std::vector<PatVTTTrack>& vttTracks);
    void prepareOutputTracks( std::vector<PatVTTTrack>& vttTracks, std::vector<LHCb::Track*>& outtracks);
    void localCleanUp(std::vector<PatVTTTrack>&);
    void getCandidates(LHCb::Track& veloTrack, std::vector<PatVTTTrack>& vtt);
    void saveCandidate( PatVeloTTHits& theClusters, PatVTTTrack& candidate);

  private:
    class compPseudoChi2  {
    public:
      bool operator() (PatVTTTrack* first, PatVTTTrack* second ) {
        return fabs(first->chi2PerDoF()) < fabs(second->chi2PerDoF()) ;
      }
    };
    double m_maxXSlope;
    double m_maxYSlope;
    double m_centralHoleSize;
    double m_maxXSize;
    double m_maxYSize;
    double m_minMomentum;
    double m_maxPseudoChi2;
    int m_maxSolutionsPerTrack;
    double m_distToMomentum;
    double m_zMidField;
    double m_xTol;
    double m_xTolSlope;
    double m_yTol;
    double m_yTolSlope;
    double m_dxGroupTol;
    double m_dxGroupFactor;
    double m_zMidTT;

    TTStationHitManager<PatVeloTTHit> *      m_ttHitManager;

    PatTTMagnetTool*    m_PatTTMagnetTool;  ///< Multipupose tool for Bdl and deflection
    bool m_debug;
    bool m_verbose;

  };
}
#endif // PATVELOTTTOOL_H
