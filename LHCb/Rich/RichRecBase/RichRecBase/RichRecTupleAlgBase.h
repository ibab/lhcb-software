
//-----------------------------------------------------------------------------
/** @file RichRecTupleAlgBase.h
 *
 *  Header file for RICH reconstruction monitor algorithm base class : 
 *  RichRecTupleAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecTupleAlgBase.h,v 1.1 2005-10-31 13:30:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECTUPLEALGBASE_H
#define RICHRECBASE_RICHRECTUPLEALGBASE_H 1

// base classes
#include "RichKernel/RichTupleAlgBase.h"
#include "RichRecBase/RichRecBase.h"

//-----------------------------------------------------------------------------
/** @class RichRecTupleAlgBase RichRecTupleAlgBase.h RichRecBase/RichRecTupleAlgBase.h
 *
 *  Abstract base class for RICH reconstruction algorithms providing
 *  some basic functionality (identical to RichRecAlgBase) but with additional
 *  histogram and ntuple functionality provided by GaudiTupleAlg base class.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

class RichRecTupleAlgBase : public RichTupleAlgBase,
                            public RichRecBase<RichTupleAlgBase>
{

public:

  /// Standard constructor
  RichRecTupleAlgBase( const std::string& name,
                       ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichRecTupleAlgBase() {};

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

#endif // RICHRECBASE_RICHRECTUPLEALGBASE_H
