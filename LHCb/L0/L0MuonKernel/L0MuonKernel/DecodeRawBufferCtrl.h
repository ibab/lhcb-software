// $Id: DecodeRawBufferCtrl.h,v 1.1 2006-03-07 11:26:10 jucogan Exp $
#ifndef L0MUONKERNEL_DECODERAWBUFFERCTRL_H 
#define L0MUONKERNEL_DECODERAWBUFFERCTRL_H 1

// Include files

/** @class DecodeRawBufferCtrl DecodeRawBufferCtrl.h L0MuonKernel/DecodeRawBufferCtrl.h
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


  class DecodeRawBufferCtrl {


  public: 
    /// Standard constructor
    DecodeRawBufferCtrl( );
  
    /// Destructor
    virtual ~DecodeRawBufferCtrl( );

    /// Set bitset
    void setBitset(boost::dynamic_bitset<> bitset);

    std::vector<MuonCandidate> candidatesCTRL();
    std::vector<MuonCandidate> candidatesBCSU();
    

  private:

    boost::dynamic_bitset<> m_buffer;

  };
}; // namespace L0Muon

#endif // L0MUONKERNEL_DECODERAWBUFFERCTRL_H
