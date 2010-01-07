// $Id: RawEvent.cpp,v 1.15 2010-01-07 10:25:17 frankb Exp $
#include "Event/RawEvent.h"
#include <cstring> // for memcpy with gcc 4.3

namespace {
  inline LHCb::RawBank* allocateBank(size_t len)  {
    size_t mem_len = LHCb::RawEvent::paddedBankLength(len);
    size_t new_len = mem_len/sizeof(unsigned int);
    unsigned int* mem = new unsigned int[new_len];
    if ( mem_len != len )  {
      mem[new_len-1] = 0;
    }
    return (LHCb::RawBank*)mem;
  }
}

// Default Constructor
LHCb::RawEvent::RawEvent() 
: m_mapped(false)
{
}

// Default Destructor
LHCb::RawEvent::~RawEvent() {
  for(std::vector<Bank>::iterator i=m_banks.begin(); i != m_banks.end(); ++i)  {
    Bank& b = *i;
    if ( b.ownsMemory() && b.buffer() )  {
      delete [] b.buffer();
    }
  }
  m_banks.clear();
}

size_t LHCb::RawEvent::paddedBankLength(size_t len)   {
  size_t mem_len = len+sizeof(LHCb::RawBank)-sizeof(unsigned int);
  if ( mem_len%sizeof(unsigned int) )  {    // Need padding
    mem_len = (mem_len/sizeof(unsigned int) + 1)*sizeof(unsigned int);
  }
  return mem_len;
}

const std::vector<LHCb::RawBank*>& LHCb::RawEvent::mapBanks(RawBank::BankType bankType)  {
  for(std::vector<Bank>::iterator i=m_banks.begin(); i != m_banks.end(); ++i)  {
    LHCb::RawBank* bank = (LHCb::RawBank*)(*i).buffer();
    m_eventMap[bank->type()].push_back(bank);
  }
  m_mapped = true;
  return m_eventMap[bankType];
}

// For offline use only: copy data into a set of banks, adding bank header internally.
void LHCb::RawEvent::addBank( int srcID, 
                              LHCb::RawBank::BankType typ, 
                              int vsn, 
                              const std::vector<unsigned int>& data)  {
  adoptBank( createBank(srcID, typ, vsn, data.size()*sizeof(unsigned int), &(*data.begin())), true );
}

LHCb::RawBank* LHCb::RawEvent::createBank( int srcID, 
                                           LHCb::RawBank::BankType typ,
                                           int vsn,
                                           size_t len,
                                           const void* data )  {
  LHCb::RawBank* bank = allocateBank(len);
  bank->setMagic();
  bank->setType(typ);
  bank->setVersion(vsn);
  bank->setSourceID(srcID);
  bank->setSize(len);
  if ( data )  {
    ::memcpy(bank->data(), data, len);
  }
  return bank;
}

/// For offline use only: copy data into a bank, adding bank header internally.
void LHCb::RawEvent::addBank(RawBank* data)     {
  size_t len = data->totalSize();
  LHCb::RawBank* bank = (LHCb::RawBank*)new unsigned int[len/sizeof(unsigned int)];
  ::memcpy(bank, data, len);
  adoptBank(bank, true);
}

/// Take ownership of a bank, including the header
void LHCb::RawEvent::adoptBank(LHCb::RawBank* bank, bool adopt_memory)     {
  size_t len = bank->totalSize();
  if ( !m_mapped ) mapBanks(bank->type());
  m_eventMap[bank->type()].push_back(bank);
  m_banks.push_back(Bank(len/sizeof(unsigned int), adopt_memory, (unsigned int*)bank));
}

/// Remove bank identified by its pointer
bool LHCb::RawEvent::removeBank(RawBank* bank)  {
  typedef std::map<RawBank::BankType,std::vector<RawBank*> > BankMap;
  RawBank::BankType type = bank->type();
  BankMap::iterator i=m_eventMap.find(type);
  if ( i != m_eventMap.end() )  {
    std::vector<RawBank*>& banks = (*i).second;
    for(std::vector<RawBank*>::iterator j=banks.begin(); j!=banks.end(); ++j)  {
      if ( (*j) == bank )  {
        // Banks to be removed found!
        // First remove bank from persistent array.
        for(std::vector<Bank>::iterator k=m_banks.begin(); k!=m_banks.end(); ++k)  {
          Bank& b = *k;
          if ( !(bank == (RawBank*)b.buffer()) ) continue;
          // The bank is owned by RawEvent: delete the allocated buffer
          // to prevent memory leak when reading data from a ROOT file...
          if ( b.ownsMemory() && b.buffer() ) delete [] b.buffer();
          m_banks.erase(k);
          break;
        }
        // Now erase bank from vector with all banks of one type.
        banks.erase(j);
        // Finally remove bank type from event map if no further bank is present.
        if ( banks.size() == 0 )  {
          m_eventMap.erase(i);
        }
        return true;
      }
    }
  }
  return false;
}
