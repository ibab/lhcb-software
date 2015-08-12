
//--------------------------------------------------------------------------
/** @file RichGlobalPIDDigitSel.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::DigitSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDDigitSel.h,v 1.9 2007-03-09 22:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDDIGITSEL_H
#define RICHGLOBALPID_RICHGLOBALPIDDIGITSEL_H 1

#include <sstream>

// Base
#include "RichGlobalPIDAlgBase.h"
#include "RichRecBase/RichRecProcCode.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      //--------------------------------------------------------------------------
      /** @class DigitSel RichGlobalPIDDigitSel.h
       *
       *  RichDigit selection algorithm for Rich Global PID
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   12/12/2002
       */
      //--------------------------------------------------------------------------

      class DigitSel : public AlgBase
      {

      public:

        /// Standard constructor
        DigitSel( const std::string& name,
                  ISvcLocator* pSvcLocator );

        virtual ~DigitSel();   ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // private data

        /// Maximum number of usable pixels
        int m_maxUsedPixels;

      };

    }
  }
}

#endif // RICHGLOBALPID_RICHGLOBALPIDDIGITSEL_H
