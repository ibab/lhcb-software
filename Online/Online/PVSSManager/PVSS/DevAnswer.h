// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/PVSS/DevAnswer.h,v 1.3 2007-03-01 20:09:09 frankb Exp $
//  ====================================================================
//  CfgDetail.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DevAnswer.h,v 1.3 2007-03-01 20:09:09 frankb Exp $
#ifndef PVSS_DEVANSER_H
#define PVSS_DEVANSER_H

#include <vector>
#include <string>

/*
 *    PVSS namespace
 */
namespace PVSS {

  class DevAnswer  {
  public:
    /// Answer state
    enum State { WAITING, WORKING, FINISHED, ERROR };

  protected:
    /// Flag to indicate that answer was received
    State           m_state;
    /// Errors 
    char**          m_errs;
    size_t m_nerrs, m_mxerrs;
    void clear();
  public:
    /// Default constructor
    DevAnswer(State st=WAITING) 
      : m_state(st), m_errs(0), m_nerrs(0), m_mxerrs(0) { }
    /// Default destructor
    ~DevAnswer()             { if ( m_nerrs>0 ) clear();  }
    /// Answer's state
    State state() const               { return m_state;   }
    /// Set answer state
    void setState(State s)            { m_state = s;      }
    /// Print errors
    void print() const;
    /// Add new error
    void addError(const char* text);
  };
}
#endif // PVSS_DEVANSER_H
