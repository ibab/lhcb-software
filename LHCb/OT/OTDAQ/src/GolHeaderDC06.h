
#ifndef OTDAQ_GolHeaderDC06_H
#define OTDAQ_GolHeaderDC06_H 1

namespace OTDAQ
{

  // Forward declarations
  
  /** @class GolHeaderDC06 GolHeaderDC06.h
   *
   * The Gol Header is a word of 32 bits that containes the address on an OT
   * Module for the OTDAQ data format 
   *
   * @author Jacopo Nardulli
   * created Wed Nov 14 17:03:08 2007
   * 
   */
  
  class GolHeaderDC06
  {
  public:

    /// constructor with word, station, layer, quarter, module, otisErFlag and size
    GolHeaderDC06(int iWord,
              int iStation,
              int iLayer,
              int iQuarter,
              int iModule,
              int iOtisErFlag,
              int iSize);
  
    /// constructor with golHeader
    GolHeaderDC06(int id) : m_data(id) {}
  
    /// Return an unsigned int from a GolHeaderDC06 
    unsigned int returnInt(const GolHeaderDC06& gol);
    
    /// Retrieve Size
    unsigned int size() const;
  
    /// Retrieve OtisErFlag
    unsigned int otisErFlag() const;
  
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
    std::ostream& operator<<(std::ostream& os) const ;

    /// Size of the bufer with hit data
    unsigned int hitBufferSize() const { return size() ; }

    /// Number of hits. Does not correct for padding!
    unsigned int numberOfHits() const { return 2*size() ; }

    /// Stream to output
    friend std::ostream& operator<<(std::ostream& os, const GolHeaderDC06& header) ;
  private:

    /// Offsets of bitfield golHeader
    enum golHeaderBits{sizeBits       = 0,
                       otisErFlagBits = 9,
                       moduleBits     = 21,
                       quarterBits    = 25,
                       layerBits      = 27,
                       stationBits    = 29,
                       wordBits       = 31};
  
    /// Bitmasks for bitfield golHeader
    enum golHeaderMasks{sizeMask       = 0x1FFL,
                        otisErFlagMask = 0x1FFE00L,
                        moduleMask     = 0x1E00000L,
                        quarterMask    = 0x6000000L,
                        layerMask      = 0x18000000L,
                        stationMask    = 0x60000000L,
                        wordMask       = 0x80000000L
                       };
  
  
    unsigned int m_data; ///< Gol Header ID
  
  }; // class GolHeaderDC06

  
  // Including forward declarations
  
  inline GolHeaderDC06::GolHeaderDC06(int iWord,
                                      int iStation,
                                      int iLayer,
                                      int iQuarter,
                                      int iModule,
                                      int iOtisErFlag,
                                  int iSize) 
  {
    
    m_data = (iWord << wordBits) +
      (iStation << stationBits) + 
      (iLayer << layerBits) + 
      (iQuarter << quarterBits) + 
      (iModule << moduleBits) + 
      (iOtisErFlag << otisErFlagBits) + 
      (iSize << sizeBits);
    
  }
 
  inline unsigned int GolHeaderDC06::size() const
  {
    return (unsigned int)((m_data & sizeMask) >> sizeBits);
  }

  inline unsigned int GolHeaderDC06::otisErFlag() const
  {
    return (unsigned int)((m_data & otisErFlagMask) >> otisErFlagBits);
  }
  
  inline unsigned int GolHeaderDC06::module() const
  {
    return (unsigned int)((m_data & moduleMask) >> moduleBits);
  }
  
  inline unsigned int GolHeaderDC06::quarter() const
  {
    return (unsigned int)((m_data & quarterMask) >> quarterBits);
  }
  
  inline unsigned int GolHeaderDC06::layer() const
  {
    return (unsigned int)((m_data & layerMask) >> layerBits);
  }
  
  inline unsigned int GolHeaderDC06::station() const
  {
    return (unsigned int)((m_data & stationMask) >> stationBits);
  }
  
  inline bool GolHeaderDC06::word() const
  {
    return 0 != ((m_data & wordMask) >> wordBits);
  }
  
  inline unsigned int GolHeaderDC06::returnInt(const GolHeaderDC06& gol) 
  {
    return gol.m_data  >> 0; 
  }

  inline std::ostream& operator<<(std::ostream& os, const GolHeaderDC06& header) 
  {
    return os << "[ data=" << header.m_data
	      << ", buffer size =" << header.hitBufferSize()
	      << ", num hits ="    << header.numberOfHits()
	      << ", module ID=("
	      << header.station() << "," << header.layer() << "," 
	      << header.quarter() << "," << header.module() << ") ]" ;
  }
}


#endif ///OTDAQ_GolHeaderDC06_H
