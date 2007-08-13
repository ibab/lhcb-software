
//-----------------------------------------------------------------------------
/** @file RichRecDataObjVerifier.h
 *
 *  Header file for algorithm class : Rich::Rec::DataObjVerifier
 *
 *  CVS Log :-
 *  $Id: RichRecDataObjVerifier.h,v 1.5 2007-08-13 12:38:49 jonrob Exp $
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
#include "RichRecBase/RichRecHistoAlgBase.h"

namespace Rich
{
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

    class DataObjVerifier : public Rich::Rec::HistoAlgBase
    {

    public:

      /// Standard constructor
      DataObjVerifier( const std::string& name,
                       ISvcLocator* pSvcLocator );

      virtual ~DataObjVerifier(); ///< Destructor

      virtual StatusCode execute();    // Algorithm execution

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
