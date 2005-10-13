
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecToolBase
 *
 * CVS Log :-
 * $Id: RichRecToolBase.h,v 1.18 2005-10-13 15:38:41 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2002-07-26
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECTOOLBASE_H
#define RICHRECBASE_RICHRECTOOLBASE_H 1

// Base classes
#include "RichKernel/RichToolBase.h"
#include "RichRecBase/RichRecBase.h"

//-----------------------------------------------------------------------------
/** @class RichRecToolBase RichRecToolBase.h RichRecBase/RichRecToolBase.h
 *
 *  Abstract base class for RICH reconstruction tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//-----------------------------------------------------------------------------

class RichRecToolBase : public RichToolBase,
                        public RichRecBase<RichToolBase>
{

public:

  /// Standard constructor
  RichRecToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecToolBase() {};

  // Initialize method
  virtual StatusCode initialize();

  // Finalize method
  virtual StatusCode finalize();

};

#endif // RICHRECBASE_RICHRECTOOLBASE_H
