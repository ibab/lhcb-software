// $Id: RichHypoData.h,v 1.1 2003-08-06 09:41:38 jonrob Exp $
#ifndef RICHRECBASE_RICHHYPODATA_H
#define RICHRECBASE_RICHHYPODATA_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

// boost
#include "boost/array.hpp"

// stl
//#include <vector>

// RichKernel
#include "RichKernel/RichParticleIDType.h"

/** @class RichHypoData RichHypoData.h RichRecBase/RichHypoData.h
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
  typedef boost::array<TYPE,Rich::NParticleTypes> Array;

  /// Default Constructor
  RichHypoData() { resetData(); }

  /// Constructor with initialisation value
  //RichHypoData() ( TYPE & init ) 
  // {
  // for ( Array::iterator i = m_data.begin(); 
  //       i != m_data.end(); ++i ) { *i = init; }
  //}
  
  /// Destructor
  ~RichHypoData() { }

  /// Read access operator
  const TYPE& operator [] ( Rich::ParticleIDType type ) const { return m_data[type]; }

  /// Write access operator
  TYPE& operator []       ( Rich::ParticleIDType type )       { return m_data[type]; }

  /// Reset data
  void resetData();

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
inline void RichHypoData<TYPE>::resetData()
{
  m_data[Rich::Electron] = -1;
  m_data[Rich::Muon]     = -1;
  m_data[Rich::Pion]     = -1;
  m_data[Rich::Kaon]     = -1;
  m_data[Rich::Proton]   = -1;
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
