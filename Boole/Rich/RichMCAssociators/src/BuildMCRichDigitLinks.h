
/** @file BuildMCRichDigitLinks.h
 *
 *  Header file for RICH DAQ algorithm : BuildMCRichDigitLinks
 *
 *  CVS Log :-
 *  $Id: BuildMCRichDigitLinks.h,v 1.4 2007-03-01 19:37:22 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

#ifndef RICHDAQ_BUILDMCRICHDIGITLINKS_H
#define RICHDAQ_BUILDMCRICHDIGITLINKS_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// Event model
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCTruth.h"

namespace Rich
{
  namespace MC
  {

    /** @class BuildMCRichDigitLinks BuildMCRichDigitLinks.h
     *
     *  Builds the links between MCRichDigits and RichDigits
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-09
     */

    class BuildMCRichDigitLinks : public RichAlgBase
    {

    public:

      /// Standard constructor
      BuildMCRichDigitLinks( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~BuildMCRichDigitLinks( ); ///< Destructor

      virtual StatusCode execute   ();   ///< Algorithm execution

    private:

      /// String containing input MCRichDigits location in TDS
      std::string m_mcRichDigitsLocation;

      /// String containing input RichDigits location in TDS
      std::string m_richDigitsLocation;

    };

  }
}

#endif // RICHDAQ_BUILDMCRICHDIGITLINKS_H
