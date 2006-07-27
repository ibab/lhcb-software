
#ifndef LESTER_RATCHET_H
#define LESTER_RATCHET_H

namespace Lester {

  template<class ComparedType>
  class Ratchet {
    // By default, records ever larger values of something, checking for size differences with the operator::< (less) applied to the ComaredType objects ...
  public:
    Ratchet() : m_is_unused(true) {};
    void reset() {
      m_is_unused = true;
    };
    bool is_unused() const {
      return m_is_unused;
    };
    /// maxSoFar has undefined action if is_unused() returns true.   It's up to you to check!
    ComparedType maxSoFar() const {
      return m_maxSoFar;
    };
    /// consider takes note of its arguments and remembers them if value is greater than all other values it has seen since being constructed or reset. (Uses operator < on ComparedType)  It return true if the value is the greatest seen so far since the last reset. 
    bool consider(const ComparedType & value) {
      const bool result = (m_is_unused || m_maxSoFar < value);
      if (result) {
	m_maxSoFar = value;
	m_is_unused = false;
      };
      return result;
    };
  private:
    bool m_is_unused;
    ComparedType m_maxSoFar;
  };

  template<class IndexType, class ComparedType>
  class IndexedRatchet {
    // By default, records ever larger values of something, checking for size differences with the operator::< (less) applied to the ComaredType objects ...
  public:
    IndexedRatchet() : m_is_unused(true) {};
    void reset() {
      m_is_unused = true;
    };
    bool is_unused() const {
      return m_is_unused;
    };
    /// maxSoFar has undefined action if is_unused() returns true.   It's up to you to check!
    ComparedType maxSoFar() const {
      return m_maxSoFar;
    };
    /// IndexForaxSoFar has undefined action if is_unused() returns true.  It's up to you to check!
    IndexType indexForMaxSoFar() const {
      return m_indexForMaxSoFar;
    };
    /// consider takes note of its arguments and remembers them if value is greater than all other values it has seen since being constructed or reset. (Uses operator < on ComparedType)  It return true if the value is the greatest seen so far since the last reset. 
    bool consider(const IndexType & index, const ComparedType & value) {
      const bool result = (m_is_unused || m_maxSoFar < value);
      if (result) {
	m_maxSoFar = value;
	m_is_unused = false;
	m_indexForMaxSoFar = index;
      };
      return result;
    };
  private:
    bool m_is_unused;
    ComparedType m_maxSoFar;
    IndexType m_indexForMaxSoFar;
  };

}

#endif
