
//-----------------------------------------------------------------------------
/** @file RichPhotonSpectra.h
 *
 *  Header file for utility class : RichPhotonSpectra
 *
 *  $Id: RichPhotonSpectra.h,v 1.14 2006-01-23 14:08:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-12
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHPHOTONSPECTRA_H
#define RICHRECBASE_RICHPHOTONSPECTRA_H 1

// stl container
#include <vector>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"

// LHCbKernel
#include "Kernel/RichParticleIDType.h"
// RichKernel
#include "RichKernel/RichHypoData.h"
#include "RichKernel/BoostMemPoolAlloc.h"

/** @class RichPhotonSpectra RichPhotonSpectra.h RichRecBase/RichPhotonSpectra.h
 *
 *  A utility class that can be used to describe a generic distribution
 *  as a function of photon energy. Uses a binned approach to discribe the
 *  distribution.
 *
 *  NB : The templation will only work for numerical (float, double etc.) types.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-12
 *
 *  @todo Review if a fixed sized boost::array could be used internally to speed up operator[] access
 */

template <class TYPE>
class RichPhotonSpectra : public Rich::BoostMemPoolAlloc< RichPhotonSpectra<TYPE> >
{

public: // definitions

  /// Definition of the underlaying distribution container
  typedef std::vector<TYPE>        PhotonData;

  /// Definition of the container used for the particle hypothesis distributions
  typedef std::vector<PhotonData>  HypoPhotonData;

public: // methods

  /** Standard constructor
   *
   *  @param enBins Number of energy bins
   *  @param minEn  Minimum energy value
   *  @param maxEn  Maximum energy value
   */
  RichPhotonSpectra( const unsigned int enBins = 5,
                     const TYPE minEn          = 0,
                     const TYPE maxEn          = 5 )
    : m_enBins   ( enBins                                   ),
      m_minEn    ( minEn                                    ),
      m_maxEn    ( maxEn                                    ),
      m_binSize  ( 0 != enBins ? (maxEn-minEn)/enBins : 0   ),
      m_photdata ( Rich::NParticleTypes, PhotonData(enBins) ) { }

  /// Destructor
  ~RichPhotonSpectra( ) { }

  /** Access the number of energy bins
   *  @return The number of energy bins
   */
  unsigned int energyBins() const;

  /** Set the number of energy bins
   *  @param bins The number of energy bins
   */
  void setEnergyBins( const unsigned int bins );

  /** Access the minimum energy for the distribution
   *  @return The minimum energy value
   */
  TYPE minEnergy() const;

  /** Sets the minimum photon energy
   *  @param en The minimum energy value
   */
  void setMinEnergy( const TYPE en );

  /** Access the maximum photon energy
   *  @return The maximum energy value
   */
  TYPE maxEnergy() const;

  /** Sets the maximum photon energy
   *  @param en The maximum energy value
   */
  void setMaxEnergy( const TYPE en );

  /** Access the energy bin size
   *  @return The energy bins size
   */
  TYPE binSize() const;

  /** Set the energy bin size
   *  @param size The energy bin size
   */
  void setBinSize( const TYPE size );

  /** Access the lower edge of the given energy bin
   *  @param bin The energy bin
   *  @return The energy of the lower edge of the bin
   */
  TYPE binEnergyLowerEdge( const unsigned int bin ) const;

  /** Access the upper edge of the energy bin
   *  @param bin The energy bin
   *  @return The energy of the upper edge of the bin
   */
  TYPE binEnergyUpperEdge( const unsigned int bin ) const;

  /** Access the average bin energy
   *  @param bin The energy bin
   *  @return The average energy value for the given bin
   */
  TYPE binEnergy( const unsigned int bin ) const;

  /** Access the energy distribution for a given mass hypothesis (non-const)
   *  @param id The mass hypothesis
   *  @return The energy distribution for the given mass hypothesis
   */
  typename RichPhotonSpectra::PhotonData & 
  energyDist( const Rich::ParticleIDType id );

  /** Access the energy distribution for a given mass hypothesis (const)
   *  @param id The mass hypothesis
   *  @return The energy distribution for the given mass hypothesis
   */
  const typename RichPhotonSpectra::PhotonData & 
  energyDist( const Rich::ParticleIDType id ) const;

  /** Access energy distribution for all mass hypotheses (non-const)
   *  @return The energy distributions
   */
  typename RichPhotonSpectra::HypoPhotonData & hypoData( );

  /** Access energy distribution for all mass hypotheses (const)
   *  @return The energy distributions
   */
  const typename RichPhotonSpectra::HypoPhotonData & hypoData( ) const;

  /** Computes the integral of the energy distribution for a given mass hypothesis
   *  @param id The mass hypothesis
   *  @return The integral of the energy distribution, over the full energy range
   */
  TYPE integral( const Rich::ParticleIDType id ) const;

  /** multiply by another distribution
   *
   *  @param id   The mass hypothesis distribution to act upon
   *  @param data The distribution to multiple by
   *
   *  @return Status of the operation
   *  @retval true  Multiplication was successful
   *  @retval false Multiplication failed
   */
  bool multiply ( const Rich::ParticleIDType id,
                  const typename RichPhotonSpectra::PhotonData & data );

  /// Reset the data
  void reset();

  /** Set the energy range
   *  @param min The minimum energy value
   *  @param max The maximum energy value
   */
  void setEnergyRange( const TYPE min, const TYPE max );

private: // data

  int m_enBins;              ///< number of energy bins
  TYPE m_minEn;              ///< minimum energy for which the distribution is defined
  TYPE m_maxEn;              ///< maximum energy for which the distribution is defined
  TYPE m_binSize;            ///< energy bin size
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
