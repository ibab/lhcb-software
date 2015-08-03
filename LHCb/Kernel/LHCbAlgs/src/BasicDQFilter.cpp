// Include files

// local
#include "BasicDQFilter.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>
using boost::algorithm::to_lower_copy;

// ----------------------------------------------------------------------------
// Implementation file for class: BasicDQFilter
//
// 04/11/2011: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(BasicDQFilter)

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
BasicDQFilter::BasicDQFilter(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent)
{
  declareProperty("IgnoredFlags",
                  m_ignoredFlagsProp,
                  "List of flags to ignore when filtering events")
    ->declareUpdateHandler(&BasicDQFilter::i_propUpdate, this);
}

void BasicDQFilter::i_propUpdate(Property&) {
  m_ignoredFlags.clear();
  BOOST_FOREACH(const std::string &s, m_ignoredFlagsProp)
  {
    m_ignoredFlags.insert(to_lower_copy(s));
  }
}

bool BasicDQFilter::accept(const FlagsType& flags) const {
  bool bad = false;
  for (FlagsType::const_iterator f = flags.begin(); (!bad) && (f != flags.end()); ++f) {
    // if the flag is not ignored
    if (m_ignoredFlags.find(to_lower_copy(f->first)) == m_ignoredFlags.end())
      bad = (f->second != 0); // no need to consider the previous value of m_bad because we do an early exit
    else {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Ignoring flag: " << f->first << endmsg;
    }
  }
  return !bad;
}

// ============================================================================
// Destructor
// ============================================================================
BasicDQFilter::~BasicDQFilter() {}

// ============================================================================
