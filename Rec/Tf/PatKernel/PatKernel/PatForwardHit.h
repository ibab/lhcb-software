#ifndef PATFORWARD_PatForwardHit_H
#define PATFORWARD_PatForwardHit_H 1

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
                            public LHCb::MemPoolAlloc<PatForwardHit>
{
private:    
    // set relies standards-conforming on integral promotion of  bool; false -> 0 , true -> 1
    // hence unsigned(-b) gives 0 for false, and ~0 for true
    unsigned set(bool b, unsigned mask, unsigned val) { return    ( val     & ~mask ) | ( unsigned(-b) & mask ) ; }
    void     set(bool b, unsigned mask)               { m_flags = ( m_flags & ~mask ) | ( unsigned(-b) & mask ) ; }
    // void set(bool b, unsigned mask) { m_flags  ^= (unsigned(-b) ^ m_flags) & mask; }
    // void set(bool b, unsigned mask) { if (b) m_flags |=  mask ; else m_flags &= ~mask ; } 
public:

    /// Constructor from an OT hit
  PatForwardHit ( const Tf::OTHit & otHit ) :
    Tf::HitExtension<Tf::LineHit>(otHit),
    m_x{otHit.xMid()},
    m_z{otHit.zMid()},
    m_driftDistance{otHit.driftDistance()},
    m_flags{ set( true, 0x8000u , 4 * otHit.station() + otHit.layer() ) } {
  };

  /// Constructor from an ST hit
  PatForwardHit ( const Tf::STHit & stHit ) :
    Tf::HitExtension<Tf::LineHit>(stHit),
    m_x{stHit.xMid()},
    m_z{stHit.zMid()},
    m_flags{ 4 * stHit.station() + stHit.layer() } {
  };

  // Accessors
  double x()             const { return m_x; }
  double z()             const { return m_z; }
  double driftDistance() const { return m_driftDistance; }
  double projection()    const { return m_projection; }
  bool isOT()            const { return   m_flags & 0x8000u; }
  int  rlAmb()           const { return ( m_flags & 0x4000u ) ? -1  :
                                        ( m_flags & 0x2000u ) ? +1  : 0 ; }
  bool hasPrevious()     const { return   m_flags & 0x1000u; }
  bool hasNext()         const { return   m_flags & 0x0800u; }
  bool isIgnored()       const { return   m_flags & 0x0400u; }
  bool isUsed()          const { return   m_flags & 0x0200u; }
  bool isSelected()      const { return   m_flags & 0x0100u; }
  int  planeCode()       const { return   m_flags & 0x00ffu; }
  int  layer()           const { return   planeCode()%4; }
  //the next is in the baseclass, chasing the 'basehit' pointer, but then it is also 'polluted' by a check for UT,TT...
  bool isStereo()        const { int l = layer(); return (l==1 || l == 2); } // TODO: check whether a 'popcount(l)==1' would be faster...
  bool isX()             const { int l = layer(); return (l==0 || l == 3); }

  // Setters
  void setRlAmb( int rl )                       { m_flags = set( rl!=0, 0x2000u, 
                                                            set( rl <0, 0x4000u, m_flags)); }
  void setHasPrevious( bool hasPrevious )       { set(hasPrevious, 0x1000u ); }
  void setHasNext( bool hasNext )               { set(hasNext,     0x0800u ); }
  void setIgnored( bool isIgnored )             { set(isIgnored,   0x0400u ); }
  void setIsUsed(bool isUsed)                   { set(isUsed,      0x0200u ); }
  void setSelected( bool isSelected )           { set(isSelected,  0x0100u ); }
  void setX( double x )                         { m_x = x; }
  void setZ( double z )                         { m_z = z; }
  void setDriftDistance( double driftDistance ) { m_driftDistance = driftDistance; }
  void setProjection( double proj )             { m_projection = proj; }

private:
  double   m_x = 0;
  double   m_z = 0;
  double   m_driftDistance = 0;
  double   m_projection    = -999.;
  unsigned m_flags         = 0u;
};

typedef PatForwardHit               PatFwdHit;
typedef std::vector<PatForwardHit*> PatFwdHits;

#endif // PATFORWARD_PatForwardHit_H
