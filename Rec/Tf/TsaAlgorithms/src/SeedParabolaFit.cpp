
#include "SeedParabolaFit.h"
#include "TsaKernel/SeedTrack.h"

using namespace Tf::Tsa;

int SeedParabolaFit::fit( SeedTrack* seed, const double &csth ) const {
//-------------------------------------------------------------------------
//  Fit a parabola x = x0 + s*(z-m_z0) + t*(z-m_z0)**2 through hits on seed track
//-------------------------------------------------------------------------

  std::vector<SeedPnt>& pnts = seed->xPnts(); 
  if ( pnts.size() < 3 ) return -1;
  bool update = 0;
  unsigned int nIt = 0;
  
  do {
    ++nIt;
    update = 0;
    reset();
    int nx = 0;
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      ++nx;
      double x = pnt.hit()->x();
      if ( pnt.hit()->OT() ) x += pnt.sign()*pnt.hit()->r()*csth;
      double z = pnt.hit()->z();
      double dz2 = (z-m_z0)*(z-m_z0);
      double dz3 = dz2*(z-m_z0);
      double dz4 = dz3*(z-m_z0);
      double dd;
      if ( pnt.hit()->OT() ) {
        dd = pnt.hit()->tfHit()->hit()->variance()*csth*csth;
        if ( pnt.sign() == 0 ) dd = csth*csth;
      }
      else {
        dd = pnt.hit()->tfHit()->hit()->variance();
      }
      double invdd = 1.0/dd;
      m_a += x * invdd;
      m_b += invdd;
      m_c += (z-m_z0) *invdd;
      m_d += dz2 *invdd;
      m_e += x*(z-m_z0) *invdd;
      m_f += dz3 *invdd;
      m_g += x*dz2 *invdd;
      m_h += dz4 * invdd;

    }
    if ( nx < 3 ) return -2;
    if ( fabs( (m_d*m_d-m_f*m_c)*(m_f*m_d-m_h*m_c)-(m_f*m_f-m_h*m_d)*(m_d*m_c-m_f*m_b) ) < 1.e-12 ||
         fabs( m_f*m_c-m_d*m_d ) < 1.e-12 || fabs(m_d) < 1.e-12 ) return -3;

    const double tx0 = x0();
    const double ts = s(tx0);
    seed->setNx( nx );
    seed->setXParams(t(tx0,ts),ts,tx0);
    
    double dMax = 0.;
    std::vector<SeedPnt>::iterator itMax;
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      if ( seed->sector() > 5 && pnt.hit()->OT() == 0 ) continue;
      double x = pnt.hit()->x();
      double z = pnt.hit()->z();
      double xSeed = seed->x(z,m_z0);
      double dx = (x - xSeed) / sqrt(pnt.hit()->tfHit()->hit()->variance());
      if ( pnt.hit()->OT() ) {
        double r = pnt.hit()->r()*csth;
        if ( pnt.sign() == 0 ) {
          update = 1;
          pnt.setSign( 1 );
          if ( fabs( x+r - xSeed ) > fabs( x-r - xSeed ) ) pnt.setSign( -1 );
        } else {
          if ( fabs( x+pnt.sign()*r - xSeed ) > fabs( x-pnt.sign()*r - xSeed ) ) {
            update = 1;
            pnt.flipSign();
          }
        }
        dx = (x + pnt.sign()*r - xSeed)*(1.0/csth) / sqrt(pnt.hit()->tfHit()->hit()->variance());
      }
      if ( fabs(dx) > dMax ) {
        dMax = fabs(dx);
        itMax = it;
      }
    }
    if ( update == 0 && dMax > m_outlierCut ) {
      update = 1;
      (*itMax).setSkip( 1 );
    }
  } while ( update && nIt < m_maxIter );
  if ( nIt == m_maxIter ) return -4;
  
  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    double x = pnt.hit()->x();
    if ( pnt.hit()->OT() ) x += pnt.sign()*pnt.hit()->r()*csth;
    pnt.setCoord( x );
    if ( pnt.skip() ) continue;
    double z = pnt.hit()->z();
    double ch = x - seed->x(z,m_z0);
    ch = ch * ch / pnt.hit()->tfHit()->hit()->variance();
    if (pnt.hit()->OT()) ch /= csth * csth;
    chi2 += ch;
  }
  seed->setXChi2( chi2 );
  return nIt;
}


int SeedParabolaFit::refit( SeedTrack* seed, const double &csth ) const{
//-------------------------------------------------------------------------
//  Fit a parabola x = x0 + s*(z-m_z0) + t*(z-m_z0)**2 through hits on seed track
//-------------------------------------------------------------------------

  std::vector<SeedPnt>& pnts = seed->xPnts(); 
  if ( pnts.size() < 3 ) return -1;

  reset();
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    if ( pnt.skip() ) continue;
    double x = pnt.coord();
    double z = pnt.z();
    double dz2 = (z-m_z0)*(z-m_z0);
    double dz3 = dz2*(z-m_z0);
    double dz4 = dz3*(z-m_z0);
    double dd = pnt.hit()->tfHit()->hit()->variance();
    if ( pnt.hit()->OT() ) dd *= csth*csth;
    double invDD = 1./dd;
    m_a += x * invDD;
    m_b += 1. * invDD;
    m_c += (z-m_z0) * invDD;
    m_d += dz2 * invDD;
    m_e += x*(z-m_z0) *invDD;
    m_f += dz3 *invDD;
    m_g += x*dz2 *invDD;
    m_h += dz4 * invDD;
  }
  if ( fabs( (m_d*m_d-m_f*m_c)*(m_f*m_d-m_h*m_c)-(m_f*m_f-m_h*m_d)*(m_d*m_c-m_f*m_b) ) < 1.e-12 ||
       fabs( m_f*m_c-m_d*m_d ) < 1.e-12 || fabs(m_d) < 1.e-12 ) return -3;


  const double tx0 = x0();
  const double ts = s(tx0);
  seed->setXParams(t(tx0,ts),ts,tx0);
  double det = m_b*(m_d*m_h-m_f*m_f) + m_c*(m_f*m_d-m_c*m_h) + m_d*(m_c*m_f-m_d*m_d);

  //error matrix
  seed->setXerr( 0, (m_d*m_h-m_f*m_f) / det );
  seed->setXerr( 1, (m_d*m_f-m_c*m_h) / det );
  seed->setXerr( 2, (m_c*m_f-m_d*m_d) / det );
  seed->setXerr( 3, (m_b*m_h-m_d*m_d) / det );
  seed->setXerr( 4, (m_c*m_d-m_b*m_f) / det );
  seed->setXerr( 5, (m_b*m_d-m_c*m_c) / det );

  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    if ( pnt.skip() ) continue;
    double ch = pnt.coord() - seed->x(pnt.z(),m_z0);
    ch = ch * ch / pnt.hit()->tfHit()->hit()->variance();
    if (pnt.hit()->OT()) ch /= csth * csth;
    chi2 += ch;
  }
  seed->setXChi2( chi2 );
  return 1;
}
