
//--------------------------------------------------------------------------
/** @file RichGlobalPIDDigitSel.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::DigitSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDDigitSel.h,v 1.8 2007-02-02 10:03:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDDIGITSEL_H
#define RICHGLOBALPID_RICHGLOBALPIDDIGITSEL_H 1

// Base
#include "RichGlobalPIDAlgBase.h"
#include "RichRecBase/RichRecProcCode.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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
    /** @namespace GlobalPID
     *
     *  General namespace for Global PID software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   04/12/2006
     */
    //-----------------------------------------------------------------------------
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
