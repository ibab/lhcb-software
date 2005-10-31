
//-----------------------------------------------------------------------------
/** @file RichRecTupleToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecTupleToolBase
 *
 * CVS Log :-
 * $Id: RichRecTupleToolBase.h,v 1.1 2005-10-31 13:30:16 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECTUPLETOOLBASE_H
#define RICHRECBASE_RICHRECTUPLETOOLBASE_H 1

// Base class
#include "RichKernel/RichTupleToolBase.h"
#include "RichRecBase/RichRecBase.h"

//-----------------------------------------------------------------------------
/** @class RichRecTupleToolBase RichRecTupleToolBase.h RichRecBase/RichRecTupleToolBase.h
 *
 *  Abstract base class for RICH reconstruction tools providing
 *  some basic functionality (identical to RichRecToolBase) but with additional
 *  histogram and ntuple functionality provided by GaudiTupleTool base class.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

class RichRecTupleToolBase : public RichTupleToolBase,
                             public RichRecBase<RichTupleToolBase>
{

public:

  /// Standard constructor
  RichRecTupleToolBase( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

  /// Destructor
  virtual ~RichRecTupleToolBase() {};

  /** Initialization of the tool after creation
   *
   * @return The status of the initialization
   * @retval StatusCode::SUCCESS Initialization was successful
   * @retval StatusCode::FAILURE Initialization failed
   */
  virtual StatusCode initialize();

  /** Finalization of the tool before deletion
   *
   * @return The status of the finalization
   * @retval StatusCode::SUCCESS Finalization was successful
   * @retval StatusCode::FAILURE Finalization failed
   */
  virtual StatusCode finalize();

};

#endif // RICHRECBASE_RICHRECTUPLETOOLBASE_H
