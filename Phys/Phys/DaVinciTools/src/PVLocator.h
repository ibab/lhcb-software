// $Id: PVLocator.h,v 1.2 2005-01-06 10:37:47 pkoppenb Exp $
#ifndef PVLOCATOR_H 
#define PVLOCATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "Kernel/IPVLocator.h"            // Interface

/** @class PVLocator PVLocator.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-10-27
 */
class PVLocator : public GaudiTool, virtual public IPVLocator {
public: 
  /// Standard constructor
  PVLocator( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual ~PVLocator( ); ///< Destructor

  /// Get PV location in TES
  std::string getPVLocation(void) const ;

  /// Set PV location in TES
  StatusCode setPVLocation(std::string&) ;

protected:

private:
  std::string m_PVLocation ;

};
#endif // PVLOCATOR_H
