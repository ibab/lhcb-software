#ifndef PACKEDDATABUFFER_H
#define PACKEDDATABUFFER_H 1

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

#include "Compression.h"


namespace PackedDataPersistence {

/// Save a serializable object to buffer.
template<typename Buffer, typename T>
auto save(Buffer& buf, const T& object)
  -> decltype(object.save(buf), void())
{
  object.save(buf);
}

/// Load a versioned serializable object from buffer.
template<typename Buffer, typename T>
auto load(Buffer& buf, T& object, unsigned int version)
  -> decltype(object.save(buf), void())
{
  object.load(buf, version);
}

/// Load a non-versioned serializable object from buffer.
template<typename Buffer, typename T>
auto load(Buffer& buf, T& object)
  -> decltype(object.save(buf), void())
{
  object.load(buf);
}

/// Save a std::pair to buffer.
template<typename Buffer, typename K, typename V>
void save(Buffer& buf, const std::pair<K,V>& pair) {
  buf.io(pair.first, pair.second);
}

/// Load a std::pair from buffer.
template<typename Buffer, typename K, typename V>
void load(Buffer& buf, std::pair<K,V>& pair, unsigned int /*version*/) {
  buf.io(pair.first, pair.second);
}


/** @class ByteBuffer PackedDataBuffer.h
 *  Helper class that represents a byte buffer.
 *
 *  @author Rosen Matev
 *  @date   2016-01-03
 */
class ByteBuffer {
public:
  using buffer_type = std::vector<uint8_t>;

  /// Standard constructor
  ByteBuffer() = default;
  /// Construct from an existing buffer
  ByteBuffer(const buffer_type& data) : m_buffer{data} {}

  /// Return the current position in the buffer.
  std::size_t pos() const { return m_pos; }
  /// Return whether the end of buffer was reached.
  bool eof() const { return m_pos >= m_buffer.size(); }
  /// Clear the buffer and reset position to zero.
  void clear() { m_buffer.clear(); m_pos = 0; }
  /// Initialize from an existing buffer and reset position to zero.
  bool init(const buffer_type& data, bool compressed=false);
  /// Return the internal buffer.
  const buffer_type& buffer() { return m_buffer; }
  /// Compress the buffer
  bool compress(ROOT::ECompressionAlgorithm algorithm, int level, buffer_type& output) const;

  /// Skip reading n bytes.
  std::size_t readNull(std::size_t n) {
    if (m_pos + n > m_buffer.size())
      throw std::runtime_error("ByteBuffer(): requested a read beyound buffer size!");
    auto index0 = m_pos;
    m_pos += n;
    return index0;
  }
  /// Read an object from the current position.
  template<typename T>
  void read(T& x) {
    if (m_pos + sizeof(x) > m_buffer.size())
      throw std::runtime_error("ByteBuffer(): requested a read beyound buffer size!");
    std::copy_n(&(m_buffer[m_pos]), sizeof(x), (uint8_t*)&x);
    m_pos += sizeof(x);
  }
  /// Read an object from a given position.
  template<typename T>
  void read(T& x, std::size_t pos) {
    if (pos + sizeof(x) > m_buffer.size())
      throw std::runtime_error("ByteBuffer(): requested a read beyound buffer size!");
    std::copy_n(&(m_buffer[pos]), sizeof(x), (uint8_t*)&x);
  }

