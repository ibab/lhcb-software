
//-----------------------------------------------------------------------------
/** @file RichRecHistoAlgBase.h
 *
 *  Header file for RICH reconstruction monitor algorithm base class :
 *  RichRecHistoAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecHistoAlgBase.h,v 1.1 2005-10-31 13:30:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECHISTOALGBASE_H
#define RICHRECBASE_RICHRECHISTOALGBASE_H 1

// base classes
#include "RichKernel/RichHistoAlgBase.h"
#include "RichRecBase/RichRecBase.h"

//-----------------------------------------------------------------------------
/** @class RichRecHistoAlgBase RichRecHistoAlgBase.h RichRecBase/RichRecHistoAlgBase.h
 *
 *  Abstract base class for RICH reconstruction algorithms providing
 *  some basic functionality (identical to RichRecAlgBase) but with additional
 *  histogram functionality provided by GaudiHistoAlg base class
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

class RichRecHistoAlgBase : public RichHistoAlgBase,
                            public RichRecBase<RichHistoAlgBase>
{

public:

  /// Standard constructor
  RichRecHistoAlgBase( const std::string& name,
                       ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichRecHistoAlgBase() {};

  /** Initialization of the algorithm after creation
   *
   * @return The status of the initialization
   * @retval StatusCode::SUCCESS Initialization was successful
   * @retval StatusCode::FAILURE Initialization failed
   */
  virtual StatusCode initialize();

  /** The main event processing method. Called once for each event
   *
   * @return The status of the event processing
   * @retval StatusCode::SUCCESS Event processing was successful
   * @retval StatusCode::FAILURE Event processing failed
   */
  virtual StatusCode execute();

  /** Finalization of the algorithm before deletion
   *
   * @return The status of the finalization
   * @retval StatusCode::SUCCESS Finalization was successful
   * @retval StatusCode::FAILURE Finalization failed
   */
  virtual StatusCode finalize();

};

#endif // RICHRECBASE_RICHRECHISTOALGBASE_H
