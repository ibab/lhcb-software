#include "RZip.h"

#include "PackedDataBuffer.h"

namespace PackedDataPersistence {

bool ByteBuffer::compress(ROOT::ECompressionAlgorithm algorithm, int level, buffer_type& output) const {
  int output_size = 0;
  int input_size = m_buffer.size();
  int buffer_size = input_size;
  output.clear();
  output.resize(buffer_size); // TODO get rid of unneeded initialization

  R__zipMultipleAlgorithm(level,
                          &input_size, (char*)&(m_buffer[0]),
                          &buffer_size, (char*)&(output[0]),
                          &output_size, algorithm);

  if (output_size == 0 || output_size >= input_size) {
    // the input cannot be compressed or size would be greater than original
    output_size = 0;
  }
  output.resize(output_size);
  return output_size > 0;
}

bool ByteBuffer::init(const buffer_type& data, bool compressed) {
  m_pos = 0;
  if (!compressed) {
    m_buffer = data;
    return true;
  }

  m_buffer.resize(data.size() * 4); // TODO get rid of unneeded initialization
  while (true) {
    int output_size = 0;
    int input_size = data.size();
    int buffer_size = m_buffer.size();
    R__unzip(&input_size, (unsigned char*)&data[0],
             &buffer_size, (unsigned char*)&m_buffer[0], &output_size);
    if (output_size > 0) {
      m_buffer.resize(output_size);
      return true;
    } else if (m_buffer.size() < 64 * data.size()) {
      // Maybe the buffer is not big enough?
      m_buffer.resize(2 * m_buffer.size());
    } else {
      // Failed to decompress data, quitting
      m_buffer.resize(0);
      return false;
    }
  }
}

}
