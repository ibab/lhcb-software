// $Id: RichHypoData.h,v 1.8 2004-07-15 15:36:53 jonrob Exp $
#ifndef RICHKERNEL_RICHHYPODATA_H
#define RICHKERNEL_RICHHYPODATA_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

// boost
#include "RichKernel/BoostArray.h"

// RichKernel
#include "RichKernel/RichParticleIDType.h"

/** @class RichHypoData RichHypoData.h RichKernel/RichHypoData.h
 *
 *  A utility class providing an efficient fixed sized array
 *  for particle hypothesis data
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */

template <class TYPE>
class RichHypoData {

public: // definitions

  /// Definition of internal data array type
  typedef boost::array<TYPE,Rich::NParticleTypes> DataArray;

  /// Definition of internal validity array type
  typedef boost::array<bool,Rich::NParticleTypes> ValidityArray;

public: // methods

  /** Constructor with initialisation value
   *
   * @param value the value use to initialise the data for each mass hypothesis
   */
  RichHypoData( const TYPE & value ) { resetData(value); }

  /// Destructor
  ~RichHypoData() { }

  /// Read access operator
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

  /// Const Accessor to data array
  const DataArray & dataArray() const;

  /** Check whether a piece of data has been initialised
   *
   *  @param type The mass hypothesis to test
   *  
   *  @return boolean indicating the status of the data
   *  @retval true  Data field has been explicitly set
   *  @retval false Data field has not been set. Value will be the initialisation (or reset) value
   */
  bool dataIsValid( const Rich::ParticleIDType type );

private: // methods

  /** Dis-allow the default constructor. 
   *  Users must specify a default initialization value.
   */
  RichHypoData() {}

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
inline const typename RichHypoData<TYPE>::DataArray & RichHypoData<TYPE>::dataArray() const
{
  return m_data;
}

template <class TYPE>
inline bool RichHypoData<TYPE>::dataIsValid( const Rich::ParticleIDType type )
{
  return m_valid[type];
}

/// Implement textual ostream << method
template <class TYPE>
inline std::ostream& operator << ( std::ostream& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) {
    s << data[static_cast<Rich::ParticleIDType>(i)] << " ";
  }
  return s;
}

/// Implement textual MsgStream << method
template <class TYPE>
inline MsgStream& operator << ( MsgStream& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) {
    s << data[static_cast<Rich::ParticleIDType>(i)] << " ";
  }
  return s;
}

/// Implement StreamBuffer << method
template <class TYPE>
inline StreamBuffer& operator << ( StreamBuffer& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) {
    s << data[static_cast<Rich::ParticleIDType>(i)];
  }
  return s;
}

/// Implement StreamBuffer >> method
template <class TYPE>
inline StreamBuffer& operator >> ( StreamBuffer& s, RichHypoData<TYPE>& data )
{
  TYPE tmp;
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) {
    s >> tmp; data.setData( static_cast<Rich::ParticleIDType>(i), tmp );
  }
  return s;
}

#endif // RICHKERNEL_RICHHYPODATA_H
