// $Id: Element.cpp,v 1.18 2009-04-17 08:54:24 cattanem Exp $
/// STL and STD 
#include <cmath>
#include <iostream>
/// DetDesc 
#include "DetDesc/MaterialException.h"
#include "DetDesc/Element.h"
#include "DetDesc/Isotope.h"


///////////////////////////////////////////////////////////////////////////////////////
Element::Element( const std::string& name    , 
                  const std::string& symb    ,
                  const double       a       , 
                  const double       z       , 
                  const double       i       ,
                  const double       density ,                    
                  const double       rl      , 
                  const double       al      ,
                  const double       temp    ,
                  const double       press   ,
                  const eState       s       )
  : Material( name , density , rl , al , temp, press , s )
    , m_Aeff ( a )
    , m_Zeff ( z )
    , m_Ieff ( i )
    , m_isotopes()
    , m_coulomb ()
    , m_tsai    ()
    , m_symb ( symb )
{


  if( 0 < Z() ) { ComputeCoulombFactor  (); } 
  if( 0 < Z() ) { ComputeLradTsaiFactor (); }
  if( 0 < Z() && fabs(I()) < 1e-20) { ComputeMeanExcitationEnergy (); }
  if( 0 < Z() && fabs(I()) > 1e-20 ) { 
    ComputeDensityEffect ();
  }
  else {
    m_C  = 0.;
    m_a  = 0.;
    m_m  = 0.;
    m_X0 = 0.;
    m_X1 = 0.;
  }
}
/////////////////////////////////////////////////////////////////////////////////
Element::~Element() { m_isotopes.clear();  }
/////////////////////////////////////////////////////////////////////////////////
void  Element::addIsotope ( const SmartRef<Isotope>& iPtr , const double Fract, const bool comp )
{ addIsotope( Entry( Fract, iPtr ) , comp ); }
/////////////////////////////////////////////////////////////////////////////////
void  Element::addIsotope ( const Entry&             iPtr                     , const bool comp )
{ 
  m_isotopes.push_back( iPtr ) ; 
  if( comp ) { compute(); } 
} 
/////////////////////////////////////////////////////////////////////////////////
void  Element::removeIsotope ( const SmartRef<Isotope>& iPtr , const bool comp )
{
  for( Isotopes::iterator it = m_isotopes.begin() ; m_isotopes.end() != it ; ++it )
    { if( it->second == iPtr ) { m_isotopes.erase(it); break; }  } 
  if( comp ) { compute(); } 
}
/////////////////////////////////////////////////////////////////////////////////
const SmartRef<Isotope>&  Element::isotope( const unsigned int i ) const 
{
  if( i >= isotopes().size() )
    { throw MaterialException("Element::isotope(indx), wrong index!", this );}
  return isotopes()[i].second;  
}
/////////////////////////////////////////////////////////////////////////////////
      SmartRef<Isotope>&  Element::isotope( const unsigned int i )       
{
  if( i >= isotopes().size() )
    { throw MaterialException("Element::isotope(indx), wrong index! ", this );}
  return isotopes()[i].second;  
}
/////////////////////////////////////////////////////////////////////////////////
double Element::isotopeFraction( const unsigned int i ) const 
{
  if( i >= isotopes().size() )
    { throw MaterialException("Element::isotope(indx), wrong index! " , this );}
  return isotopes()[i].first;  
}
/////////////////////////////////////////////////////////////////////////////////
void Element::compute()
{
  // Effective Z is taken from the first isotope, anyway they should be the
  // same for all the isotopes in the element
  
  setZ( isotope(0)->Z() );

  double sum = 0.0;
  double aEff = 0.0;
  
  for( Isotopes::const_iterator it = m_isotopes.begin() ; m_isotopes.end() != it ; ++it )
    {
      aEff += (it->first) * ( it->second->A() ) ;
      sum  += (it->first) ;
    } 
  
  aEff /= sum;
  
  setA( aEff );
  
  ComputeCoulombFactor();
  ComputeLradTsaiFactor();

  // radiation length
  ComputeRadiationLength();
 
  // compute and set the interaction length
  ComputeInteractionLength(); 
  
  // compute mean excitation energy
  ComputeMeanExcitationEnergy();

  // compute density effect parameters
  ComputeDensityEffect();

}

