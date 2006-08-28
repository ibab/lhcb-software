
//-----------------------------------------------------------------------------
/** @file RichMCTrueTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : RichMCTrueTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichMCTrueTrackSelector.h,v 1.1 2006-08-28 11:13:29 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichMCTrueTrackSelector_H
#define RICHRECTOOLS_RichMCTrueTrackSelector_H 1

// base class
#include "RichRecBase/RichTrackSelectorBase.h"

// interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"

// Event model
#include "Event/MCParticle.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class RichMCTrueTrackSelector RichMCTrueTrackSelector.h
   *
   *  Extends the core track selection in RichTrackSelectorBase to add a check
   *  to see if the track has an MC association
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-08-12
   */
  //-----------------------------------------------------------------------------

  class RichMCTrueTrackSelector : public RichTrackSelectorBase
  {

  public: // Gaudi methods

    /// Standard constructor
    RichMCTrueTrackSelector( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

    /// Destructor
    virtual ~RichMCTrueTrackSelector( );

    /// Tool initialization
    virtual StatusCode initialize();

  public: // interface methods

    /// Test if the given Track is selected
    virtual bool trackSelected( const LHCb::Track * track ) const;

    /// Test it the given RichRecTrack is selected
    virtual bool trackSelected( const LHCb::RichRecTrack * track ) const;

  private: // data

    /// Pointer to RichRecMCTruthTool interface
    const IRichRecMCTruthTool* m_richRecMCTruth;  

    /// MCParticle association weight
    double m_weight;

  };

} // RICH namespace

#endif // RICHRECTOOLS_RichMCTrueTrackSelector_H
