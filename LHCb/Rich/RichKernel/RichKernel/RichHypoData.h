// $Id: RichHypoData.h,v 1.3 2003-10-13 16:04:52 jonrob Exp $
#ifndef RICHRECBASE_RICHHYPODATA_H
#define RICHRECBASE_RICHHYPODATA_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

// boost
#include "RichKernel/BoostArray.h"

// RichKernel
#include "RichKernel/RichParticleIDType.h"

/** @class RichHypoData RichHypoData.h RichRecBase/RichHypoData.h
 *
 *  A utility class providing an efficient fixed sized array
 *  for particle hypothesis data
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 *
 *  @todo   Fix data validification to work without test on data value
 */

template <class TYPE>
class RichHypoData {

public:

  /// Definition of internal array types
  typedef boost::array<TYPE,Rich::NParticleTypes> Array;

  /// Default Constructor
  RichHypoData( TYPE initValue = (TYPE)-1 ) { resetData(initValue); }
  
  /// Destructor
  ~RichHypoData() { }

  /// Read access operator
  const TYPE& operator [] ( Rich::ParticleIDType type ) const { return m_data[type]; }

  /// Write access operator
  TYPE& operator []       ( Rich::ParticleIDType type )       { return m_data[type]; }

  /// Reset data
  void resetData( TYPE initValue = (TYPE)-1 );

  /// Const Accessor
  const Array & dataArray() const { return m_data; }

  /// Accessor
  Array & dataArray() { return m_data; }

  /// Check whether a piece of data has been initialised
  bool dataIsValid( Rich::ParticleIDType type );

private:

  /// The internal representation of the data
  Array m_data;

};

template <class TYPE>
inline bool RichHypoData<TYPE>::dataIsValid( Rich::ParticleIDType type ) 
{
  return ( m_data[type] > -0.5 );
}

template <class TYPE>
inline void RichHypoData<TYPE>::resetData( TYPE initValue )
{
  m_data[Rich::Electron] = initValue;
  m_data[Rich::Muon]     = initValue;
  m_data[Rich::Pion]     = initValue;
  m_data[Rich::Kaon]     = initValue;
  m_data[Rich::Proton]   = initValue;
}

/// Implement textual ostream << method
template <class TYPE>
inline std::ostream& operator << ( std::ostream& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) { s << (data.dataArray())[i] << " "; }
  return s;
}

/// Implement textual MsgStream << method
template <class TYPE>
inline MsgStream& operator << ( MsgStream& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) { s << (data.dataArray())[i] << " "; }
  return s;
}

/// Implement StreamBuffer << method
template <class TYPE>
inline StreamBuffer& operator << ( StreamBuffer& s, const RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) { s << (data.dataArray())[i]; }
  return s;
}

/// Implement StreamBuffer >> method
template <class TYPE>
inline StreamBuffer& operator >> ( StreamBuffer& s, RichHypoData<TYPE>& data )
{
  for ( int i = 0; i < Rich::NParticleTypes; ++i ) { s >> (data.dataArray())[i]; }
  return s;
}

#endif // RICHRECBASE_RICHHYPODATA_H
