// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/Mixture.cpp,v 1.2 2001-01-22 10:55:36 mato Exp $
#include "DetDesc/Mixture.h"

/// RCS Id for identification of object version
///static const char* rcsid = "$Id: Mixture.cpp,v 1.2 2001-01-22 10:55:36 mato Exp $";


// Class Mixture 

Mixture::Mixture( std::string name, double dens, double rl, double al,
                       double temp, double press, eState s
                )
: Material( name, dens, rl, al, temp, press, s ),  m_nOfItems(0)  {
  m_elements.clear();
  m_fractions.clear();
  m_atoms.clear();
  m_aEff = 0.0;
  m_zEff = 0.0;
}

Mixture::Mixture( std::string name, double a, double z, double density,
                         double rl, double al, double temp, double press, eState s
                )
: Material( name, density, rl, al, temp, press, s ), m_nOfItems(1)  {
  m_elements.clear();
  m_fractions.clear();
  m_atoms.clear();
  m_aEff = 0.0;
  m_zEff = 0.0;
  
  Element* body = new Element( "element"+name, a, z );
  
  addElement( body, 100*perCent );
  
  computeByFraction();
}

Mixture::Mixture( char* name, double a, double z, double density,
                   double rl, double al, double temp, double press, eState s
                )
: Material( name, density, rl, al, temp, press, s ), m_nOfItems(1)  {
  m_elements.clear();
  m_fractions.clear();
  m_atoms.clear();
  m_aEff = 0.0;
  m_zEff = 0.0;
  
  Element* body = new Element( std::string("element")+std::string(name), a, z );
  
  addElement( body, 100*perCent );
  
  computeByFraction();
}

Mixture::Mixture( std::string name, double density, int nOfItms )
: Material( name, density, 0.0, 0.0 ), m_nOfItems(nOfItms)  {
  m_elements.clear();
  m_fractions.clear();
  m_atoms.clear();
  m_aEff = 0.0;
  m_zEff = 0.0;
}

Mixture::Mixture( char* name, double density, int nOfItms )
: Material( name, density, 0.0, 0.0 ), m_nOfItems(nOfItms)  {
  m_elements.clear();
  m_fractions.clear();
  m_atoms.clear();
  m_aEff = 0.0;
  m_zEff = 0.0;
}

Mixture::~Mixture() {
}

#include <iostream>

void Mixture::addElement( Element* e, int nOfAtoms, bool comp )  {

  m_elements.push_back( e );
  m_atoms.push_back( nOfAtoms );
  
  if( true == comp )    {
    computeByAtoms();
  }
}

void Mixture::addElement( Element* e, double fraction, bool comp )  {
  
  int j;
  
  // Loop over our elements to find whether we have already such an
  // element or not
  bool bElementFound = false;
  
  for( j = 0; j < nOfItems(); j++ ) {
    if( e->name() == m_elements[j]->name() ) {
      bElementFound = true;
      break;
    }
  }
  
  if( bElementFound ) {
    m_fractions[j] += fraction;
  }
  else                {
    m_elements.push_back( e );
    m_fractions.push_back( fraction );
  }
  
  if( true == comp )    {
    computeByFraction();
  }
}

void Mixture::addMixture( Mixture* mx, double fraction, bool comp)  {

  // Add all the elements from the mixture mx
  int i;

  // Loop over elements of a mixture we want to add
  for( i = 0; i < mx->nOfItems(); i++ ) {
      addElement( mx->element( i ), fraction*(mx->elementFraction( i )) );
  }
  
  if( true == comp )    {
    computeByFraction();
  }
}

void Mixture::computeByAtoms()  {

  // Compute molecular weight and charge
  double Amol     = 0.0;
  double Zmol     = 0.0;
  double radlen   = 0.0;
  double lambda   = 0.0;
  double division = 0.0;
  
  int    iMax = m_elements.size();
  int    i;

  std::vector<double> noOfAtomsPerVolume;
  
  for( i = 0; i < iMax; i++ )
  {
    Amol += m_atoms[i] * m_elements[i]->A();
    Zmol += m_atoms[i] * m_elements[i]->Z();
  }
  
  // Compute proprotion by weight for each element
  for( i = 0; i < iMax; i++ )
  {
    double fraction = ( m_atoms[i] * m_elements[i]->A() ) / Amol;
    m_fractions.push_back( fraction );
    
    // Compute effective A and Z
    m_aEff += fraction * m_elements[i]->A();
    m_zEff += fraction * m_elements[i]->Z();
    
    double napv = Avogadro*density()*fraction/m_elements[i]->A();
    noOfAtomsPerVolume.push_back(napv);

    // Compute radiation length sum
    division += noOfAtomsPerVolume[i]*m_elements[i]->tsaiFactor();

    // Compute absorption length sum
    lambda = fraction / m_elements[i]->absorptionLength();
  }
  
  // Finish radiation length
  if( 0.0 < division )
  {
    radlen   = 1. / division;
    setRadiationLength( radlen );
  }
  
  // Finish lambda
  if( 0.0 < lambda )
  {
    lambda = 1. / lambda;
    setAbsorptionLength( lambda );
  }
}

void Mixture::computeByFraction()  {

  
  // Compute number of atoms for each element
  double Ai       = 0.0;
  double Zi       = 0.0;
  double Atomsi   = 0.0;
  double radlen   = 0.0;
  double lambda   = 0.0;
  double division = 0.0;
  int    i;
  int    iMax  = m_elements.size();
  
  for( i = 0; i < iMax; i++ )    {

    double fraction = m_fractions[i];
    
    Ai          = m_elements[i]->A();
    Zi          = m_elements[i]->Z();
    
    // Compute effective A and Z
    m_aEff     += ( m_fractions[i] * Ai );
    m_zEff     += ( m_fractions[i] * Zi );

    // Radiation length sum
    Atomsi      = Avogadro*density()*m_fractions[i]/Ai;
    division   += Atomsi*m_elements[i]->tsaiFactor();

    // Compute absorption length sum
    lambda = fraction / m_elements[i]->absorptionLength();
  }
  
  // Finish radiation length
  if( 0.0 < division )
  {
    radlen   =  1./division;
    setRadiationLength( radlen );
  }
  
  // Finish lambda
  if( 0.0 < lambda )    {
    lambda = ( 1. / lambda );
    setAbsorptionLength( lambda );
  }
  
  double fracsum = 0.0;

  for( unsigned int cou = 0; cou < m_fractions.size(); cou++ ) {
    fracsum += m_fractions[cou];
  }
}

