// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Element.cpp,v 1.1.1.1 2000-11-23 13:44:46 ranjard Exp $
#include "GaudiKernel/StatusCode.h"

#include "DetDesc/Element.h"
#include "DetDesc/Isotope.h"

#include <math.h>

/// RCS Id for identification of object version
///static const char* rcsid = "$Id: Element.cpp,v 1.1.1.1 2000-11-23 13:44:46 ranjard Exp $";

const CLID& CLID_Element = 1202;

// Class Element 

Element::Element( std::string name )
: Material( name ), m_Aeff(0.0), m_Zeff(0), m_Neff(0), m_nOfIsotopes(0)
{
    m_isotopes.clear();
    m_fractions.clear();
}

Element::Element( char* name )
: Material( name ), m_Aeff(0.0), m_Zeff(0), m_Neff(0), m_nOfIsotopes(0)
{
    m_isotopes.clear();
    m_fractions.clear();
}

Element::Element( std::string name, double a, double z )
: Material( name ), m_Aeff( a ), m_Zeff( z ), m_Neff( 0.0 )
{
  m_isotopes.clear();
  m_fractions.clear();
  ComputeCoulombFactor();
  ComputeLradTsaiFactor();
}

Element::Element( char* name, double a, double z )
: Material( name ), m_Aeff( a ), m_Zeff( z ), m_Neff( 0.0 )
{
  m_isotopes.clear();
  m_fractions.clear();
  ComputeCoulombFactor();
  ComputeLradTsaiFactor();
}

Element::Element( std::string name, double a, double z, double n )
: Material( name ), m_Aeff( a ), m_Zeff( z ), m_Neff( n )
{
  m_isotopes.clear();
  m_fractions.clear();
  ComputeCoulombFactor();
  ComputeLradTsaiFactor();
}

Element::Element( char* name, double a, double z, double n )
: Material( name ), m_Aeff( a ), m_Zeff( z ), m_Neff( n )
{
  m_isotopes.clear();
  m_fractions.clear();
  ComputeCoulombFactor();
  ComputeLradTsaiFactor();
}

Element::Element( std::string name, double a, double z, double n, double density )
: Material( name ), m_Aeff( a ), m_Zeff( z ), m_Neff( n )
{
  m_isotopes.clear();
  m_fractions.clear();
  setDensity( density );
  ComputeCoulombFactor();
  ComputeLradTsaiFactor();
}

Element::Element( char* name, double a, double z, double n, double density )
: Material( name ), m_Aeff( a ), m_Zeff( z ), m_Neff( n )
{
  m_isotopes.clear();
  m_fractions.clear();
  setDensity( density );
  ComputeCoulombFactor();
  ComputeLradTsaiFactor();
}

// This is used for creation of an empty element which will
// be filled by isotopes
Element::Element( std::string name, int nOfI )
: Material( name ), m_Aeff(0.0), m_Zeff(0), m_Neff(0), m_nOfIsotopes( nOfI )
{
    m_isotopes.clear();
    m_fractions.clear();
}

// This is used for creation of an empty element which will
// be filled by isotopes
Element::Element( char* name, int nOfI )
: Material( name ), m_Aeff(0.0), m_Zeff(0), m_Neff(0), m_nOfIsotopes( nOfI )
{
  m_isotopes.clear();
  m_fractions.clear();
}

Element::~Element()
{}

void Element::addIsotope( Isotope* iPtr, double fract, bool comp )
{
  m_isotopes.push_back( iPtr );
  m_fractions.push_back( fract );
  
  if( true == comp )
  {
    compute();
  }
}

void Element::removeIsotope( Isotope* iPtr, bool comp )
{
  std::vector<Isotope*>::iterator it;
  std::vector<double>::iterator ifr;
  
  for( it =  m_isotopes.begin(), ifr = m_fractions.begin();
  it != m_isotopes.end();
  it++, ifr++ )
  {
    if( *it == iPtr )
    {
      m_isotopes.erase( it );
      m_fractions.erase( ifr );
    }
  }
  
  if( true == comp )
  {
    compute();
  }
}

void Element::compute()
{
  // Effective Z is taken from the first isotope, anyway they should be the
  // same for all the isotopes in the element
  setZ( m_isotopes[0]->Z() );
  
  double sum = 0.0; double nEff = 0.0; double aEff = 0.0;
  
  int iMax = nOfIsotopes();
  
  for( int i = 0; i < iMax; i++ )
  {
    aEff +=  m_fractions[i] * m_isotopes[i]->A();
    nEff +=  m_fractions[i] * m_isotopes[i]->N();
    sum  +=  m_fractions[i];
  }
  
  aEff /= sum;
  nEff /= sum;
  
  setA( aEff );
  setN( nEff );
  
  ComputeCoulombFactor();
  ComputeLradTsaiFactor();
}

#include <iostream>

void Element::ComputeCoulombFactor()
{
  // Compute Coulomb correction factor (Phys Rev. D50 3-1 (1994) page 1254)
  const double k1 = 0.0083 , k2 = 0.20206 ,k3 = 0.0020 , k4 = 0.0369 ;

  double az2 = (fine_structure_const * m_Zeff ) * ( fine_structure_const * m_Zeff );
  double az4 = az2 * az2;

  m_coulomb = (k1*az4 + k2 + 1./(1.+az2))*az2 - (k3*az4 + k4)*az4;
}

void Element::ComputeLradTsaiFactor()
{
  // Compute Tsai's Expression for the Radiation Length
  // (Phys Rev. D50 3-1 (1994) page 1254)

  const double Lrad_light[]  = {5.31  , 4.79  , 4.74 ,  4.71} ;
  const double Lprad_light[] = {6.144 , 5.621 , 5.805 , 5.924} ;
  
  const double logZ3 = log(m_Zeff)/3.;

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

  m_tsai = 4*alpha_rcl2*m_Zeff*(m_Zeff*(Lrad-m_coulomb) + Lprad); 
}
