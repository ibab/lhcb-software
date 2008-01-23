
#ifndef DAQEVENT_RAWBANK_H
#define DAQEVENT_RAWBANK_H 1

#include <stddef.h>
#include <vector>

/** @class LHCb::RawBank RawBank.h
  *
  * Raw data bank sent by the TELL1 boards of the LHCb DAQ.
  *
  * For a detailed description of the raw bank format,
  * see <a href="https://edms.cern.ch/document/565851/5">EDMS-565851/5</a>
  *
  * Note concerning the changes done 06/03/2006:
  * - The bank size is in BYTES
  * - The full size of a bank in memory is long word (32 bit) aligned
  *   ie. a bank of 17 Bytes length actually uses 20 Bytes in memory.
  * - The bank length accessors size() and setSize() do not contain
  *   the bank header, ie. size() = (total size) - (header size).
  * - The length passed to the RawEvent::createBank should NOT
  *   contain the size of the header !
  * - If the full padded bank size is required use the utility
  *   function RawBank::totalSize = size + header size + padding size.
  *
  * @author Helder Lopes
  * @author Markus Frank
  * created Tue Oct 04 14:45:20 2005
  * 
  */
namespace LHCb 
{
  class RawBankSubClass;
  
  class RawBank  {
    /// Need to declare some friend to avoid compiler warnings
    friend class RawBankSubClass;
  private:
    /// Default Constructor
    RawBank() {}

    /// Default Destructor
    ~RawBank() {}

  public:   



    // typedef for std::vector of RawBank
    typedef std::vector<RawBank*> Vector;
    typedef std::vector<const RawBank*> ConstVector;


    /// Define bank types for RawBank
    enum BankType{ L0Calo=0,        //  0
                   L0DU,            //  1
                   PrsE,            //  2
                   EcalE,           //  3
                   HcalE,           //  4
                   PrsTrig,         //  5
                   EcalTrig,        //  6
                   HcalTrig,        //  7
                   Velo,            //  8
                   Rich,            //  9
                   TT,              // 10
                   IT,              // 11
                   OT,              // 12
                   Muon,            // 13
                   L0PU,            // 14
                   DAQ,             // 15
                   ODIN,            // 16
                   HLT,             // 17
                   VeloFull,        // 18
                   TTFull,          // 19
                   ITFull,          // 20
                   EcalPacked,      // 21
                   HcalPacked,      // 22
                   PrsPacked,       // 23
                   L0Muon,          // 24
                   ITError,         // 25
                   TTError,         // 26
                   ITPedestal,      // 27
                   TTPedestal,      // 28
                   VeloError,       // 29
                   VeloPedestal,    // 30
                   VeloProcFull,    // 31
                   OTRaw,           // 32
                   OTError,         // 33
                   EcalPackedError, // 34
                   HcalPackedError, // 35  
                   PrsPackedError,  // 36
                   L0CaloFull,      // 37
                   L0CaloError,     // 38
                   L0MuonCtrlAll,   // 39
                   L0MuonProcCand,  // 40
                   L0MuonProcData,  // 41
                   L0MuonRaw,       // 42
                   L0MuonError,     // 43
                   GaudiSerialize,  // 44
                   GaudiHeader,     // 45
                   TTProcFull,      // 46
                   ITProcFull,      // 47
                   TAEHeader,       // 48
                   MuonFull,        // 49
                   MuonError,       // 50
                   LastType    // LOOP Marker; add new bank types ONLY before!
    };
    static inline std::string  typeName(LHCb::RawBank::BankType e){
      switch ( e ) {
      case LHCb::RawBank::L0Calo          : return  "L0Calo";
      case LHCb::RawBank::L0DU            : return  "L0DU";            
      case LHCb::RawBank::PrsE            : return  "PrsE";            
      case LHCb::RawBank::EcalE           : return  "EcalE";           
      case LHCb::RawBank::HcalE           : return  "HcalE";           
      case LHCb::RawBank::PrsTrig         : return  "PrsTrig";         
      case LHCb::RawBank::EcalTrig        : return  "EcalTrig";        
      case LHCb::RawBank::HcalTrig        : return  "HcalTrig";        
      case LHCb::RawBank::Velo            : return  "Velo";            
      case LHCb::RawBank::Rich            : return  "Rich";            
      case LHCb::RawBank::TT              : return  "TT";              
      case LHCb::RawBank::IT              : return  "IT";              
      case LHCb::RawBank::OT              : return  "OT";              
      case LHCb::RawBank::Muon            : return  "Muon";            
      case LHCb::RawBank::L0PU            : return  "L0PU";            
      case LHCb::RawBank::DAQ             : return  "DAQ";             
      case LHCb::RawBank::ODIN            : return  "ODIN";            
      case LHCb::RawBank::HLT             : return  "HLT";             
      case LHCb::RawBank::VeloFull        : return  "VeloFull";        
      case LHCb::RawBank::TTFull          : return  "TTFull";         
      case LHCb::RawBank::ITFull          : return  "ITFull";          
      case LHCb::RawBank::EcalPacked      : return  "EcalPacked";      
      case LHCb::RawBank::HcalPacked      : return  "HcalPacked";      
      case LHCb::RawBank::PrsPacked       : return  "PrsPacked";       
      case LHCb::RawBank::L0Muon          : return  "L0Muon";          
      case LHCb::RawBank::ITError         : return  "ITError";         
      case LHCb::RawBank::TTError         : return  "TTError";  
      case LHCb::RawBank::ITPedestal      : return  "ITPedestal";      
      case LHCb::RawBank::TTPedestal      : return  "TTPedestal";      
      case LHCb::RawBank::VeloError       : return  "VeloError";       
      case LHCb::RawBank::VeloPedestal    : return  "VeloPedestal";    
      case LHCb::RawBank::VeloProcFull    : return  "VeloProcFull";    
      case LHCb::RawBank::OTRaw           : return  "OTRaw";           
      case LHCb::RawBank::OTError         : return  "OTError";         
      case LHCb::RawBank::EcalPackedError : return  "EcalPackedError"; 
      case LHCb::RawBank::HcalPackedError : return  "HcalPackedError"; 
      case LHCb::RawBank::PrsPackedError  : return  "PrsPackedError";  
      case LHCb::RawBank::L0CaloFull      : return  "L0CaloFull";      
      case LHCb::RawBank::L0CaloError     : return  "L0CaloError";     
      case LHCb::RawBank::L0MuonCtrlAll   : return  "L0MuonCtrlAll";   
      case LHCb::RawBank::L0MuonProcCand  : return  "L0MuonProcCand";  
      case LHCb::RawBank::L0MuonProcData  : return  "L0MuonProcData";  
      case LHCb::RawBank::L0MuonRaw       : return  "L0MuonRaw";       
      case LHCb::RawBank::L0MuonError     : return  "L0MuonError";     
      case LHCb::RawBank::GaudiSerialize  : return  "GaudiSerialize";  
      case LHCb::RawBank::GaudiHeader     : return  "GaudiHeader";     
      case LHCb::RawBank::TTProcFull      : return  "TTProcFull";      
      case LHCb::RawBank::ITProcFull      : return  "ITProcFull";      
      case LHCb::RawBank::TAEHeader       : return  "TAEHeader";       
      case LHCb::RawBank::MuonFull        : return  "MuonFull";        
      case LHCb::RawBank::MuonError       : return  "MuonError";
      default                             : return  "Undefined name";
      };
    }  
    inline std::string  typeName(){return LHCb::RawBank::typeName( type() ); }
    

