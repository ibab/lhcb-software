// $Id: $
#ifndef ISMUONTOOL_H
#define ISMUONTOOL_H 1

// STL
#include <vector>

// from Gaudi
#include <GaudiAlg/GaudiTool.h>

// LHCb
#include <TrackInterfaces/ITracksFromTrack.h> // Interface

// local
#include "Hlt1MuonHit.h"

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

    virtual ~IsMuonTool(); ///< Destructor

    virtual StatusCode initialize();

    virtual StatusCode tracksFromTrack( const LHCb::Track& seed,
                                        std::vector<LHCb::Track*>& tracks );

  private:
    // data members
    unsigned int m_nStations;
    unsigned int m_nRegions;

    // Muon Detector
    DeMuonDetector* m_det;

    // Conditions
    double m_foiFactor;

    // Tools
    Hlt1MuonHitManager* m_hitManager;

    mutable std::vector<double> m_trackX; // position of track in x(mm) in each
                                          // station
    mutable std::vector<double> m_trackY; // position of track in y(mm) in each
                                          // station

    std::vector<double> m_stationZ;
    std::vector<double> m_padSizeX;
    std::vector<double> m_padSizeY;

    // local array of region sizes
    std::vector<double> m_regionInnerX; // inner edge in abs(x)
    std::vector<double> m_regionOuterX; // outer edge in abs(x)
    std::vector<double> m_regionInnerY; // inner edge in abs(y)
    std::vector<double> m_regionOuterY; // outer edge in abs(y)

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
    std::vector<unsigned int> m_occupancy;

    Hlt1ConstMuonHits m_hits;
    // FIXME:TODO -- can this be a local variable in findHits???
    // FIXME:TODO -- replace by std::array<Hlt1ConstMuonHits,4> m_regionHits;
    std::vector<Hlt1ConstMuonHits> m_regionHits;

    // Helper methods
    void extrapolateTrack( const LHCb::Track& track );

    bool preSelection( const LHCb::Track& track ) const;

    void findHits( const LHCb::Track& track );

    bool isMuon( const double p ) const;

    double foiX( const int station, const int region, const double p ) const;

    double foiY( const int station, const int region, const double p ) const;
};
#endif // ISMUONTOOL_H
