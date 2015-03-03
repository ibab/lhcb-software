#ifndef PATFORWARD_PatForwardHit_H
#define PATFORWARD_PatForwardHit_H 1

#include <cstdint>

#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/OTHit.h"
#include "TfKernel/STHit.h"
#include "Kernel/MemPoolAlloc.h"


/** @class PatForwardHit
 *  Extended hit class for PatForward and PatSeed algorithms
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 **/

class PatForwardHit 
#ifndef __GCCXML__
final
#endif
                          : public Tf::HitExtension<Tf::LineHit>,
                            public LHCb::MemPoolAlloc<PatForwardHit,256,false>
                            // WARNING: the 'false' in the above insures that no mutex is
                            //          aquired when accessing the static memory pool
                            //          Hence, if multiple threads 'new' PatForwardHits,
                            //          bad things WILL happen...
{
private:    
    // set relies standards-conforming on integral promotion of  bool; false -> 0 , true -> 1
    // hence unsigned(-b) gives 0 for false, and ~0 for true
    static unsigned set(bool b, unsigned mask, unsigned val) noexcept { return    ( val     & ~mask ) | ( unsigned(-b) & mask ) ; }
    void     set(bool b, unsigned mask)               noexcept { m_flags = ( m_flags & ~mask ) | ( unsigned(-b) & mask ) ; }
    // void set(bool b, unsigned mask) { m_flags  ^= (unsigned(-b) ^ m_flags) & mask; }
    // void set(bool b, unsigned mask) { if (b) m_flags |=  mask ; else m_flags &= ~mask ; } 
public:

    /// Constructor from an OT hit
  PatForwardHit ( const Tf::OTHit & otHit ) :
    Tf::HitExtension<Tf::LineHit>(otHit),
    m_x{otHit.xMid()},
    m_z{otHit.zMid()},
    m_driftDistance{otHit.driftDistance()},
    m_flags{ 0x800u + 4 * otHit.station() + otHit.layer() } {
  };

  /// Constructor from an ST hit
  PatForwardHit ( const Tf::STHit & stHit ) :
    Tf::HitExtension<Tf::LineHit>(stHit),
    m_x{stHit.xMid()},
    m_z{stHit.zMid()},
    m_flags{ 4 * stHit.station() + stHit.layer() } {
  };

  // Accessors
  double x()             const noexcept { return m_x; }
  double z()             const noexcept { return m_z; }
  double driftDistance() const noexcept { return m_driftDistance; }
  double projection()    const noexcept { return m_projection; }
  bool isOT()            const noexcept { return m_flags & 0x800u; }
  int  rlAmb()           const noexcept { return int(m_flags << 21) >> 30; }
  bool hasPrevious()     const noexcept { return m_flags & 0x100u; }
  bool hasNext()         const noexcept { return m_flags & 0x080u; }
  bool isIgnored()       const noexcept { return m_flags & 0x040u; }
  bool isUsed()          const noexcept { return m_flags & 0x020u; }
  bool isSelected()      const noexcept { return m_flags & 0x010u; }
  int  planeCode()       const noexcept { return m_flags & 0x00fu; }
  int  layer()           const noexcept { return m_flags & 3u; }
  //the next is in the baseclass, chasing the 'basehit' pointer, but then it is also 'polluted' by a check for UT,TT...
  bool isStereo()        const noexcept { return  ((m_flags + 1u) & 2u); }
  bool isX()             const noexcept { return !((m_flags + 1u) & 2u); }

  // Setters
  void setRlAmb( int rl )                       noexcept { m_flags &= ~0x600u; m_flags |= (unsigned(rl) & 3u) << 9; }
  void setHasPrevious( bool hasPrevious )       noexcept { set(hasPrevious, 0x100u ); }
  void setHasNext( bool hasNext )               noexcept { set(hasNext,     0x080u ); }
  void setIgnored( bool isIgnored )             noexcept { set(isIgnored,   0x040u ); }
  void setIsUsed(bool isUsed)                   noexcept { set(isUsed,      0x020u ); }
  void setSelected( bool isSelected )           noexcept { set(isSelected,  0x010u ); }
  void setX( double x )                         noexcept { m_x = x; }
  void setZ( double z )                         noexcept { m_z = z; }
  void setDriftDistance( double driftDistance ) noexcept { m_driftDistance = driftDistance; }
  void setProjection( double proj )             noexcept { m_projection = proj; }

private:
  double   m_x = 0;
  double   m_z = 0;
  double   m_driftDistance = 0;
  double   m_projection    = -999.;
  uint32_t m_flags         = 0u;
};

typedef PatForwardHit               PatFwdHit;
typedef std::vector<PatForwardHit*> PatFwdHits;

#endif // PATFORWARD_PatForwardHit_H
