// $Id: DaVinciInitialisation.h,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#ifndef DAVINCITOOLS_DAVINCIINITIALISATION__H
#define DAVINCITOOLS_DAVINCIINITIALISATION__H 1


// Include files
#include "GaudiKernel/Algorithm.h"

/** @class DaVinciInitialisation DaVinciInitialisation.h DaVinciTools/DaVinciInitialisation.h
 *  
 *  Generic DaVinci initialisation to load all necessary info for fortran
 *
 *  @author Gloria Corti
 *  @date   10/06/2001
 */
class DaVinciInitialisation : public Algorithm {
public:
  /// Standard constructor
  DaVinciInitialisation( const std::string& name, ISvcLocator *svcloc );

  virtual ~DaVinciInitialisation(); ///< Standard destructor
  
  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization
private:

  bool m_useSICB; ///< Flag to specify if some fortran SICB is to be used

};

#endif    // DAVINCITOOLS_DAVINCIINITIALISATION__H
