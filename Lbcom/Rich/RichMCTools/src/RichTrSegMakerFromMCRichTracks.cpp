
//-----------------------------------------------------------------------------
/** @file RichTrSegMakerFromMCRichTracks.cpp
 *
 * Implementation file for class : RichTrSegMakerFromMCRichTracks
 *
 * CVS Log :-
 * $Id: RichTrSegMakerFromMCRichTracks.cpp,v 1.4 2004-07-26 17:56:09 jonrob Exp $
 * $Log: not supported by cvs2svn $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTrSegMakerFromMCRichTracks.h"

// Declaration of the Algorithm Factory
static const  ToolFactory<RichTrSegMakerFromMCRichTracks>          Factory ;
const        IToolFactory& RichTrSegMakerFromMCRichTracksFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichTrSegMakerFromMCRichTracks::
RichTrSegMakerFromMCRichTracks( const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : RichToolBase ( type, name, parent )
{
  declareInterface<IRichTrSegMaker>(this);
}

//=============================================================================
// Destructor
//=============================================================================
RichTrSegMakerFromMCRichTracks::~RichTrSegMakerFromMCRichTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichTrSegMakerFromMCRichTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichMCTruthTool", m_truth );

  // get the radiators
  const std::string aeroDefault = "/dd/Structure/LHCb/Rich1/Aerogel";
  m_radiators[Rich::Aerogel] = getDet<DeRichRadiator>( aeroDefault );
  m_radiators[Rich::C4F10]   = getDet<DeRichRadiator>( DeRichRadiatorLocation::C4F10 );
  m_radiators[Rich::CF4]     = getDet<DeRichRadiator>( DeRichRadiatorLocation::CF4   );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichTrSegMakerFromMCRichTracks::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}


//=============================================================================
// Constructs the track segments for a given input object
//=============================================================================
int
RichTrSegMakerFromMCRichTracks::constructSegments( const ContainedObject * obj,
                                                   std::vector<RichTrackSegment>& segments )
  const {

  // get MCRichTrack
  const MCRichTrack * track = mcRichTrack(obj);
  if ( !track ) {
    Warning("MCRichTrack not available for input data object");
    return 0;
  }

  // loop over radiators
  for ( Radiators::const_iterator radiator = m_radiators.begin();
        radiator != m_radiators.end();
        ++radiator ) {
    const Rich::RadiatorType rad = (*radiator)->radiatorID();

    // See if there is an MCRichSegment for this radiator
    const MCRichSegment * segment = track->segmentInRad(rad);
    if ( !segment ) continue;

    // Get entry information
    const HepPoint3D & entryPoint          = segment->entryPoint();
    const HepVector3D & entryStateMomentum = segment->entryMomentum();

    // Get exit information
    const HepPoint3D & exitPoint          = segment->exitPoint();
    const HepVector3D & exitStateMomentum = segment->exitMomentum();

    if ( Rich::Aerogel == rad ) {

      // Using this information, make radiator segment and add to vector
      // assuming straight line between entry and exit
      segments.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                            entryPoint, entryStateMomentum,
                                            exitPoint, exitStateMomentum,
                                            rad, (*radiator)->rich() ) );

      // printout
      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Found MCRichSegment for " << rad << endreq
                  << " Entry Point : " << entryPoint << endreq
                  << " Entry Mom.  : " << entryStateMomentum << endreq
                  << " Exit Point  : " << exitPoint << endreq
                  << " Exit Mom.   : " << exitStateMomentum << endreq;
      }

    } else {

      // Get middle point information
      const HepPoint3D midPoint          = segment->bestPoint(0.5);
      const HepVector3D midStateMomentum = segment->bestMomentum(0.5);

      // Using this information, make radiator segment and add to vector
      // Use a middle state as well as entry and exit ones
      segments.push_back( RichTrackSegment( RichTrackSegment::UseAllStateVectors(),
                                            entryPoint, entryStateMomentum,
                                            midPoint, midStateMomentum,
                                            exitPoint, exitStateMomentum,
                                            rad, (*radiator)->rich() ) );

      // printout
      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Found MCRichSegment for " << rad << endreq
                  << " Entry Point : " << entryPoint << endreq
                  << " Entry Mom.  : " << entryStateMomentum << endreq
                  << " Mid Point   : " << midPoint << endreq
                  << " Mid Mom.    : " << midStateMomentum << endreq
                  << " Exit Point  : " << exitPoint << endreq
                  << " Exit Mom.   : " << exitStateMomentum << endreq;
      }

    }

  }

  // return value is number of segments formed
  return segments.size();
}

//============================================================================
const MCRichTrack *
RichTrSegMakerFromMCRichTracks::mcRichTrack( const ContainedObject * obj ) const
{
  // Work out what we have been given
  const MCRichTrack * track = dynamic_cast<const MCRichTrack *>(obj);
  if ( track ) {
    verbose() << "Input data is of type MCRichTrack" << endreq;
  } else {
    const TrStoredTrack * trTrack = dynamic_cast<const TrStoredTrack *>(obj);
    if ( trTrack ) {
      verbose() << "Input data is of type TrStoredTrack" << endreq;
      track = m_truth->mcRichTrack( trTrack );
    }
  }

  // return final object
  return track;
}
