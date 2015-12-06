// $Id$
#ifndef HLTRAWBANKDECODERBASE_H 
#define HLTRAWBANKDECODERBASE_H 1

// Include files
// from Gaudi
#include "DAQKernel/DecoderAlgBase.h"

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/VectorMap.h"
#include "Kernel/IANNSvc.h"
#include "Kernel/IIndexedANNSvc.h"
#include "DAQKernel/DecoderAlgBase.h"
#include "Event/RawBank.h"

class HltRawBankDecoderBase : public Decoder::AlgBase {
public:
    HltRawBankDecoderBase( const std::string& name, ISvcLocator* pSvcLocator);
    ~HltRawBankDecoderBase() override = default;
    StatusCode initialize() override;

    std::vector<const LHCb::RawBank*> selectRawBanks( LHCb::RawBank::BankType ) const;

    class element_t {
        Gaudi::StringKey m_key;
        bool             m_decode;
    public:
        element_t( Gaudi::StringKey key, bool decode ) 
            : m_key{ std::move(key) }, m_decode{ decode } 
        {}
        operator const Gaudi::StringKey& () const { return m_key; }
        const std::string& str() const { return m_key.str(); }
        operator const std::string&() const { return str(); }
        bool operator!() const { return !m_decode; }
    };

    const GaudiUtils::VectorMap<int,element_t>& id2string(unsigned int tck) const {
        auto itbl =  m_idTable.find(tck) ;
        if ( itbl  == std::end(m_idTable) ) itbl = fetch_id2string(tck);
        return itbl->second;
    };
    const GaudiUtils::VectorMap<int,Gaudi::StringKey>& info2string(unsigned int tck) const {
        auto itbl =  m_infoTable.find(tck) ;
        if ( itbl  == std::end(m_infoTable) ) itbl = fetch_info2string(tck);
        return itbl->second;
    }
    unsigned int tck() const;

private:
    SmartIF<IANNSvc> m_hltANNSvc;
    SmartIF<IIndexedANNSvc> m_TCKANNSvc;

    using IdTable_t =  GaudiUtils::VectorMap<unsigned int, GaudiUtils::VectorMap<int, element_t>>;
    using Table_t =  GaudiUtils::VectorMap<unsigned int, GaudiUtils::VectorMap<int, Gaudi::StringKey>>;
    mutable IdTable_t m_idTable;
    IdTable_t::const_iterator fetch_id2string(unsigned int tck) const;
    mutable Table_t m_infoTable;
    Table_t::const_iterator fetch_info2string(unsigned int tck) const;
    
    /// SourceID to decode: 0=Hlt 1=Hlt1 2=Hlt2 ... (1,2 will decode from 0 if 1,2 not found)
    UnsignedIntegerProperty m_sourceID;

    enum SourceIDs { kSourceID_Dummy=0,
  		   kSourceID_Hlt=kSourceID_Dummy,
  		   kSourceID_Hlt1=1,
  		   kSourceID_Hlt2=2,
  		   kSourceID_Max=7,
  		   kSourceID_BitShift=13,
  		   kSourceID_MinorMask=0x1FFF,
  		   kSourceID_MajorMask=0xE000
    };
};
#endif
