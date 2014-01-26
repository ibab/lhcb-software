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

// local
#include "Candidate.h"
#include "Hlt1MuonHit.h"

namespace LHCb
{
class Track;
}
class Hlt1MuonHitManager;
class Candidate;

namespace Hlt
{
/** @class HltVeloIsMuon HltVeloIsMuon.h
 *  Look for Muon hits which match a velo track.
 *
 *  @author Roel Aaij
 *  @date   2010-12-02
 */

class HltVeloIsMuon : virtual public extends1<GaudiHistoTool, ITracksFromTrack>
{
  public:
    enum { nStations = 5u, nRegions = 4u };
    /// Standard constructor
    HltVeloIsMuon( const std::string& type, const std::string& name,
                   const IInterface* parent );

    virtual ~HltVeloIsMuon(); ///< Destructor

    virtual StatusCode initialize();

    virtual StatusCode finalize();

    virtual StatusCode tracksFromTrack( const LHCb::Track& seed,
                                        std::vector<LHCb::Track*>& tracks );

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
    Hlt1MuonHitManager* m_hitManager;
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
    Hlt1MuonHit* m_magnetHit;
    Candidates m_seeds;

    std::array<double,nRegions> m_regionFoIX;
    std::array<double,nRegions> m_regionFoIY;

    // Helper methods
    void findSeeds( const Candidate* seed, const unsigned int seedStation );

    void addHits( Candidate* seed );

    void fitCandidate( Candidate* seed ) const;

    void clean();

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
