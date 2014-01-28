// $Id:
#ifndef PATMATCHL0MUONTOOL_H
#define PATMATCHL0MUONTOOL_H 1

// Include files
#include <string>

// from Gaudi
#include <GaudiAlg/GaudiTool.h>
#include <GaudiKernel/IRegistry.h>

// Event
#include <Event/Track.h>

// TrackInterfaces
#include <TrackInterfaces/ITrackMatch.h>
#include <TrackInterfaces/ITrackMomentumEstimate.h>

/** @class PatMatchL0MuonTool PatMatchL0MuonTool.h
 *
 *  Match Velo tracks to L0Muon candidates.
 *  Based on PatMatchTool by Olivier Callot.
 *
 *  @author Roel Aaij
 *  @data   2010-11-17
 *
 */
class PatMatchL0MuonTool : public GaudiTool, virtual public ITrackMatch
{
  public:
    /// Standard constructor
    PatMatchL0MuonTool( const std::string& type, const std::string& name,
                        const IInterface* parent );

    virtual ~PatMatchL0MuonTool(); ///< Destructor

    virtual StatusCode initialize(); ///< Initialization

    /// Function for matching in Hlt
    StatusCode match( const LHCb::Track& veloTrack, const LHCb::Track& tTrack,
                      LHCb::Track& matchedTrack, double& quality, double& quality2 );

  private:
    double getChi2Match( const LHCb::Track& velo, const LHCb::Track& seed );

    void makeTrack( const LHCb::Track& velo, const LHCb::Track& seed,
                    LHCb::Track& output, double chi2 );

    // use tool for momentum parametrisation
    std::string m_fastMomentumToolName;
    ITrackMomentumEstimate* m_fastMomentumTool;

    double m_zMagnet;
    double m_zMagnetTx2;
    double m_zMagnetDsl2;
    double m_zMatchY;
    double m_dxTol;
    double m_dxTolSlope;
    double m_dyTol;
    double m_dyTolSlope;
    double m_magnetBend;
    double m_maxChi2;
};
#endif // PATMATCHL0MUONTOOL_H
