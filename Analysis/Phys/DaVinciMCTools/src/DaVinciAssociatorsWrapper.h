#ifndef DAVINCIASSOCIATORSWRAPPER_H 
#define DAVINCIASSOCIATORSWRAPPER_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"            // Interface


/** @class DaVinciAssociatorsWrapper DaVinciAssociatorsWrapper.h
 *  
 *  Wrapper around Particle2MCLinker class
 *
 *  @author V. Gligorov (based on code by P. Koppenburg)
 *  @date   2008-06-26
 */
class DaVinciAssociatorsWrapper : public GaudiTool, 
                                      virtual public IIncidentListener,
                                      virtual public IDaVinciAssociatorsWrapper {
public: 
  /// Standard constructor
  DaVinciAssociatorsWrapper( const std::string& type, 
                                 const std::string& name,
                                 const IInterface* parent);

  virtual ~DaVinciAssociatorsWrapper( ); ///< Destructor

  StatusCode initialize() ;
  StatusCode finalize() ;

  Particle2MCLinker* linker(std::string, std::vector< std::string >) ; ///< returns a linker

  //-------------------------------------------------------------
  virtual void handle(const Incident&); ///< clean desktop

private:
  
  Particle2MCLinker* m_linker ; ///< linker
  std::string m_algType ; ///< Association method
  std::vector< std::string > m_locations ; ///< Locations for association
};
#endif // DAVINCIASSOCIATORSWRAPPER_H
