// $Id: RichPhotonSpectrum.cpp,v 1.1.1.1 2002-07-28 10:46:21 jonesc Exp $

#include <cassert>
#include <numeric>
#include "CLHEP/Random/RandFlat.h"

#include "RichRecTools/RichPhotonSpectrum.h"

RichPhotonSpectrum::RichPhotonSpectrum( const RichPhotonSpectrum &right ) :
  m_lowerEnergy( right.m_lowerEnergy ),
  m_deltaEnergy( right.m_deltaEnergy ),
  m_value( right.m_value ) { }

RichPhotonSpectrum::RichPhotonSpectrum ( const int nbin,
                                         const double lower,
                                         const double upper ) :
  m_lowerEnergy( lower ),
  m_deltaEnergy( (upper - lower) / nbin ),
  m_value( nbin ) { }

RichPhotonSpectrum::RichPhotonSpectrum ( const double lower,
                                         const double delta,
                                         const double* value ) :
  m_lowerEnergy( lower ),
  m_deltaEnergy( delta ) {

  while ( *value > 0 ) {
    m_value.push_back( *value );
    ++value;
  }

}

RichPhotonSpectrum::~RichPhotonSpectrum() {}

RichPhotonSpectrum & RichPhotonSpectrum::operator=(const RichPhotonSpectrum &right)
{
  m_lowerEnergy = right.m_lowerEnergy;
  m_deltaEnergy = right.m_deltaEnergy;
  m_value       = right.m_value;
  return *this;
}

RichPhotonSpectrum & RichPhotonSpectrum::operator *= (const double scalar)
{
  for ( vector<double>::iterator i = m_value.begin();
        i != m_value.end();
        i++ ) {
    (*i) *= scalar;
  };

  return *this;
}

RichPhotonSpectrum & RichPhotonSpectrum::operator *= (const RichPhotonSpectrum &spectrum)
{
  //
  // Exceptions would be more elegent
  //

  assert( this->m_value.size() == spectrum.m_value.size() );
  assert( this->m_lowerEnergy  == spectrum.m_lowerEnergy );
  assert( this->m_deltaEnergy  == spectrum.m_deltaEnergy );

  vector<double>::const_iterator j = spectrum.m_value.begin();
  for ( vector<double>::iterator i = this->m_value.begin();
        i != this->m_value.end();
        i++ ) {
    *i *= *j++;
  };

  return *this;

}

double RichPhotonSpectrum::random () const
{
  double sum       = this->integral();
  double randValue = RandFlat::shoot(sum);
  double energy    = m_lowerEnergy;
  for(vector<double>::const_iterator value = m_value.begin();
      value != m_value.end();
      ++value) {
    if ( *value > 0. ) {
      randValue -= *value;
      if ( randValue <= 0. ) {
        return energy - ( randValue / *value ) * m_deltaEnergy;
      }
      energy += m_deltaEnergy;
    }
  }
  assert(false);
  return energy;
}

double RichPhotonSpectrum::averageEnergy () const
{
  double sumValues = 0;
  double avgEnergy = 0;
  double energy = m_lowerEnergy + 0.5 * m_deltaEnergy;
  for(vector<double>::const_iterator value = m_value.begin();
      value != m_value.end();
      ++value) {
    sumValues += *value;
    avgEnergy += *value * energy;
    energy += m_deltaEnergy;
  }
  return avgEnergy / sumValues;
}

