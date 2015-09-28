
#include "SeedLineFit.h"
#include "TsaKernel/SeedTrack.h"

using namespace Tf::Tsa;

int SeedLineFit::fit( SeedTrack* seed) const 
{
  
  //
  //  Fit a line y = y0 + s*(z-m_z0) through hits on seed track
  //-------------------------------------------------------------------------

  std::vector<SeedPnt>& pnts = seed->yPnts();

  msgStream() << MSG::DEBUG 
              << " -> SeedTrack before fit " << *seed << endmsg;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) 
  {
    msgStream() << MSG::DEBUG  << "  -> SeedPtn " << *it << endmsg;
  }

  if ( pnts.size() < 2 ) return -1;
  bool update = false;
  unsigned int nIt = 0;
  do {
    ++nIt;
    update = 0;

    reset();
    int ny = 0;

    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      ++ny;
      double y = pnt.hit()->y();
      if ( pnt.hit()->OT() ) y += pnt.sign()*pnt.hit()->r()*m_scth;
      double z = pnt.hit()->z();
      double dz2 = (z-m_z0)*(z-m_z0);
      double dd = pnt.hit()->tfHit()->hit()->variance()*m_scth*m_scth;
      if ( pnt.hit()->OT() ) {
        if ( pnt.sign() == 0 ) dd = m_scth*m_scth;
      }
      double invDD = 1.0/dd;
      m_a += y*(z-m_z0)*invDD;
      m_b += (z-m_z0)* invDD;
      m_c += dz2 * invDD;
      m_d += y * invDD;
      m_e += invDD;
    }
    if ( ny < 2 ) return -2;
    double invDet = 1.0/(m_c*m_e - m_b*m_b);
    if ( fabs( 1.0/invDet ) < 1.e-12 || fabs(m_b) < 1.e-12 ) {
      return -3;
    } else {
      const double s = ( m_a*m_e - m_b*m_d ) * invDet;
      const double y0 = ( m_a - m_c*s ) / m_b;
      seed->setNy( ny );
      seed->setYParams(s,y0);
      seed->setYerr( 0,  m_c * invDet );
      seed->setYerr( 1, -m_b * invDet );
      seed->setYerr( 2,  m_e * invDet );
    }

    double dMax = 0.;
    std::vector<SeedPnt>::iterator itMax;
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      if ( seed->sector() > 5 && pnt.hit()->OT() == 0 ) continue;
      double y = pnt.hit()->y();
      double ySeed = seed->y(pnt.hit()->z(),m_z0);
      double dy = ( y - ySeed ) / ( sqrt(pnt.hit()->tfHit()->hit()->variance())*m_scth );
      if ( pnt.hit()->OT() ) {
        double r = pnt.hit()->r()*m_scth;
        if ( pnt.sign() == 0 ) {
          update = 1;
          pnt.setSign( 1 );
          if ( fabs( y+r - ySeed ) > fabs( y-r - ySeed ) ) pnt.setSign( -1 );
        } else {
          if ( fabs( y+pnt.sign()*r - ySeed ) > fabs( y-pnt.sign()*r - ySeed ) ) {
            update = 1;
            pnt.flipSign();
          }
        }
        dy = ( y + pnt.sign()*r - ySeed ) / ( sqrt(pnt.hit()->tfHit()->hit()->variance())*m_scth );
      }
      if ( fabs(dy) > dMax ) {
        dMax = fabs(dy);
        itMax = it;
      }
    }
    if ( update == 0 && dMax > 4. ) {
      update = 1;
      (*itMax).setSkip( 1 );
    }
  } while ( update && nIt < m_maxIter );
  if ( nIt == m_maxIter ) return -4;

  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    double y = pnt.hit()->y();
    if ( pnt.hit()->OT() ) y += pnt.sign()*pnt.hit()->r()/m_sth;
    const double z = pnt.hit()->z();
    pnt.setCoord( y );
    pnt.setZ( z );
    if ( pnt.skip() ) continue;
    const double ch = y - seed->y(z,m_z0);
    chi2 += ch*ch*m_sth*m_sth / pnt.hit()->tfHit()->hit()->variance();
  }
  seed->setYChi2( chi2 );

  msgStream() << MSG::DEBUG  << " -> SeedTrack after fit " << *seed << endmsg;

  return nIt;
}
