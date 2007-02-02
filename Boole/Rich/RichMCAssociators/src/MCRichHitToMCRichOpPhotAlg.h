
/** @file MCRichHitToMCRichOpPhotAlg.h
 *
 *  Header file for algorithm : MCRichHitToMCRichOpPhotAlg
 *
 *  CVS Log :-
 *  $Id: MCRichHitToMCRichOpPhotAlg.h,v 1.5 2007-02-02 10:13:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */

#ifndef RICHMCASSOCIATORS_MCRICHHITTOMCRICHOPPHOTALG_H
#define RICHMCASSOCIATORS_MCRICHHITTOMCRICHOPPHOTALG_H  1

// from STL
#include <string>

// Linker
#include "Linker/LinkerWithKey.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"
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

    /** @class MCRichHitToMCRichOpPhotAlg MCRichHitToMCRichOpPhotAlg.h
     *
     *  Builds the association tables between MCRichHits and MCRichOpticalPhotons
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   2004-02-11
     */

    class MCRichHitToMCRichOpPhotAlg : public RichAlgBase
    {

    public:

      /// Standard constructor
      MCRichHitToMCRichOpPhotAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~MCRichHitToMCRichOpPhotAlg( ); ///< Destructor

      // Algorithm initialization
      virtual StatusCode initialize();

      // Algorithm execution
      virtual StatusCode execute   ();

    private: // definitions

      /// Shortcut to linker name
      typedef LinkerWithKey<LHCb::MCRichOpticalPhoton,LHCb::MCRichHit> MCRichHitsToPhotons;

    private: // methods

      /// Add a given event to the linker object
      StatusCode addEvent( const std::string & evtLoc );

      /// Return a pointer to the linker for this event
      MCRichHitsToPhotons * linker();

      /// Reset linker object for new event
      void resetLinker();

    private: // data

      /// typedef of event locations to process
      typedef std::vector<std::string> EventList;

      /// event locations to process
      EventList m_evtLocs;

      /// Pointer to linker object
      MCRichHitsToPhotons * m_linker;

    };

    inline void MCRichHitToMCRichOpPhotAlg::resetLinker()
    {
      if ( m_linker ) { delete m_linker; m_linker = NULL; }
    }

  }
}

#endif // RICHMCASSOCIATORS_MCRICHHITTOMCRICHOPPHOTALG_H
