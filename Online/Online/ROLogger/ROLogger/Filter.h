// $Id: Filter.h,v 1.1 2008-05-13 07:56:02 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: 
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/Filter.h,v 1.1 2008-05-13 07:56:02 frankb Exp $
#ifndef ROLOGGER_FILTER_H
#define ROLOGGER_FILTER_H

// C++ include files
#include <string>
#include <iostream>

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {
  class MessageLine;

  struct Filter {
    enum MatchType {
      MATCH_IGNORE  = 0,
      MATCH_EXACT   = 1,
      MATCH_WILD    = 1<<1,
      MATCH_NOCASE  = 1<<2,
      MATCH_SELECT  = 1<<3,
      MATCH_INVERSE = 1<<4,
      MATCH_REQUIRE = 1<<5
    };
    std::string node;
    std::string utgid;
    std::string component;
    std::string message;
    int         type;

    int acceptMatch(const std::string& cand, const std::string& pattern, unsigned char match) const;
    /// Write filter object to file
    std::ostream& write(std::ostream& os) const;
    /// read filter object from file
    size_t read(std::istream& is);    
    /// Dump filter info
    void dump() const;
    /// Dump filter info
    void dump(std::ostream& os) const;

    enum { NODE=0, UTGID, COMPONENT, MESSAGE };


    Filter() : type(MATCH_IGNORE) {}
    Filter(const Filter& f) : node(f.node), utgid(f.utgid), component(f.component), message(f.message), type(f.type) {}
    Filter& operator=(const Filter& f);
    const unsigned char* match() const {  return (const unsigned char*)&type; }
    bool acceptMessage(const std::string& message) const;
    bool acceptMessage(const MessageLine& msg) const;
    void setNodeMatch(const std::string& value, unsigned char typ);
    void setUtgidMatch(const std::string& value, unsigned char typ);
    void setComponentMatch(const std::string& value, unsigned char typ);
    void setMessageMatch(const std::string& value, unsigned char typ);
  };
}
#endif /* ROLOGGER_FILTER_H */
