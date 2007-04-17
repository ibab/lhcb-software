// $Id: TrackTrajFitter.cpp,v 1.1 2007-04-17 15:22:13 graven Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Measurement.h"
// from TrackFitEvent
#include "Event/LineDifTraj.h"


// from std
#include <vector>
// from Boost
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/construct.hpp"


#include "TrackInterfaces/ITrajFitter.h"
// local
#include "TrackTrajFitter.h"

using namespace std;
using namespace boost;
using namespace boost::lambda;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackTrajFitter
//
//  Original Author: Gerhard Raven
//  Created: 31-10-2006
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackTrajFitter );

//=========================================================================
// Standard Constructor, initializes variables
//=========================================================================
TrackTrajFitter::TrackTrajFitter( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : GaudiTool( type, name, parent)
{
  declareInterface<ITrackFitter>( this );
}

//=========================================================================
// Destructor
//=========================================================================
TrackTrajFitter::~TrackTrajFitter() {
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode TrackTrajFitter::initialize() 
{
  m_trajfitter = tool<ITrajFitter>("TrajFitter","TrajFitter",this);
  return GaudiTool::initialize();
}

//=========================================================================
// Fit the track
//=========================================================================
StatusCode TrackTrajFitter::fit( Track& track )
{
  StatusCode sc;
  // grab the (OT,Velo,...) measurements from the input track and clone them.
  const LHCb::Measurement::Vector& in = track.measurements();
  LHCb::Measurement::Vector out;
  for(LHCb::Measurement::Vector::const_iterator i=in.begin();i!=in.end();++i) {
       // grab all velo hits of the track...
       if ( *i == 0 ) { debug() << " null pointer to measurement! " << endmsg; continue; }
       if ( !(*i)->checkType(Measurement::VeloR) && !(*i)->checkType(Measurement::VeloPhi) ) continue; 
       out.push_back( (*i)->clone() );
  }
  debug() << "  picked up " 
          <<  out.size() 
          << " measurements  out of " 
          << in.size() << endmsg;
  if (out.size()<10) return sc;

  LineDifTraj traj(track.position(),track.slopes(),make_pair(-10000.0,100000.0));;
  debug() << " creating initial traj : " << traj.parameters() << endmsg;
  typedef ITrajFitter::Types<LineDifTraj::kSize>::ResDerivatives Residuals;
  Residuals res;
  sc = m_trajfitter->fit(  traj, res, out );
  debug() << " trajfitter status: " << sc << endmsg;
  debug() << " back from TrajFitter: " << traj.parameters() <<  endmsg;
  double chisq=0;
  for (Residuals::iterator r=res.begin();r!=res.end();++r) {
        debug() << " " << r->first << " : " << r->second << endmsg;
        chisq+=r->first*r->first;
  }
  debug() << " got chisq = " << chisq
          << " for " << out.size()-LineDifTraj::kSize << " D.O.F. " << endmsg;

  for_each(out.begin(),out.end(),bind(delete_ptr(),_1)); // delete the measurements we made...
  return sc;
}
