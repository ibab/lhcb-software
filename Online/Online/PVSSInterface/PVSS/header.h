// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/header.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  DataPoint.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DATAPOINT_H
#define ONLINE_PVSS_DATAPOINT_H

// Framework include files

// C++ include files
#include <string>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  /** @class DataPoint   DataPoint.h  PVSS/DataPoint.h
    *
    *  This class does nothing.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DataPoint   {
  protected:
    /// Datapoint name
    std::string m_name;

  public:
    /// Default constructor
    DataPoint();
    /// Initializing constructor
    DataPoint(const std::string& nam);
    /// Standard destructor
    virtual ~DataPoint();
    /// Access to the Datapoint's name
    const std::string& name()          {    return m_name;  }

  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DATAPOINT_H