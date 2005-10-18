// $Id: ByteStream.h,v 1.2 2005-10-18 06:45:49 mneedham Exp $
#ifndef EVENT_BYTESTREAM_H 
#define EVENT_BYTESTREAM_H 1

// Include files
#include <stddef.h>

/** @class LHCb::ByteStream ByteStream.h Event/ByteStream.h
 *  Read a stream of bytes that are ints, shorts, chars in any order
 *
 *  @author Matt Needham
 *  @date   2005-10-13
 */
namespace LHCb 
{
  class ByteStream {

  public: 
    /**
     *  Constructor
     * @param pointer to data
     * @param data size in bytes
     */
    ByteStream(unsigned int* bank, size_t size);

    ~ByteStream(){} ///< Destructor

    /** templated streamer */
    template<class TYPE>
    ByteStream & operator >> (TYPE& value);

  private:

    unsigned char* m_start;
    unsigned int m_size;
    unsigned int m_pos;

  };
  inline ByteStream::ByteStream(unsigned int* bank, 
                                size_t size):
    m_start(0),
    m_size(size),
    m_pos(0){
    // constructor
    m_start = reinterpret_cast<unsigned char*>(bank);
  }

  template <class TYPE>
  inline ByteStream& ByteStream::operator >> (TYPE& value){

    TYPE* tmp = reinterpret_cast<TYPE*>(&m_start[m_pos]);
    value = *tmp;
    m_pos += sizeof(TYPE)/ sizeof(char);
    return *this;
  }

} // namespace LHCb

#endif // EVENT_BYTESTREAM_H
