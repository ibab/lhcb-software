#ifndef PATVELOTTTOOL_H
#define PATVELOTTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "TfKernel/TTStationHitManager.h"

#include "TrackInterfaces/ITracksFromTrack.h"

// local
#include "PatVTTTrack.h"
#include "PatTTMagnetTool.h"
#include "PatKernel/PatTTHit.h"

static const InterfaceID IID_PatVeloTTTool ( "PatVeloTTTool", 1, 0 );

  /** @class PatVeloTTTool PatVeloTTTool.h
   *
   *  PatVeloTT tool
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */


class PatVeloTTTool : public GaudiTool, virtual public ITracksFromTrack {
public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatVeloTTTool; }

    /// Standard constructor
    PatVeloTTTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    ~PatVeloTTTool( ) override; ///< Destructor

    StatusCode initialize ( ) override;

    
    StatusCode tracksFromTrack(const LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks ) const override;
   
    void simpleFit(PatVTTTrack& vtt) const;

  private:
    void simpleFitTracks( std::vector<PatVTTTrack>&) const;
    void selectBestTracks( std::vector<PatVTTTrack>& vttTracks) const;
    std::vector<std::unique_ptr<LHCb::Track>> prepareOutputTracks( std::vector<PatVTTTrack>& vttTracks) const;
    void localCleanUp(std::vector<PatVTTTrack>&) const;
    void getCandidates(const LHCb::Track& veloTrack, std::vector<PatVTTTrack>& vtt) const;
    void saveCandidate( PatTTHits& theClusters, PatVTTTrack& candidate) const;

  private:
    double m_maxXSlope;
    double m_maxYSlope;
    double m_centralHoleSize;
    double m_maxXSize;
    double m_maxYSize;
    double m_minMomentum;
    double m_minPT;
    double m_maxPseudoChi2;
    int m_maxSolutionsPerTrack;
    double m_xTol;
    double m_xTolSlope;
    double m_yTol;
    double m_yTolSlope;
    double m_dxGroupTol;
    double m_dxGroupFactor;
    double m_zMidTT;

    Tf::TTStationHitManager<PatTTHit> *      m_ttHitManager = nullptr;

    PatTTMagnetTool*    m_PatTTMagnetTool = nullptr;  ///< Multipupose tool for Bdl and deflection
    bool m_debug;
    bool m_verbose;
    bool m_passUnmatched;                   ///< flag to toggle passing unmatched velo candidates to output (default=true) 
    bool m_PrintVariables;

  };

#endif // PATVELOTTTOOL_H
