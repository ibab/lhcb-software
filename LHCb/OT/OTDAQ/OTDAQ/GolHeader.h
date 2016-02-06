// $Id: GolHeader.h,v 1.1 2008-06-03 12:59:25 wouter Exp $
#ifndef OTDAQ_GOLHEADER_H 
#define OTDAQ_GOLHEADER_H 1

namespace OTDAQ {

  class GolHeader
  {
  public:
    
    /// constructor with word, station, layer, quarter, module, otisErFlag and size
    GolHeader(int iWord,
                int iStation,
                int iLayer,
		int iQuarter,
                int iModule,
                int iOtisErFlag,
                int iNumberOfHits);
    
    /// constructor with golHeader
    GolHeader(int id=0) : m_data(id) {}
  
    /// Return an unsigned int from a GolHeader 
    unsigned int returnInt(const GolHeader& gol);
  
    /// Retrieve NumberOfHits
    unsigned int numberOfHits() const;
  
    /// Retrieve the total size of the buffer with hit data (includign padding)
    unsigned int hitBufferSize() const { return numberOfHits()/2 + numberOfHits()%2 ; }
   
    /// Retrieve OtisErFlag
    unsigned int otisErFlag() const;
  
    /// Retrieve Otis0 SEU
    unsigned int otis0SEU() const;
    /// Retrieve Otis1 SEU
    unsigned int otis1SEU() const;
    /// Retrieve Otis2 SEU
    unsigned int otis2SEU() const;
    /// Retrieve Otis3 SEU
    unsigned int otis3SEU() const;
  
    /// Retrieve Otis0 bufferOverflow
    unsigned int otis0bufferOverflow() const;
    /// Retrieve Otis1 bufferOverflow
    unsigned int otis1bufferOverflow() const;
    /// Retrieve Otis2 bufferOverflow
    unsigned int otis2bufferOverflow() const;
    /// Retrieve Otis3 bufferOverflow
    unsigned int otis3bufferOverflow() const;
  
    /// Retrieve Otis0 truncation
    unsigned int otis0truncation() const;
    /// Retrieve Otis1 truncation
    unsigned int otis1truncation() const;
    /// Retrieve Otis2 truncation
    unsigned int otis2truncation() const;
    /// Retrieve Otis3 truncation
    unsigned int otis3truncation() const;
  
    /// Retrieve Data Process Mode
    unsigned int dataProcessMode() const;
    
    /// Retrieve Optical Transmission ok ? bit
    unsigned int opticalTransOK() const;
    

    /// Retrieve Module
    unsigned int module() const;
  
    /// Retrieve Quarter
    unsigned int quarter() const;
  
    /// Retrieve Layer
    unsigned int layer() const;
  
    /// Retrieve Station
    unsigned int station() const;
  
    /// Retrieve Word
    bool word() const;
    
    /// Stream to output
    friend std::ostream& operator<<(std::ostream& os, const GolHeader& header) ;
  private:

    /// Offsets of bitfield golHeader
    enum golHeaderBits{NumberOfHitsBits       = 24,
                       otisErFlagBits = 10,
                       otis0SEUBits   = 10,
                       otis0BufOvBits = 11,
                       otis0TruncBits = 12,
                       otis1SEUBits   = 13,
                       otis1BufOvBits = 14,
                       otis1TruncBits = 15,
                       otis2SEUBits   = 16,
                       otis2BufOvBits = 17,
                       otis2TruncBits = 18,
                       otis3SEUBits   = 19,
                       otis3BufOvBits = 20,
                       otis3TruncBits = 21,
                       dataProModBits = 22,
                       optTransOKBits = 23,
                       moduleBits     = 0,
                       quarterBits    = 4,
                       layerBits      = 6,
                       stationBits    = 8,
                       wordBits       = 23};
  
    /// Bitmasks for bitfield golHeader
    enum golHeaderMasks{NumberOfHitsMask       = 0xFF000000L,
                        otisErFlagMask = 0x003FFC00L,
                        otis0SEUMask   = 0x00000400L,
                        otis0BufOvMask = 0x00000800L,
                        otis0TruncMask = 0x00001000L,
                        otis1SEUMask   = 0x00002000L,
                        otis1BufOvMask = 0x00004000L,
                        otis1TruncMask = 0x00008000L,
                        otis2SEUMask   = 0x00010000L,
                        otis2BufOvMask = 0x00020000L,
                        otis2TruncMask = 0x00040000L,
                        otis3SEUMask   = 0x00080000L,
                        otis3BufOvMask = 0x00100000L,
                        otis3TruncMask = 0x00200000L,
                        dataProModMask = 0x00400000L,
                        optTransOKMask = 0x00800000L,
                        moduleMask     = 0x0000000FL,
                        quarterMask    = 0x00000030L,
                        layerMask      = 0x000000C0L,
                        stationMask    = 0x00000300L,
                        wordMask       = 0x00800000L
                       };
  
  
    unsigned int m_data; ///< Gol Header ID
  
  }; // class GolHeader

 
// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

