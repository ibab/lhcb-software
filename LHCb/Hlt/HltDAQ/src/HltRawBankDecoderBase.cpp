#include "HltRawBankDecoderBase.h"
#include "Event/RawEvent.h"

namespace {
const Gaudi::StringKey Hlt1SelectionID{"Hlt1SelectionID"};
const Gaudi::StringKey Hlt2SelectionID{"Hlt2SelectionID"};
const Gaudi::StringKey InfoID{"InfoID"};
}



HltRawBankDecoderBase::HltRawBankDecoderBase( const std::string& name, ISvcLocator* pSvcLocator)
    : Decoder::AlgBase ( name , pSvcLocator ), 
      m_hltANNSvc{nullptr},
      m_TCKANNSvc{nullptr}
{
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Copied, LHCb::RawEventLocation::Default};
  initRawEventSearch();

  declareProperty("SourceID", m_sourceID= HltRawBankDecoderBase::kSourceID_Dummy );  
}

HltRawBankDecoderBase::~HltRawBankDecoderBase() 
{}

StatusCode HltRawBankDecoderBase::initialize() 
{
    StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
    m_hltANNSvc = svc<IANNSvc>("HltANNSvc");
    m_TCKANNSvc = svc<IIndexedANNSvc>("TCKANNSvc");
    if ( m_sourceID > HltRawBankDecoderBase::kSourceID_Max ){
        return Error("Illegal SourceID specified; maximal allowed value is 7" , StatusCode::FAILURE, 50 );
    }
    return sc;
}
    
std::vector<const LHCb::RawBank*>
HltRawBankDecoderBase::selectRawBanks( LHCb::RawBank::BankType reqType ) const 
{
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  if ( !rawEvent) return {};

  const std::vector<LHCb::RawBank*> rawbanks = rawEvent->banks( reqType );
  if( rawbanks.empty() ) return {};

  // TODO: do we need a reqType dependent version check ???
  std::vector<const LHCb::RawBank*> mybanks;
  std::copy_if( std::begin(rawbanks), std::end(rawbanks), 
                std::back_inserter(mybanks),
                [=](const LHCb::RawBank* bank) {
    unsigned int sourceID = bank->sourceID() >> HltRawBankDecoderBase::kSourceID_BitShift;
    return sourceID == m_sourceID; 
  } );

  // first pass didn't find the requested bank -- try to find 'unsplit' bank 
  // if we find something here, we will 'partially' decode it downstream
  // (steered by the entries in the id lookup table)
  if (mybanks.empty() && ( m_sourceID == HltRawBankDecoderBase::kSourceID_Hlt1 ||
                           m_sourceID == HltRawBankDecoderBase::kSourceID_Hlt2 ) ) {
      std::copy_if( std::begin(rawbanks), std::end(rawbanks), 
                    std::back_inserter(mybanks),
                    [](const LHCb::RawBank* bank) {
        unsigned int sourceID = bank->sourceID() >> HltRawBankDecoderBase::kSourceID_BitShift;
        return sourceID == HltRawBankDecoderBase::kSourceID_Hlt; 
      } );
  }
    return mybanks;
}

unsigned int 
HltRawBankDecoderBase::tck() const {
    std::vector<const LHCb::RawBank*> hltdecreportsRawBanks = selectRawBanks( LHCb::RawBank::HltDecReports );
    if ( !hltdecreportsRawBanks.empty() ) {
        const LHCb::RawBank *rb = hltdecreportsRawBanks.front();
        if ( rb && rb->magic() == LHCb::RawBank::MagicPattern  && rb->version() > 0 )  { 
            auto content = rb->begin<unsigned int>();  
            return *content; 
        }
    }
    return 0;
}


HltRawBankDecoderBase::IdTable_t::const_iterator
HltRawBankDecoderBase::fetch_id2string(unsigned int tck) const
{
        IdTable_t::mapped_type tbl;

        auto append0 =  [&]( const Gaudi::StringKey& id, bool decode ) { 
               for ( const auto& item : m_hltANNSvc->item_map( id ) ) {
                   tbl.insert( { item.second, { item.first , decode }  } );
               }
        };
        auto append1 =  [&]( const Gaudi::StringKey& id, bool decode ) { 
               for ( const auto& item : m_TCKANNSvc->i2s( tck,  id ) ) {
                   tbl.insert( { item.first, { item.second , decode  } } ); // TODO: check for clashes...
               }
        };

        bool doHlt1 = ( m_sourceID == HltRawBankDecoderBase::kSourceID_Hlt1 || m_sourceID == 0);
        bool doHlt2 = ( m_sourceID == HltRawBankDecoderBase::kSourceID_Hlt2 || m_sourceID == 0);
        if (tck==0) {
           warning() << "TCK from rawbanks seems to be 0 -- assuming that the current HltANNSvc has the proper mapping configured. Proceed at your own risk..." << endmsg;
           append0(Hlt1SelectionID,doHlt1);
           append0(Hlt2SelectionID,doHlt2);
        } else {
           append1(Hlt1SelectionID,doHlt1);
           append1(Hlt2SelectionID,doHlt2);
        } 
        auto res = m_idTable.insert( tck, tbl );
        assert(res.second);
        return res.first;
}

HltRawBankDecoderBase::Table_t::const_iterator
HltRawBankDecoderBase::fetch_info2string(unsigned int tck) const
{
        Table_t::mapped_type tbl;

        auto append0 =  [&]( const Gaudi::StringKey& id ) { 
               for ( const auto& item : m_hltANNSvc->item_map( id ) ) {
                   tbl.insert( { item.second, item.first } );
               }
        };
        auto append1 =  [&]( const Gaudi::StringKey& id ) { 
               for ( const auto& item : m_TCKANNSvc->i2s( tck,  id ) ) {
                   tbl.insert( { item.first, item.second  } ); // TODO: check for clashes...
               }
        };

        if (tck==0) {
           warning() << "TCK from rawbanks seems to be 0 -- assuming that the current HltANNSvc has the proper mapping configured. Proceed at your own risk..." << endmsg;
           append0(InfoID);
        } else {
           append1(InfoID);
        } 
        auto res = m_infoTable.insert( tck, tbl );
        assert(res.second);
        return  res.first;
}
