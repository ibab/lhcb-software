
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecToolBase
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
      virtual StatusCode initialize() override;

      // Finalize method
      virtual StatusCode finalize() override;

    };

  } // Rec

} // Rich

#endif // RICHRECBASE_RICHRECTOOLBASE_H
