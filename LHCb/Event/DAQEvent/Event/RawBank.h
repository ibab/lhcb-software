#ifndef DAQEVENT_RAWBANK_H
#define DAQEVENT_RAWBANK_H 1


/** @class LHCb::RawBank RawBank.h
  *
  * Raw data bank sent by the TELL1 boards of the LHCb DAQ.
  *
  * For a detailed description of the raw bank format,
  * see EDMS note "565851 v.3  Raw-data format"
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

    /// Define bank types for RawBank
    enum BankType{ L0Calo=0,
                   L0DU,
                   PrsE,
                   EcalE,
                   HcalE,
                   PrsTrig,
                   EcalTrig,
                   HcalTrig,
                   Velo,
                   Rich,
                   TT,
                   IT,
                   OT,
                   Muon,
                   L0PU,
                   DAQ,
                   ODIN,
                   HLT,
                   VeloFull,
                   TTFull,
                   ITFull,
                   EcalPacked,
                   HcalPacked,
                   PrsPacked,
                   L0Muon,
                   ITError,
                   TTError,
                   ITPedestal,
                   TTPedestal,
                   LastType    // LOOP Marker; add new bank types ONLY before!
    };

    /// Magic pattern for Raw bank headers
    enum RawPattern{ MagicPattern=0xCBCB };

    /// Access to magic word for integrity check
    int magic()  const              {    return m_magic;            }

    /// Set magic word
    void setMagic()                 {    m_magic = MagicPattern;    }

    /// Return size of the data body part of the bank
    int size() const                {    return m_length;           }

    /// Set data size of the bank in bytes
    void setSize(size_t val)        {    m_length = (val&0xFFFF);   }

    /// Return bankType of this bank 
    BankType type() const           {    return BankType(int(m_type)); }

    /// Set data size of the bank in bytes
    void setType(BankType val)      {    m_type = (int(val)&0xFF);  }

    /// Return version of this bank 
    int version() const             {    return m_version;          }

    /// Set data size of the bank in bytes
    void setVersion(int val)        {    m_version = (val&0xFF);    }

    /// Return SourceID of this bank 
    int sourceID() const            {    return m_sourceID;         }

    /// Set data size of the bank in bytes
    void setSourceID(int val)       {    m_sourceID = (val&0xFFFF); }

    /// Return pointer to begining of data body of the bank
    unsigned int* data()            {    return &m_data[0];         }

    /// Return pointer to begining of data body of the bank (const data access)
    const unsigned int* data() const {   return &m_data[0];         }

    /// Begin iterator 
    template <typename T> T* begin(){    return (T*)m_data;         }

    /// End iterator 
    template <typename T> T* end()  {    return ((T*)m_data) + m_length/sizeof(T);  }

    /// Begin iterator over const iteration
    template <typename T> const T* begin() const {return (T*)m_data;}

    /// End iterator of const iteration
    template <typename T>  const T* end() const  {return ((T*)m_data) + m_length/sizeof(T);  }

  private:
    /// Magic word (by definition 0xCBCB)
    unsigned short m_magic;
    /// Bank length (must be >= 0)
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
