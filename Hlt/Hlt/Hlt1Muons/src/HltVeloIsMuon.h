// $Id: $
#ifndef MATCHVELOMUON_H
#define MATCHVELOMUON_H 1

// Include files
#include <vector>

// from Gaudi
#include <GaudiKernel/ToolFactory.h>
#include <GaudiAlg/GaudiHistoTool.h>

// LHCb
#include <Kernel/ILHCbMagnetSvc.h>
#include <TrackInterfaces/ITracksFromTrack.h>

// Hlt1Muons
#include <Hlt1Muons/Candidate.h>

// from MuonID
#include <MuonID/CommonMuonHit.h>

namespace LHCb
{
class Track;
}
class CommonMuonHitManager;
class Candidate;

namespace Hlt
{
/** @class HltVeloIsMuon HltVeloIsMuon.h
 *  Look for Muon hits which match a velo track.
 *
 *  @author Roel Aaij
 *  @date   2010-12-02
 */

class HltVeloIsMuon : public extends1<GaudiHistoTool, ITracksFromTrack>
{
  public:
    enum { nStations = 5u, nRegions = 4u };
    /// Standard constructor
    HltVeloIsMuon( const std::string& type, const std::string& name,
                   const IInterface* parent );

    StatusCode initialize() override;
    StatusCode finalize() override;
    StatusCode tracksFromTrack( const LHCb::Track& seed,
                                std::vector<LHCb::Track*>& tracks ) const override;

  private:
    friend class ToolFactory<HltVeloIsMuon>;

    // Properties
    double m_za;
    double m_zb;

    double m_xWindow;
    double m_yWindow;
    double m_FoITolerance;

    double m_minMomentum;
    double m_kickScale;
    double m_kickOffset;

    double m_maxChi2DoFX;

    unsigned int m_maxMissed;

    bool m_setQOverP;

    // Tools and services
    CommonMuonHitManager* m_hitManager;
    ILHCbMagnetSvc* m_fieldSvc;

    // Muon Detector
    DeMuonDetector* m_det;

    // Data from detector description and conditions
    std::vector<double> m_padSizeX;
    std::vector<double> m_padSizeY;

    // function that defines the field of interest size
    // formula is p(1) + p(2)*exp(-p(3)*momentum)
    std::vector<double> m_xFoIParam1;
    std::vector<double> m_xFoIParam2;
    std::vector<double> m_xFoIParam3;
    std::vector<double> m_yFoIParam1;
    std::vector<double> m_yFoIParam2;
    std::vector<double> m_yFoIParam3;

    std::vector<double> m_momentumCuts; // vector of momentum ranges

    double m_preSelMomentum;
    double m_FoIFactor;


    // Temporary storage
    mutable std::unique_ptr<CommonMuonHit> m_magnetHit;
    mutable std::vector<Candidate>  m_seeds;

    mutable std::array<double,nRegions> m_regionFoIX;
    mutable std::array<double,nRegions> m_regionFoIY;

    // Helper methods
    void findSeeds( const Candidate& seed, const unsigned int seedStation ) const;

    void addHits( Candidate& seed ) const;

    void fitCandidate( Candidate& seed ) const;

    void clean() const;

    inline double dtx( const double p ) const
    {
        return m_kickScale / ( p - m_kickOffset );
    }

    inline double momentum( const double dtx ) const
    {
        return m_kickScale / fabs( dtx ) + m_kickOffset;
    }

    double FoIX( const int station, const int region, const double p ) const;

    double FoIY( const int station, const int region, const double p ) const;
};
} // namespace Hlt

#endif // MATCHVELOMUON_H
