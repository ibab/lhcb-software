// $Id: $
#ifndef HLT1MUONSTATION_H
#define HLT1MUONSTATION_H 1

// STL
#include <vector>

// Gaudi
#include <GaudiKernel/Range.h>

// Include files
#include "Hlt1MuonHit.h"
#include "Hlt1MuonRegion.h"

class DeMuonDetector;

/** @class Hlt1MuonStation Hlt1MuonStation.h
 *  Class to hold regions of hits and
 *  provide some geometry information
 *
 *  @author Roel Aaij
 *  @date   2010-12-07
 */
class Hlt1MuonStation
{
  public:
    /// Standard constructor
    Hlt1MuonStation( DeMuonDetector* det, int station,
                     const std::vector<double>& regions );

    virtual ~Hlt1MuonStation(); ///< Destructor

    double z() const
    {
        return m_z;
    }

    unsigned int station() const
    {
        return m_station;
    }

    Hlt1MuonHitRange hits( double xmin, unsigned int region ) const;

    Hlt1MuonHitRange hits( unsigned int region ) const;

    unsigned int nRegions() const
    {
        return m_nRegionsX * m_nRegionsY;
    }

    const Hlt1MuonRegion& region( const unsigned int id ) const
    {
        return m_regions[id];
    }

  private:
    friend class Hlt1MuonHitManager;

    void setHits( const Hlt1MuonHits& hits );

    void addHit( Hlt1MuonHit* hit );

    inline unsigned int xRegion( const double x );

    void clearHits();

    DeMuonDetector* m_muonDet;

    std::vector<Hlt1MuonHits> m_hits;
    std::vector<Hlt1MuonRegion> m_regions;

    const int m_station;
    double m_z;

    unsigned int m_nRegionsX;
    unsigned int m_nRegionsY;

    double m_ymin;
    double m_ymax;
    double m_dy;
};
#endif // HLT1MUONSTATION_H
