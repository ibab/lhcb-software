// $Id: RichRecPhotonKey.h,v 1.3 2004-04-17 09:28:04 jonesc Exp $
#ifndef RichRecEvent_RichRecPhotonKey_H
#define RichRecEvent_RichRecPhotonKey_H 1

// Gaudi
#include "GaudiKernel/GaudiException.h"

// LHCb
#include "Kernel/CLHEPStreams.h"

/** @class RichRecPhotonKey RichRecPhotonKey.h RichRecBase/RichRecPhotonKey.h
 *
 *  Smart Key for RichRecPhotons. Encodes the parent RichRecSegment and 
 *  RichRecPixel key values into a unique key for each RichRecPhoton
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 15/02/2002
 */

class RichRecPhotonKey {

public:

  /// Constructor from long int
  RichRecPhotonKey( const long key = 0 ) : m_key ( key ) { }

  /// Constructor from segment and pixel numbers
  RichRecPhotonKey( const long pixelKey, 
                    const long segmentKey ) 
    : m_key ( pixelKey%65536 | segmentKey<<16 ) 
  {
    if ( pixelKey != pixelNumber() || segmentKey != segmentNumber() ) {
      throw GaudiException( "pixel/segment number error", 
                            "RichRecPhotonKey", StatusCode::FAILURE );
    }
  }

  /// Destructor
  ~RichRecPhotonKey() {}

  /// Retrieve 32 bit integer key
  inline long key() const
  {
    return m_key;
  }

  /// long operator
  inline operator long() const 
  { 
    return key(); 
  }
  
  /// Update 32 bit integer key
  inline void setKey(const long key)
  {
    m_key = key;
  }

  /// Retrieve associated RichRecSegment
  inline int segmentNumber() const
  {
    return key()/65536;
  }

  /// Retrieve associated RichRecPixel
  inline int pixelNumber() const
  {
    return key()%65536;
  }

  /// Serialize the object for writing
  StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  std::ostream& fillStream(std::ostream& s) const;

private:

  /// 32 bit integer key.
  /// First 16 bits are segment number, last 16 pixel number.
  unsigned long m_key; 

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline std::ostream& RichRecPhotonKey::fillStream(std::ostream& s) const
{
  s << "{ "
    << " key:\t" << key() << " } ";
  return s;
}

inline StreamBuffer& RichRecPhotonKey::serialize(StreamBuffer& s) const
{
  s << key();
  return s;
}

inline StreamBuffer& RichRecPhotonKey::serialize(StreamBuffer& s)
{
  s >> m_key;
  return s;
}

/// Implement StreamBuffer >> method for RichRecPhotonKey
inline StreamBuffer& operator >> (StreamBuffer& s, RichRecPhotonKey& key)
{
  long intkey; s >> intkey; key.setKey(intkey);
  return s;
}

/// Implement StreamBuffer << method for RichRecPhotonKey
inline StreamBuffer& operator << (StreamBuffer& s, const RichRecPhotonKey& key) 
{
  s << key.key();
  return s;
}

#endif   // RichRecEvent_RichRecPhotonKey_H
