// $Id: $
#ifndef HLT1MUONSTATION_H
#define HLT1MUONSTATION_H 1

// STL
#include <vector>

// Gaudi
#include <GaudiKernel/Range.h>

// Hlt1Muons
#include <Hlt1Muons/Hlt1MuonHit.h>

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

    virtual ~Hlt1MuonStation() {} ///< Destructor

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
    Hlt1MuonHitRange hits( double xmin, double xmax, unsigned int region ) const;

    Hlt1MuonHitRange hits( unsigned int region ) const;

    unsigned int nRegions() const
    {
        return (m_xboundaries.size()-1) * nRegionsY;
    }

    //TODO: generate range of id which overlap (xMin,xMax,yMin,yMax)...
    bool overlaps( unsigned id, double xmin, double xmax, double ymin_, double ymax ) const {
        auto j = id % nRegionsY ;
        auto i = id/nRegionsY;
        auto y = ymin() + j * dy();
        return !( xmax < m_xboundaries[i] || xmin > m_xboundaries[ i+1 ] || ymin_ > y+dy() || ymax < y );
    }

  private:
    friend class Hlt1MuonHitManager;

    void setHits( Hlt1MuonHits&&  hts );

    inline double dy() const { return 2 * m_ymax / nRegionsY; }
    inline double ymin() const { return -m_ymax; }

    const std::vector<double>  m_xboundaries;
    Hlt1MuonHits  m_hits; 
    std::vector<typename Hlt1MuonHits::iterator> m_index; // indices into m_hits

    double m_z;
    double m_ymax;

    const int m_station;
};
#endif // HLT1MUONSTATION_H
