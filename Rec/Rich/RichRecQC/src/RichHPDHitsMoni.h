
//-----------------------------------------------------------------------------
/** @file RichHPDHitsMoni.h
 *
 *  Header file for algorithm class : RichHPDHitsMoni
 *
 *  CVS Log :-
 *  $Id: RichHPDHitsMoni.h,v 1.2 2006-12-01 16:02:32 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichHPDHitsMoni_H
#define RICHRECMONITOR_RichHPDHitsMoni_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

//-----------------------------------------------------------------------------
/** @class RichHPDHitsMoni RichHPDHitsMoni.h
 *
 *  Simple monitor for the hits in each HPD
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichHPDHitsMoni : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichHPDHitsMoni( const std::string& name,
                   ISvcLocator* pSvcLocator );

  virtual ~RichHPDHitsMoni( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  /// Raw Buffer Decoding tool
  const IRichRawBufferToSmartIDsTool * m_decoder;

  /// Pointer to RICH system detector element
  const DeRichSystem * m_richSys;

};

#endif // RICHRECMONITOR_RichHPDHitsMoni_H
