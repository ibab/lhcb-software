
//------------------------------------------------------------------------
/** @file RichHypoData.h
 *
 *  Header file for utility class : RichHypoData
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHYPODATA_H
#define RICHKERNEL_RICHHYPODATA_H 1

// STL
#include <ostream>

// LHCbKernel
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichParticleIDType.h"

// RichKernel
#include "Kernel/MemPoolAlloc.h"
#include "RichKernel/StlArray.h"

namespace Rich
{

  //------------------------------------------------------------------------
  /** @class HypoData RichHypoData.h RichKernel/RichHypoData.h
   *
   *  A utility class providing an efficient fixed sized array
   *  for particle hypothesis data
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2003-07-31
   */
  //------------------------------------------------------------------------

  template <class TYPE>
  class HypoData : public LHCb::MemPoolAlloc< HypoData<TYPE> >
  {

  public: // definitions

    /// Definition of internal data array type
    typedef std::array<TYPE,Rich::NParticleTypes> DataArray;
    /// Definition of internal validity array type
    typedef std::array<bool,Rich::NParticleTypes> ValidityArray;

  public: // constructors and destructors

    /// Default Constructor
    HypoData() { resetData(); }

    /// Default Destructor
    ~HypoData() = default;

    /// Default Copy Constructor
    HypoData( const HypoData& id ) = default;

    /// Default Copy Operator
    HypoData& operator=( const HypoData& id ) = default;

    /// Default Move Constructor
    HypoData( HypoData&& id ) = default;

    /// Default Move Operator
    HypoData& operator=( HypoData&& id ) = default;

    /** Constructor with explicit data initialisation value
     *
     *  @param value The data initialisation value for each mass hypothesis
     */
    explicit HypoData( const TYPE & value ) { resetData(value); }

  public: // methods

    /** Read access operator
     *
     *  @param type  The mass hypothesis for which the data is requested
     *  @return The data value
     */
    const TYPE & operator[] ( const Rich::ParticleIDType type ) const;

    /** Set the data value for a given particle hypothesis
     *
     *  @param type  The mass hypothesis for which the data is for
     *  @param value The data value
     */
    void setData( const Rich::ParticleIDType type, const TYPE & value );

    /** Reset the data for all mass hypotheses. Following this call all data
     *  fields will be flagged as invalid (i.e. unset)
     *
     *  @param value The reset value
     */
    void resetData( const TYPE & value );

    /** Reset data for given particle hypothesis. Following this call the
     *  data for the given mas hypothesis will be flagged as invalid (i.e. unset)
     *
     *  @param type  The mass hypothesis to reset
     *  @param value The reset value
     */
    void resetData( const Rich::ParticleIDType type, const TYPE & value );

    /** Reset the data for all mass hypotheses. Following this call all data
     *  fields will be flagged as invalid (i.e. unset)
     *
     *  @attention The data values themselves are unaffected
     */
    void resetData();

    /** Reset data for given particle hypothesis. Following this call the
     *  data for the given mas hypothesis will be flagged as invalid (i.e. unset)
     *
     *  @param type  The mass hypothesis to reset
     *
     *  @attention The data values themselves are unaffected
     */
    void resetData( const Rich::ParticleIDType type );

    /** Const Accessor to data array
     *
     *  @return Const reference to the internal data array
     */
    const DataArray & dataArray() const;

    /** Check whether a piece of data has been initialised
     *
     *  @param type The mass hypothesis to test
     *
     *  @return boolean indicating the status of the data
     *  @retval true  Data field has been explicitly set
     *  @retval false Data field has not been set. 
     *                Value will be the initialisation (or reset) value
     */
    bool dataIsValid( const Rich::ParticleIDType type ) const;

    /// Implement textual ostream << method
    friend inline std::ostream& operator << ( std::ostream& ost, 
                                              const HypoData<TYPE>& data )
    {
      for ( int i = 0; i < Rich::NParticleTypes; ++i )
      {
        ost << data[static_cast<Rich::ParticleIDType>(i)] << " ";
      }
      return ost;
    }

  private: // data

    /// The internal representation of the data
    DataArray     m_data;

    /// The validity flags
    ValidityArray m_valid;

  };

  template <class TYPE>
  inline const TYPE & 
  HypoData<TYPE>::operator[] ( const Rich::ParticleIDType type ) const
  {
    return m_data[type];
  }

  template <class TYPE>
  inline void HypoData<TYPE>::setData( const Rich::ParticleIDType type,
                                       const TYPE & value )
  {
    m_valid[type] = true; 
    m_data[type]  = value;
  }

  template <class TYPE>
  inline void HypoData<TYPE>::resetData( const TYPE & value )
  {
    m_valid.fill ( false );
    m_data.fill  ( value );
  }

  template <class TYPE>
  inline void HypoData<TYPE>::resetData( const Rich::ParticleIDType type,
                                         const TYPE & value )
  {
    m_valid[type] = false; 
    m_data[type]  = value;
  }

  template <class TYPE>
  inline void HypoData<TYPE>::resetData()
  {
    m_valid.fill ( false );
    m_data.fill  ( 0     );
  }

  template <class TYPE>
  inline void HypoData<TYPE>::resetData( const Rich::ParticleIDType type )
  {
    m_valid[type] = false;
    m_data[type]  = 0;
  }

  template <class TYPE>
  inline const typename HypoData<TYPE>::DataArray & HypoData<TYPE>::dataArray() const
  {
    return m_data;
  }

  template <class TYPE>
  inline bool HypoData<TYPE>::dataIsValid( const Rich::ParticleIDType type ) const
  {
    return m_valid[type];
  }

} // Rich namespace

#endif // RICHKERNEL_RICHHYPODATA_H
