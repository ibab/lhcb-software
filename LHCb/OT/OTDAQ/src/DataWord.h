
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef OTDAQ_DataWord_H
#define OTDAQ_DataWord_H 1

// Include files
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "GaudiKernel/boost_allocator.h"
#include <ostream>

// Forward declarations

namespace LHCb 
{

  // Forward declarations

  /** @class DataWord DataWord.h
   *
   * The Data Word is a word of 32 bits that containes the data of 2 hits of the
 * OT for the OTDAQ data format 
   *
   * @author Jacopo Nardulli
   * created Wed Nov 14 17:03:07 2007
   * 
   */

  class DataWord
  {
  public:

    /// constructor with word, firstOtis, firstChannel, firstTime, null, nextOtis, nextChannel, nextTime, 
    DataWord(int iWord,
             int ifirstOtis,
             int ifirstChannel,
             int ifirstTime,
             int iNull,
             int inextOtis,
             int inextChannel,
             int inextTime);
  
    /// copy constructor
    DataWord(const DataWord& aDataWord);
  
    /// constructor with DataWord
    DataWord(int id) : m_dataWord(id) {}
  
    /// Default Constructor
    DataWord() : m_dataWord(0) {}
  
    /// Default Destructor
     ~DataWord() {}
  
    /// Fill the ASCII output stream
   std::ostream& fillStream(std::ostream& s) const;
  
    /// Return an unsigned int from a DataWord 
    unsigned int returnInt(const DataWord& dataWord);
  
    /// Retrieve const  Data Word ID
    unsigned int dataWord() const;
  
    /// Update  Data Word ID
    void setDataWord(unsigned int value);
  
    /// Retrieve nextTdcTime
    unsigned int nextTime() const;
  
    /// Retrieve nextChannelID
    unsigned int nextChannel() const;
  
    /// Retrieve nextOtisID
    unsigned int nextOtis() const;
  
    /// Retrieve Null
    bool null() const;
  
    /// Retrieve firstTdcTime
    unsigned int firstTime() const;
  
    /// Retrieve firstChannelIF
    unsigned int firstChannel() const;
  
    /// Retrieve firstOtisID
    unsigned int firstOtis() const;
  
    /// Retrieve Word
    bool word() const;
  
  
  #ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(DataWord) == size ? 
               boost::singleton_pool<DataWord, sizeof(DataWord)>::malloc() :
               ::operator new(size) );
    }
  
    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }
  
    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<DataWord, sizeof(DataWord)>::is_from(p) ?
      boost::singleton_pool<DataWord, sizeof(DataWord)>::free(p) :
      ::operator delete(p);
    }
  
    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
  #endif
  protected:

  private:

    /// Offsets of bitfield dataWord
    enum dataWordBits{nextTimeBits     = 0,
                      nextChannelBits  = 8,
                      nextOtisBits     = 13,
                      nullBits         = 15,
                      firstTimeBits    = 16,
                      firstChannelBits = 24,
                      firstOtisBits    = 29,
                      wordBits         = 31};
  
    /// Bitmasks for bitfield dataWord
    enum dataWordMasks{nextTimeMask     = 0xFFL,
                       nextChannelMask  = 0x1F00L,
                       nextOtisMask     = 0x6000L,
                       nullMask         = 0x8000L,
                       firstTimeMask    = 0xFF0000L,
                       firstChannelMask = 0x1F000000L,
                       firstOtisMask    = 0x60000000L,
                       wordMask         = 0x80000000L
                      };
  
  
    unsigned int m_dataWord; ///< Data Word ID
  
  }; // class DataWord

  inline std::ostream& operator<< (std::ostream& str, const DataWord& obj)
  {
    return obj.fillStream(str);
  }
  
} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline LHCb::DataWord::DataWord(int iWord,
                                int ifirstOtis,
                                int ifirstChannel,
                                int ifirstTime,
                                int iNull,
                                int inextOtis,
                                int inextChannel,
                                int inextTime) 
{
    
                  m_dataWord  = (iWord << wordBits) +
                                (ifirstOtis << firstOtisBits) +
                                (ifirstChannel << firstChannelBits) +
                                (ifirstTime << firstTimeBits) +
                                (iNull << nullBits) +
                                (inextOtis << nextOtisBits) +
                                (inextChannel << nextChannelBits) +
                                (inextTime << nextTimeBits);
 
        
}

inline LHCb::DataWord::DataWord(const DataWord& aDataWord) 
{

          m_dataWord = aDataWord.m_dataWord;
        
}

inline std::ostream& LHCb::DataWord::fillStream(std::ostream& s) const
{
  s << "{ " << "dataWord :	" << m_dataWord << std::endl << " }";
  return s;
}


inline unsigned int LHCb::DataWord::dataWord() const 
{
  return m_dataWord;
}

inline void LHCb::DataWord::setDataWord(unsigned int value) 
{
  m_dataWord = value;
}

inline unsigned int LHCb::DataWord::nextTime() const
{
  return (unsigned int)((m_dataWord & nextTimeMask) >> nextTimeBits);
}

inline unsigned int LHCb::DataWord::nextChannel() const
{
  return (unsigned int)((m_dataWord & nextChannelMask) >> nextChannelBits);
}

inline unsigned int LHCb::DataWord::nextOtis() const
{
  return (unsigned int)((m_dataWord & nextOtisMask) >> nextOtisBits);
}

inline bool LHCb::DataWord::null() const
{
  return 0 != ((m_dataWord & nullMask) >> nullBits);
}

inline unsigned int LHCb::DataWord::firstTime() const
{
  return (unsigned int)((m_dataWord & firstTimeMask) >> firstTimeBits);
}

inline unsigned int LHCb::DataWord::firstChannel() const
{
  return (unsigned int)((m_dataWord & firstChannelMask) >> firstChannelBits);
}

inline unsigned int LHCb::DataWord::firstOtis() const
{
  return (unsigned int)((m_dataWord & firstOtisMask) >> firstOtisBits);
}

inline bool LHCb::DataWord::word() const
{
  return 0 != ((m_dataWord & wordMask) >> wordBits);
}

inline unsigned int LHCb::DataWord::returnInt(const DataWord& dataWord) 
{
 return dataWord.m_dataWord  >> 0; 
}


#endif ///OTDAQ_DataWord_H
