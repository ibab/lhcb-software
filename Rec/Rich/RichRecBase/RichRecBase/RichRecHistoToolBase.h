
//-----------------------------------------------------------------------------
/** @file RichRecHistoToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecHistoToolBase
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECHISTOTOOLBASE_H
#define RICHRECBASE_RICHRECHISTOTOOLBASE_H 1

// Base class
#include "RichKernel/RichHistoToolBase.h"
#include "RichRecBase/RichRecBase.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class HistoToolBase RichRecHistoToolBase.h RichRecBase/RichRecHistoToolBase.h
     *
     *  Abstract base class for RICH reconstruction tools providing
     *  some basic functionality (identical to RichRecToolBase) but with additional
     *  histogram functionality provided by GaudiHistoTool base class.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2005/01/13
     */
    //-----------------------------------------------------------------------------

    class HistoToolBase : public Rich::HistoToolBase,
                          public Rich::Rec::CommonBase<Rich::HistoToolBase>
    {

    public:

      /// Standard constructor
      HistoToolBase( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

      /// Destructor
      virtual ~HistoToolBase() {};

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

  }
} // RICH

/** Backwards compatibility typedef
 * @todo Remove eventually
 */
typedef Rich::Rec::HistoToolBase RichRecHistoToolBase;

#endif // RICHRECBASE_RICHRECHISTOTOOLBASE_H
