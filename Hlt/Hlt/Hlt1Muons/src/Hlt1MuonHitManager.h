// $Id: $
#ifndef HLT1MUONHITMANAGER_H
#define HLT1MUONHITMANAGER_H 1

#include <array>
#include <vector>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"

// Muon detector
#include <Event/MuonCoord.h>
#include <Hlt1Muons/Hlt1MuonHit.h>

// local
#include "Hlt1MuonStation.h"

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

    void handle( const Incident& incident ) override;

    virtual Hlt1MuonHitRange hits( double xmin, unsigned int station,
                                   unsigned int region );
    virtual Hlt1MuonHitRange hits( double xmin, double xmax, unsigned int station,
                                   unsigned int region );

    virtual unsigned int nRegions( unsigned int station ) const;

    virtual const Hlt1MuonStation& station( unsigned int id ) const;

  private:
    // Properties
    std::string m_coordLocation;

    // Data members
    DeMuonDetector* m_muonDet;

    std::vector<Hlt1MuonStation> m_stations;
    std::bitset<5> m_prepared;

    std::array<std::vector<const LHCb::MuonCoord*>,5>  m_coords;
    bool m_loaded;

    // Functions
    void prepareHits( unsigned int station );

    void loadCoords();
};
#endif // HLT1MUONHITMANAGER_H
