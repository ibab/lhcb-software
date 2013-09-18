// Include files 

// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/PhysicalConstants.h"

// include files
#include<math.h>

// local
#include "PatGenericFitter.h"

#define sign(a)  ((a)>0?1:((a)<0?-1:0))

//-----------------------------------------------------------------------------
// Implementation file for class : PatGenericFitter
//
// 2005-10-18 : Tomas LASTOVICKA
// 2008-02-11 : Kurt Rinnert (port to Tf framework)
//-----------------------------------------------------------------------------

namespace Tf {
  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  PatGenericFitter::PatGenericFitter( PatVeloRHitManager* /* rHitManager */,
                                      PatVeloPhiHitManager* /* phiHitManager */,
      bool alignment ) 
    : 
    //m_rHitManager(rHitManager)
    //, m_phiHitManager(phiHitManager)
      m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi)
    , m_valid(true)
    , m_align(alignment)
    {
      m_rCoord.reserve(42);
      m_pCoord.reserve(42);
      m_par[0] = 0.;
      m_par[1] = 0.;
      m_par[2] = 0.;
      m_par[3] = 0.;  

    }

  //=============================================================================
  // Destructor
  //=============================================================================
  PatGenericFitter::~PatGenericFitter() {} 

  //=============================================================================
  // Various class methods
  //=============================================================================
  void PatGenericFitter::reset() {
    std::vector<PatVeloRHit*>::iterator   cR0 = m_rCoord.begin();  
    std::vector<PatVeloPhiHit*>::iterator cP0 = m_pCoord.begin();
    double R = (*cR0)->coordHalfBox();
    //    double dto = m_innerDTO;      
    unsigned int zone = (*cP0)->zone();
    //    if ( 1 == zone ) dto = m_outerDTO;
    
    double phi = m_angleUtils.add((*cP0)->coordHalfBox(),(*cP0)->sensor()->halfboxPhiOffset(zone,R));
    
    m_par[0] = R*cos(phi);
    m_par[1] = R*sin(phi);
    m_par[2] = 0.;
    m_par[3] = 0.;    
  } 

  void PatGenericFitter::addPair( PatVeloRHit* cR, PatVeloPhiHit* cP ) {    
    m_rCoord.push_back(cR);
    m_pCoord.push_back(cP);   
  }

  bool PatGenericFitter::shareClustersLite( PatGenericFitter* fitter ) {
    std::vector<PatVeloRHit*>&   rCoord2 = fitter->rCoords();
    std::vector<PatVeloPhiHit*>& pCoord2 = fitter->phiCoords();  

    std::vector<PatVeloRHit*>::iterator itRSeed1;
    std::vector<PatVeloRHit*>::iterator itRSeed2;    
    for (itRSeed1 = m_rCoord.begin(); m_rCoord.begin()+1 != itRSeed1; ++itRSeed1 ) {
      for (itRSeed2 = rCoord2.begin(); rCoord2.begin()+1 != itRSeed2; ++itRSeed2 ) {
        if ( (*itRSeed2) == (*itRSeed1) && (*itRSeed1) != 0 ) return true;
      }
    }
    std::vector<PatVeloPhiHit*>::iterator itPhiSeed1;
    std::vector<PatVeloPhiHit*>::iterator itPhiSeed2;    
    for (itPhiSeed1 = m_pCoord.begin(); m_pCoord.begin()+1 != itPhiSeed1; ++itPhiSeed1 ) {
      for (itPhiSeed2 = pCoord2.begin(); pCoord2.begin()+1 != itPhiSeed2; ++itPhiSeed2 ) {
        if ( (*itPhiSeed2) == (*itPhiSeed1) && (*itPhiSeed1) != 0 ) return true;
      }
    }    
    return false;
  }

  double PatGenericFitter::lastSensorZ() {
    std::vector<PatVeloRHit*>::iterator itRC;
    double zmax = -1000.0;    
    for (itRC = m_rCoord.begin(); m_rCoord.end() != itRC; ++itRC) {
      if ((*itRC) == 0) continue;      
      if ((*itRC)->z() > zmax) zmax = (*itRC)->z();   
    }
    std::vector<PatVeloPhiHit*>::iterator itPC;
    for (itPC = m_pCoord.begin(); m_pCoord.end() != itPC; ++itPC) {
      if ((*itPC) == 0) continue;
      if ((*itPC)->z() > zmax) zmax = (*itPC)->z();
    }
    return zmax;    
  }

  double PatGenericFitter::firstSensorZ() {
    std::vector<PatVeloRHit*>::iterator itRC;
    double zmin = 2000.0;    
    for (itRC = m_rCoord.begin(); m_rCoord.end() != itRC; ++itRC) {
      if ((*itRC) == 0) continue;      
      if ((*itRC)->z() < zmin) zmin = (*itRC)->z();   
    }
    std::vector<PatVeloPhiHit*>::iterator itPC;
    for (itPC = m_pCoord.begin(); m_pCoord.end() != itPC; ++itPC) {
      if ((*itPC) == 0) continue;
      if ((*itPC)->z() < zmin) zmin = (*itPC)->z();
    }
    return zmin;    
  }

  const DeVeloSensor* PatGenericFitter::lastSensor() {
    std::vector<PatVeloRHit*>::iterator itRC;
    double zmax = -1000.0;    
    const DeVeloSensor* sensor = 0;    
    for (itRC = m_rCoord.begin(); m_rCoord.end() != itRC; ++itRC) {
      if ((*itRC) == 0) continue;
      if ((*itRC)->z() > zmax) { 
        zmax = (*itRC)->z(); 
        sensor=(*itRC)->sensor(); 
      }      
    }
    std::vector<PatVeloPhiHit*>::iterator itPC;
    for (itPC = m_pCoord.begin(); m_pCoord.end() != itPC; ++itPC) {
      if ((*itPC) == 0) continue;
      if ((*itPC)->z() > zmax) {
        zmax = (*itPC)->z();
        sensor = (*itPC)->sensor();        
      }
    }    
    return sensor;    
  }

  void PatGenericFitter::markUsed() {
    std::vector<PatVeloRHit*>::iterator itRC;
    for (itRC = m_rCoord.begin(); m_rCoord.end() != itRC; ++itRC) {
      if ((*itRC) == 0) continue;
      (*itRC)->hit()->setStatus(HitBase::UsedByVeloGeneric);
    }
    std::vector<PatVeloPhiHit*>::iterator itPC;
    for (itPC = m_pCoord.begin(); m_pCoord.end() != itPC; ++itPC) {
      if ((*itPC) == 0) continue;
      (*itPC)->hit()->setStatus(HitBase::UsedByVeloGeneric);
    }    
  }

  void PatGenericFitter::sortCoordinates() {
    std::sort( m_rCoord.begin(), m_rCoord.end(),
        VeloRHit::DecreasingByZ() );
    std::sort( m_pCoord.begin(), m_pCoord.end(),
        VeloPhiHit::DecreasingByZ() );    
  }

  double PatGenericFitter::rExtrap(double z) { 
    double x = m_par[0]+m_par[2] * z;
    double y = m_par[1]+m_par[3] * z;
    return sqrt(x*x+y*y);
  }

  double PatGenericFitter::pExtrap(double z) { 
    double x = m_par[0]+m_par[2] * z;
    double y = m_par[1]+m_par[3] * z;
    double ata = atan2(y,x);    
    return ata;    
  }

  //=============================================================================
  // The main fitting method
  //=============================================================================

  StatusCode PatGenericFitter::fit() {

    if ( 2 > m_rCoord.size() ) return StatusCode::FAILURE;

    const int m_maxIter = 10;  
    bool iter = true;        
    int nIter = 0;           

    while ( iter ) {            
      ++nIter;
      fillMatrix();
      bool sc = solve();            
      if ( !sc ) return StatusCode::FAILURE;            

      if ( 0.0001 >  maxVariation() ) break;            
      if ( m_maxIter < nIter ) return StatusCode::FAILURE;    
    }          
    
    return StatusCode::SUCCESS;  

  }

  void PatGenericFitter::fillMatrix() {

    std::vector<PatVeloRHit*>::iterator   itR;
    std::vector<PatVeloPhiHit*>::iterator itP;
    itR = m_rCoord.begin();
    itP = m_pCoord.begin();

    const int m_Entries = m_rCoord.size();
    int iLoop;

    double a, b, c;  
    double z, xx, yy, cc;
    //double err, multi, R;
    double multi, R;

    for ( unsigned int ii=0; 9 > ii; ++ii ) {
      if ( 4 > ii ) m_vector[ii] = 0;
      *(m_matrix.begin()+ii) = 0;
    }

    for ( iLoop=0; m_Entries > iLoop; ++iLoop ) {
      if ( 0 != (*itR) ) {
        PatVeloRHit* coordR = (*itR);
        z = coordR->z();
        if ( m_align ) {        
          cc = coordR->coordHalfBox();
        } else {
          cc = coordR->coordIdeal();
        }      
        a = m_par[0]+m_par[2]*z;
        b = m_par[1]+m_par[3]*z;
        R = sqrt(a*a+b*b);
        c = -cc*R;
      
        multi = 1./R;      
        multi *= multi;

        multi *= coordR->weight();
        *(m_matrix.begin()+0) += multi*a*a;
        *(m_matrix.begin()+1) += multi*a*b;
        *(m_matrix.begin()+2) += multi*b*b;
        *(m_matrix.begin()+3) += multi*a*a*z;
        *(m_matrix.begin()+4) += multi*a*b*z;;
        *(m_matrix.begin()+5) += multi*a*a*z*z;
        *(m_matrix.begin()+6) += multi*a*b*z;
        *(m_matrix.begin()+7) += multi*b*b*z;
        *(m_matrix.begin()+8) += multi*a*b*z*z; 
        *(m_matrix.begin()+9) += multi*b*b*z*z;

        multi *= -c;    
        m_vector[0] += multi*a;
        m_vector[1] += multi*b;
        m_vector[2] += multi*a*z;
        m_vector[3] += multi*b*z;      
      }

      if ( 0 != (*itP) ) {
        PatVeloPhiHit* coordP = (*itP);
        z = coordP->z();
        xx = m_par[0]+m_par[2]*z;
        yy = m_par[1]+m_par[3]*z;
        if ( m_align ) {        
          cc = pseudoPhi(coordP);
          cc += coordP->coordHalfBox() - coordP->coordIdeal();
          c   = coordP->sensor()->halfboxDistToOrigin(coordP->zone());
        } else {
          cc = pseudoPhi(coordP);
          c  = coordP->sensor()->idealDistToOrigin(coordP->zone());
        }      
        a =  sin(cc);      
        b = -cos(cc);

        multi = 1.0/(coordP->hit()->variance()*(xx*xx+yy*yy));
        *(m_matrix.begin()+0) += multi*a*a;
        *(m_matrix.begin()+1) += multi*a*b;
        *(m_matrix.begin()+2) += multi*b*b;
        *(m_matrix.begin()+3) += multi*a*a*z;
        *(m_matrix.begin()+4) += multi*a*b*z;;
        *(m_matrix.begin()+5) += multi*a*a*z*z;
        *(m_matrix.begin()+6) += multi*a*b*z;
        *(m_matrix.begin()+7) += multi*b*b*z;
        *(m_matrix.begin()+8) += multi*a*b*z*z; 
        *(m_matrix.begin()+9) += multi*b*b*z*z;

        multi *= -c;
        m_vector[0] += multi*a;
        m_vector[1] += multi*b;
        m_vector[2] += multi*a*z;
        m_vector[3] += multi*b*z;    
      }

      ++itR;
      ++itP;
    }
  }

  bool PatGenericFitter::solve() {

    bool sc = m_matrix.Invert();
    if ( !sc ) return sc;
    m_par_old = m_par;  
    m_par = m_matrix*m_vector;
    return true;  
  }

  double PatGenericFitter::maxVariation() { 
    Gaudi::Vector4 dif = m_par-m_par_old;
    double max_x = dif[0];
    double max_y = dif[1];  
    if ( sign(dif[0]) == sign(dif[2]) ) {
      max_x += 800.*dif[2];    
    } else {
      max_x += -200.*dif[2];
    }
    if ( sign(dif[1]) == sign(dif[3]) ) {
      max_x += 800.*dif[3];
    } else {
      max_x += -200.*dif[3];
    }
    max_x = fabs(max_x);
    max_y = fabs(max_y);
    if ( max_x > max_y ) return max_x;
    return max_y;
  }

  double PatGenericFitter::pseudoPhi(const PatVeloPhiHit* phiHit)
  {
    double pp = phiHit->sensor()->angleOfStrip(phiHit->hit()->strip(),phiHit->hit()->interStripFraction());
    if ( phiHit->sensor()->isDownstream() ) pp  = -pp;
    if ( phiHit->sensor()->isRight() )      pp += Gaudi::Units::pi;
    return pp;
  }

}

