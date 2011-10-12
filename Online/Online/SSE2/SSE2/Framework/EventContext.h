#ifndef BITS_EVENTCONTEXT_H
#define BITS_EVENTCONTEXT_H

// Framework include files
#include "Framework/IOPort.h"
#include "Framework/Status.h"
#include <sys/time.h>

/*
*    Framework namespace declaration
*/
namespace Framework {

  /** @class EventContext EventContext.h Framework/EventContext.h
  *
  * @author  M.Frank
  * @version 1.0
  * @date    20/06/2011
  */
  class EventContext   {
  private:
    enum { IO_WORDS  = 1, MASK_WIDTH = IO_WORDS, DATA_WIDTH = IO_WORDS*128 };
    enum { ALG_WORDS = 3, ALG_NUM    = ALG_WORDS*128 };
  public:
    typedef AVXMask<IO_WORDS>  DataMask;
    typedef AVXMask<ALG_WORDS> AlgorithmMask;
    //typedef AVXMask<1> AlgorithmMask;
    DataMask       _ioMask;
    AlgorithmMask  _algMask;
    AlgorithmMask  _doneMask;
    long _id;
    struct timeval _start;
  public:
    typedef DataMask Mask;

    /// Default constructor
    EventContext(long id);
    /// Initializing constructor
    EventContext(long id,const IOMask& m);
    /// Default destructor
    virtual ~EventContext()                       {                                      }
    /// Get the current time stamp
    struct timeval now() const;
    /// Calculate the execution time of the entrie event
    struct timeval execTime() const;
    /// Access the start time of this event
    struct timeval start() const                  {  return _start;                      }
    long id() const                               {  return _id;                         }
    //bool operator<(const EventContext& c) const {  return _ioMask.less(c._ioMask);     }
    void dumpIO(const std::string& txt)  const    {  _ioMask.dump(txt);                  }
    bool matchIO(const EventContext& other) const {  return _ioMask.match(other._ioMask);}
    void addData(const IOMask& other)             {  _ioMask.mask_or(other);             }
    bool matchIO(const IOMask& other) const       {  return _ioMask.match(other);        }
    void clearAlgBit(size_t bit)                  {  _algMask.clear_bit(bit);            }
    void clearDoneBit(size_t bit)                 {  _doneMask.clear_bit(bit);           }
    void setAlgMask(const AlgorithmMask& mask)    {
      _algMask.assign(mask);
      _doneMask.assign(mask);
    }
    bool algMaskEmpty() const                     {  return _algMask.empty();            }
    bool doneMaskEmpty() const                    {  return _doneMask.empty();           }
    bool testAlgBit(int bit) const                { return _algMask.test_bit(bit); }
  };

  /** @class EventContextFactory EventContext.h Framework/EventContext.h
  *
  * @author  M.Frank
  * @version 1.0
  * @date    20/06/2011
  */
  class EventContextFactory   {
  protected:
    /// Default destructor
    virtual ~EventContextFactory() {}
  public:
    /// Release factory. Factory may not be used anymore afterwards
    virtual void release() = 0;
    /// Declare input data content to data registry
    virtual Status declareData() = 0;
    /// Create a new event context. On error the returned context is invalid (NULL)
    virtual EventContext* create() = 0;
    /// Get access to the IOMask to resolve the dependencies
    virtual const IOMask& dataMask() const = 0;
  };
}
#endif // BITS_EVENTCONTEXT_H

