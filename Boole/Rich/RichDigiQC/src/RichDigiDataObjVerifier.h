
//------------------------------------------------------------------------------------
/** @file RichDigiDataObjVerifier.h
 *
 *  Header file for RICH Digitisation Quality Control algorithm : Rich::MC::Digi::DataObjVerifier
 *
 *  CVS Log :-
 *  $Id: RichDigiDataObjVerifier.h,v 1.12 2007-02-02 10:12:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//------------------------------------------------------------------------------------

#ifndef RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H
#define RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// Event model
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichHit.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"

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

  //-----------------------------------------------------------------------------
  /** @namespace MC
   *
   *  General namespace for RICH MC related software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   05/12/2006
   */
  //-----------------------------------------------------------------------------
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @namespace Digi
     *
     *  General namespace for RICH Digitisation simuation related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   17/01/2007
     */
    //-----------------------------------------------------------------------------
    namespace Digi
    {

      /** @class DataObjVerifier RichDigiDataObjVerifier.h
       *
       *  Low level printout of Rich Digitisation objects
       *
       *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
       *  @date   2003-09-08
       */

      class DataObjVerifier : public Rich::AlgBase
      {

      public:

        /// Standard constructor
        DataObjVerifier( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~DataObjVerifier( ); ///< Destructor

        virtual StatusCode execute   ();    ///< Algorithm execution

      private: // methods

        /// Check the MCRichHits at the given location
        void checkHitsAt( const std::string & location ) const;

        /// Check the MCRichOpticalPhotons at the given location
        void checkPhotsAt( const std::string & location ) const;

      private: // data

        // job options
        bool m_bdMcDigits;      ///< Flag to turn on/off the checking and printing of MCRichDigits
        bool m_bdMCHits;        ///< Flag to turn on/off the checking and printing of MCRichHits
        bool m_bdMCPhots;       ///< Flag to turn on/off the checking and printing of MCRichOpticalPhotons

      };

    }
  }
}

#endif // RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H
