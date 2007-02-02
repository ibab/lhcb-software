
//---------------------------------------------------------------------------
/** @file RichRecBackgroundEsti.h
 *
 *  Header file for algorithm class : Rich::Rec::BackgroundEsti
 *
 *  CVS Log :-
 *  $Id: RichRecBackgroundEsti.h,v 1.6 2007-02-02 10:05:51 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
#define RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"

// RichKernel
//--------------------------------------------------------------------
#include "RichKernel/RichMap.h"

// Interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"

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
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class BackgroundEsti RichRecBackgroundEsti.h
     *
     *  Background estimation algorithm for RICH HPDs
     *
     *  Compares the expected signal yield in each HPD, based on the observed tracks
     *  and current set of mass hypotheses for those tracks, to the total signal in
     *  each HPD.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class BackgroundEsti : public RichRecAlgBase
    {

    public:

      /// Standard constructor
      BackgroundEsti( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~BackgroundEsti();   ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution

    private:   // Private data members

      /// Pointers to expected track signal tool
      const IExpectedTrackSignal * m_tkSignal;

      /// Maximum number of iterations in background normalisation
      int m_maxBkgIterations;

      /// Number of pixels per HPD without cathode acceptance
      double m_nPixelsPerPD;

    };

  }
}

#endif // RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
