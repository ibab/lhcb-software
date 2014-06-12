// $Id: $
#ifndef HLT1MUONHIT_H
#define HLT1MUONHIT_H 1

// Gaudi
#include <GaudiKernel/Range.h>
#include <GaudiKernel/boost_allocator.h>

// LHCb
#include <Kernel/MuonTileID.h>

/** @class Hlt1MuonHit Hlt1MuonHit.h
 *
 *
 *  @author Roel Aaij
 *  @date   2010-12-02
 */
class Hlt1MuonHit final // please do not inherit from this class...
{
  public:
    Hlt1MuonHit() = delete;

    Hlt1MuonHit( const LHCb::MuonTileID& tile, double x, double dx, double y,
                 double dy, double z, double dz )
        : m_params{x,y,dx,z,dy,dz}, m_tile{tile}
    {
    }

    const LHCb::MuonTileID& tile() const
    {
        return m_tile;
    }
    unsigned int station() const
    {
        return m_tile.station();
    }
    double x() const
    {
        return m_params[0];
    }
    double dx() const
    {
        return m_params[2];
    }
    double y() const
    {
        return m_params[1];
    }
    double dy() const
    {
        return m_params[4];
    }
    double z() const
    {
        return m_params[3];
    }
    double dz() const
    {
        return m_params[5];
    }

    /// operator new
    static void* operator new( size_t size )
    {
        return (
            sizeof( Hlt1MuonHit ) == size
                ? boost::singleton_pool<Hlt1MuonHit, sizeof( Hlt1MuonHit )>::malloc()
                : ::operator new( size ) );
    }

    /// operator delete
    static void operator delete( void* p )
    {
        boost::singleton_pool<Hlt1MuonHit, sizeof( Hlt1MuonHit )>::is_from( p )
            ? boost::singleton_pool<Hlt1MuonHit, sizeof( Hlt1MuonHit )>::free( p )
            : ::operator delete( p );
    }

  private:
    std::array<double,6> m_params;
    LHCb::MuonTileID m_tile;
};

typedef std::vector<Hlt1MuonHit*> Hlt1MuonHits;
typedef std::vector<const Hlt1MuonHit*> Hlt1ConstMuonHits;
typedef const Gaudi::Range_<Hlt1MuonHits> Hlt1MuonHitRange;
typedef const Gaudi::Range_<Hlt1ConstMuonHits> Hlt1ConstMuonHitRange;

#endif // HLT1MUONHIT_H
