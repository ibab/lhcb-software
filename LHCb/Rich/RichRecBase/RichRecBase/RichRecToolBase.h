
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecToolBase
 *
 * CVS Log :-
 * $Id: RichRecToolBase.h,v 1.19 2007-02-01 17:26:22 jonrob Exp $
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
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace Rich::Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class ToolBase RichRecToolBase.h RichRecBase/RichRecToolBase.h
     *
     *  Abstract base class for RICH reconstruction tools providing
     *  some basic functionality.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2002-07-26
     */
    //-----------------------------------------------------------------------------

    class ToolBase : public Rich::ToolBase,
                     public Rich::Rec::CommonBase<Rich::ToolBase>
    {

    public:

      /// Standard constructor
      ToolBase( const std::string& type,
                const std::string& name,
                const IInterface* parent );

      /// Destructor
      virtual ~ToolBase() {};

      // Initialize method
      virtual StatusCode initialize();

      // Finalize method
      virtual StatusCode finalize();

    };

  }
} // RICH

/** Backwards compatibility typedef
 * @todo Remove eventually
 */
typedef Rich::Rec::ToolBase RichRecToolBase;

#endif // RICHRECBASE_RICHRECTOOLBASE_H
