// $Id: RichPhotonSpectra.h,v 1.5 2004-02-02 14:23:04 jonesc Exp $
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
 *  A utility class describing a photon energy spectra
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
  RichPhotonSpectra( const unsigned int enBins = 5,
                     const double minEn = 0,
                     const double maxEn = 5 )
    : m_enBins   ( enBins ),
      m_minEn    ( minEn ),
      m_maxEn    ( maxEn ),
      m_binSize  ( 0 != enBins ? (maxEn-minEn)/enBins : 0 ),
      m_photdata ( Rich::NParticleTypes, PhotonData(enBins) ) { }
  
  ~RichPhotonSpectra( ) {}; ///< Destructor

  unsigned int energyBins() const;    ///< Returns the number of energy bins

  void setEnergyBins( const unsigned int bins ); ///< Set the number of energy bins

  double minEnergy() const;  ///< Returns the minimum photon energy

  void setMinEnergy( const double en ); ///< Sets the minimum photon energy

  double maxEnergy() const;  ///< Returns the maximum photon energy

  void setMaxEnergy( const double en ); ///< Sets the maximum photon energy

  double binSize() const;    ///< Returns the energy bin size

  void setBinSize( const double size ); ///< Set the energy bin size

  double binEnergyLowerEdge( const unsigned int bin ) const;   ///< The lower edge of the energy bin

  double binEnergyUpperEdge( const unsigned int bin ) const;   ///< The upper edge of the energy bin

  double binEnergy( const unsigned int bin ) const;   ///< The average bin energy

  /// Returns the energy distribution for a given mass hypothesis
  RichPhotonSpectra::PhotonData & energyDist( const Rich::ParticleIDType id );

  /// Returns the energy distribution for a given mass hypothesis
  const RichPhotonSpectra::PhotonData & energyDist( const Rich::ParticleIDType id ) const;

  /// Returns the energy distribution for a given mass hypothesis
  RichPhotonSpectra::HypoPhotonData & hypoData( );

  /// Returns the energy distribution for a given mass hypothesis
  const RichPhotonSpectra::HypoPhotonData & hypoData( ) const;

  /// Returns the integral of the distribution
  double integral( const Rich::ParticleIDType id ) const;

  /// multiply by another distribution
  bool multiply(  const Rich::ParticleIDType id,
                  const RichPhotonSpectra::PhotonData & data );

  /// Reset the data
  void reset();

  /// Set the energy range
  void setEnergyRange( const double min, const double max );

private: // data

  int m_enBins;     ///< number of energy bins
  double m_minEn;   ///< minimum energy
  double m_maxEn;   ///< maximum energy
  double m_binSize; ///< bin size

  HypoPhotonData m_photdata;

};

inline unsigned int RichPhotonSpectra::energyBins() const
{
  return m_enBins;
}

inline void RichPhotonSpectra::setEnergyBins( const unsigned int bins ) 
{
  m_enBins = bins;
} 

inline double RichPhotonSpectra::minEnergy() const
{
  return m_minEn;
}

inline void RichPhotonSpectra::setMinEnergy( const double en )
{
  m_minEn = en;
}

inline double RichPhotonSpectra::maxEnergy() const
{
  return m_maxEn;
}

inline void RichPhotonSpectra::setMaxEnergy( const double en )
{
  m_maxEn = en;
}

inline double RichPhotonSpectra::binSize() const
{
  return m_binSize;
}

inline void RichPhotonSpectra::setBinSize( const double size ) 
{
  m_binSize = size;
}

inline double RichPhotonSpectra::binEnergyLowerEdge( const unsigned int bin ) const
{
  return ( minEnergy() + static_cast<double>(bin)*binSize() );
}

inline double RichPhotonSpectra::binEnergyUpperEdge( const unsigned int bin ) const
{
  return ( minEnergy() + static_cast<double>(1+bin)*binSize() );
}

inline double RichPhotonSpectra::binEnergy( const unsigned int bin ) const
{
  return ( minEnergy() + static_cast<double>(0.5+bin)*binSize() );
}

inline void RichPhotonSpectra::setEnergyRange( const double min, const double max )
{
  m_minEn = min;
  m_maxEn = max;
}

inline RichPhotonSpectra::PhotonData &
RichPhotonSpectra::energyDist( const Rich::ParticleIDType id )
{
  return m_photdata[id];
}

inline const RichPhotonSpectra::PhotonData &
RichPhotonSpectra::energyDist( const Rich::ParticleIDType id ) const
{
  return m_photdata[id];
}

inline RichPhotonSpectra::HypoPhotonData & RichPhotonSpectra::hypoData()
{
  return m_photdata;
}

inline const RichPhotonSpectra::HypoPhotonData & 
RichPhotonSpectra::hypoData() const
{
  return m_photdata;
}

inline void RichPhotonSpectra::reset() 
{
  // Nothing here yet....
}

// Methods for GOD

/// Implement StreamBuffer << method for RichPhotonSpectra
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const RichPhotonSpectra& spectra )
{
  s << spectra.energyBins()
    << spectra.minEnergy()
    << spectra.maxEnergy()
    << spectra.binSize()
    << spectra.hypoData();
  return s;
}

/// Implement StreamBuffer >> method for RichPhotonSpectra
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   RichPhotonSpectra& spectra )
{
  unsigned int i;
  double d;
  s >> i; spectra.setEnergyBins(i);
  s >> d; spectra.setMinEnergy(d);
  s >> d; spectra.setMaxEnergy(d);
  s >> d; spectra.setBinSize(d);
  s >> spectra.hypoData();
  return s;
}

/// Implement ostream << method for RichPhotonSpectra
inline std::ostream& operator << ( std::ostream& s,
                                   const RichPhotonSpectra& spectra )
{
  s << "{ " << std::endl
    << " energy bins:\t" << spectra.energyBins() << std::endl
    << " minimum energy:\t" << spectra.minEnergy() << std::endl
    << " maximum energy:\t" << spectra.maxEnergy() << std::endl
    //<< " electron data:\t" << spectra.energyDist(Rich::Electron) << std::endl
    //<< " muon data:\t" << spectra.energyDist(Rich::Muon) << std::endl
    //<< " pion data:\t" << spectra.energyDist(Rich::Pion) << std::endl
    //<< " kaon data:\t" << spectra.energyDist(Rich::Kaon) << std::endl
    //<< " proton data:\t" << spectra.energyDist(Rich::Proton) << std::endl
    << " } " << std::endl;
  return s;
}

#endif // RICHRECBASE_RICHPHOTONSPECTRA_H
