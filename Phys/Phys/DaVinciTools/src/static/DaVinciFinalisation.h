// $Id: DaVinciFinalisation.h,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#ifndef DAVINCITOOLS_DAVINCIFINALISATION__H
#define DAVINCITOOLS_DAVINCIFINALISATION__H 1

// Include files
#include "GaudiKernel/Algorithm.h"

/** @class DaVinciFinalisation DaVinciFinalisation.h DaVinciTools/DaVinciFinalisation.h
 *  Generic DaVinci finalization, to handle calling suanal
 *
 *  @author Gloria Corti
 *  @date   05/06/2001
 */
class DaVinciFinalisation : public Algorithm {
public:
  /// Standard construtor
  DaVinciFinalisation( const std::string& name, ISvcLocator *svcloc );

  virtual ~DaVinciFinalisation();  ///< Standard destructor
	
  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution 
  StatusCode finalize  ();    ///< Algorithm finalization
private:

};

#endif    // DAVINCITOOLS_DAVINCIFINALISATION__H
