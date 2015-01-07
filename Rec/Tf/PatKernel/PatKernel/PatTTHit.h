#ifndef PATFORWARD_PATTTHIT_H 
#define PATFORWARD_PATTTHIT_H 1

#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/STHit.h"
#include "Kernel/MemPoolAlloc.h"


  /** @class PatTTHit
   *  Extended hit class for PatAddTT algorithms
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-019
   **/

  class PatTTHit 
#ifndef __GCCXML__
  final 
#endif
                       : public Tf::HitExtension<Tf::LineHit>,
                         public LHCb::MemPoolAlloc<PatTTHit,128,false>
                            // WARNING: the 'false' in the above insures that no mutex is
                            //          aquired when accessing the static memory pool
                            //          Hence, if multiple threads 'new' PatForwardHits,
                            //          bad things WILL happen...
  {

  public:

    /// Constructor from an ST hit
    PatTTHit ( const Tf::STHit & stHit ) :
      Tf::HitExtension<Tf::LineHit>(stHit),
      m_x(stHit.xMid()),
      m_z(stHit.zMid()),
      m_projection(-999),
      m_planeCode( 2 * stHit.station() + (stHit.layer()%2) ) { }

    /// Accessors
    double x()             const { return m_x; }
    double z()             const { return m_z; }   
    int    mask()          const { return 1 << m_planeCode; }
    double projection()    const { return m_projection; }
    int    planeCode()     const { return m_planeCode; }

    /// Setters
    void setX( double x )                     { m_x = x; }
    void setZ( double z )                     { m_z = z; }
    void setProjection( double proj )         { m_projection = proj; }

  private:
    double  m_x;
    double  m_z;
    double  m_projection;
    int     m_planeCode;
 
  };
 
  typedef std::vector<PatTTHit*> PatTTHits;
 

#endif // PATFORWARD_PATTTHit_H
