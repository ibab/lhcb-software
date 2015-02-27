// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/RangeAlarm.cpp,v 1.4 2007-03-12 18:56:03 frankb Exp $
//  ====================================================================
//  RangeAlarm.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: RangeAlarm.cpp,v 1.4 2007-03-12 18:56:03 frankb Exp $

// Framework include files
#include "PVSS/CfgType.h"
#include "PVSS/CfgDetail.h"
#include "PVSS/DataPoint.h"
#include "PVSS/RangeAlarm.h"

using namespace PVSS;

template <typename T> 
RangeDefinition<T>::RangeDefinition() : lower(0), upper(0), inc_lower(true), inc_upper(false)
{}

template <typename T> 
RangeDefinition<T>::RangeDefinition(const std::string t,
				    const std::string& cl,
				    const T& l,
				    bool  il,
				    const T& u,
				    bool  iu) 
: text(t), clazz(cl), lower(l), upper(u), inc_lower(il), inc_upper(iu)
{}

template <typename T> 
RangeDefinition<T>::RangeDefinition(const RangeDefinition<T>& c) 
: text(c.text), clazz(c.clazz),
lower(c.lower), upper(c.upper), inc_lower(c.inc_lower), inc_upper(c.inc_upper)
{}

template <typename T> 
RangeDefinition<T>& RangeDefinition<T>::operator=(const RangeDefinition<T>& c)  {
  if ( &c != this )  {
    text=c.text; 
    clazz=c.clazz;
    lower=c.lower;
    upper=c.upper;
    inc_lower=c.inc_lower;
    inc_upper=c.inc_upper;
  }
  return *this;
}

template <typename T> 
bool RangeDefinition<T>::operator==(const RangeDefinition<T>& c) const  {
  if ( &c == this ) return true;
  return text==c.text && clazz==c.clazz && lower==c.lower &&
         upper==c.upper && inc_lower==c.inc_lower && inc_upper==c.inc_upper;
}

template <typename T> 
bool RangeDefinition<T>::operator<(const RangeDefinition<T>& c) const  {
  if ( &c == this ) return false;
  else if ( text<c.text ) return true;
  else if ( text>c.text ) return false;
  else if ( clazz<c.clazz ) return true;
  else if ( clazz>c.clazz ) return false;
  else if ( lower<c.lower ) return true;
  else if ( lower>c.lower ) return false;
  else if ( upper<c.upper ) return true;
  else if ( upper>c.upper ) return false;
  else if ( inc_lower<c.inc_lower ) return true;
  else if ( inc_lower>c.inc_lower ) return false;
  else if ( inc_upper<c.inc_upper ) return true;
  else if ( inc_upper>c.inc_upper ) return false;
  return false;
}

/// Default constructor
template <typename T> 
RangeAlarm<T>::RangeAlarm() : BasicAlarm() {
}

/// Copy constructor
template <typename T> 
RangeAlarm<T>::RangeAlarm(const RangeAlarm<T>& copy) 
: BasicAlarm(copy), m_ranges(copy.m_ranges)
{
}

/// Initializing constructor
template <typename T> 
RangeAlarm<T>::RangeAlarm(const std::string& dp_name) 
: BasicAlarm(dp_name) 
{
}

/// Initializing constructor
template <typename T> 
RangeAlarm<T>::RangeAlarm(const DataPoint& dp)
: BasicAlarm(dp.name()) 
{
}
/// Standard destructor
template <typename T> 
RangeAlarm<T>::~RangeAlarm()   {
}

/// Clear all alarm ranges
template <typename T> 
void RangeAlarm<T>::clear()    {
  m_ranges.clear();
}

/// Set alarm range
template <typename T> 
void RangeAlarm<T>::addRange( const std::string& text, 
                              const std::string& cl, 
                              const T& lower, 
                              bool inc_lower, 
                              const T& upper, 
                              bool inc_upper)  {
  m_ranges.push_back(Range(text,cl,lower,inc_lower,upper,inc_upper));
}

/// Apply alarm settings to datapoint identified by datapoint name
template <typename T> 
void RangeAlarm<T>::i_apply(const Transaction& tr, const std::string& dp)    
{
  tr->setValue(dp+".:_alert_hdl.._type", CfgType::ALERT_NONBINARYSIGNAL);
  for(size_t i=0; i < m_ranges.size(); ++i)  {
    const Range& r = m_ranges[i];
    std::string n = dp+".:_alert_hdl.";
    n += char('0'+i+1);
    tr->setValue(n+"._type",CfgDetail::RANGE_MINMAX);
    if ( i != 0 )  {
      tr->setValue(n+"._l_limit",r.lower);
    }
    tr->setValue(n+"._l_incl",r.inc_lower);
    if ( i != m_ranges.size()-1 )  {
      tr->setValue(n+"._u_limit",r.upper);
    }
    tr->setValue(n+"._u_incl",r.inc_upper);
    if ( !r.text.empty() )  {
      tr->setValue(n+"._text",r.text);
    }
    if ( !r.clazz.empty() ) {
      tr->setValue(n+"._class",DPRef(r.clazz+"."));
    }
  }
  BasicAlarm::i_apply(tr,dp);
}

/// Assignment operator for insertion in STL containers
template <typename T> 
RangeAlarm<T>& RangeAlarm<T>::operator=(const RangeAlarm& alm)  {
  BasicAlarm::operator=(alm);
  m_ranges = alm.ranges();
  return *this;
}

/// Equality operator for insertion in STL containers
template <typename T> 
bool RangeAlarm<T>::operator==(const RangeAlarm& alm) const  {
  return BasicAlarm::operator==(alm) && m_ranges == alm.ranges();
}

/// Operator less for insertion in STL associative classes
template <typename T> 
bool RangeAlarm<T>::operator<(const RangeAlarm& alm) const  {
  if ( & alm == this ) return false;
  if ( BasicAlarm::operator<(alm) ) return true;
  else if ( BasicAlarm::operator==(alm) )
    return m_ranges<alm.ranges();
  return false;
}

template class RangeDefinition<char>;
template class RangeDefinition<unsigned char>;
template class RangeDefinition<short>;
template class RangeDefinition<unsigned short>;
template class RangeDefinition<int>;
template class RangeDefinition<unsigned int>;
template class RangeDefinition<long>;
template class RangeDefinition<unsigned long>;
template class RangeDefinition<float>;
template class RangeDefinition<double>;

template class RangeAlarm<char>;
template class RangeAlarm<unsigned char>;
template class RangeAlarm<short>;
template class RangeAlarm<unsigned short>;
template class RangeAlarm<int>;
template class RangeAlarm<unsigned int>;
template class RangeAlarm<long>;
template class RangeAlarm<unsigned long>;
template class RangeAlarm<float>;
template class RangeAlarm<double>;
