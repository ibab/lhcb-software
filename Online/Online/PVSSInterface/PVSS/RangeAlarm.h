// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/RangeAlarm.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  RangeAlarm.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_RANGEALARM_H
#define ONLINE_PVSS_RANGEALARM_H

// Framework include files
#include "PVSS/BasicAlarm.h"

// C++ include files
#include <memory>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  class DataPoint;

  /// Range definition
  /** 
    *  PVSS configuration attribute.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  template <typename T> struct RangeDefinition  {
      std::string text;
      std::string clazz;
      T lower, upper;
      bool inc_lower, inc_upper;
      RangeDefinition();
      RangeDefinition(const std::string t,
            const std::string& cl,
            const T& l,
            bool  il,
            const T& u,
            bool  iu);
      RangeDefinition(const RangeDefinition& c);
      RangeDefinition& operator=(const RangeDefinition& r);
      bool operator==(const RangeDefinition& r) const;
      bool operator<(const RangeDefinition& r) const;
    };

  /// RangeAlarm class
  /** 
    *  PVSS configuration attribute.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  template <typename T> class RangeAlarm : virtual public BasicAlarm {
  public:
    typedef RangeDefinition<T> Range;
    typedef std::vector<Range > Ranges;
  protected:
    /// Defined ranges
    Ranges m_ranges;
    /// Apply alarm settings to datapoint identified by datapoint name
    virtual void i_apply(Transaction& tr, const std::string& dp_name);
  public:
    /// Default constructor
    RangeAlarm();
    /// Copy constructor
    RangeAlarm(const RangeAlarm& copy);
    /// Initializing constructor
    RangeAlarm(const std::string& dp_name);
    /// Initializing constructor
    RangeAlarm(const DataPoint& dp);
    /// Standard destructor
    virtual ~RangeAlarm();
    /// Access alarm ranges
    const Ranges& ranges()  const  {  return m_ranges; }
    /// Clear all alarm ranges
    void clear();
    /// Set alarm range
    void addRange(const std::string& text, 
                  const std::string& cl,
                  const T& lower,
                  bool  inc_lower,
                  const T& upper,
                  bool  inc_upper);
    /// Assignment operator for insertion in STL containers
    RangeAlarm& operator=(const RangeAlarm& alm);
    /// Equality operator for insertion in STL containers
    bool operator==(const RangeAlarm& alm) const;
    /// Operator less for insertion in STL associative classes
    bool operator<(const RangeAlarm& alm) const;
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_RANGEALARM_H