/////////////////////////////////////////////////////////////////////////////////
void Element::ComputeCoulombFactor()
{
  // Compute Coulomb correction factor (Phys Rev. D50 3-1 (1994) page 1254)
  const double k1 = 0.0083 , k2 = 0.20206 ,k3 = 0.0020 , k4 = 0.0369 ;
  
  double az2 = ( Gaudi::Units::fine_structure_const * Z() ) *
               ( Gaudi::Units::fine_structure_const * Z() );
  double az4 = az2 * az2;
  
  m_coulomb = (k1*az4 + k2 + 1./(1.+az2))*az2 - (k3*az4 + k4)*az4;
}
/////////////////////////////////////////////////////////////////////////////////7
void Element::ComputeLradTsaiFactor()
{
  // Compute Tsai's Expression for the Radiation Length
  // (Phys Rev. D50 3-1 (1994) page 1254)

  const double Lrad_light[]  = {5.31  , 4.79  , 4.74 ,  4.71} ;
  const double Lprad_light[] = {6.144 , 5.621 , 5.805 , 5.924} ;
  
  const double logZ3 = log( Z() )/3.;

  double Lrad, Lprad;
  int iz = (int)(m_Zeff+0.5) - 1 ;
  if (iz <= 3)
    {
      Lrad = Lrad_light[iz] ;  Lprad = Lprad_light[iz];
    }
  else
    {
      Lrad = log(184.15) - logZ3 ; Lprad = log(1194.) - 2*logZ3 ;
    }
  
  m_tsai = 4*Gaudi::Units::alpha_rcl2*m_Zeff*(m_Zeff*(Lrad-m_coulomb) + Lprad); 
}
/////////////////////////////////////////////////////////////////////////////////
MsgStream&        Element::fillStream ( MsgStream&   s ) const 
{
  Material::fillStream( s ) ;
  s << " \t Symbol='"     << symbol() << "'\t" 
    << " CoulombFactor="  << std::setw(8) << coulombFactor() 
    << " TsaiFactor="     << std::setw(8) << tsaiFactor   ()
    << " #isotopes="      << std::setw(2) << isotopes().size() 
    << endmsg;
  for( Isotopes::const_iterator it = isotopes().begin() ; isotopes().end() != it ; ++it )
    { 
      s << " \t\tisotope#"      << std::setw(2) << it - isotopes().begin() 
        << " fraction="  << std::setw(8) << it->first 
        << endmsg 
        << "\t"           << it->second; 
    }
  return s;
}
/////////////////////////////////////////////////////////////////////////////////
std::ostream&     Element::fillStream ( std::ostream& s ) const 
{
  Material::fillStream( s ) ;
  s << " \t Symbol='"     << symbol()     << "'\t" 
    << " CoulombFactor="  << std::setw(8) << coulombFactor() 
    << " TsaiFactor="     << std::setw(8) << tsaiFactor   ()
    << " #isotopes="      << std::setw(2) << isotopes().size() 
    << std::endl;
  for( Isotopes::const_iterator it = isotopes().begin() ; isotopes().end() != it ; ++it )
    { 
      s << " \t\tisotope#"      << std::setw(2) << it - isotopes().begin() 
        << " fraction="  << std::setw(8) << it->first 
        << std::endl 
        << "\t"           << it->second; 
    }
  return s;
}

void Element::ComputeInteractionLength(){
  double intLen = 35.0 * std::cbrt(N()) * (1./density()) * Gaudi::Units::g/Gaudi::Units::cm2;
  setAbsorptionLength(intLen);
}


void  Element::ComputeRadiationLength(){

 // compute radiation length
 double radleninv = Gaudi::Units::Avogadro * tsaiFactor() / A();
 setRadiationLength(1.0 / radleninv / density());

}


void Element::ComputeMeanExcitationEnergy(){
 
  if (fabs(I()) < 1e-20){ 
    if (Z() > 13){
      m_Ieff = (12*Z()+7)*Gaudi::Units::eV;
    }
    else{
      m_Ieff = (9.76*Z()+58.8*pow(Z(),-0.19))*Gaudi::Units::eV;
    }
  }
}

void Element::ComputeDensityEffect(){
    
    double A1 = 1*Gaudi::Units::g/Gaudi::Units::mole;
    double D1 = 1*Gaudi::Units::g/Gaudi::Units::cm3;
    
    double plasma = 28.816*sqrt(density()/D1* Z()*A1/A())*Gaudi::Units::eV;
    
    m_C = 1+2*log(I()/plasma);
    double x_a = m_C/4.606;
    
    if ( I() < 100*Gaudi::Units::eV )
      if ( m_C > 3.681 ){
        m_X0 = 0.326*m_C-1.0;
        m_X1 = 2.0;
      } else {
        m_X0 = 0.2;
        m_X1 = 2.0;
      }
    else 
      if ( m_C > 5.215 ){
        m_X0 = 0.326*m_C-1.5;
        m_X1 = 3.0;
      } else {
        m_X0 = 0.2;
        m_X1 = 3.0;
      }

    m_a = 4.606*(x_a-m_X0)/pow(m_X1-m_X0,3);
    m_m = 3.0;

}
