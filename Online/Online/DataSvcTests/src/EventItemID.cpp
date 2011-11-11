// $Id:$

#include <string>

namespace Gaudi {

  struct StringRegistry {
  private:
    StringRegistry(const StringRegistry& ) {}
  protected:
    StringRegistry() {}
    virtual ~StringRegistry() {}
  public:
    static StringRegistry* instance(const std::string& instance_name);
    static unsigned int index(StringRegistry& reg, const char* name);
    static unsigned int hash(const StringRegistry& reg, unsigned int bit);
    static const char*  name(const StringRegistry& reg, unsigned int bit);
  };
}

// $Id:$
#include <map>
#include <memory>

using namespace std;
using namespace Gaudi;
namespace {
  struct RegistryImp : public StringRegistry  {
    struct Entry {
      string name;
      unsigned int bit;
      unsigned int hash;
    };
    map<string,Entry* > values;
    map<unsigned int,Entry* > bits;
    map<unsigned int,Entry* > hashes;

    RegistryImp() : StringRegistry() { }
    virtual ~RegistryImp()           { }
    unsigned int index(const char* name);
    const char*  name(unsigned int bit)  const;
    unsigned int hash(unsigned int bit)  const;
  };
  unsigned int hash32(const char* key) {
    unsigned int hash;
    const char* k;
    for (hash = 0, k = key; *k; k++) {
      hash += *k; hash += (hash << 10); hash ^= (hash >> 6);
    }
    hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
    return hash;
  }
}

StringRegistry* StringRegistry::instance(const string& instance_name)    {
  static map<string, RegistryImp* > s_regMap;
  map<string, RegistryImp* >::iterator i=s_regMap.find(instance_name);
  if ( i==s_regMap.end() ) {
    RegistryImp* reg = new RegistryImp();
    i = s_regMap.insert(make_pair(instance_name,reg)).first;
  }
  return (*i).second;
}

unsigned int StringRegistry::index(StringRegistry& reg, const char* tag) {
  return tag ? ((RegistryImp&)reg).index(tag) : 0;
}

const char*  StringRegistry::name(const StringRegistry& reg, unsigned int bit) {
  return ((RegistryImp&)reg).name(bit);
}

unsigned int StringRegistry::hash(const StringRegistry& reg, unsigned int bit) {
  return ((RegistryImp&)reg).hash(bit);
}

const char* RegistryImp::name(unsigned int bit)  const {
  if ( bit < bits.size() ) {
    map<unsigned int,Entry*>::const_iterator i = bits.find(bit);
    if ( i != bits.end() ) return (*i).second->name.c_str();
  }
  return 0;
}

unsigned int RegistryImp::hash(unsigned int bit)   const {
  if ( bit < bits.size() ) {
    map<unsigned int,Entry*>::const_iterator i = bits.find(bit);
    if ( i != bits.end() ) return (*i).second->hash;
  }
  return 0;
}

unsigned int RegistryImp::index(const char* name)   {
  map<string,Entry*>::iterator i = values.find(name);
  if ( i == values.end() ) {
    Entry* e = new Entry();
    e->name = name;
    e->hash = hash32(name);
    e->bit  = values.size()+1;
    i = values.insert(make_pair(name,e)).first;
    hashes.insert(make_pair(e->hash,e));
    bits.insert(make_pair(e->bit,e));
  }
  return (*i).second->bit;
}



// $Id:$
#include "GaudiKernel/EventItemID.h"
using namespace Gaudi;
using namespace std;

static StringRegistry& registry() {
  static StringRegistry* reg = StringRegistry::instance("Event");
  return *reg;
}

/// Standard constructor with initializing argument
EventItemID::EventItemID(const char* tag_name) : m_bit(0)   {
  m_bit = StringRegistry::index(registry(),tag_name);
}

/// Default destructor
EventItemID::~EventItemID() {
}

/// Accessor to the tag name, return NULL if tag not in registry: Expensive operation!
const char* EventItemID::name() const {
  return StringRegistry::name(registry(),m_bit);
}

/// Accessor to the hash code of the tag name: Expensive operation!
unsigned int EventItemID::hash() const {
  return StringRegistry::hash(registry(),m_bit);
}

/// Access to string registry
const char* EventItemID::entry_name(unsigned int bit) {
  return StringRegistry::name(registry(),bit);
}

/// Access to string registry
unsigned int EventItemID::entry_hash(unsigned int bit) {
  return StringRegistry::hash(registry(),bit);
}
