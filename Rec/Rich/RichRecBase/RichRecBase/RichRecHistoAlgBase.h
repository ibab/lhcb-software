
//-----------------------------------------------------------------------------
/** @file RichRecHistoAlgBase.h
 *
 *  Header file for RICH reconstruction monitor algorithm base class :
 *  Rich::Rec::HistoAlgBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECHISTOALGBASE_H
#define RICHRECBASE_RICHRECHISTOALGBASE_H 1

// base classes
#include "RichKernel/RichHistoAlgBase.h"
#include "RichRecBase/RichRecBase.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class HistoAlgBase RichRecHistoAlgBase.h RichRecBase/RichRecHistoAlgBase.h
     *
     *  Abstract base class for RICH reconstruction algorithms providing
     *  some basic functionality (identical to RichRecAlgBase) but with additional
     *  histogram functionality provided by GaudiHistoAlg base class
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2005/01/13
     */
    //-----------------------------------------------------------------------------

    class HistoAlgBase : public Rich::HistoAlgBase,
                         public Rich::Rec::CommonBase<Rich::HistoAlgBase>
    {

    public:

      /// Standard constructor
      HistoAlgBase( const std::string& name,
                    ISvcLocator* pSvcLocator );

      /// Destructor
      virtual ~HistoAlgBase() {}

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

  }
} // RICH

#endif // RICHRECBASE_RICHRECHISTOALGBASE_H
