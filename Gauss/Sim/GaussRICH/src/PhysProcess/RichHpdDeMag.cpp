#include <gsl/gsl_math.h>
#include "GaudiKernel/SmartDataPtr.h"
#include "DetDesc/DetectorElement.h"
#include "RichG4GaussPathNames.h"

#include "RichHpdDeMag.h"

RichHpdDeMag::RichHpdDeMag(){ ; }

RichHpdDeMag::RichHpdDeMag(IDataProviderSvc* detSvc, 
			   int hpdnumb, int richdetnumb)
{  
  m_curHpdNumb=hpdnumb;
  m_curRichDetNumb=richdetnumb;

  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName );
  if( Rich1DE ) { 
    m_RichHpdQWToSiMaxDist = Rich1DE->param<double>("RichHpdQWToSiMaxDist");
    m_activeRadius = Rich1DE->param<double>("RichHpdActiveInpRad");
    m_Rcurv = Rich1DE->param<double>("RichHpdQWInnerSphericalRadius");
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

void RichHpdDeMag::setCurrentDemagnification(TabulatedProperty::Table& a ){
  m_demag = a;
}

G4ThreeVector RichHpdDeMag::getPositionOnAnode( double CathodeRadius,
                                                double CathodePhi ) {
  G4ThreeVector v(0);
  //assumes that r and phi are stored in increasing order in the class.
  //this means in the db they should be in increasing order.

  int length = m_demag.size()-1;
  if(length<1) {
    std::cout <<"RichHpdDeMag ERROR: demag table empty. "<<std::endl;
    return v;
  }

  double binwidth = m_activeRadius / length;
  int bin         = int( CathodeRadius / binwidth);

  std::pair<double, double> pair1 = m_demag.at(bin);
  std::pair<double, double> pair2 = m_demag.at(bin+1);
  double r1 = pair1.first;
  double r2 = pair2.first;
  double p1 = pair1.second;
  double p2 = pair2.second;
  double result_r  = (r2-r1)/binwidth * (CathodeRadius - binwidth*bin) + r1;
  double result_phi= (p2-p1)/binwidth * (CathodeRadius - binwidth*bin) + p1;
  double new_phi=0;

  //  if( CathodePhi+result_phi+ 3.1416 >  6.283 ) new_phi = CathodePhi+result_phi - 3.1416;
  // if( CathodePhi+result_phi+ 3.1416 <= 6.283 ) new_phi = CathodePhi+result_phi + 3.1416;
  // avoiding hard coded values SE 26-10-2006.
  if( CathodePhi+result_phi+ M_PI  >  2 * M_PI   ) new_phi = CathodePhi+result_phi - M_PI;
  if( CathodePhi+result_phi+ M_PI <=  2 * M_PI ) new_phi = CathodePhi+result_phi + M_PI;

  // std::cout<<" pi and two pi value test "<< M_PI<<"  "<<M_2_PI<<std::endl;
  //std::cout <<"RichHpdDeMag r: "
  // 	    <<CathodeRadius<<" "<<r1<<"-"<<result_r<<"-"<<r2
  // 	    <<"   bin="<<bin<<"/"<<length<<std::endl;
  //std::cout <<"RichHpdDeMag phi: "
  //   	    <<CathodePhi<<" "<<p1<<"-"<<result_phi<<"-"<<p2<<std::endl;

  v.set( result_r * std::cos(new_phi), result_r * std::sin(new_phi), 
	 -(m_RichHpdQWToSiMaxDist-m_Rcurv) );

  return v;
}
