// $Id: $
#ifndef HLT1MUONHITMANAGER_H
#define HLT1MUONHITMANAGER_H 1

// boost
#include <boost/unordered_map.hpp>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

// Muon detector
#include <Event/MuonCoord.h>

// local
#include "Hlt1MuonHit.h"
#include "Hlt1MuonStation.h"

class Hlt1MuonRegion;
class DeMuonDetector;

static const InterfaceID IID_Hlt1MuonHitManager( "Hlt1MuonHitManager", 1, 0 );

/** @class Hlt1MuonHitManager Hlt1MuonHitManager.h
 *  Muon hit manager for Hlt1
 *
 *  @author Roel Aaij
 *  @date   2010-12-02
 */
class Hlt1MuonHitManager : public GaudiTool, public IIncidentListener
{
  public:
    // Return the interface ID
    static const InterfaceID& interfaceID()
    {
        return IID_Hlt1MuonHitManager;
    }

    /// Standard constructor
    Hlt1MuonHitManager( const std::string& type, const std::string& name,
                        const IInterface* parent );

    virtual ~Hlt1MuonHitManager();

    virtual StatusCode initialize();

    virtual StatusCode finalize();

    virtual void handle( const Incident& incident );

    virtual Hlt1MuonHitRange hits( const double xmin, const unsigned int station,
                                   const unsigned int region );

    virtual unsigned int nRegions( const unsigned int station ) const;

    virtual const Hlt1MuonStation& station( const unsigned int id ) const;

    virtual const Hlt1MuonRegion& region( const unsigned int station,
                                          const unsigned int region ) const;

  protected:
  private:
    // Properties
    bool m_makeClusters;
    std::string m_coordLocation;

    // Data members
    DeMuonDetector* m_muonDet;

    std::vector<std::vector<double>> m_regions;

    std::vector<Hlt1MuonStation> m_stations;
    std::vector<bool> m_prepared;

    LHCb::MuonCoords* m_muonCoords;

    std::vector<LHCb::MuonTileID*> m_tilesInStation;

    bool m_loaded;
    typedef boost::unordered_multimap<unsigned int, const LHCb::MuonCoord*> coords_t;
    coords_t m_coords;
    std::vector<unsigned int> m_nHits;
    std::vector<Hlt1MuonHit*> m_hits;

    // Functions
    void prepareHits( const unsigned int station );

    void loadCoords();
};
#endif // HLT1MUONHITMANAGER_H