  /// Skip writing n bytes.
  std::size_t writeNull(std::size_t n) {
    auto index0 = m_pos;
    m_buffer.resize(m_pos + n);
    m_pos += n;
    return index0;
  }
  /// Write an object at the current position.
  template<typename T>
  void write(const T& x) {
    m_buffer.resize(m_pos + sizeof(x));
    std::copy_n((uint8_t*)&x, sizeof(x), &(m_buffer[m_pos]));
    m_pos += sizeof(x);
  }
  /// Write an object at a given position.
  template<typename T>
  void write(const T& x, std::size_t pos) {
    if (pos + sizeof(x) > m_buffer.size())
      throw std::runtime_error("ByteBuffer(): requested a write beyound buffer size!");
    std::copy_n((uint8_t*)&x, sizeof(x), &(m_buffer[pos]));
  }

private:
  buffer_type m_buffer; ///< Internal buffer
  std::size_t m_pos{0}; ///< Position in buffer
};


/** @class SerializationOffsets PackedDataBuffer.h
 *  Helper class for determining the offsets of serializable class fields.
 *
 *  @author Rosen Matev
 *  @date   2016-01-03
 */
template<typename POD>
class SerializationOffsets {
public:
  /// Construct offsets for saving (serialization)
  SerializationOffsets() {
    POD x;
    PackedDataPersistence::save(*this, x);
  }
  /// Construct offsets for loading (de-serialization)
  SerializationOffsets(unsigned int version) {
    POD x;
    PackedDataPersistence::load(*this, x, version);
  }
  /// Return the offsets in bytes of the class fields.
  const std::vector<unsigned int>& offsets() const {
    return m_offsets;
  }
  /// Function called by serializable object save/load methods.
  template <typename ...Args>
  void io(Args&&... args) {
    auto list = {(io_impl(args),1)...};
    (void)list;
  }

private:
  /// Implementation of the io() function for single argument.
  template<typename T>
  void io_impl(T x) {
    m_offsets.push_back(m_offsets.back() + sizeof(x));
  }
  /// Offsets in bytes of the class fields
  std::vector<unsigned int> m_offsets{0};
};


/** @class SOABuffer PackedDataBuffer.h
 *  Helper class for (de-)serializing a vector of serializable objects into a
 *  structure of arrays (column) format.
 *
 *  @author Rosen Matev
 *  @date   2016-01-03
 */
template<typename Buffer>
class SOABuffer {
public:
  /// Constructor
  template<typename POD>
  SOABuffer(Buffer* buffer, std::size_t pos, std::size_t length, const SerializationOffsets<POD>& offsets)
    : m_buffer{buffer}, m_pos{pos},
      m_nmembers{offsets.offsets().size() - 1},
      m_offsets{offsets.offsets()},
      m_index{0}
  {
    for (auto& x: m_offsets)
      x *= length;
  }
  /// Function called by serializable object save/load methods.
  template <typename ...Args>
  void io(Args&&... args) {
    auto list = {(io_impl(args),1)...};
    (void)list;
  }
  
private:
  /// Implementation of the io() function for single argument.
  template<typename T>
  void io_impl(T& x) {
    auto memberIndex = m_index % m_nmembers;
    auto arrayIndex = m_index / m_nmembers;
    auto offset = m_offsets[memberIndex] + sizeof(x) * arrayIndex;
    m_buffer->ioAt(x, m_pos + offset);
    ++m_index;
  }
  
  Buffer* m_buffer; ///< Main buffer
  std::size_t m_pos; ///< Position in main buffer where to save/load the SOA data
  std::size_t m_nmembers; ///< Number of fields in the structure
  std::vector<unsigned int> m_offsets; ///< Offsets of arrays in SOA data
  std::size_t m_index; ///< The sequential index of the next element to be save/loaded
};


/** @class PackedDataOutBuffer PackedDataBuffer.h
 *  Helper class for serializing data.
 *
 *  @author Rosen Matev
 *  @date   2016-01-03
 */
class PackedDataOutBuffer {
public:
  /// Clear the internal byte buffer.
  void clear() { m_buffer.clear(); }
  /// Return a reference to the internal buffer.
  const std::vector<uint8_t>& buffer() { return m_buffer.buffer(); }
  /// Compress the buffer
  bool compress(ROOT::ECompressionAlgorithm algorithm, int level, ByteBuffer::buffer_type& output) const {
    return m_buffer.compress(algorithm, level, output);
  }
  /// Function called by serializable objects' save method.
  template <typename ...Args>
  void io(Args&&... args) {
    auto list = {(save(args),1)...};
    (void)list;
  }
  /// Save an object at a given position.
  template<typename T> void ioAt(T x, std::size_t pos) { saveAt(x, pos); }

  // ===========================================================================
  // Dealing with scalars
  // ===========================================================================

  /// Save a scalar.
  template<typename T>
  typename std::enable_if<std::is_scalar<T>::value, std::pair<std::size_t, std::size_t> >::type save(T x) {
    auto index0 = m_buffer.pos();
    m_buffer.write(x);
    return std::make_pair(index0, sizeof(x));
  }
  /// Save a scalar at a given position.
  template<typename T>
  void saveAt(T x, std::size_t pos) {
    m_buffer.write(x, pos);
  }
  /// Save a size integer.
  void saveSize(std::size_t x) {
    save<uint32_t>(x);
  }
  /// Save a vector of scalars.
  template<typename T>
  typename std::enable_if<std::is_scalar<T>::value>::type
  save(const std::vector<T>& v) {
    saveSize(v.size());
    for (const auto& x: v)
      save(x);
  }

  // ===========================================================================
  // Dealing with objects
  // ===========================================================================

