
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.h
 *
 *  Header file for reconstruction algorithm base class : RichRecAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecAlgBase.h,v 1.21 2005-10-13 15:38:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECALGBASE_H
#define RICHRECBASE_RICHRECALGBASE_H 1

// base classes
#include "RichKernel/RichAlgBase.h"
#include "RichRecBase/RichRecBase.h"

//-----------------------------------------------------------------------------
/** @class RichRecAlgBase RichRecAlgBase.h RichRecBase/RichRecAlgBase.h
 *
 *  Abstract base class for RICH reconstruction algorithms providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichRecAlgBase : public RichAlgBase,
                       public RichRecBase<RichAlgBase>
{

public:

  /// Standard constructor
  RichRecAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichRecAlgBase() {};

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

#endif // RICHRECBASE_RICHRECALGBASE_H
