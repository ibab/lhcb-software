
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecToolBase
 *
 * CVS Log :-
 * $Id: RichRecToolBase.h,v 1.20 2007-03-09 18:04:34 jonrob Exp $
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

namespace Rich
{
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
