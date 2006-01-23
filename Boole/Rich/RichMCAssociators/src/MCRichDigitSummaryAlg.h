
//-----------------------------------------------------------------------------
/** @file MCRichDigitSummaryAlg.h
 *
 *  Header file for algorithm : MCRichDigitSummaryAlg
 *
 *  CVS Log :-
 *  $Id: MCRichDigitSummaryAlg.h,v 1.2 2006-01-23 13:52:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   04/10/2005
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCASSOCIATORS_MCRichDigitSummaryAlg_H
#define RICHMCASSOCIATORS_MCRichDigitSummaryAlg_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event model
#include "Event/MCRichDigit.h"
#include "Event/MCRichDigitSummary.h"
#include "Event/MCParticle.h"

// base class
#include "RichKernel/RichAlgBase.h"

// interfaces
#include "RichKernel/IRichMCTruthTool.h"

//-----------------------------------------------------------------------------
/** @class MCRichDigitSummaryAlg MCRichDigitSummaryAlg.h
 *
 *  Algorithm to fill the MCRichDigitSummary objects.
 *  Used to provide direct navigation from RichSmartIDs to MCParticles on the DST,
 *  and also to provide some history information.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-10-04
 */
//-----------------------------------------------------------------------------

class MCRichDigitSummaryAlg : public RichAlgBase
{

public:

  /// Standard constructor
  MCRichDigitSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCRichDigitSummaryAlg( ); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialisation
  virtual StatusCode execute();    ///< Algorithm execution
  virtual StatusCode finalize();   ///< Algorithm finalisation

private: // methods

  /// test if given hit is in main event ?
  bool hitInMainEvent( const LHCb::MCRichHit * hit ) const;

  /// test if hit is in given spillover event
  bool hitInSpillEvent( const LHCb::MCRichHit * hit,
                        const std::string & loc ) const;

private: // data members

  /// Flag to turn off storing of spillover summaries
  bool m_storeSpill;

  /// Pointer to RichMCTruth tool
  const IRichMCTruthTool * m_truth;

  // locations of MCRichHits in TES
  std::string m_RichHitLoc;
  std::string m_RichPrevLoc;
  std::string m_RichPrevPrevLoc;
  std::string m_RichNextLoc;
  std::string m_RichNextNextLoc;

};

inline bool
MCRichDigitSummaryAlg::hitInMainEvent( const LHCb::MCRichHit * hit ) const
{
  // get location of this hits parent container
  const std::string location = objectLocation( hit->parent() );
  // compare to main event location
  return ( location == LHCb::MCRichHitLocation::Default ||
           location == "/Event/"+LHCb::MCRichHitLocation::Default );
}

inline bool
MCRichDigitSummaryAlg::hitInSpillEvent( const LHCb::MCRichHit * hit,
                                        const std::string & loc ) const
{
  return ( objectLocation(hit->parent()) == loc );
}

#endif // RICHMCASSOCIATORS_MCRichDigitSummaryAlg_H
