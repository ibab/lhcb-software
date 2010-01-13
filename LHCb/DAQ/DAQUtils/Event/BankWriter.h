// $Id: BankWriter.h,v 1.5 2006/03/15 15:00:44 cattanem Exp $
#ifndef EVENT_BANKWRITER_H 
#define EVENT_BANKWRITER_H 1

// Include files
#include <stddef.h>
#include <vector>
#include <cmath>
#include <algorithm>

/** @class LHCb::BankWriter BankWriter.h Event/BankWriter.h
 *  Helper class for writing banks. 
 *  It holds a std::vector<unsigned int> and takes care of writing the
 *  data to the correct place, adding more elements if the initial space
 *  runs out and trimming if there is too much space
 *
 *  @author Matt Needham
 *  @date   2005-10-13
 */
namespace LHCb 
{
  class BankWriter {
  public: 

    /**
     *  Constructor
     * @param blockSize size in 32 bit words
     */
    BankWriter(size_t blockSize);

    ~BankWriter(){} ///< Destructor

    /** get bank data
     *  @return data
     **/
    const std::vector<unsigned int>& dataBank() const;

    /** templated streamer */
    template<class TYPE>
    BankWriter & operator << (TYPE value);

    /** size in bytes */
    std::size_t byteSize() const;

  private:

    /** check if full 
     *  @return bool
     */
    bool isFull() const;

    /** extend the bank */
    void extend();

    mutable std::vector<unsigned int> m_dataBank;
    unsigned char* m_cData;
    size_t m_byteSize;
    size_t m_pos;
  };

  inline BankWriter::BankWriter(size_t blockSize):
    m_pos(0){
    // constructor
    size_t startSize = std::max(size_t(1),blockSize); 
    m_dataBank.resize(startSize);
    m_byteSize = sizeof(int)*startSize;
    m_cData = reinterpret_cast<unsigned char*>(&m_dataBank[0]);
  }

  inline const std::vector<unsigned int>& BankWriter::dataBank() const{
    size_t used = (unsigned int)ceil((double)m_pos/(double)sizeof(int));
    if (m_dataBank.size() > used) m_dataBank.resize(used);
    return m_dataBank;
  }

  bool BankWriter::isFull() const{
    return ( m_pos >= m_byteSize ? true : false );
  }

  void BankWriter::extend(){
    m_dataBank.resize(m_dataBank.size()+1);
    m_cData = reinterpret_cast<unsigned char*>(&m_dataBank[0]);
    m_byteSize += sizeof(int);
  }

  template <class TYPE>
  inline BankWriter& BankWriter::operator << (TYPE value){

    size_t startPos = m_pos;
    m_pos += sizeof(TYPE)/ sizeof(char);

    // extend the bank if necessary 
    if (isFull() == true) {
      extend();
    }

    TYPE* tmp = reinterpret_cast<TYPE*>(&m_cData[startPos]);
    *tmp = value;
    return *this;
  }

  inline std::size_t BankWriter::byteSize() const{
    return m_pos;
  }

} // namespace LHCb

#endif // EVENT_BANKWRITER_H
