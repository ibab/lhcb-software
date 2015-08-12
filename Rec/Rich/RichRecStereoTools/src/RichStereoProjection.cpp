
//-----------------------------------------------------------------------------
/** @file RichStereoProjection.cpp
 *
 *  Implementation file for tool : Rich::Rec::StereoProjection
 *
 *  @author Luigi Delbuono   delbuono@in2p3.fr
 *  @date   20/06/2007
 */
//-----------------------------------------------------------------------------

// local
#include "RichStereoProjection.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// namespaces
using namespace LHCb;
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( StereoProjection )

//-----------------------------------------------------------------------------

// Standard constructor
StereoProjection::StereoProjection ( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
  : Rich::Rec::ToolBase ( type, name, parent )
{
  // interface
  declareInterface<IStereoProjection>(this);
}

StatusCode StereoProjection::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichParticleProperties",  m_richPartProp );

  return sc;
}

int StereoProjection::Project( const RichRecSegment *segment, RichRecRing &recRing ) const
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "RichRecSegment " << segment->key()
                                      << " Projection stereo" << endmsg;

  if (segment!=0) 
  {
    //Acquire all photon candidates for this segment
    const RichRecSegment::Photons & photons = segment->richRecPhotons();

    Gaudi::XYZPoint avgEmissionPntSeg;  //geom quantities at "segment" level
    Gaudi::Rotation3D toUnitSphereSeg;
    // loop over photons for this segment
    if ( avgEmissionPnt(segment, avgEmissionPntSeg) != 0 && toUnitSphere(segment, toUnitSphereSeg) != 0 )
    {
      RichRecPointOnRing::Vector & recPoints = recRing.ringPoints();
      for ( RichRecSegment::Photons::const_iterator iPhot = photons.begin();
            iPhot != photons.end(); ++iPhot ) 
      {
        //--Perform stereographic projection of photon
        //compute photon direction on the Cerenkov cone
        Gaudi::XYZVector photdir=((*iPhot)->geomPhoton().sphMirReflectionPoint() - avgEmissionPntSeg);
        Gaudi::Polar3DVector photUnitDirPol = (Gaudi::Polar3DVector)photdir;
        photUnitDirPol.SetR(1);
        Gaudi::XYZVector photUnitDir(photUnitDirPol);

        //compute the photon direction in stero frame (Z along the track direction)
        Gaudi::XYZVector rotPhotUnitDir = toUnitSphereSeg * photUnitDir;

        //do the final 1/(1+Z) transformation (South pole projection)
        double base = 1+rotPhotUnitDir.z();
        Gaudi::XYZPoint locPhotPos;
        if (base != 0) locPhotPos.SetCoordinates(rotPhotUnitDir.x()/base,
                                                 rotPhotUnitDir.y()/base, 0); //singular point at South pole...
        else locPhotPos.SetCoordinates(0, 0, 0);

        Gaudi::XYZPoint globPhotPos(0, 0, 0);
        //two last params: use default
        recPoints.push_back( RichRecPointOnRing(globPhotPos,locPhotPos) );
      }

      //Final fill
      recRing.setRichRecSegment(segment);

      return(1);
    }
  }
  return(0);
}



