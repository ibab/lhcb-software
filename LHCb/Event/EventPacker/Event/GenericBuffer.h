#ifndef __EVENTPACKER_EVENT_GENERICBUFFER_H__
#define __EVENTPACKER_EVENT_GENERICBUFFER_H__

#include "GaudiKernel/DataObject.h"

namespace LHCb
{

  /** @class GenericBuffer
   *
   *  Generic char buffer for storing data.
   *
   *  @author Wouter Hulsbergen
   *  @date   2015-10-01
   */

  static const CLID CLID_GenericBuffer = 1556 ;

  class GenericBuffer : public DataObject
  {

  public:

    /// type of data holder
    typedef std::vector<char> DataType ;

    /// non-const accessor
    DataType& data() { return m_data; }

    /// const accessor
    const DataType& data() const { return m_data; }

    /// Class ID
    static const CLID& classID() { return CLID_GenericBuffer ; }

    /// Class ID
    virtual const CLID& clID() const { return GenericBuffer::classID(); }

  private:

    std::vector<char> m_data ;

  } ;
}

#endif
