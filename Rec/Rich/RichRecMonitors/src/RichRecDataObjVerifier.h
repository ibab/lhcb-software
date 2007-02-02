
//-----------------------------------------------------------------------------
/** @file RichRecDataObjVerifier.h
 *
 *  Header file for algorithm class : Rich::Rec::DataObjVerifier
 *
 *  CVS Log :-
 *  $Id: RichRecDataObjVerifier.h,v 1.3 2007-02-02 10:07:13 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECALGS_RICHRECDATAOBJVERIFIER_H
#define RICHRECALGS_RICHRECDATAOBJVERIFIER_H 1

// from Gaudi
#include "GaudiKernel/IProperty.h"

// Rich Rec base class
#include "RichRecBase/RichRecAlgBase.h"

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

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class DataObjVerifier RichRecDataObjVerifier.h
     *
     *  Low level debugging algorithm to verify the contents of the RichRecEvent
     *  data objects.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   05/04/2002
     */
    //-----------------------------------------------------------------------------

    class DataObjVerifier : public Rich::Rec::AlgBase
    {

    public:

      /// Standard constructor
      DataObjVerifier( const std::string& name,
                       ISvcLocator* pSvcLocator );

      virtual ~DataObjVerifier(); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution
      virtual StatusCode finalize  ();    // Algorithm finalization

    private:

      /// Flag to debug RichRecPixels
      bool m_bdPixels;

      /// Flag to debug RichRecTracks
      bool m_bdTracks;

      /// Flag to debug RichRecPhotons
      bool m_bdPhotons;

      /// Flag to debug RichRecSegments
      bool m_bdSegments;

    };

  }
}

#endif // RICHRECALGS_RICHRECDATAOBJVERIFIER_H
