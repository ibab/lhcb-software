
//---------------------------------------------------------------------------
/** @file RichRecInit.h
 *
 *  Header file for algorithm class : RichRecInit
 *
 *  CVS Log :-
 *  $Id: RichRecInit.h,v 1.2 2006-12-19 09:46:30 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
#define RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/RichRecStatus.h"

// Event
#include "Event/ProcStatus.h"

//---------------------------------------------------------------------------
/** @class RichRecInit RichRecInit.h
 *
 *  General initialisation algorithm for RICH reconstruction
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

class RichRecInit : public RichRecAlgBase
{

public:

  /// Standard constructor
  RichRecInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichRecInit();   ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private:   // Private data members



};

#endif // RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
