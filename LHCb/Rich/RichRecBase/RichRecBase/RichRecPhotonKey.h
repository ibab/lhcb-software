// $Id: RichRecPhotonKey.h,v 1.2 2003-06-30 15:11:57 jonrob Exp $
#ifndef RichRecEvent_RichRecPhotonKey_H
#define RichRecEvent_RichRecPhotonKey_H 1

// LHCb
#include "Kernel/CLHEPStreams.h"

// Rich Kernel
//#include "RichKernel/RichDefinitions.h"

/** @class RichRecPhotonKey RichRecPhotonKey.h
 *
 *  Smart Key for RichRecPhotons. Encodes the parent RichRecSegment and 
 *  RichRecPixel key values.
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  created Tue Feb 26 09:25:55 2002
 *
 */

class RichRecPhotonKey {

public:

  /// Constructor from long int
  RichRecPhotonKey(long longKey) { m_longKey = longKey; }

  /// Default Constructor
  RichRecPhotonKey() { m_longKey = 0; }

  /// Destructor
  virtual ~RichRecPhotonKey() {}

  /// Retrieve 32 bit integer key
  long longKey() const;

  /// Update 32 bit integer key
  void setLongKey(long value);

  /// Set the associated RichRecSegment
  void setSegmentNumber(const long segmentNumber);

  /// Set the associated RichRecPixel
  void setPixelNumber(const long pixelNumber);

  /// Retrieve associated RichRecSegment
  int segmentNumber() const;

  /// Retrieve associated RichRecPixel
  int pixelNumber() const;

  /// Serialize the object for writing
  StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  std::ostream& fillStream(std::ostream& s) const;

  /// long operator
  // operator long() const { return m_longKey|0xFF000000; }
  operator long() const { return m_longKey; }

protected:

private:

  /// 32 bit integer key
  /// First 16 bits are segment number, last 16 pixel number.
  unsigned long m_longKey; 

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline void RichRecPhotonKey::setSegmentNumber(const long segmentNumber)
{
  m_longKey = m_longKey%65536;
  m_longKey += segmentNumber<<16;
}

inline void RichRecPhotonKey::setPixelNumber(const long pixelNumber)
{
  m_longKey = (m_longKey/65536)<<16;
  m_longKey += pixelNumber;
}

inline int RichRecPhotonKey::segmentNumber() const
{
  return m_longKey/65536;
}

inline int RichRecPhotonKey::pixelNumber() const
{
  return m_longKey%65536;
}

inline long RichRecPhotonKey::longKey() const
{
  return m_longKey;
}

inline void RichRecPhotonKey::setLongKey(long value)
{
  m_longKey = value;
}

inline StreamBuffer& RichRecPhotonKey::serialize(StreamBuffer& s) const
{
  s << m_longKey;
  return s;
}

inline StreamBuffer& RichRecPhotonKey::serialize(StreamBuffer& s)
{
  s >> m_longKey;
  return s;
}

inline std::ostream& RichRecPhotonKey::fillStream(std::ostream& s) const
{
  s << "{ "
    << " longKey:\t" << m_longKey << " } ";
  return s;
}

/// Impliment StreamBuffer >> method for RichRecPhotonKey
inline StreamBuffer& operator >> (StreamBuffer& s, RichRecPhotonKey& key)  {
  long longKey;
  s >> longKey;
  key.setLongKey(longKey);
  return s;
}

/// Impliment StreamBuffer << method for RichRecPhotonKey
inline StreamBuffer& operator << (StreamBuffer& s, const RichRecPhotonKey& key) {
  s << key.longKey();
  return s;
}

#endif   ///RichRecEvent_RichRecPhotonKey_H
