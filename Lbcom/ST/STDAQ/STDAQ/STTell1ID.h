// $Id: STTell1ID.h,v 1.1.1.1 2005-12-20 12:47:27 mneedham Exp $
#ifndef _STTell1ID_H
#define _STTell1ID_H 1

/** @class STTell1ID STTell1ID.h "STDAQ/STTell1ID.h"
 *
 *  Class to describe a STTell1ID
 *
 *  @author M.Needham
 *  @date   7/01/2004
 */

#include <iostream>


class STTell1ID{

public:

  /// constructer with strip and waferIndex
  STTell1ID(unsigned int aRegion,
            unsigned int aSubID){

    m_id = (aRegion<<regionBits) +
           (aSubID<<subIDBits);                  
  }
  
  explicit STTell1ID(int id):
  m_id(id){}

  /// Default Constructor
  STTell1ID()
    : m_id(0) {}

  /// Destructor
  virtual ~STTell1ID() {}

  /// wafer 
  unsigned int region() const; 

  /// strip
  unsigned int subID() const;

  /// cast
  operator int() const;
 
  /// Retrieve IT Channel ID
  unsigned int id() const;

  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& s, const STTell1ID& obj)
  {
    return obj.fillStream(s);
  }

  // Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  enum General { nullBoard =  0x0000ffff};

private:

  enum bits  {subIDBits = 0,  regionBits = 4}; /// Enumeration to store the bit packing offsets
  enum masks {subIDMask = 0x0000000f, regionMask = 0x00000030};

  unsigned int m_id; /// STell1ID
};

inline STTell1ID::operator int() const
{
   return m_id;
}

inline unsigned int STTell1ID::id() const
{
  return m_id;
}

inline unsigned int STTell1ID::region() const 
{
  return ((m_id & regionMask) >> regionBits);
}

inline unsigned int STTell1ID::subID() const
{
  return ((m_id & subIDMask) >> subIDBits);
}

inline std::ostream& STTell1ID::fillStream(std::ostream& s) const
{
  s << "{ "
    << " STTell1ID:\t" << std::hex << id() << std::endl
    << std::dec << " region:\t"     << region() << std::endl
    << " subID:\t"     << subID() << " } ";

  return s;
}

#endif // _STTell1ID_H


