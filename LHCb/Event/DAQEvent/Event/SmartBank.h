// $Id: SmartBank.h,v 1.1 2005-10-14 12:51:59 cattanem Exp $
#ifndef EVENT_SMARTBANK_H 
#define EVENT_SMARTBANK_H 1

// Include files
#include <stddef.h>

/** @class LHCb::SmartBank SmartBank.h Event/SmartBank.h
 *  Convert a bank (e.g. RawBank or subpart) to iterators over short or char  
 *
 *  @author Matt Needham
 *  @date   2005-10-13
 */

namespace LHCb 
{
  template<class TYPE>
  class SmartBank {

  public:

    typedef const TYPE* iterator;

    /**
     *  Constructor
     * @param pointer to data
     * @param data size in bytes
     */
    SmartBank(unsigned int* data, size_t dataSize);

    ~SmartBank(){} ///< Destructor

    /** iterator to start of data
     * @return begin 
     */
    iterator begin() const;
  
    /** iterator to end of data
     * @return end
     */
    iterator end() const;

  private:

    TYPE* m_data;
    size_t m_size;
  };

  template <class TYPE>
  inline typename SmartBank<TYPE>::iterator SmartBank<TYPE>::begin() const{
    return m_data;
  }

  template <class TYPE>
  inline typename SmartBank<TYPE>::iterator SmartBank<TYPE>::end() const{
    return begin() + m_size;
  }

  template <class TYPE>
  inline SmartBank<TYPE>::SmartBank(unsigned int* data, size_t dataSize ){

    m_data = reinterpret_cast<TYPE*>(data);
    unsigned int dBytes = sizeof(TYPE)/sizeof(char);  
    m_size = dataSize/dBytes;
  }
} // namespace LHCb

#endif // EVENT_SMARTBANK_H
