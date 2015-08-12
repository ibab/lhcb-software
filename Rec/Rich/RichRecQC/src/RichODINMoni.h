
//-----------------------------------------------------------------------------
/** @file RichODINMoni.h
 *
 *  Header file for algorithm class : Rich::DAQ::ODINMoni
 *
 *  CVS Log :-
 *  $Id: RichODINMoni.h,v 1.6 2009-08-05 23:14:59 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECQC_RichODINMoni_H
#define RICHRECQC_RichODINMoni_H 1

// STD
#include <sstream>

// base class
#include "RichKernel/RichHistoAlgBase.h"

// Event
#include "Event/ODIN.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace DAQ
  {
    //-----------------------------------------------------------------------------
    /** @class ODINMoni RichODINMoni.h
     *
     *  Simple monitor for the hits in each HPD
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   05/04/2002
     */
    //-----------------------------------------------------------------------------

    class ODINMoni : public HistoAlgBase
    {

    public:

      /// Standard constructor
      ODINMoni( const std::string& name,
                ISvcLocator* pSvcLocator );

      virtual ~ODINMoni( ); ///< Destructor

      virtual StatusCode execute(); ///< Algorithm execution

    protected:

      /// Pre-Book all (non-MC) histograms
      virtual StatusCode prebookHistograms();

    private: // data

    };

  }
}

#endif // RICHRECQC_RichODINMoni_H
