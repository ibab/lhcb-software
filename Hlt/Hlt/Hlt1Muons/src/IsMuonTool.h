// $Id: $
#ifndef ISMUONTOOL_H
#define ISMUONTOOL_H 1

// STL
#include <vector>

// from Gaudi
#include <GaudiAlg/GaudiTool.h>

// LHCb
#include <TrackInterfaces/ITracksFromTrack.h> // Interface

// Hlt1Muons
#include <Hlt1Muons/Hlt1MuonHit.h>

namespace LHCb
{
class Track;
}
class Hlt1MuonHitManager;
class DeMuonDetector;

/** @class IsMuonTool IsMuonTool.h
 *  This tool performs an IsMuon check on a long track. The decision is not stored.
 *
 *  @author Roel Aaij
 *  @date   2011-01-19
 */
class IsMuonTool : public GaudiTool, virtual public ITracksFromTrack
{
  public:
    /// Standard constructor
    IsMuonTool( const std::string& type, const std::string& name,
                const IInterface* parent );

    ~IsMuonTool() override = default; ///< Destructor

    StatusCode initialize() override;

    StatusCode tracksFromTrack( const LHCb::Track& seed,
                                std::vector<LHCb::Track*>& tracks ) override;

  private:
    enum { nStations = 5u, nRegions = 4u };

    // Muon Detector
    DeMuonDetector* m_det;

    // Conditions
    double m_foiFactor;

    // Tools
    Hlt1MuonHitManager* m_hitManager;

    mutable std::array<std::pair<double,double>,nStations> m_track; // position of track in x(mm),y(mm) in each station

    std::array<double,nStations> m_stationZ;

    std::array<std::pair<double,double>,nStations*nRegions> m_padSize;

    // local array of region sizes
    std::array<std::pair<double,double>,nStations> m_regionInner; // inner edge in abs(x), abs(y)
    std::array<std::pair<double,double>,nStations> m_regionOuter; // outer edge in abs(x), abs(y)

    // function that defines the field of interest size
    // formula is p(1) + p(2)*exp(-p(3)*momentum)
    std::vector<double> m_xfoiParam1;
    std::vector<double> m_xfoiParam2;
    std::vector<double> m_xfoiParam3;
    std::vector<double> m_yfoiParam1;
    std::vector<double> m_yfoiParam2;
    std::vector<double> m_yfoiParam3;

    double m_preSelMomentum;

    std::vector<double> m_momentumCuts; // vector of momentum ranges

    // number of hits in the FOI found in the muon stations
    std::array<unsigned int,nStations> m_occupancy;

    // Helper methods
    void extrapolateTrack( const LHCb::Track& track );

    bool preSelection( const LHCb::Track& track ) const;

    Hlt1ConstMuonHits findHits( const LHCb::Track& track );

    bool isMuon( const double p ) const;

    std::pair<double,double> foi( const int station, const int region, const double p ) const;

};
#endif // ISMUONTOOL_H