  inline GolHeader::GolHeader(int iWord,
                                  int iStation,
                                  int iLayer,
                                  int iQuarter,
                                  int iModule,
                                  int iOtisErFlag,
                                  int iNumberOfHits) 
  {
    
    m_data = (iWord << wordBits) +
      (iStation << stationBits) + 
      (iLayer << layerBits) + 
      (iQuarter << quarterBits) + 
      (iModule << moduleBits) + 
      (iOtisErFlag << otisErFlagBits) + 
      (iNumberOfHits << NumberOfHitsBits);
    
  }
  
  inline unsigned int GolHeader::numberOfHits() const
  {
    return (unsigned int)((m_data & NumberOfHitsMask) >> NumberOfHitsBits);
  }
  
  inline unsigned int GolHeader::otisErFlag() const
  {
    return (unsigned int)((m_data & otisErFlagMask) >> otisErFlagBits);
  }
  
  
  inline unsigned int GolHeader::otis0SEU() const
  {
    return (unsigned int)((m_data & otis0SEUMask ) >> otis0SEUBits );
  }
  
  inline unsigned int GolHeader::otis1SEU() const
  {
    return (unsigned int)((m_data & otis1SEUMask ) >> otis1SEUBits );
}
  
  inline unsigned int GolHeader::otis2SEU() const
  {
    return (unsigned int)((m_data & otis2SEUMask ) >> otis2SEUBits );
  }
  
  inline unsigned int GolHeader::otis3SEU() const
  {
    return (unsigned int)((m_data & otis3SEUMask ) >> otis3SEUBits );
  }
  
  
  inline unsigned int GolHeader::otis0bufferOverflow() const
  {
    return (unsigned int)((m_data & otis0BufOvMask ) >> otis0BufOvBits );
  }
  
  inline unsigned int GolHeader::otis1bufferOverflow() const
  {
    return (unsigned int)((m_data & otis1BufOvMask ) >> otis1BufOvBits );
  }
  
  inline unsigned int GolHeader::otis2bufferOverflow() const
  {
    return (unsigned int)((m_data & otis2BufOvMask ) >> otis2BufOvBits );
  }
  
  inline unsigned int GolHeader::otis3bufferOverflow() const
  {
    return (unsigned int)((m_data & otis3BufOvMask ) >> otis3BufOvBits );
  }
  
  
  inline unsigned int GolHeader::otis0truncation() const
  {
    return (unsigned int)((m_data & otis0TruncMask ) >> otis0TruncBits );
  }
  
  inline unsigned int GolHeader::otis1truncation() const
  {
    return (unsigned int)((m_data & otis1TruncMask ) >> otis1TruncBits );
  }
  
  inline unsigned int GolHeader::otis2truncation() const
  {
  return (unsigned int)((m_data & otis2TruncMask ) >> otis2TruncBits );
  }
  
  inline unsigned int GolHeader::otis3truncation() const
  {
    return (unsigned int)((m_data & otis3TruncMask ) >> otis3TruncBits );
  }
  
  inline unsigned int GolHeader::dataProcessMode() const
  {
    return (unsigned int)((m_data & dataProModMask ) >> dataProModBits );
  }
  
  inline unsigned int GolHeader::opticalTransOK() const
  {
    return (unsigned int)((m_data & optTransOKMask ) >> optTransOKBits );
  }

  inline unsigned int GolHeader::module() const
  {
    return (unsigned int)((m_data & moduleMask) >> moduleBits);
  }
  
  inline unsigned int GolHeader::quarter() const
  {
    return (unsigned int)((m_data & quarterMask) >> quarterBits);
  }
  
  inline unsigned int GolHeader::layer() const
  {
    return (unsigned int)((m_data & layerMask) >> layerBits);
  }
  
  inline unsigned int GolHeader::station() const
  {
    return (unsigned int)((m_data & stationMask) >> stationBits);
  }
  
  inline bool GolHeader::word() const
  {
    return 0 != ((m_data & wordMask) >> wordBits);
  }
  
  inline unsigned int GolHeader::returnInt(const GolHeader& gol) 
  {
    return gol.m_data  >> 0; 
  } 

  inline std::ostream& operator<<(std::ostream& os, const GolHeader& header) 
  {
    return os << "[ data=" << header.m_data 
	      << ", buffer size=" << header.hitBufferSize()
	      << ", num hits="    << header.numberOfHits()
	      << ", module ID=("
	      << header.station() << "," << header.layer() << "," 
	      << header.quarter() << "," << header.module() << ") ]" ;
  }
}



#endif // GOLHEADERV3_H
