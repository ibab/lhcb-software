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

  class PatForwardHit : public Tf::HitExtension<Tf::LineHit>,
                        public LHCb::MemPoolAlloc<PatForwardHit>
  {

  public:

    // Default constructor
    PatForwardHit ( ) :
      Tf::HitExtension<Tf::LineHit>(),
      m_rlAmb(0),
      m_isUsed(false),
      m_isIgnored(false),
      m_isSelected(false),
      m_driftDistance(0),
      m_x(0),
      m_z(0),
      m_projection(-999),
      m_hasNext(false),
      m_hasPrevious(false),
      m_planeCode( 0 ) {
    };

      /// Constructor from an OT hit
    PatForwardHit ( const Tf::OTHit & otHit ) :
      Tf::HitExtension<Tf::LineHit>(otHit),
      m_rlAmb(0),
      m_isUsed(false),
      m_isIgnored(false),
      m_isSelected(false),
      m_driftDistance(otHit.driftDistance()),
      m_x(otHit.xMid()),
      m_z(otHit.zMid()),
      m_projection(-999),
      m_hasNext(false),
      m_hasPrevious(false),
      m_planeCode( 4 * otHit.station() + otHit.layer() ) {
    };

    /// Constructor from an ST hit
    PatForwardHit ( const Tf::STHit & stHit ) :
      Tf::HitExtension<Tf::LineHit>(stHit),
      m_rlAmb(0),
      m_isUsed(false),
      m_isIgnored(false),
      m_isSelected(false),
      m_driftDistance(0),
      m_x(stHit.xMid()),
      m_z(stHit.zMid()),
      m_projection(-999),
      m_hasNext(false),
      m_hasPrevious(false),
      m_planeCode( 4 * stHit.station() + stHit.layer() ) {
    };

    /// Desctructor
    ~PatForwardHit() { }

    // Accessors
    int  rlAmb()           const { return m_rlAmb; }
    bool isIgnored()       const { return m_isIgnored; }
    bool isSelected()      const { return m_isSelected; }
    bool isUsed()          const { return m_isUsed; }
    double driftDistance() const { return m_driftDistance; }
    double x()             const { return m_x; }
    double z()             const { return m_z; }
    double projection()    const { return m_projection; }
    int  planeCode()       const { return m_planeCode; }
    bool hasNext()         const { return m_hasNext; }
    bool hasPrevious()     const { return m_hasPrevious; }


    // Setters
    void setIsUsed(bool isUsed)                     { m_isUsed = isUsed; }
    void setHasNext( bool hasNext )                 { m_hasNext = hasNext; }
    void setHasPrevious( bool hasPrevious )         { m_hasPrevious = hasPrevious; }
    void setRlAmb( int rl )                         { m_rlAmb = rl; }
    void setIgnored( bool isIgnored )               { m_isIgnored = isIgnored; }
    void setSelected( bool isSelected )             { m_isSelected = isSelected; }
    void setDriftDistance( double driftDistance )   { m_driftDistance = driftDistance; }
    void setX( double x )                           { m_x = x; }
    void setZ( double z )                           { m_z = z; }
    void setProjection( double proj )               { m_projection = proj; }

  private:
    int     m_rlAmb;
    bool    m_isUsed;
    bool    m_isIgnored;
    bool    m_isSelected;
    double  m_driftDistance;
    double  m_x;
    double  m_z;
    double  m_projection;
    bool    m_hasNext;
    bool    m_hasPrevious;
    int     m_planeCode;
  };

  typedef PatForwardHit               PatFwdHit;
  typedef std::vector<PatForwardHit*> PatFwdHits;


#endif // PATFORWARD_PatForwardHit_H

