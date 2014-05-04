// $Id: $
#ifndef HLT1MUONHITMANAGER_H
#define HLT1MUONHITMANAGER_H 1

#include <unordered_map>

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

    ~Hlt1MuonHitManager() override = default;

    StatusCode initialize() override;

    StatusCode finalize() override;

    void handle( const Incident& incident ) override;

    virtual Hlt1MuonHitRange hits( double xmin, unsigned int station,
                                   unsigned int region );

    virtual unsigned int nRegions( unsigned int station ) const;

    virtual const Hlt1MuonStation& station( unsigned int id ) const;

    virtual Hlt1MuonRegion region( unsigned int station,
                                   unsigned int region ) const;

  private:
    // Properties
    std::string m_coordLocation;

    // Data members
    DeMuonDetector* m_muonDet;

    std::vector<Hlt1MuonStation> m_stations;
    std::bitset<5> m_prepared;

    std::unordered_multimap<unsigned int, const LHCb::MuonCoord*> m_coords;
    std::array<unsigned int,5> m_nHits;
    bool m_loaded;

    // Functions
    void prepareHits( unsigned int station );

    void loadCoords();
};
#endif // HLT1MUONHITMANAGER_H
