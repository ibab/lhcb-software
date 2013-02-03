// $Id: PatVeloUTTool.h,v 1.8 2010-01-09 12:46:27 witekma Exp $
#ifndef PATVELOUTTOOL_H
#define PATVELOUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "TfKernel/UTStationHitManager.h"

// local
#include "PatVUTTrack.h"
#include "PatUTMagnetTool.h"
#include "PatKernel/PatTTHit.h"

static const InterfaceID IID_PatVeloUTTool ( "PatVeloUTTool", 1, 0 );


  /** @class PatVeloUTTool PatVeloUTTool.h
   *
   *  PatVeloUT tool
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

  class PatVeloUTTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatVeloUTTool; }

    /// Standard constructor
    PatVeloUTTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    virtual ~PatVeloUTTool( ); ///< Destructor

    StatusCode initialize ( );


    void recoVeloUT(LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks );
    void simpleFit( PatVUTTrack& vtt);

  protected:

    void simpleFitTracks( std::vector<PatVUTTrack>&);
    void selectBestTracks( std::vector<PatVUTTrack>& vttTracks);
    void prepareOutputTracks( std::vector<PatVUTTrack>& vttTracks, std::vector<LHCb::Track*>& outtracks);
    void localCleanUp(std::vector<PatVUTTrack>&);
    void getCandidates(LHCb::Track& veloTrack, std::vector<PatVUTTrack>& vtt);
    void saveCandidate( PatTTHits& theClusters, PatVUTTrack& candidate);

  private:
    class compPseudoChi2  {
    public:
      bool operator() (PatVUTTrack* first, PatVUTTrack* second ) {
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
    double m_xTol;
    double m_xTolSlope;
    double m_yTol;
    double m_yTolSlope;
    double m_dxGroupTol;
    double m_dxGroupFactor;
    double m_zMidUT;

    Tf::UTStationHitManager<PatTTHit> *      m_ttHitManager;

    PatUTMagnetTool*    m_PatUTMagnetTool;  ///< Multipupose tool for Bdl and deflection
    bool m_debug;
    bool m_verbose;

  };

#endif // PATVELOUTTOOL_H
