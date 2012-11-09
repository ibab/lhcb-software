#include "GaudiKernel/SmartDataPtr.h"
#include "DetDesc/DetectorElement.h"
#include "GaussRICH/RichG4GaussPathNames.h"

#include "RichHpdDeMag.h"

RichHpdDeMag::RichHpdDeMag(){ }

RichHpdDeMag::RichHpdDeMag(IDataProviderSvc* detSvc, 
			   int hpdnumb, int richdetnumb)
{  
  m_curHpdNumb     = hpdnumb;
  m_curRichDetNumb = richdetnumb;

  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName );
  if( Rich1DE ) { 
    m_RichHpdQWToSiMaxDist = Rich1DE->param<double>("RichHpdQWToSiMaxDist");
    m_activeRadius         = Rich1DE->param<double>("RichHpdActiveInpRad");
    m_Rcurv                = Rich1DE->param<double>("RichHpdQWInnerSphericalRadius");
  }  
}

RichHpdDeMag::~RichHpdDeMag(){;}

std::vector<double> RichHpdDeMag::HpdDemagVect() const
{
  std::vector<double>Demag(2);
  Demag[0]= m_richHpdDemagLinearTerm;
  Demag[1]= m_richHpdDemagQuadraticTerm;
  return Demag;
}

void RichHpdDeMag::setCurrentHPDDemag(const std::vector<double> & demagVect) 
{
  m_richHpdDemagLinearTerm    = demagVect[0];
  m_richHpdDemagQuadraticTerm = demagVect[1]; 
}

///////////////////////NEW STUFF USING Rich1DTabFunc///////////////////////
void RichHpdDeMag::setCurrentDemagnification( const Rich1DTabFunc* r, 
					      const Rich1DTabFunc* phi )
{
  if(r->nDataPoints()==0) {
    std::cout <<"RichHpdDeMag ERROR: r demag Rich1DTabFunc empty. "<<std::endl;
    return;
  }
  if(phi->nDataPoints()==0) {
    std::cout <<"RichHpdDeMag ERROR: phi demag Rich1DTabFunc empty. "<<std::endl;
    return;
  }

  m_Rdemag   = r;
  m_Phidemag = phi;

}

/////////////////////////////////////////////////////////////////////////////////////////
G4ThreeVector RichHpdDeMag::getPositionOnAnode( double CathodeRadius, double CathodePhi )
{
  G4ThreeVector v(0);

  double result_r  = m_Rdemag  ->value(CathodeRadius);
  double result_phi= m_Phidemag->value(CathodeRadius);//must be CathodeRadius not CathodePhi

  double new_phi=0;
  if( CathodePhi+result_phi+ M_PI  >  2 * M_PI ) new_phi = CathodePhi+result_phi - M_PI;
  if( CathodePhi+result_phi+ M_PI <=  2 * M_PI ) new_phi = CathodePhi+result_phi + M_PI;

//   std::cout <<"RichHpdDeMag r: "<<CathodeRadius<<" -> "<<result_r
// 	    <<"   phi: "<<CathodePhi<<" -> "<<new_phi<<std::endl;

  v.set( result_r * std::cos(new_phi),
         result_r * std::sin(new_phi),
         -(m_RichHpdQWToSiMaxDist-m_Rcurv) );

  return v;
}
