// $Id: RichPhotonSpectrum.h,v 1.1.1.1 2002-07-28 10:46:23 jonesc Exp $
#ifndef RICHRECTOOLS_RICHPHOTONSPECTRUM_H
#define RICHRECTOOLS_RICHPHOTONSPECTRUM_H 1

// local
#include <vector>

/** @class RichPhotonSpectrum  RichPhotonSpectrum.h RichRecTools/RichPhotonSpectrum.h
 *
 *  RichPhotonSpectrum
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   18/07/2002
 */

class RichPhotonSpectrum {

public:

  RichPhotonSpectrum( const RichPhotonSpectrum &right );

  RichPhotonSpectrum( const int nbin,
                      const double lower,
                      const double upper );

  RichPhotonSpectrum( const double lower,
                      const double delta,
                      const double* value );

  ~RichPhotonSpectrum();

  RichPhotonSpectrum & operator = ( const RichPhotonSpectrum &right );

  RichPhotonSpectrum & operator *= ( const double scalar );

  RichPhotonSpectrum & operator *= ( const RichPhotonSpectrum &spectrum );

  double integral () const;

  const double & operator [] ( const double energy ) const;

  const double & operator [] (const int bin) const;

  double & operator [] (const double energy);

  double & operator [] (const int bin);

  double lowerEnergy () const;

  double upperEnergy () const;

  double deltaEnergy () const;

  int energyBins () const;

  double random () const;

  double averageEnergy () const;

private:

  RichPhotonSpectrum();

  double m_lowerEnergy;

  double m_deltaEnergy;

  std::vector<double> m_value;

};

inline double RichPhotonSpectrum::integral () const
{
  return accumulate( m_value.begin(), m_value.end(), 0.);
}

inline const double & RichPhotonSpectrum::operator [] (const double energy) const
{
  int bin = int( ( energy - m_lowerEnergy ) / m_deltaEnergy );
  return m_value[bin];
}

inline const double & RichPhotonSpectrum::operator [] (const int bin) const
{
  return m_value[bin];
}

inline double & RichPhotonSpectrum::operator [] (const double energy)
{
  int bin = int( ( energy - m_lowerEnergy ) / m_deltaEnergy );
  return m_value[bin];
}

inline double & RichPhotonSpectrum::operator [] (const int bin)
{
  return m_value[bin];
}

inline double RichPhotonSpectrum::lowerEnergy () const
{
  return m_lowerEnergy;
}

inline double RichPhotonSpectrum::upperEnergy () const
{
  return m_lowerEnergy + m_deltaEnergy * m_value.size();
}

inline double RichPhotonSpectrum::deltaEnergy () const
{
  return m_deltaEnergy;
}

inline int RichPhotonSpectrum::energyBins () const
{
  return m_value.size();
}

#endif // RICHRECTOOLS_RICHPHOTONSPECTRUM_H
