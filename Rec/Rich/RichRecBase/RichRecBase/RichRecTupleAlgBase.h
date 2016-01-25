
//-----------------------------------------------------------------------------
/** @file RichRecTupleAlgBase.h
 *
 *  Header file for RICH reconstruction monitor algorithm base class :
 *  RichRecTupleAlgBase
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

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class TupleAlgBase RichRecTupleAlgBase.h RichRecBase/RichRecTupleAlgBase.h
     *
     *  Abstract base class for RICH reconstruction algorithms providing
     *  some basic functionality (identical to RichRecAlgBase) but with additional
     *  histogram and ntuple functionality provided by GaudiTupleAlg base class.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2005/01/13
     */
    //-----------------------------------------------------------------------------

    class TupleAlgBase : public Rich::TupleAlgBase,
                         public Rich::Rec::CommonBase<Rich::TupleAlgBase>
    {

    public:

      /// Standard constructor
      TupleAlgBase( const std::string& name,
                    ISvcLocator* pSvcLocator );

      /// Destructor
      virtual ~TupleAlgBase() {}

      /** Initialization of the algorithm after creation
       *
       * @return The status of the initialization
       * @retval StatusCode::SUCCESS Initialization was successful
       * @retval StatusCode::FAILURE Initialization failed
       */
      virtual StatusCode initialize() override;

      /** The main event processing method. Called once for each event
       *
       * @return The status of the event processing
       * @retval StatusCode::SUCCESS Event processing was successful
       * @retval StatusCode::FAILURE Event processing failed
       */
      virtual StatusCode execute() override;

      /** Finalization of the algorithm before deletion
       *
       * @return The status of the finalization
       * @retval StatusCode::SUCCESS Finalization was successful
       * @retval StatusCode::FAILURE Finalization failed
       */
      virtual StatusCode finalize() override;

    };

  }
} // RICH

/** Backwards compatibility typedef
 * @todo Remove eventually
 */
typedef Rich::Rec::TupleAlgBase RichRecTupleAlgBase;

#endif // RICHRECBASE_RICHRECTUPLEALGBASE_H
