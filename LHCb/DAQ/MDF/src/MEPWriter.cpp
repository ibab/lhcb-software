// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MEPWriter.cpp,v 1.1 2005-12-20 16:33:39 frankb Exp $
//	====================================================================
//  MEPWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/StorageTypes.h"
#include "MDF/MEPWriter.h"
#include "MDF/MEPEvent.h"
#include "Event/RawEvent.h"

typedef LHCb::MEPWriter MEPDataStream;
DECLARE_ALGORITHM_FACTORY(MEPDataStream)

using namespace LHCb;

/// Standard algorithm constructor
LHCb::MEPWriter::MEPWriter(const std::string& name, ISvcLocator* pSvcLocator)
: MDFWriter(name, pSvcLocator)
{
  declareProperty("PackingFactor",  m_packingFactor=20);
}

/// Standard Destructor
LHCb::MEPWriter::~MEPWriter()     {
}

/// Execute procedure
StatusCode LHCb::MEPWriter::execute()    {
  SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
  if ( raw )  {
    raw->addRef();
    m_events.push_back(raw.ptr());
    if ( m_events.size() == m_packingFactor )  {
      StatusCode sc = dumpEvents(m_events);
      for(Events::iterator i=m_events.begin(); i != m_events.end(); ++i)  {
        (*i)->release();
      }
      m_events.clear();
      return sc;
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Dump stored events
StatusCode LHCb::MEPWriter::dumpEvents(const Events& evts) {
  typedef std::vector<RawBank*>    _BankV;
  typedef std::map<int, _BankV >   BankMap;
  typedef std::map<int, BankMap >  BankMap2;
  typedef std::map<int, BankMap2 > BankMap3;

  int eid = 0, nbank=0, nbank2=0;
  size_t evtlen = MEPEvent::sizeOf();
  BankMap3 pointers;
  for(Events::iterator i=m_events.begin(); i != m_events.end(); ++i, ++eid)  {
    for(size_t len=0, m=RawBank::L0Calo; m<RawBank::LastType; ++m)  {
      const _BankV& banks = (*i)->banks(RawBank::BankType(m));
      for(_BankV::const_iterator j=banks.begin(); j != banks.end(); ++j)  {
        pointers[(*j)->sourceID()][eid][(*j)->type()].push_back((*j));
        evtlen += RawEvent::paddedBankLength((*j)->size());
        nbank++;
      }
    }
  }
  for(BankMap3::const_iterator j=pointers.begin(); j != pointers.end(); ++j) {
    const BankMap2& bm2 = (*j).second;
    evtlen += bm2.size()*MEPMultiFragment::sizeOf();
    for(BankMap2::const_iterator k=bm2.begin(); k != bm2.end(); ++k)
      evtlen += (*k).second.size()*MEPFragment::sizeOf();
  }
  m_data.reserve(evtlen);
  int partID = 0x103;
  int packing = m_events.size();
  MEPEvent* me = new(m_data.data()) MEPEvent(0);
  for(BankMap3::iterator j=pointers.begin(); j != pointers.end(); ++j) {
    int srcID = (*j).first;
    BankMap2& bm2 = (*j).second;
    MEPMultiFragment* mf = ::new(me->first()) MEPMultiFragment(eid&0xFFFF0000,0,partID,packing);
    for(BankMap2::iterator k=bm2.begin(); k != bm2.end(); ++k)   {
      eid = (*k).first;
      BankMap& bm = (*k).second;      
      MEPFragment* f = ::new(mf->first()) MEPFragment(eid&0xFFFF, 0);
      for(BankMap::iterator l=bm.begin(); l != bm.end(); ++l)   {
        _BankV& banks = (*l).second;
        RawBank* b = f->first();
        for(_BankV::const_iterator m=banks.begin(); m != banks.end(); ++m)  {
          size_t s = RawEvent::paddedBankLength((*m)->size());
          ::memcpy(b, *m, s);
          f->setSize(s+f->size());
          b = f->next(b);
          nbank2++;
        }
        mf->setSize(mf->size()+f->size()+f->sizeOf());
        f = ::new(mf->next(f)) MEPFragment(eid&0xFFFF, 0);
      }
      int nsiz = me->size()+mf->size()+mf->sizeOf();
      me->setSize(nsiz);
      mf = ::new(me->next(mf)) MEPMultiFragment(eid&0xFFFF0000,0,partID,packing);
    }
  }
  return Descriptor::write(m_connection,m_data.data(),me->size()+me->sizeOf())
    ? StatusCode::SUCCESS : StatusCode::FAILURE;
}
