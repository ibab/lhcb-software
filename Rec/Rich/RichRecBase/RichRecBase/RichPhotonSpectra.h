
//-----------------------------------------------------------------------------
/** @file RichPhotonSpectra.h
 *
 *  Header file for utility class : Rich::PhotonSpectra
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-12
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHPHOTONSPECTRA_H
#define RICHRECBASE_RICHPHOTONSPECTRA_H 1

// stl container
#include <vector>

// LHCbKernel
#include "Kernel/RichParticleIDType.h"
#include "Kernel/MemPoolAlloc.h"

namespace Rich
{

  /** @class PhotonSpectra RichPhotonSpectra.h RichRecBase/RichPhotonSpectra.h
   *
   *  A utility class that can be used to describe a generic distribution
   *  as a function of photon energy. Uses a binned approach to discribe the
   *  distribution.
   *
   *  NB : The templation will only work for numerical (float, double etc.) types.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2003-07-12
   */

  template <class TYPE>
  class PhotonSpectra : public LHCb::MemPoolAlloc< PhotonSpectra<TYPE> >
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
    PhotonSpectra( const unsigned int enBins = 5,
                   const TYPE minEn          = 0,
                   const TYPE maxEn          = 5 )
      : m_enBins   ( enBins                                   ),
        m_minEn    ( minEn                                    ),
        m_maxEn    ( maxEn                                    ),
        m_binSize  ( 0 != enBins ? (maxEn-minEn)/enBins : 0   ),
        m_photdata ( Rich::NParticleTypes, PhotonData(enBins) ) { }

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
    typename PhotonSpectra::PhotonData &
    energyDist( const Rich::ParticleIDType id );

    /** Access the energy distribution for a given mass hypothesis (const)
     *  @param id The mass hypothesis
     *  @return The energy distribution for the given mass hypothesis
     */
    const typename PhotonSpectra::PhotonData &
    energyDist( const Rich::ParticleIDType id ) const;

    /** Access energy distribution for all mass hypotheses (non-const)
     *  @return The energy distributions
     */
    typename PhotonSpectra::HypoPhotonData & hypoData( );

    /** Access energy distribution for all mass hypotheses (const)
     *  @return The energy distributions
     */
    const typename PhotonSpectra::HypoPhotonData & hypoData( ) const;

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
                    const typename PhotonSpectra::PhotonData & data );

    /// Reset the data
    void reset();

    /** Set the energy range
     *  @param min The minimum energy value
     *  @param max The maximum energy value
     */
    void setEnergyRange( const TYPE min, const TYPE max );

  public:

    /// Implement ostream << method for PhotonSpectra<TYPE>
    friend inline std::ostream& operator << ( std::ostream& photS_os,
                                              const PhotonSpectra<TYPE>& spectra )
    {
      photS_os << "{ " << std::endl
               << " energy bins:\t" << spectra.energyBins() << std::endl
               << " minimum energy:\t" << spectra.minEnergy() << std::endl
               << " maximum energy:\t" << spectra.maxEnergy() << std::endl
        //<< " electron data:\t" << spectra.energyDist(Rich::Electron) << std::endl
        //<< " muon data:\t" << spectra.energyDist(Rich::Muon) << std::endl
        //<< " pion data:\t" << spectra.energyDist(Rich::Pion) << std::endl
        //<< " kaon data:\t" << spectra.energyDist(Rich::Kaon) << std::endl
        //<< " proton data:\t" << spectra.energyDist(Rich::Proton) << std::endl
               << " } " << std::endl;
      return photS_os;
    }

  private: // data

    int m_enBins   = 5;        ///< number of energy bins
    TYPE m_minEn   = 0;        ///< minimum energy for which the distribution is defined
    TYPE m_maxEn   = 5;        ///< maximum energy for which the distribution is defined
    TYPE m_binSize = 1;        ///< energy bin size
    HypoPhotonData m_photdata; ///< The data container

  };

  template <class TYPE>
  inline unsigned int PhotonSpectra<TYPE>::energyBins() const
  {
    return m_enBins;
  }

  template <class TYPE>
  inline void PhotonSpectra<TYPE>::setEnergyBins( const unsigned int bins )
  {
    m_enBins = bins;
  }

  template <class TYPE>
  inline TYPE PhotonSpectra<TYPE>::minEnergy() const
  {
    return m_minEn;
  }

  template <class TYPE>
  inline void PhotonSpectra<TYPE>::setMinEnergy( const TYPE en )
  {
    m_minEn = en;
  }

  template <class TYPE>
  inline TYPE PhotonSpectra<TYPE>::maxEnergy() const
  {
    return m_maxEn;
  }

  template <class TYPE>
  inline void PhotonSpectra<TYPE>::setMaxEnergy( const TYPE en )
  {
    m_maxEn = en;
  }

  template <class TYPE>
  inline TYPE PhotonSpectra<TYPE>::binSize() const
  {
    return m_binSize;
  }

  template <class TYPE>
  inline void PhotonSpectra<TYPE>::setBinSize( const TYPE size )
  {
    m_binSize = size;
  }

  template <class TYPE>
  inline TYPE PhotonSpectra<TYPE>::binEnergyLowerEdge( const unsigned int bin ) const
  {
    return ( minEnergy() + ((TYPE)(bin))*binSize() );
  }

  template <class TYPE>
  inline TYPE PhotonSpectra<TYPE>::binEnergyUpperEdge( const unsigned int bin ) const
  {
    return ( minEnergy() + ((TYPE)(1+bin))*binSize() );
  }

  template <class TYPE>
  inline TYPE PhotonSpectra<TYPE>::binEnergy( const unsigned int bin ) const
  {
    return ( minEnergy() + ((TYPE)(0.5+bin))*binSize() );
  }

  template <class TYPE>
  inline void PhotonSpectra<TYPE>::setEnergyRange( const TYPE min, const TYPE max )
  {
    m_minEn = min;
    m_maxEn = max;
  }

  template <class TYPE>
  inline typename PhotonSpectra<TYPE>::PhotonData &
  PhotonSpectra<TYPE>::energyDist( const Rich::ParticleIDType id )
  {
    return m_photdata[id];
  }

  template <class TYPE>
  inline const typename PhotonSpectra<TYPE>::PhotonData &
  PhotonSpectra<TYPE>::energyDist( const Rich::ParticleIDType id ) const
  {
    return m_photdata[id];
  }

  template <class TYPE>
  inline typename PhotonSpectra<TYPE>::HypoPhotonData &
  PhotonSpectra<TYPE>::hypoData()
  {
    return m_photdata;
  }

  template <class TYPE>
  inline const typename PhotonSpectra<TYPE>::HypoPhotonData &
  PhotonSpectra<TYPE>::hypoData() const
  {
    return m_photdata;
  }

  template <class TYPE>
  inline TYPE PhotonSpectra<TYPE>::integral( const Rich::ParticleIDType id ) const
  {
    TYPE sum = 0;
    for ( const auto& i : energyDist(id) ) { sum += i; }
    return sum;
  }

  template <class TYPE>
  inline bool PhotonSpectra<TYPE>::multiply( const Rich::ParticleIDType id,
                                             const typename PhotonSpectra<TYPE>::PhotonData & data )
  {
    if ( this->energyBins() != data.size() ) return false;
    auto j = data.begin();
    for ( auto i = energyDist(id).begin();
          i != energyDist(id).end(); ++i ) { *i *= *j; ++j; }
    return true;
  }

  template <class TYPE>
  inline void PhotonSpectra<TYPE>::reset()
  {
    // Nothing here yet....
  }

} // RICH

#endif // RICHRECBASE_RICHPHOTONSPECTRA_H
