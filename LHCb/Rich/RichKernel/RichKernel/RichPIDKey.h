#ifndef Event_RichPIDKey_H
#define Event_RichPIDKey_H 1

// Include files
#include "Kernel/CLHEPStreams.h"
#include "RichKernel/RichDefinitions.h"

/** @class RichPIDKey RichPIDKey.h
 *
 *  Smart Key for Rich PID Objects
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  created Tue Feb 26 09:25:55 2002
 *
 */

class RichPIDKey {

public:

  /// Constructor from long int
  RichPIDKey(long longKey) { m_pidKey = longKey; }

  /// Default Constructor
  RichPIDKey() { m_pidKey = 0; }

  /// Destructor
  virtual ~RichPIDKey() {}

  /// Encodes the Algorithm type into the 32bit integer key
  void setPIDAlgorithmType(Rich::AlgorithmType richPIDAlgorithm);

  /// Returns the parent PID algorithm type
  Rich::AlgorithmType PIDAlgorithmType() const;

  /// Encodes the PID object serial number into 32bit integer
  void setPIDSerialNumber(int richPIDserialNumber);

  /// Returns object serial number
  int PIDSerialNumber() const;

  /// Retrieve 32 bit integer key
  long int pidKey() const;

  /// Update 32 bit integer key
  void setPidKey(long value);

  /// Serialize the object for writing
  StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  std::ostream& fillStream(std::ostream& s) const;

  /// long operator
  // operator long() const { return m_pidKey|0xFF000000; }
  operator long() const { return m_pidKey; }

protected:

private:

  long m_pidKey; ///<  32 bit integer key

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


inline void RichPIDKey::setPIDAlgorithmType( Rich::AlgorithmType 
                                             richPIDAlgorithm    )
{
  m_pidKey = (m_pidKey % 100000) + ((int)richPIDAlgorithm)*100000;
}

inline Rich::AlgorithmType RichPIDKey::PIDAlgorithmType() const
{
  return (Rich::AlgorithmType)(m_pidKey / 100000);
}

inline void RichPIDKey::setPIDSerialNumber(int richPIDserialNumber)
{
  m_pidKey = m_pidKey - (m_pidKey % 100000) + richPIDserialNumber;
}

inline int RichPIDKey::PIDSerialNumber() const
{
  return (m_pidKey % 100000);
}

inline long int RichPIDKey::pidKey() const
{
  return m_pidKey;
}

inline void RichPIDKey::setPidKey(long int value)
{
  m_pidKey = value;
}

inline StreamBuffer& RichPIDKey::serialize(StreamBuffer& s) const
{
  s << m_pidKey;
  return s;
}

inline StreamBuffer& RichPIDKey::serialize(StreamBuffer& s)
{
  s >> m_pidKey;
  return s;
}

inline std::ostream& RichPIDKey::fillStream(std::ostream& s) const
{
  s << "{ "
    << " pidKey:\t" << m_pidKey << " } ";
  return s;
}

/// Impliment StreamBuffer >> method for RichPIDKey
inline StreamBuffer& operator >> (StreamBuffer& s, RichPIDKey& key)  {
  long longKey;
  s >> longKey;
  key.setPidKey(longKey);
  return s;
}

/// Impliment StreamBuffer << method for RichPIDKey
inline StreamBuffer& operator << (StreamBuffer& s, const RichPIDKey& key)  {
  s << key.pidKey();
  return s;
}

#endif   ///Event_RichPIDKey_H
