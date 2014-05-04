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
class Hlt1MuonStation final
{
  public:
    /// Standard constructor
    Hlt1MuonStation( DeMuonDetector* det, int station,
                     std::vector<double> regions );

    ~Hlt1MuonStation(); ///< Destructor

    enum { nRegionsY = 7u };

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
        return (m_xboundaries.size()-1) * nRegionsY;
    }

    Hlt1MuonRegion region( unsigned int id ) const
    {
        auto j = id % nRegionsY ;
        auto i = 1 + id/nRegionsY;
        auto y = ymin() + j * dy();
        return { id, m_xboundaries[ i-1], m_xboundaries[i],  y, y+dy() };
    }

  private:
    friend class Hlt1MuonHitManager;

    void setHits( Hlt1MuonHits hts ); // assumes ownership of pointers in vector

    inline double dy() const { return 2 * m_ymax / nRegionsY; }
    inline double ymin() const { return -m_ymax; }

    void clearHits();

    const std::vector<double>  m_xboundaries;
    Hlt1MuonHits  m_hits; 
    std::vector<Hlt1MuonHits::iterator> m_index; // indices into m_hits

    double m_z;
    double m_ymax;

    const int m_station;
};
#endif // HLT1MUONSTATION_H
