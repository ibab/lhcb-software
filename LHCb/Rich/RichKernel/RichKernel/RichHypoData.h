
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
#include <type_traits>

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
   *  Type traits are used to only support arithmetic or pointer types.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2003-07-31
   */
  //------------------------------------------------------------------------

  template < typename TYPE,
             typename = typename std::enable_if< std::is_arithmetic <TYPE> ::value ||
                                                 std::is_pointer    <TYPE> ::value >::type
               >
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

    /** Constructor with explicit data initialisation value
     *
     *  @param value The data initialisation value for each mass hypothesis
     */
    explicit HypoData( const TYPE value ) { resetData(value); }

    /// Default Destructor
    ~HypoData() = default;

    /// Default Copy Constructor
    HypoData( const HypoData& ) = default;

    /// Default Copy Operator
    HypoData& operator=( const HypoData& ) = default;

    /// Default Move Constructor
    HypoData( HypoData&& ) = default;

    /// Default Move Operator
    HypoData& operator=( HypoData&& ) = default;

  public: // methods

    /** Read access operator
     *
     *  @param type  The mass hypothesis for which the data is requested
     *  @return The data value
     */
    inline const TYPE & operator[] ( const Rich::ParticleIDType type ) const
    {
      return m_data[type];
    }

    /** Set the data value for a given particle hypothesis
     *
     *  @param type  The mass hypothesis for which the data is for
     *  @param value The data value
     */
    inline void setData( const Rich::ParticleIDType type, const TYPE value ) noexcept
    {
      m_valid[type] = true; 
      m_data[type]  = value;
    }

    /** Reset the data for all mass hypotheses. Following this call all data
     *  fields will be flagged as invalid (i.e. unset)
     *
     *  @param value The reset value
     */
    inline void resetData( const TYPE value ) noexcept
    {
      m_valid.fill ( false );
      m_data.fill  ( value );
    }

    /** Reset the data for all mass hypotheses. Following this call all data
     *  fields will be flagged as invalid (i.e. unset)
     *
     *  This implementation is for arithemtic types, and sets the value to 0
     *
     *  @attention The data values themselves are unaffected
     */
    template< typename T = TYPE >
    inline 
    typename std::enable_if< std::is_arithmetic<T>::value >::type
    resetData() noexcept { resetData( 0 );  }

    /** Reset the data for all mass hypotheses. Following this call all data
     *  fields will be flagged as invalid (i.e. unset)
     *
     *  This implementation is for pointer types, and sets the value to nullptr
     *
     *  @attention The data values themselves are unaffected
     */
    template< typename T = TYPE >
    inline 
    typename std::enable_if< std::is_pointer<T>::value >::type
    resetData() noexcept { resetData( nullptr ); }
    
    /** Reset data for given particle hypothesis. Following this call the
     *  data for the given mas hypothesis will be flagged as invalid (i.e. unset)
     *
     *  @param type  The mass hypothesis to reset
     *  @param value The reset value
     */
    inline void resetData( const Rich::ParticleIDType type, const TYPE value ) noexcept
    {
      m_valid[type] = false; 
      m_data[type]  = value;
    }
    
    /** Reset data for given particle hypothesis. Following this call the
     *  data for the given mas hypothesis will be flagged as invalid (i.e. unset)
     *
     *  This implementation is for arithemtic types, and sets the value to 0
     *
     *  @param type  The mass hypothesis to reset
     */
    template< typename T = TYPE >
    inline 
    typename std::enable_if< std::is_arithmetic<T>::value >::type
    resetData( const Rich::ParticleIDType type ) noexcept
    {
      resetData( type, 0 );
    }

    /** Reset data for given particle hypothesis. Following this call the
     *  data for the given mas hypothesis will be flagged as invalid (i.e. unset)
     *
     *  This implementation is for pointer types, and sets the value to nullptr
     *
     *  @param type  The mass hypothesis to reset
     */
    template< typename T = TYPE >
    inline 
    typename std::enable_if< std::is_pointer<T>::value >::type
    resetData( const Rich::ParticleIDType type ) noexcept
    {
      resetData( type, nullptr );
    }

    /** Const Accessor to data array
     *
     *  @return Const reference to the internal data array
     */
    inline const DataArray & dataArray() const & noexcept
    {
      return m_data;
    }    

    /** Check whether a piece of data has been initialised
     *
     *  @param type The mass hypothesis to test
     *
     *  @return boolean indicating the status of the data
     *  @retval true  Data field has been explicitly set
     *  @retval false Data field has not been set. 
     *                Value will be the initialisation (or reset) value
     */
    inline bool dataIsValid( const Rich::ParticleIDType type ) const noexcept
    {
      return m_valid[type];
    }

  public:
    
    /// Implement textual ostream << method
    friend inline std::ostream& operator << ( std::ostream& os, 
                                              const HypoData<TYPE>& data )
    {
      os << "[ ";
      for ( const auto id : Rich::particles() ) { os << data[id] << " "; }
      return os << "]";
    }

  private: // data

    /// The internal representation of the data
    DataArray      m_data;

    /// The validity flags
    ValidityArray  m_valid;

  };


} // Rich namespace

#endif // RICHKERNEL_RICHHYPODATA_H
