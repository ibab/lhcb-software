
//--------------------------------------------------------------------------
/** @file RichGlobalPIDFinalize.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Finalize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDFinalize.h,v 1.12 2008-03-25 16:26:44 jonrob Exp $
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

// interfaces
#include "IRichGlobalPIDTrackCreator.h"

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
       */
      //--------------------------------------------------------------------------

      class Finalize : public AlgBase
      {

      public:

        /// Standard constructor
        Finalize( const std::string& name,
                  ISvcLocator* pSvcLocator );

        virtual ~Finalize();   ///< Destructor

        virtual StatusCode initialize(); // Algorithm initialize
        virtual StatusCode execute();    // Algorithm execution

      private:
        
        /// RichGlobalPIDTrack creator
        const ITrackCreator * m_gtkCreator;
        
      };

    }
  }
}

#endif // RICHGLOBALPID_RICHGLOBALPIDFINALIZE_H
