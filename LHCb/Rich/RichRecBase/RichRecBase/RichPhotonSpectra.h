// $Id: RichPhotonSpectra.h,v 1.2 2003-08-06 12:22:19 jonrob Exp $
#ifndef RICHRECBASE_RICHPHOTONSPECTRA_H
#define RICHRECBASE_RICHPHOTONSPECTRA_H 1

// stl container
#include <vector>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "Kernel/CLHEPStreams.h"

#include "RichKernel/RichParticleIDType.h"

/** @class RichPhotonSpectra RichPhotonSpectra.h RichRecBase/RichPhotonSpectra.h
 *
 *
 *  @author Chris Jones
 *  @date   2003-07-12
 */

class RichPhotonSpectra {

public:

  typedef std::vector<double>      PhotonData;
  typedef std::vector<PhotonData>  HypoPhotonData;

public:

  /// Standard constructor
  RichPhotonSpectra( unsigned int enBins = 5,
                     double minEn = 0,
                     double maxEn = 5 )
    : m_enBins   ( enBins ),
      m_minEn    ( minEn ),
      m_maxEn    ( maxEn ),
      m_binSize  ( 0 != enBins ? (maxEn-minEn)/enBins : 0 ),
      m_photdata ( Rich::NParticleTypes, PhotonData(enBins) ) { }
  
  ~RichPhotonSpectra( ) {}; ///< Destructor

  unsigned int energyBins();    ///< Returns the number of energy bins

  double minEnergy();  ///< Returns the minimum photon energy

  double maxEnergy();  ///< Returns the maximum photon energy

  double binSize();    ///< Returns the energy bin size

  double binEnergyLowerEdge( unsigned int bin );   ///< The lower edge of the energy bin

  double binEnergyUpperEdge( unsigned int bin );   ///< The upper edge of the energy bin

  double binEnergy( unsigned int bin );   ///< The average bin energy

  /// Returns the energy distribution for a given mass hypothesis
  RichPhotonSpectra::PhotonData & energyDist( const Rich::ParticleIDType id );

  /// Returns the integral of the distribution
  double integral( const Rich::ParticleIDType id );

  /// multiply by another distribution
  bool multiply(  const Rich::ParticleIDType id,
                  const RichPhotonSpectra::PhotonData & data );

  /// Reset the data
  void reset();

  /// Set the energy range
  void setEnergyRange( double min, double max );

private: // data

  int m_enBins;     ///< number of energy bins
  double m_minEn;   ///< minimum energy
  double m_maxEn;   ///< maximum energy
  double m_binSize; ///< bin size

  HypoPhotonData m_photdata;

};

inline unsigned int RichPhotonSpectra::energyBins()
{
  return m_enBins;
}

inline double RichPhotonSpectra::minEnergy()
{
  return m_minEn;
}

inline double RichPhotonSpectra::maxEnergy()
{
  return m_maxEn;
}

inline double RichPhotonSpectra::binSize()
{
  return m_binSize;
}

inline double RichPhotonSpectra::binEnergyLowerEdge( unsigned int bin )
{
  return ( minEnergy() + (double)bin*binSize() );
}

inline double RichPhotonSpectra::binEnergyUpperEdge( unsigned int bin )
{
  return ( minEnergy() + (double)(1+bin)*binSize() );
}

inline double RichPhotonSpectra::binEnergy( unsigned int bin )
{
  return ( minEnergy() + (double)(0.5+bin)*binSize() );
}

inline void RichPhotonSpectra::setEnergyRange( double min, double max )
{
  m_minEn = min;
  m_maxEn = max;
}

inline RichPhotonSpectra::PhotonData &
RichPhotonSpectra::energyDist( const Rich::ParticleIDType id )
{
  return m_photdata[id];
}

// Methods for GOD

/// Implement StreamBuffer >> method for RichPhotonSpectra
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   RichPhotonSpectra& spectra )
{
  // add details later on
  return s;
}

/// Implement StreamBuffer << method for RichPhotonSpectra
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const RichPhotonSpectra& spectra )
{
  // add details later on
  return s;
}

/// Implement ostream << method for RichPhotonSpectra
inline std::ostream& operator << ( std::ostream& s,
                                   const RichPhotonSpectra& spectra )
{
  //s << "{ " << std::endl
  //  << " energy bins:\t" << spectra.energyBins() << std::endl
  // << " minimum energy:\t" << spectra.minEnergy() << std::endl
  //  << " maximum energy:\t" << spectra.maxEnergy() << std::endl
  // << " photon data:\t" << spectra.photonData() << std::endl
  //  << " } ";
  return s;
}

#endif // RICHRECBASE_RICHPHOTONSPECTRA_H
