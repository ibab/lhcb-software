#ifndef FRAMEWORK_AVX_H
#define FRAMEWORK_AVX_H

// C++ include files
#include <emmintrin.h>
#include <algorithm>
#include <vector>
#include <string>

/*
*    Framework namespace declaration
*/
namespace Framework  {

  class AVXMemory  {
  public:
    typedef std::pair<const __m128i*,size_t> Registers;
    /// Enumeration to be checked if a bit is set
    enum { NO_BIT_SET=~0x0, SSE2_BITS=128, ALIGNMENT=15 };
  public:
    __m128i* first;
    size_t   second;
    /// Access to static bitmap [0...128]
    static const Registers SSE_bits();
    /// Access to static NOT bitmap [0...128]
    static const Registers SSE_not_bits();

    /// Default constructor
    AVXMemory()  {  first = 0; second=0;                   }
    /// Copy constructor. Careful: This does not copy the memory to a new location
    AVXMemory(const AVXMemory& copy) {  first = copy.first; second = copy.second;     }
    /// Default destructor
    ~AVXMemory()                                      {                       }
    /// Automatic data conversion to access aligned bitfield
    operator Registers() const              { return Registers(first,second); }
    /// Accessor for registers
    Registers registers() const             { return Registers(first,second); }
    /// Set bit mask to NULL
    void zero();
    /// Check if any bit is set in the mask
    bool empty() const;
    /// Copy mask content
    void assign(const Registers& bits);
    /// Set bit in mask
    void set_bit(register size_t bit);
    /// Clear a bit in mask
    void clear_bit(register size_t bit);
    /// Test if a bit is set. If the return code is true, the bit is set. else not.
    bool test_bit(register size_t bit) const;
    /// Search the first free bit
    size_t first_set()  const { return first_set_int32(); }
    /// Search the first free bit. 32-bit scan version
    size_t first_set_int32()  const;
    /// Search the first free bit. 32-bit scan version
    size_t first_set_SSE()  const;
    /// Apply AND mask to existing bits. If other mask has less words, only apply the mask to existing words
    void mask_and(const Registers& mask);
    /// Apply OR mask to existing bits. If other mask has less words, only apply the mask to existing words
    void mask_or(const Registers& mask);
    /// Check if 2 bit masks are equivalent
    bool match(const AVXMemory& other)  const;

    /// Retrieve the items set in the mask
    std::vector<size_t> bits() const;
    /// Retrieve the items NOT set in this mask, but set in the test mask
    std::vector<size_t> missing(const AVXMemory& test) const;

  };

  class AVXHeap  : public AVXMemory  {
  protected:
    unsigned char* _buffer;
    void _deallocate();
    void _extend(size_t bits);
  public:
    /// Default constructor
    AVXHeap() : _buffer(0)                             {                   }
    /// Initializing constructor
    AVXHeap(size_t bits) : _buffer(0)                  {  extend(bits);    }
    /// Copy constructor
    AVXHeap(const AVXMemory& copy);
    /// Default destructor
    ~AVXHeap()                    { if ( _buffer ) _deallocate();          }
    /// Extend register buffer using the heap
    void extend(size_t bits)      { if ((second<<7) < bits) _extend(bits); }
    /// Dump content to standard output
    void dump(const std::string& txt = "") const;
    /// Convert values to printable string
    std::string toString() const;
  };

  template <int WORDS> struct AVXMask : public AVXMemory {
    private:
    public:
    char _buffer[WORDS*ALIGNMENT+ALIGNMENT];
    public:
    /// Default constructor
    AVXMask();
    /// Copy constructor
    AVXMask(const AVXMemory& copy);
    /// Initializing constructor
    AVXMask(unsigned int* mask);
    /// Dump content to standard output
    void dump(const std::string& txt="") const;
    /// Convert values to printable string
    std::string toString() const;

    static size_t max_bits()   {  return SSE2_BITS*WORDS;  }

    void set_bits(const Registers& bits);
    /// Set bit in mask
    void set_bit(register size_t bit);
    /// Clear a bit in mask
    void clear_bit(register size_t bit);
    /// Test if a bit is set. If the return code is true, the bit is set. else not.
    bool test_bit(register size_t bit) const;
    template <class T> void mask_and(const T& mask);
    template <class T> void mask_or (const T& mask);
    template <class T> bool match   (const T& mask)  const;
  };
}
#endif // FRAMEWORK_AVX_H
