
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.h
 *
 *  Header file for reconstruction algorithm base class : Rich::Rec::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecAlgBase.h,v 1.22 2007-02-01 17:26:22 jonrob Exp $
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
    /** @class AlgBase RichRecAlgBase.h RichRecBase/RichRecAlgBase.h
     *
     *  Abstract base class for RICH reconstruction algorithms providing
     *  some basic functionality.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   05/04/2002
     */
    //-----------------------------------------------------------------------------

    class AlgBase : public Rich::AlgBase,
                    public Rich::Rec::CommonBase<Rich::AlgBase>
    {

    public:

      /// Standard constructor
      AlgBase( const std::string& name,
               ISvcLocator* pSvcLocator );

      /// Destructor
      virtual ~AlgBase() {};

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

/** Backwards compatibility typedef
 * @todo Remove eventually
 */
typedef Rich::Rec::AlgBase RichRecAlgBase;

#endif // RICHRECBASE_RICHRECALGBASE_H
