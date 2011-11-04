//====================================================================
//	EvtHeader.h
//--------------------------------------------------------------------
//
//	Package    : PersistencySvc (The LHCb PersistencySvc service)
//
//  Description: Definition of example EvtHeader data object
//
//	Author     : M.Frank
//  Created    : 13/1/99
//
//====================================================================
#ifndef PERSISTENCYSVC_EVTHEADER_H
#define PERSISTENCYSVC_EVTHEADER_H

// C++ includes
#include <iostream>
// Framework includes
#include "GaudiKernel/DataObject.h"

namespace Tests {

  // External declarations
  extern const CLID& CLID_EvtHeader;

  /** Definition of Class EvtHeader: Basic Event Header information                              
      <P> Class:      EvtHeader                                    
      <P> BaseClasses:public DataObject
      <P> Purpose:
      Example Basic Event Header information.                       
      Here: Run, Event number and a date                            
      @author M.Frank 
  */
  class EvtHeader : public DataObject {
  protected:
    /// Event number
    unsigned int          m_EvtNum;
    /// Run Number
    unsigned int          m_RunNum;
    /// Event time tag
    unsigned int          m_Date;

  public:
    /// Standard Constructor
    EvtHeader() 
      : DataObject(), m_EvtNum(0), m_RunNum(0), m_Date(0)
      {
      }
      /// Standard Destructor
      virtual ~EvtHeader() {
	std::cout << " ~EvtHeader:" << this << " .. ";
      }
      /// Accessor: Retrieve Event Number
      unsigned int  evtNum() const { 
	return m_EvtNum;
      }
      /// Accessor: Retrive Run Number
      unsigned int  runNum() const { 
	return m_RunNum;
      }
      /// Accessor: Retrive Date
      unsigned int  date() const { 
	return m_Date; 
      }
      /// Initialisation of the Event Header
      EvtHeader& set(unsigned int evt_num, unsigned int run_num, unsigned int date)     {
	m_EvtNum    = evt_num; 
	m_RunNum    = run_num; 
	m_Date      = date; 
	return *this;
      }
      /// Retrieve Pointer to class defininition structure
      virtual const CLID& clID() const  {
	return CLID_EvtHeader;
      }
  };
}
#endif // PERSISTENCYSVC_EVTHEADER_H
