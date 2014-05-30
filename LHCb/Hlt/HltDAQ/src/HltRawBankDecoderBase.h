// $Id$
#ifndef HLTRAWBANKDECODERBASE_H 
#define HLTRAWBANKDECODERBASE_H 1

// Include files
// from Gaudi
#include "DAQKernel/DecoderAlgBase.h"

#include "GaudiKernel/VectorMap.h"
#include "Kernel/IANNSvc.h"
#include "Kernel/IIndexedANNSvc.h"
#include "DAQKernel/DecoderAlgBase.h"
#include "Event/RawBank.h"

class HltRawBankDecoderBase : public Decoder::AlgBase {
public:
    HltRawBankDecoderBase( const std::string& name, ISvcLocator* pSvcLocator);
    ~HltRawBankDecoderBase() override;
    StatusCode initialize() override;
    std::vector<const LHCb::RawBank*> selectRawBanks( LHCb::RawBank::BankType ) const;
    const GaudiUtils::VectorMap<int,Gaudi::StringKey>& id2string(unsigned int tck) const;
    const GaudiUtils::VectorMap<int,Gaudi::StringKey>& info2string(unsigned int tck) const;
    unsigned int tck() const;

private:
    IANNSvc* m_hltANNSvc;
    IIndexedANNSvc* m_TCKANNSvc;

    mutable GaudiUtils::VectorMap<unsigned int, GaudiUtils::VectorMap<int, Gaudi::StringKey>> m_idTable;
    mutable GaudiUtils::VectorMap<unsigned int, GaudiUtils::VectorMap<int, Gaudi::StringKey>> m_infoTable;
    
    /// SourceID to decode 0=Hlt 1=Hlt1 2=Hlt2 ... (1,2 will decode from 0 if 1,2 not found)
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