    /// Magic pattern for Raw bank headers
    enum RawPattern{ MagicPattern=0xCBCB };

    /// Access to magic word for integrity check
    int magic()  const              {    return m_magic;            }

    /// Set magic word
    void setMagic()                 {    m_magic = MagicPattern;    }

    /// Header size
    int hdrSize()  const            {    return sizeof(RawBank)-sizeof(m_data);}

    /// Return size of the data body part of the bank
    int size() const                {    return m_length-hdrSize();            }

    /// Set data size of the bank in bytes
    void setSize(size_t val)        {    m_length = (val&0xFFFF)+hdrSize();    }

    /// Access the full (padded) size of the bank
    int totalSize() const           { 
      typedef unsigned int T;
      return m_length%sizeof(T)==0 ? m_length : (m_length/sizeof(T)+1)*sizeof(T);
    }
    /// Return bankType of this bank 
    BankType type() const           {    return BankType(int(m_type));         }

    /// Set the bank type
    void setType(BankType val)      {    m_type = (int(val)&0xFF);  }

    /// Return version of this bank 
    int version() const             {    return m_version;          }

    /// Set the version information of this bank
    void setVersion(int val)        {    m_version = (val&0xFF);    }

    /// Return SourceID of this bank  (TELL1 board ID)
    int sourceID() const            {    return m_sourceID;         }

    /// Set the source ID of this bank (TELL1 board ID)
    void setSourceID(int val)       {    m_sourceID = (val&0xFFFF); }

    /// Return pointer to begining of data body of the bank
    unsigned int* data()            {    return &m_data[0];         }

    /// Return pointer to begining of data body of the bank (const data access)
    const unsigned int* data() const {   return &m_data[0];         }

    /// Begin iterator 
    template <typename T> T* begin(){    return (T*)m_data;         }

    /// End iterator 
    template <typename T> T* end()  {    return ((T*)m_data) + size()/sizeof(T);  }

    /// Begin iterator over const iteration
    template <typename T> const T* begin() const {return (T*)m_data;}

    /// End iterator of const iteration
    template <typename T>  const T* end() const  {return ((T*)m_data) + size()/sizeof(T);  }

  private:
    /// Magic word (by definition 0xCBCB)
    unsigned short m_magic;
    /// Bank length in bytes (must be >= 0)
    unsigned short m_length;
    /// Bank type (must be >= 0)
    unsigned char  m_type;
    /// Version identifier (must be >= 0)
    unsigned char  m_version;
    /// Source ID (valid source IDs are > 0; invalid ones 0)
    short          m_sourceID;
    /// Opaque data block
    unsigned int   m_data[1];
  }; // class RawBank

} // namespace LHCb




#endif ///DAQEvent_RawBank_H

