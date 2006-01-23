
//------------------------------------------------------------------------
/** @file RichHypoData.h
 *
 *  Header file for utility class : RichHypoData
 *
 *  CVS Log :-
 *  $Id: RichHypoData.h,v 1.13 2006-01-23 13:48:35 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHYPODATA_H
#define RICHKERNEL_RICHHYPODATA_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

// boost
#include "RichKernel/BoostArray.h"

// LHCbKernel
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichParticleIDType.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

//------------------------------------------------------------------------
/** @class RichHypoData RichHypoData.h RichKernel/RichHypoData.h
 *
 *  A utility class providing an efficient fixed sized array
 *  for particle hypothesis data
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//------------------------------------------------------------------------

template <class TYPE>
class RichHypoData : public Rich::BoostMemPoolAlloc< RichHypoData<TYPE> >
{

public: // definitions

  /// Definition of internal data array type
  typedef boost::array<TYPE,Rich::NParticleTypes> DataArray;

  /// Definition of internal validity array type
  typedef boost::array<bool,Rich::NParticleTypes> ValidityArray;

public: // methods

  /** Default Constructor
   *  @attention The data values are un-initialised using this constructor
   *             although the validty flags are set to false.
   */
  RichHypoData() { resetData(); }

  /** Constructor with explicit data initialisation value
   *
   *  @param value The data initialisation value for each mass hypothesis
   */
  explicit RichHypoData( const TYPE & value ) { resetData(value); }

  /// Destructor
  ~RichHypoData() { }

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
   *  @retval false Data field has not been set. Value will be the initialisation (or reset) value
   */
  bool dataIsValid( const Rich::ParticleIDType type ) const;

private: // data

  /// The internal representation of the data
  DataArray     m_data;

  /// The validity flags
  ValidityArray m_valid;

};

template <class TYPE>
inline const TYPE & RichHypoData<TYPE>::operator[] ( const Rich::ParticleIDType type ) const
{
  return m_data[type];
}

template <class TYPE>
inline void RichHypoData<TYPE>::setData( const Rich::ParticleIDType type,
                                         const TYPE & value )
{
  m_valid[type] = true; m_data[type]  = value;
}

template <class TYPE>
inline void RichHypoData<TYPE>::resetData( const TYPE & value )
{
  resetData( Rich::Electron, value );
  resetData( Rich::Muon,     value );
  resetData( Rich::Pion,     value );
  resetData( Rich::Kaon,     value );
  resetData( Rich::Proton,   value );
}

template <class TYPE>
inline void RichHypoData<TYPE>::resetData( const Rich::ParticleIDType type,
                                           const TYPE & value )
{
  m_valid[type] = false; m_data[type]  = value;
}

template <class TYPE>
inline void RichHypoData<TYPE>::resetData()
{
  resetData( Rich::Electron );
  resetData( Rich::Muon     );
  resetData( Rich::Pion     );
  resetData( Rich::Kaon     );
  resetData( Rich::Proton   );
}

template <class TYPE>
inline void RichHypoData<TYPE>::resetData( const Rich::ParticleIDType type )
{
  m_valid[type] = false;
}

template <class TYPE>
inline const typename RichHypoData<TYPE>::DataArray & RichHypoData<TYPE>::dataArray() const
{
  return m_data;
}

template <class TYPE>
inline bool RichHypoData<TYPE>::dataIsValid( const Rich::ParticleIDType type ) const
{
  return m_valid[type];
}

/// Implement textual ostream << method
template <class TYPE>
inline std::ostream& operator << ( std::ostream& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i )
  {
    s << data[static_cast<Rich::ParticleIDType>(i)] << " ";
  }
  return s;
}

#endif // RICHKERNEL_RICHHYPODATA_H
