
//-----------------------------------------------------------------------------
/** @file RichRecMoniToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecMoniToolBase
 *
 * CVS Log :-
 * $Id: RichRecMoniToolBase.h,v 1.4 2005-10-13 15:38:41 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECALGS_RICHRECMONITOOLBASE_H
#define RICHRECALGS_RICHRECMONITOOLBASE_H 1

// Base class
#include "RichKernel/RichMoniToolBase.h"
#include "RichRecBase/RichRecBase.h"

//-----------------------------------------------------------------------------
/** @class RichRecMoniToolBase RichRecMoniToolBase.h RichRecBase/RichRecMoniToolBase.h
 *
 *  Abstract base class for RICH reconstruction tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

class RichRecMoniToolBase : public RichMoniToolBase,
                            public RichRecBase<RichMoniToolBase>
{

public:

  /// Standard constructor
  RichRecMoniToolBase( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~RichRecMoniToolBase() {};

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

#endif // RICHRECALGS_RICHRECMONITOOLBASE_H
