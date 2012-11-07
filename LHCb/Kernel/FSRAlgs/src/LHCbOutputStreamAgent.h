//====================================================================
//	LHCbOutputStreamAgent.h
//--------------------------------------------------------------------
//
//	Package    :  (The LHCb PersistencySvc service)
//
//  Description: Definition the LHCbOutputStream Agent
//
//	Author     : M.Frank
//  Created    : 13/1/99
//	Changes    : 
//
//====================================================================
#ifndef LHCB_OUTPUTSTREAMAGENT_H
#define LHCB_OUTPUTSTREAMAGENT_H

// Framework includes
#include "GaudiKernel/IDataStoreAgent.h"
// Foreward declarations
class IRegistry;
class LHCbOutputStream;

/** @name The LHCbOutputStreamAgent class.
 
  Data store Agent to traverse data store trees and select all
  items to be written to the output file.
 
  @author Markus Frank
 */
class LHCbOutputStreamAgent : virtual public IDataStoreAgent  {
  /// Reference to data writer
  LHCbOutputStream*  m_OutputStream;
public:
  /// Standard Constructor
  LHCbOutputStreamAgent(LHCbOutputStream* OutputStream);
  /// Standard Destructor
  virtual ~LHCbOutputStreamAgent();
  /// Analysis callback
  virtual bool analyse(IRegistry* dir, int level);
};
#endif // LHCB_OUTPUTSTREAMAGENT_H
