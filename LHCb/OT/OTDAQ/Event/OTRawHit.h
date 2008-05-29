
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

#include <vector>
#include "LoKi/Range.h"

namespace LHCb
{

  // Forward declarations

  /** @class RawHit RawHit.h
   *
   * The Data Word is a word of 32 bits that containes the data of 2 hits of the
 * OT for the OTDAQ data format 
   *
   * @author Jacopo Nardulli, Wouter Hulsbergen
   * created Wed Nov 14 17:03:07 2007
   * 
   */

  class OTRawHit
  {
  public:

    /// Default Constructor
    OTRawHit() : m_data(0) {}
  
    /// constructor with data word in raw bank buffer
    OTRawHit(unsigned short data) : m_data(data) {}
  
    /// constructor with word, channel in OTIS (0 -127), tdctime
    OTRawHit( unsigned int word,
            unsigned int channel,
            unsigned int tdcTime ) ;

    /// Retrieve tdc time
    unsigned int time() const;
  
    /// Retrieve channel number (0-127)
    unsigned int channel() const;

    /// Retrieve channel number in otis (0-31)
    unsigned int channelInOtis() const;
  
    /// Retrieve otis number (0-3)
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
                       ChannelInOtisMask  = 0x1F00L,
                       OtisMask     = 0x6000L,
		       ChannelMask  = ChannelInOtisMask | OtisMask,
                       wordMask     = 0x8000L,
    };
    
  private:
    unsigned short m_data; ///< Data Word ID
  
  }; // class OTRawHit
  
  /// Type for a container of OTRawHit  objects
  typedef std::vector<OTRawHit> OTRawHitContainer ;
  
  /// Range object for OTRawHitContainer
  typedef LoKi::Range_<OTRawHitContainer> OTRawHitRange ;
  
  // -----------------------------------------------------------------------------
  // end of class
  // -----------------------------------------------------------------------------
  
  inline OTRawHit::OTRawHit( unsigned int word,
			     unsigned int channel,
			     unsigned int tdcTime )
  {
    m_data = ( word << wordBits ) + ( channel << ChannelBits ) + ( tdcTime << TimeBits );
  }
  
  inline unsigned int OTRawHit::time() const
  {
    return (unsigned int)((m_data & TimeMask) >> TimeBits);
  }
  
  inline unsigned int OTRawHit::channel() const
  {
    return (unsigned int)((m_data & ChannelMask) >> ChannelBits);
  }
  
  inline unsigned int OTRawHit::channelInOtis() const
  {
    return (unsigned int)((m_data & ChannelInOtisMask) >> ChannelBits);
  }
  
  inline unsigned int OTRawHit::otis() const
  {
    return (unsigned int)((m_data & OtisMask) >> OtisBits);
  }
  
  inline bool OTRawHit::word() const
  {
    return 0 != ((m_data & wordMask) >> wordBits);
  }
}


#endif ///OTDAQ_RawHit_H
