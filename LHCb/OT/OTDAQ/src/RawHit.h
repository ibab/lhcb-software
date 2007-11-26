
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

#ifndef OTDAQ_OTChannelHit_H
#define OTDAQ_OTChannelHit_H 1

// Include files
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "GaudiKernel/boost_allocator.h"
#include <ostream>

// Forward declarations

namespace OTDAQ
{

  // Forward declarations

  /** @class RawHit RawHit.h
   *
   * The Data Word is a word of 32 bits that containes the data of 2 hits of the
 * OT for the OTDAQ data format 
   *
   * @author Jacopo Nardulli
   * created Wed Nov 14 17:03:07 2007
   * 
   */

  class RawHit
  {
  public:

    /// constructor with RawHit
    RawHit(unsigned short data) : m_data(data) {}
  
    /// Default Constructor
    RawHit() : m_data(0) {}
  
    /// Retrieve tdc time
    unsigned int time() const;
  
    /// Retrieve channel number
    unsigned int channel() const;
  
    /// Retrieve otis number
    unsigned int otis() const;

    /// Retrieve another bit that I do not know the meaning of
    bool word() const;

    /// Retrieve the contents
    unsigned short data() const { return m_data ; }

  public:

    /// Offsets of bitfield dataWord
    enum dataWordBits{TimeBits     = 0,
                      ChannelBits  = 8,
                      OtisBits     = 13,
                      wordBits     = 15
    };
    
    /// Bitmasks for bitfield dataWord
    enum dataWordMasks{TimeMask     = 0xFFL,
                       ChannelMask  = 0x1F00L,
                       OtisMask     = 0x6000L,
                       wordMask     = 0x8000L,
    };
    
  private:
    unsigned short m_data; ///< Data Word ID
  
  }; // class RawHit

  // -----------------------------------------------------------------------------
  // end of class
  // -----------------------------------------------------------------------------
  
  
  inline unsigned int RawHit::time() const
  {
    return (unsigned int)((m_data & TimeMask) >> TimeBits);
  }
  
  inline unsigned int RawHit::channel() const
  {
    return (unsigned int)((m_data & ChannelMask) >> ChannelBits);
  }
  
  inline unsigned int RawHit::otis() const
  {
    return (unsigned int)((m_data & OtisMask) >> OtisBits);
  }
  
  inline bool RawHit::word() const
  {
    return 0 != ((m_data & wordMask) >> wordBits);
  }
}


#endif ///OTDAQ_RawHit_H
