
//-----------------------------------------------------------------------------
/** @file RichRecPhotonKey.h
 *
 * Header file for utility class : RichRecPhotonKey
 *
 * $Id: RichRecPhotonKey.h,v 1.7 2005-03-04 16:52:56 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-06-20
 */
//-----------------------------------------------------------------------------

#ifndef RichRecEvent_RichRecPhotonKey_H
#define RichRecEvent_RichRecPhotonKey_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"

/** @class RichRecPhotonKey RichRecPhotonKey.h RichRecBase/RichRecPhotonKey.h
 *
 *  Custom Key for RichRecPhotons. Encodes the parent RichRecSegment and
 *  RichRecPixel key values into a unique key for each RichRecPhoton.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 15/02/2002
 */

class RichRecPhotonKey {

public:

  /** Constructor from long int
   *  @param key The raw data key to use as the bit-packed data
   */
  RichRecPhotonKey( const long key = 0 ) : m_key ( key ) { }

  /** Constructor from segment and pixel numbers
   *
   *  @param pixelKey    The key for the associated RichRecPixel
   *  @param segmentKey  The key for the associated RichRecSegment
   */
  RichRecPhotonKey ( const long pixelKey,
                     const long segmentKey )
    : m_key ( pixelKey%65536 | segmentKey<<16 ) { }

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

  /// Retrieve associated RichRecSegment key
  inline int segmentNumber() const
  {
    return key()/65536;
  }

  /// Retrieve associated RichRecPixel key
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
