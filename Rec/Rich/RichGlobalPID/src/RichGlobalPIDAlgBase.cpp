
//-----------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.cpp
 *
 *  Implementation file for RICH Global PID algorithm base class : Rich::Rec::GlobalPID::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.cpp,v 1.9 2007-02-02 10:03:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichGlobalPIDAlgBase.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
AlgBase::AlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name , pSvcLocator ),
    m_GPIDtracks       ( NULL ),
    m_GPIDSummary      ( NULL ),
    m_GPIDs            ( NULL )
{

  declareProperty( "RichGlobalPIDTrackLocation",
                   m_richGPIDTrackLocation =
                   LHCb::RichGlobalPIDTrackLocation::Default );

  declareProperty( "RichGlobalPIDLocation",
                   m_richGPIDLocation = LHCb::RichGlobalPIDLocation::Default );

  declareProperty( "RichGlobalPIDSummaryLocation",
                   m_richGPIDSummaryLocation =
                   LHCb::RichGlobalPIDSummaryLocation::Default );

  declareProperty( "GlobalName", m_richGPIDName = "RichGloPID" );

  declareProperty( "ProcStatusLocation",
                   m_procStatLocation =LHCb:: ProcStatusLocation::Default );

}

// Destructor
AlgBase::~AlgBase() { }
