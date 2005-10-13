
//-----------------------------------------------------------------------------
/** @file RichRecMoniAlgBase.h
 *
 *  Header file for reconstruction monitor algorithm base class : RichRecMoniAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecMoniAlgBase.h,v 1.5 2005-10-13 15:38:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECMONIALGBASE_H
#define RICHRECBASE_RICHRECMONIALGBASE_H 1

// base classes
#include "RichKernel/RichMoniAlgBase.h"
#include "RichRecBase/RichRecBase.h"

//-----------------------------------------------------------------------------
/** @class RichRecMoniAlgBase RichRecMoniAlgBase.h RichRecBase/RichRecMoniAlgBase.h
 *
 *  Abstract base class for RICH reconstruction algorithms providing
 *  some basic functionality (identical to RichRecAlgBase) but with additional
 *  histogram and ntuple functionality provided by GaudiTupleAlg base class
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 *
 *  @todo Review if common functionality between RichRecMoniAlgBase 
 *        and RichRecAlgBase can be merged
 */
//-----------------------------------------------------------------------------

class RichRecMoniAlgBase : public RichMoniAlgBase,
                           public RichRecBase<RichMoniAlgBase>
{

public:

  /// Standard constructor
  RichRecMoniAlgBase( const std::string& name,
                      ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichRecMoniAlgBase() {};

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

#endif // RICHRECBASE_RICHRECMONIALGBASE_H