// Projection with other input
int StereoProjection::Project( RichRecPhoton::ConstVector & richRecPhotons, RichRecRing &recRing ) const
{
  if ( msgLevel(MSG::DEBUG) )  debug() << "Projection stereo" << endmsg;

  if ( !richRecPhotons.empty() ) 
  {
    // get segment associated to first recphoton (!should be the same for all photons in input vector!)
    const RichRecSegment *segment = richRecPhotons[0]->richRecSegment();

    Gaudi::XYZPoint avgEmissionPntPhot; //geom quantities for photon set or "segment" level
    Gaudi::Rotation3D toUnitSphereSeg;
    if(avgEmissionPnt(richRecPhotons,avgEmissionPntPhot)!= 0 &&  toUnitSphere(segment,toUnitSphereSeg)!= 0 ) {
      RichRecPointOnRing::Vector & recPoints = recRing.ringPoints();
      // loop over photons
      for ( RichRecPhoton::ConstVector::iterator iPhot = richRecPhotons.begin();
            iPhot != richRecPhotons.end(); ++iPhot ) {

        //--Perform stereographic projection of photon
        //compute photon direction on the Cerenkov cone
        Gaudi::XYZVector photdir=((*iPhot)->geomPhoton().sphMirReflectionPoint() - avgEmissionPntPhot);
        Gaudi::Polar3DVector photUnitDirPol = (Gaudi::Polar3DVector)photdir;
        photUnitDirPol.SetR(1);
        Gaudi::XYZVector photUnitDir(photUnitDirPol);

        //compute the photon direction in stero frame (Z along the track direction)
        Gaudi::XYZVector rotPhotUnitDir = toUnitSphereSeg * photUnitDir;

        //do the final 1/(1+Z) transformation (South pole projection)
        double base = 1+rotPhotUnitDir.z();
        Gaudi::XYZPoint locPhotPos;
        if (base != 0) locPhotPos.SetCoordinates(rotPhotUnitDir.x()/base,
                                                 rotPhotUnitDir.y()/base, 0); //singular point at South pole...
        else locPhotPos.SetCoordinates(0, 0, 0);

        Gaudi::XYZPoint globPhotPos(0, 0, 0);
        //two last params: use default
        recPoints.push_back(RichRecPointOnRing(globPhotPos, locPhotPos));
      }
      //Final fill
      recRing.setRichRecSegment(segment);

      return(1);
    }
  }
  return(0);
}




// Returns the average photons emission point (for a segment)
int StereoProjection::avgEmissionPnt(const RichRecSegment *segment, Gaudi::XYZPoint &avgPoint) const
{
  if(segment!=0) {
    //Acquire all photon candidates for this segment
    const RichRecSegment::Photons &photons = segment->richRecPhotons();

    if(photons.size()>0) {
      avgPoint.SetCoordinates(0,0,0);
      //compute the average
      for ( RichRecSegment::Photons::const_iterator iPhot = photons.begin();
            iPhot != photons.end(); ++iPhot ) {
        avgPoint += (Gaudi::XYZVector)((*iPhot)->geomPhoton().emissionPoint());
      }
      avgPoint/=((double)photons.size());
      return(1);
    }
  }
  return(0);
}



// Returns the average photons emission point (for a vector of richRecPhotons)
int StereoProjection::avgEmissionPnt(RichRecPhoton::ConstVector & richRecPhotons, Gaudi::XYZPoint &avgPoint) const
{
  if(richRecPhotons.size()>0) {
    avgPoint.SetCoordinates(0,0,0);
    //compute the average
    for ( RichRecPhoton::ConstVector::iterator iPhot = richRecPhotons.begin();
          iPhot != richRecPhotons.end(); ++iPhot ) {
      avgPoint += (Gaudi::XYZVector)((*iPhot)->geomPhoton().emissionPoint());
    }
    avgPoint/=((double)richRecPhotons.size());
    return(1);
  }
  return(0);
}

// Compute rotation to stereo frame (Z direction along the track)
int StereoProjection::toUnitSphere(const RichRecSegment *segment, Gaudi::Rotation3D &rot) const
{
  if(segment!=0) {
    RichRecSegment *segment_nonconst = (RichRecSegment *) segment;   //problem with "const" in RichRecSegment
    // Segment direction
    Gaudi::XYZVector segUnitDir = segment_nonconst->trackSegment().bestMomentum().unit();

    // construct transform to stereographic sphere
    Gaudi::Polar3DVector yPol = (Gaudi::Polar3DVector) segUnitDir.Cross( Gaudi::XYZVector(1,0,0) );
    yPol.SetR(1);
    Gaudi::XYZVector y(yPol);
    Gaudi::XYZVector x = y.Cross(segUnitDir);
    rot.SetComponents(x,y,segUnitDir);
    rot.Invert();
    return(1);
  }
  return(0);
}
