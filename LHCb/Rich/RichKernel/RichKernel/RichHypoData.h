// $Id: RichHypoData.h,v 1.4 2003-11-02 21:38:04 jonrob Exp $
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

public:

  /// Definition of internal array types
  typedef boost::array<TYPE,Rich::NParticleTypes> DataArray;
  typedef boost::array<bool,Rich::NParticleTypes> ValidityArray;

  /// Default Constructor
  RichHypoData( TYPE value = (TYPE)-1 ) { resetData(value); }

  /// Destructor
  ~RichHypoData() { }

  /// Read access operator
  inline const TYPE& operator [] ( const Rich::ParticleIDType type ) const
  {
    return m_data[type];
  }

  /// Set the data vlaue for a given particle hypothesis
  void setData( const Rich::ParticleIDType type, const TYPE value );

  /// Reset all data
  void resetData( const TYPE value = (TYPE)-1 );

  /// Reset data for given particle hypothesis
  void resetData( const Rich::ParticleIDType type, const TYPE value = (TYPE)-1 );

  /// Const Accessor (To be removed ?)
  inline const DataArray & dataArray() const
  {
    return m_data;
  }

  /// Check whether a piece of data has been initialised
  inline bool dataIsValid( const Rich::ParticleIDType type )
  {
    return m_valid[type];
  }

private:

  /// The internal representation of the data
  DataArray     m_data;
  ValidityArray m_valid;

};

template <class TYPE>
inline void RichHypoData<TYPE>::setData( const Rich::ParticleIDType type,
                                         const TYPE value )
{
  m_valid[type] = true;
  m_data[type]  = value;
}

template <class TYPE>
inline void RichHypoData<TYPE>::resetData( const TYPE value )
{
  resetData( Rich::Electron, value );
  resetData( Rich::Muon,     value );
  resetData( Rich::Pion,     value );
  resetData( Rich::Kaon,     value );
  resetData( Rich::Proton,   value );
}

template <class TYPE>
inline void RichHypoData<TYPE>::resetData( const Rich::ParticleIDType type, 
                                           const TYPE value )
{
  m_valid[type] = false;
  m_data[type]  = value;
}

/// Implement textual ostream << method
template <class TYPE>
inline std::ostream& operator << ( std::ostream& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) {
    s << data[(Rich::ParticleIDType)i] << " ";
  }
  return s;
}

/// Implement textual MsgStream << method
template <class TYPE>
inline MsgStream& operator << ( MsgStream& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) {
    s << data[(Rich::ParticleIDType)i] << " ";
  }
  return s;
}

/// Implement StreamBuffer << method
template <class TYPE>
inline StreamBuffer& operator << ( StreamBuffer& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) {
    s << data[(Rich::ParticleIDType)i];
  }
  return s;
}

/// Implement StreamBuffer >> method
template <class TYPE>
inline StreamBuffer& operator >> ( StreamBuffer& s, RichHypoData<TYPE>& data )
{
  TYPE tmp;
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) {
    s >> tmp; data.setData( (Rich::ParticleIDType)i, tmp );
  }
  return s;
}

#endif // RICHKERNEL_RICHHYPODATA_H