  /// Save a serializable object.
  template<typename T>
  auto save(const T& object) 
    -> decltype(PackedDataPersistence::save(*this, object), std::pair<std::size_t, std::size_t>())
  {
    auto index0 = m_buffer.pos();
    PackedDataPersistence::save(*this, object);
    return std::make_pair(index0, m_buffer.pos() - index0);
  }
  /// Save a vector of serializable objects in AOS format.
  template<typename T>
  void saveAOS(const std::vector<T>& v) {
    saveSize(v.size());
    for (const auto& object: v)
      PackedDataPersistence::save(*this, object);
  }
  /// Save a vector of serializable objects in SOA format.
  template<typename T>
  void saveSOA(const std::vector<T>& v) {
    saveSize(v.size());
    if (v.empty()) return;
    static const SerializationOffsets<T> offsets{};
    auto totalSize = offsets.offsets().back() * v.size();
    auto pos = m_buffer.writeNull(totalSize); // reserve the space
    SOABuffer<PackedDataOutBuffer> soabuf{this, pos, v.size(), offsets};
    for (const auto& object: v)
      PackedDataPersistence::save(soabuf, object);
  }
  /// Save a vector of serializable objects in the default format.
  template<typename T>
  auto save(const std::vector<T>& v)
    -> decltype(PackedDataPersistence::save(*this, v[0]), void())
  {
    saveSOA(v);
  }

private:
  ByteBuffer m_buffer; ///< Internal byte buffer
};


/** @class PackedDataInBuffer PackedDataBuffer.h
 *  Helper class for de-serializing data.
 *
 *  @author Rosen Matev
 *  @date   2016-01-03
 */
class PackedDataInBuffer {
public:
  /// Return whether the end of buffer was reached.
  bool eof() const { return m_buffer.eof(); }
  /// Skip a number of bytes from the buffer.
  void skip(std::size_t n) { m_buffer.readNull(n); }
  /// Initialize from an existing byte buffer.
  bool init(const ByteBuffer::buffer_type& data, bool compressed=false) {
    return m_buffer.init(data, compressed);
  }
  /// Clear the internal byte buffer.
  void clear() { m_buffer.clear(); }

  /// Function called by serializable objects' load method.
  template <typename ...Args>
  void io(Args&&... args) {
    auto list = {(load(args),1)...};
    (void)list;
  }
  /// Load an object from a given position.
  template<typename T> void ioAt(T& x, std::size_t i) { loadAt(x, i); }

  // ===========================================================================
  // Dealing with scalars
  // ===========================================================================

  /// Load a scalar in place.
  template<typename T>
  typename std::enable_if<std::is_scalar<T>::value>::type
  load(T& x) {
    m_buffer.read(x);
  }
  /// Load a scalar from a given position.
  template<typename T>
  void loadAt(T& x, std::size_t i) {
    m_buffer.read(x, i);
  }
  /// Load a scalar and return it.
  template<typename T>
  T load() {
    T x;
    m_buffer.read(x);
    return x;
  }
  /// Load a size integer and return it.
  std::size_t loadSize() {
    uint32_t y;
    io(y);
    return y;
  }
  /// Load a vector of scalars.
  template<typename T>
  typename std::enable_if<std::is_scalar<T>::value>::type load(std::vector<T>& v) {
    v.resize(loadSize());
    for (const auto& x: v)
      load(x);
  }

  // ===========================================================================
  // Dealing with objects
  // ===========================================================================

  /// Load a serializable object.
  template<typename T>
  auto load(T& object) 
    -> decltype(PackedDataPersistence::load(*this, object), std::size_t())
  {
    auto index0 = m_buffer.pos();
    PackedDataPersistence::load(*this, object);
    return m_buffer.pos() - index0;
  }
  /// Load a vector of serializable objects with AOS format.
  template<typename T>
  void loadAOS(std::vector<T>& v, unsigned int version) {
    v.resize(loadSize());
    for (auto& object: v)
      PackedDataPersistence::load(*this, object, version);
  }
  /// Load a vector of serializable objects with SOA format.
  template<typename T>
  void loadSOA(std::vector<T>& v, unsigned int version) {
    v.resize(loadSize());
    if (v.empty()) return;
    const SerializationOffsets<T> offsets{version}; // TODO can we optimize this?
    auto totalSize = offsets.offsets().back() * v.size();
    auto pos = m_buffer.readNull(totalSize); // mark the data as read
    SOABuffer<PackedDataInBuffer> soabuf{this, pos, v.size(), offsets};
    for (auto& object: v)
      PackedDataPersistence::load(soabuf, object, version);
  }
  /// Load a vector of serializable objects with the default format.
  template<typename T>
  auto load(std::vector<T>& v, unsigned int version)
    -> decltype(PackedDataPersistence::load(*this, v[0], version), void())
  {
    loadSOA(v, version);
  }

private:
  ByteBuffer m_buffer; ///< Internal byte buffer
};

} // PackedDataPersistence

#endif // PACKEDDATABUFFER_H