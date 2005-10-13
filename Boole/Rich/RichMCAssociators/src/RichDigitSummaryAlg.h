
//-----------------------------------------------------------------------------
/** @file RichDigitSummaryAlg.h
 *
 *  Header file for algorithm : RichDigitSummaryAlg
 *
 *  CVS Log :-
 *  $Id: RichDigitSummaryAlg.h,v 1.1.1.1 2005-10-13 15:13:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   04/10/2005
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCASSOCIATORS_RICHDIGITSUMMARYALG_H
#define RICHMCASSOCIATORS_RICHDIGITSUMMARYALG_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event model
#include "Event/MCRichDigit.h"
#include "Event/MCParticle.h"
#include "Event/RichDigitSummary.h"

// base class
#include "RichKernel/RichAlgBase.h"

//-----------------------------------------------------------------------------
/** @class RichDigitSummaryAlg RichDigitSummaryAlg.h
 *
 *  Algorithm to create RichDigitSummary objects to store association and history
 *  information for Rich Data on the DST files
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-10-04
 */
//-----------------------------------------------------------------------------

class RichDigitSummaryAlg : public RichAlgBase
{

public:

  /// Standard constructor
  RichDigitSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigitSummaryAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  bool hitInMainEvent( const MCRichHit * hit ) const;

private: // data members

  /// Flag to turn off storing of spillover summaries
  bool m_storeSpill;

};

inline bool 
RichDigitSummaryAlg::hitInMainEvent( const MCRichHit * hit ) const
{
  // get location of this hits parent container
  const std::string location = objectLocation( hit->parent() );
  // compare to main event location
  return ( location == MCRichHitLocation::Default ||
           location == "/Event/"+MCRichHitLocation::Default );
}

#endif // RICHMCASSOCIATORS_RICHDIGITSUMMARYALG_H
