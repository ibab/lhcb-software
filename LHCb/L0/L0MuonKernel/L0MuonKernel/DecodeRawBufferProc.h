// $Id: DecodeRawBufferProc.h,v 1.1 2006-03-07 11:26:10 jucogan Exp $
#ifndef L0MUONKERNEL_DECODERAWBUFFERPROC_H 
#define L0MUONKERNEL_DECODERAWBUFFERPROC_H 1

// Include files

/** @class DecodeRawBufferProc DecodeRawBufferProc.h L0MuonKernel/DecodeRawBufferProc.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2006-02-13
 */

#include "Kernel/MuonTileID.h"
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/MuonCandidate.h"

namespace L0Muon {


  class DecodeRawBufferProc {


  public: 
    /// Standard constructor
    DecodeRawBufferProc( );
  
    /// Destructor
    virtual ~DecodeRawBufferProc( );

    /// Set bitset
    void setBitset(boost::dynamic_bitset<> bitset, bool compressed=true);

    std::vector<MuonCandidate> candidatesBCSU();
    std::vector<MuonCandidate> candidatesPU();
    boost::dynamic_bitset<> inputDATA(bool expand=true);
    boost::dynamic_bitset<> nonEmptyPU();

  private:
    
    void uncompress(int start=-1, int stop=-1);
    boost::dynamic_bitset<> unapplyRLE(boost::dynamic_bitset<> compressed);
    boost::dynamic_bitset<> unapplyOCounting(boost::dynamic_bitset<> compressed);

  private:

    bool m_compressed;
    boost::dynamic_bitset<> m_buffer;

  };
}; // namespace L0Muon

#endif // L0MUONKERNEL_DECODERAWBUFFERPROC_H
