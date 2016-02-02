
//-----------------------------------------------------------------------------
/** @file RichRecTupleToolBase.h
 *
 * Header file for reconstruction tool base class : RichRecTupleToolBase
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

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class TupleToolBase RichRecTupleToolBase.h RichRecBase/RichRecTupleToolBase.h
     *
     *  Abstract base class for RICH reconstruction tools providing
     *  some basic functionality (identical to RichRecToolBase) but with additional
     *  histogram and ntuple functionality provided by GaudiTupleTool base class.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2005/01/13
     */
    //-----------------------------------------------------------------------------

    class TupleToolBase : public Rich::TupleToolBase,
                          public Rich::Rec::CommonBase<Rich::TupleToolBase>
    {

    public:

      /// Standard constructor
      TupleToolBase( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

      /// Destructor
      virtual ~TupleToolBase() {}

      /** Initialization of the tool after creation
       *
       * @return The status of the initialization
       * @retval StatusCode::SUCCESS Initialization was successful
       * @retval StatusCode::FAILURE Initialization failed
       */
      virtual StatusCode initialize() override;

      /** Finalization of the tool before deletion
       *
       * @return The status of the finalization
       * @retval StatusCode::SUCCESS Finalization was successful
       * @retval StatusCode::FAILURE Finalization failed
       */
      virtual StatusCode finalize() override;

    };

  } // Rec
} // RICH

#endif // RICHRECBASE_RICHRECTUPLETOOLBASE_H
