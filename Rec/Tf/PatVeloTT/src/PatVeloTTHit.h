#ifndef PATVELOTT_PATVELOTTHIT_H 
#define PATVELOTT_PATVELOTTHIT_H 1

#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/STHit.h"
#include "Kernel/MemPoolAlloc.h"

namespace Tf {

  /** @class PatVeloTTHit
   *  Extended hit class for PatVeloTT algorithms
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-019
   **/

  class PatVeloTTHit : public HitExtension<LineHit>,
                       public LHCb::MemPoolAlloc<Tf::PatVeloTTHit>
  {

  public:

    /// Constructor from an ST hit
    PatVeloTTHit ( const STHit & stHit ) :
      HitExtension<LineHit>(stHit),
      m_x(stHit.x()),
      m_z(stHit.z()),
      m_planeCode(2 * stHit.station() + (stHit.layer()%2)){}

    /// Desctructor
    ~PatVeloTTHit() { }

    /// Accessors
    double x()                    const { return m_x; }
    double z()                    const { return m_z; }
    int    mask()                 const { return 1 << m_planeCode; }
    int    planeCode()            const { return m_planeCode; }


    /// Setters
    void setX( double x )                     { m_x = x; }
    void setZ( double z )                     { m_z = z; }

  private:
    double  m_x;
    double  m_z;
    int     m_planeCode;
  };

  typedef std::vector<PatVeloTTHit*> PatVeloTTHits;

}

#endif // PATVELOTT_PATVELOTTHIT_H
