// $Id: RichPhotonSpectra.h,v 1.8 2004-07-23 08:47:39 cattanem Exp $
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

template <class TYPE>
class RichPhotonSpectra {

public:

  typedef std::vector<TYPE>        PhotonData;
  typedef std::vector<PhotonData>  HypoPhotonData;

public:

  /// Standard constructor
  RichPhotonSpectra( const unsigned int enBins = 5,
                     const TYPE minEn          = 0,
                     const TYPE maxEn          = 5 )
    : m_enBins   ( enBins ),
      m_minEn    ( minEn  ),
      m_maxEn    ( maxEn  ),
      m_binSize  ( 0 != enBins ? (maxEn-minEn)/enBins : 0 ),
      m_photdata ( Rich::NParticleTypes, PhotonData(enBins) ) { }

  ~RichPhotonSpectra( ) {}; ///< Destructor

  unsigned int energyBins() const;    ///< Returns the number of energy bins

  void setEnergyBins( const unsigned int bins ); ///< Set the number of energy bins

  TYPE minEnergy() const;  ///< Returns the minimum photon energy

  void setMinEnergy( const TYPE en ); ///< Sets the minimum photon energy

  TYPE maxEnergy() const;  ///< Returns the maximum photon energy

  void setMaxEnergy( const TYPE en ); ///< Sets the maximum photon energy

  TYPE binSize() const;    ///< Returns the energy bin size

  void setBinSize( const TYPE size ); ///< Set the energy bin size

  TYPE binEnergyLowerEdge( const unsigned int bin ) const;   ///< The lower edge of the energy bin

  TYPE binEnergyUpperEdge( const unsigned int bin ) const;   ///< The upper edge of the energy bin

  TYPE binEnergy( const unsigned int bin ) const;   ///< The average bin energy

  /// Returns the energy distribution for a given mass hypothesis
  typename RichPhotonSpectra::PhotonData & energyDist( const Rich::ParticleIDType id );

  /// Returns the energy distribution for a given mass hypothesis
  const typename RichPhotonSpectra::PhotonData & energyDist( const Rich::ParticleIDType id ) const;

  /// Returns the energy distribution for a given mass hypothesis
  typename RichPhotonSpectra::HypoPhotonData & hypoData( );

  /// Returns the energy distribution for a given mass hypothesis
  const typename RichPhotonSpectra::HypoPhotonData & hypoData( ) const;

  /// Returns the integral of the distribution
  TYPE integral( const Rich::ParticleIDType id ) const;

  /// multiply by another distribution
  bool multiply(  const Rich::ParticleIDType id,
                  const typename RichPhotonSpectra::PhotonData & data );

  /// Reset the data
  void reset();

  /// Set the energy range
  void setEnergyRange( const TYPE min, const TYPE max );

private: // data

  int m_enBins;              ///< number of energy bins
  TYPE m_minEn;            ///< minimum energy
  TYPE m_maxEn;            ///< maximum energy
  TYPE m_binSize;          ///< energy bin size
  HypoPhotonData m_photdata; ///< The data container

};

template <class TYPE>
inline unsigned int RichPhotonSpectra<TYPE>::energyBins() const
{
  return m_enBins;
}

template <class TYPE>
inline void RichPhotonSpectra<TYPE>::setEnergyBins( const unsigned int bins )
{
  m_enBins = bins;
}

template <class TYPE>
inline TYPE RichPhotonSpectra<TYPE>::minEnergy() const
{
  return m_minEn;
}

template <class TYPE>
inline void RichPhotonSpectra<TYPE>::setMinEnergy( const TYPE en )
{
  m_minEn = en;
}

template <class TYPE>
inline TYPE RichPhotonSpectra<TYPE>::maxEnergy() const
{
  return m_maxEn;
}

template <class TYPE>
inline void RichPhotonSpectra<TYPE>::setMaxEnergy( const TYPE en )
{
  m_maxEn = en;
}

template <class TYPE>
inline TYPE RichPhotonSpectra<TYPE>::binSize() const
{
  return m_binSize;
}

template <class TYPE>
inline void RichPhotonSpectra<TYPE>::setBinSize( const TYPE size )
{
  m_binSize = size;
}

template <class TYPE>
inline TYPE RichPhotonSpectra<TYPE>::binEnergyLowerEdge( const unsigned int bin ) const
{
  return ( minEnergy() + static_cast<TYPE>(bin)*binSize() );
}

template <class TYPE>
inline TYPE RichPhotonSpectra<TYPE>::binEnergyUpperEdge( const unsigned int bin ) const
{
  return ( minEnergy() + static_cast<TYPE>(1+bin)*binSize() );
}

template <class TYPE>
inline TYPE RichPhotonSpectra<TYPE>::binEnergy( const unsigned int bin ) const
{
  return ( minEnergy() + static_cast<TYPE>(0.5+bin)*binSize() );
}

template <class TYPE>
inline void RichPhotonSpectra<TYPE>::setEnergyRange( const TYPE min, const TYPE max )
{
  m_minEn = min;
  m_maxEn = max;
}

template <class TYPE>
inline typename RichPhotonSpectra<TYPE>::PhotonData &
RichPhotonSpectra<TYPE>::energyDist( const Rich::ParticleIDType id )
{
  return m_photdata[id];
}

template <class TYPE>
inline const typename RichPhotonSpectra<TYPE>::PhotonData &
RichPhotonSpectra<TYPE>::energyDist( const Rich::ParticleIDType id ) const
{
  return m_photdata[id];
}

template <class TYPE>
inline typename RichPhotonSpectra<TYPE>::HypoPhotonData & 
RichPhotonSpectra<TYPE>::hypoData()
{
  return m_photdata;
}

template <class TYPE>
inline const typename RichPhotonSpectra<TYPE>::HypoPhotonData &
RichPhotonSpectra<TYPE>::hypoData() const
{
  return m_photdata;
}

template <class TYPE>
inline void RichPhotonSpectra<TYPE>::reset()
{
  // Nothing here yet....
}

// Methods for GOD

/// Implement StreamBuffer << method for RichPhotonSpectra<TYPE>
template <class TYPE>
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const RichPhotonSpectra<TYPE>& spectra )
{
  s << spectra.energyBins()
    << spectra.minEnergy()
    << spectra.maxEnergy()
    << spectra.binSize()
    << spectra.hypoData();
  return s;
}

/// Implement StreamBuffer >> method for RichPhotonSpectra<TYPE>
template <class TYPE>
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   RichPhotonSpectra<TYPE>& spectra )
{
  unsigned int i;
  TYPE d;
  s >> i; spectra.setEnergyBins(i);
  s >> d; spectra.setMinEnergy(d);
  s >> d; spectra.setMaxEnergy(d);
  s >> d; spectra.setBinSize(d);
  s >> spectra.hypoData();
  return s;
}

/// Implement ostream << method for RichPhotonSpectra<TYPE>
template <class TYPE>
inline std::ostream& operator << ( std::ostream& s,
                                   const RichPhotonSpectra<TYPE>& spectra )
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
