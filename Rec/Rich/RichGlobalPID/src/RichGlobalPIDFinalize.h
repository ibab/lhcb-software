
//--------------------------------------------------------------------------
/** @file RichGlobalPIDFinalize.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Finalize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDFinalize.h,v 1.10 2007-03-09 22:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDFINALIZE_H
#define RICHGLOBALPID_RICHGLOBALPIDFINALIZE_H 1

// Base class
#include "RichGlobalPIDAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// GSL
#include "gsl/gsl_sf_erf.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      //--------------------------------------------------------------------------
      /** @class Finalize RichGlobalPIDFinalize.h
       *
       *  Finalises the RichPID objects for the Global PID algorithm
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   12/12/2002
       *
       *  @todo Remove temporary hack "correcting" final DLL values once
       *        likelihood algorithm is rewritten
       */
      //--------------------------------------------------------------------------

      class Finalize : public AlgBase
      {

      public:

        /// Standard constructor
        Finalize( const std::string& name,
                  ISvcLocator* pSvcLocator );

        virtual ~Finalize();   ///< Destructor

        virtual StatusCode execute   ();    // Algorithm execution

      };

    }
  }
}

#endif // RICHGLOBALPID_RICHGLOBALPIDFINALIZE_H
