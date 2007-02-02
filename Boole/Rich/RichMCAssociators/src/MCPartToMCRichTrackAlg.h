
//-----------------------------------------------------------------------------
/** @file MCPartToMCRichTrackAlg.h
 *
 *  Header file for algorithm : MCPartToMCRichTrackAlg
 *
 *  CVS Log :-
 *  $Id: MCPartToMCRichTrackAlg.h,v 1.6 2007-02-02 10:13:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCASSOCIATORS_MCPARTTOMCRICHTRACKALG_H
#define RICHMCASSOCIATORS_MCPARTTOMCRICHTRACKALG_H 1

// Linker
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTool.h"
#include "Linker/LinkedTo.h"

// Event model
#include "Event/MCRichTrack.h"
#include "Event/MCParticle.h"

// base class
#include "RichKernel/RichAlgBase.h"

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
    /** @class MCPartToMCRichTrackAlg MCPartToMCRichTrackAlg.h
     *
     *  Builds the association tables between MCParticles and MCRichTracks
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   2004-02-11
     */
    //-----------------------------------------------------------------------------

    class MCPartToMCRichTrackAlg : public RichAlgBase
    {

    public:

      /// Standard constructor
      MCPartToMCRichTrackAlg( const std::string& name, ISvcLocator* pSvcLocator );

      /// Destructor
      virtual ~MCPartToMCRichTrackAlg( );

      virtual StatusCode execute   ();    ///< Algorithm execution

    private: // definitions

      /// typedef for building linker
      typedef LinkerWithKey<LHCb::MCRichTrack,LHCb::MCParticle> MCPartToRichTracks;

    private: // methods

      /// Add a given event to the linker object
      StatusCode addEvent( const std::string & evtLoc );

      /// Return a pointer to the linker for this event
      MCPartToRichTracks * linker();

      /// Reset linker object for new event
      void resetLinker();

    private: // data

      /// typedef for List of event locations to process
      typedef std::vector<std::string> EventList;

      /// List of event locations to process
      EventList m_evtLocs;

      /// Pointer to linker object
      MCPartToRichTracks * m_linker;

    };

    inline void MCPartToMCRichTrackAlg::resetLinker()
    {
      if ( m_linker ) { delete m_linker; m_linker = NULL; }
    }

  }
}

#endif // RICHMCASSOCIATORS_MCPARTTOMCRICHTRACKALG_H
