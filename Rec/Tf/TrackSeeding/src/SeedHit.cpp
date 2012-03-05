#include "SeedHit.h"
#include "TrackSeedPoint.h"
#include "STDet/DeSTLayer.h"
#include "STDet/DeSTSector.h"
#include "OTDet/DeOTModule.h"
#include "Kernel/Trajectory.h"
//#include "GaudiKernel/Transform3D.h"

using namespace std; // for the vectors

/*
 Class SeedHit follows LTHit, from longtracks, but can cope with alignments.
 
 author: H.J. Bulten
 created: 15-12-2005
 modified: 20-7-2006. I removed links to TrackSeedPlaneDef and do wire drift time
  calculations in the constructor. I only store linear offsets to calculate x,y, and
  z of hits (this includes wire drift time effects). Since there were up to billions of
  calls to SeedHit.y() per event, this implementation speeds up the code with about a factor
  of three, at the cost of clarity. I also removed standard containers in favor of arrays.
*/

// the static members:
double SeedHit::Y_margin=5; // may be overwritten from TrackSeedFind
double SeedHit::OT_cellradius=2.625; // may be overwritten from TrackSeedFind::initialize()
double SeedHit::OT_driftspeed=0.2; // will be overwritten from TrackSeedFind::initialize()
double SeedHit::OT_wiredrifttime=2.5; // will be overwritten from TrackSeedFind::initialize()
double SeedHit::OTResolution = 0.5;
double SeedHit::ITResolution = 0.1;


SeedHit::SeedHit() : m_ITRef(0), m_OTRef(0) {
    m_isIT = m_isOT = false;
}
       
SeedHit::SeedHit(LHCb::STLiteCluster* itclus, DeSTDetector* itdet) : m_ITRef(0), m_OTRef(0) {
   m_isOT = false;
   m_isIT = false;
   if (!itclus) return;
   if (!itdet) return;

   LHCb::STChannelID ITChan = itclus->channelID();
   m_station = itdet->findStation(ITChan)->id(); // IT runs from 1 to 5, OT from 1 to 3
   DeSTSector* sect = itdet->findSector(ITChan);
   if (!sect) return;
// Ultimately, the wafers will be detector elements and the orientation 
// and offsets can be found from them
// Currently, only DeSTDetector is a DetectorElement. Only the IT as a
// whole is shifted, I think

   m_x = sect->pitch()* itclus->interStripFraction(); // distance of cluster to
// central strip

   std::auto_ptr <LHCb::Trajectory> traj = sect->trajectory(ITChan,0.0);
   //if (!traj) return;
// direction of the strip
// the Trajectory method may be expected to have all offsets in it.
// I use it for the geometry.
// the only thing that is missing, is the direction of the active plane
// in which the wire/strip is laying. Therefore, I use the geometry() method 
// as well
   Gaudi::XYZVector dir=traj->direction(1.0);

   Gaudi::Transform3D trans = (sect->geometry())->toLocalMatrix();
   double xx,xy,xz,dx;
   double yx,yy,yz,dy;
   double zx,zy,zz,dz;
   trans.GetComponents(xx,xy,xz,dx,yx,yy,yz,dy,zx,zy,zz,dz);
// Now, without offsets, yx,yy,and yz should be equal to dir.x(), dir.y(), dir.z()
// have to check this in debugger

   TrackSeedPoint wire(dir.x(),dir.y(),dir.z());
// if the wire runs from up till down, mirror x and y
   // if (dir.y()<0) wire.scale(-1.0);
   TrackSeedPoint coord(xx,xy,xz);
   TrackSeedPoint plane(zx,zy,zz);
   TrackSeedPoint geowire(yx,yy,yz);
   TrackSeedPoint outprod = wire.outprod(geowire);
   TrackSeedPoint zero(0,0,0);
   if (outprod.dist(zero)>1e-10) {
// definitions of wire from detectorElement and Trajectory are not parallel!
// make new right-handed system; assuming that wire is parallel to the
// strip and that plane is almost perpendicular to the module
      coord=geowire.outprod(plane);
      coord.norm();
      plane=coord.outprod(geowire);
      plane.norm();
   }
       
// Position of center strip
   Gaudi::XYZPoint p0=traj->position(0.0);
   TrackSeedPoint orig(p0.x(),p0.y(),p0.z());
   TrackSeedPoint hit=coord;
   hit.scale(m_x);
   orig.translate(hit); // position center of cluster
   xoff[0]=orig.x();
   yoff[0]=orig.y();
   zoff[0]=orig.z();


// to be checked: is the stereo angle in the yx,yy,yz definition or not?
// rotate the y-axis with stereo angle to get it pointing in the direction of
// the strip

   double wl = sect->stripLength()/2.0;
   TrackSeedPoint end1=wire;
   end1.scale(wl);
   if (end1.y()>0) {
      m_ymin=yoff[0]-end1.y();
      m_ymax=yoff[0]+end1.y();
   } else {
      m_ymin=yoff[0]+end1.y();
      m_ymax=yoff[0]-end1.y();
   }

   // x-direction
   xoff[3]=xoff[5]=xoff[1]=xoff[0]-yoff[0]*wire.x()/wire.y();
   xoff[6]=xoff[4]=xoff[2]=wire.x()/wire.y();

   // y-direction
   if (fabs(wire.x())<1e-4) {
      yoff[6]=yoff[4]=yoff[2]=0;
      yoff[5]=yoff[3]=yoff[1]=yoff[0];
   } else {
      yoff[5]=yoff[3]=yoff[1]=yoff[0]-xoff[0]*wire.y()/wire.x();
      yoff[6]=yoff[4]=yoff[2]=wire.y()/wire.x();
   }
   // z-direction
   zoff[1]=plane.inprod(orig);
   zoff[2]=-plane.x();
   zoff[3]=-plane.y();
   for (unsigned int i=1; i<4; ++i) zoff[i]/=plane.z();

   m_error = SeedHit::ITResolution; // for the moment.
   m_ITRef = itclus;
   m_isIT = true;
   
   m_ymin -= SeedHit::Y_margin;
   m_ymax += SeedHit::Y_margin;
}


SeedHit::SeedHit(LHCb::OTTime* ott, DeOTDetector* otdet) : m_ITRef(0), m_OTRef(0) {
   m_isIT = m_isOT = false;
   if (!otdet) return;
   LHCb::OTChannelID chan = ott->channel();
   DeOTModule * mod = otdet->findModule(chan); 
   if (!mod) return;
   LHCb::LHCbID id(chan);
   std::auto_ptr<LHCb::Trajectory> traj = otdet->trajectory(id);
   Gaudi::XYZVector dir = traj->direction(0.0);
   TrackSeedPoint wire(dir.x(),dir.y(),dir.z());
   // if (dir.y()<0) wire.scale(-1); // negative y- wires run the other direction.
// in Seeding, I want one global coordinate system for all wires. Else, the
// z-direction is mirrored.

   m_station = mod->stationID();
   Gaudi::Transform3D trans = (mod->geometry())->toLocalMatrix();
   double xx,xy,xz,dx;
   double yx,yy,yz,dy;
   double zx,zy,zz,dz;
   trans.GetComponents(xx,xy,xz,dx,yx,yy,yz,dy,zx,zy,zz,dz);
   TrackSeedPoint plane(zx,zy,zz); // local z axis
   plane.norm();
//   if (zz<0) pl.scale(-1.0);
//   Gaudi::XYZPoint center = mod->centerOfStraw(chan.straw());
   Gaudi::XYZPoint center=traj->position(0.0);
   xoff[0]=center.x();
   yoff[0]=center.y();
   zoff[0]=center.z();
//   TrackSeedPoint wire(yx,yy,yz);
   wire.norm();
   TrackSeedPoint coord = wire.outprod(plane);
//   double wl = mod->wireLength()/2.0;
   double wl = traj->length()/2.0;

   m_OTRef = ott;
   m_error = SeedHit::OTResolution; // filled from FindSeed
 
   m_x = ott->calibratedTime(); // real drift distance needs to be calculated based on y-value
// already correct for drifttime along wire, add the value of 1/2 wire length
// 
   double maxtime=SeedHit::OT_cellradius/SeedHit::OT_driftspeed;

// calculate "new" wires that run through the drift-time cone

   double m_top = m_x;
   m_x -= wl*SeedHit::OT_wiredrifttime; // time for center wire, this should be stored
   double m_bot = m_x-wl*SeedHit::OT_wiredrifttime;
/*
   double m_top = m_x+wl*SeedHit::OT_wiredrifttime; // time for center wire, this should be stored
   double m_bot = m_x-wl*SeedHit::OT_wiredrifttime;
*/
   if (m_top>maxtime) {
      if (m_top>maxtime+20) m_error += 0.5; // add 0.5 mm for this hit
      else m_error+=(m_top-maxtime)*SeedHit::OT_driftspeed*0.5; // half the correction is added to the error 
      m_top=maxtime;
   }
   if (m_top<0) m_top=0;
   if (m_bot>maxtime) m_bot=maxtime;
   if (m_bot<0) {
       if (m_bot<-20) m_error+= 0.5;
       else m_error-=(m_bot)/2.0*SeedHit::OT_driftspeed; // half the correction
       m_bot=0;
   }
   m_top*=SeedHit::OT_driftspeed;
   m_bot*=SeedHit::OT_driftspeed;
   double y_top = yoff[0]+wl*wire.y();
   double y_bot = yoff[0]-wl*wire.y();
   double x_top = xoff[0]+wl*wire.x();
   double x_bot = xoff[0]-wl*wire.x();
   xoff[6]=(x_top-x_bot)/(y_top-y_bot);
   xoff[5]=x_bot-y_bot*xoff[6];
   bool y_slope = (fabs(wire.x())>0.01); // only calculate y-positions if wires are rotated appreciably
   if (y_slope) {
      yoff[6]=(y_top-y_bot)/(x_top-x_bot);
      yoff[5]=y_top-x_top*yoff[6];
   } else {
      yoff[1]=yoff[3]=yoff[5]=yoff[0];
      yoff[2]=yoff[4]=yoff[6]=0;
   }
   y_top+=m_top*coord.y(); // add hit in "positive" direction
   x_top+=m_top*coord.x();
   y_bot+=m_bot*coord.y();
   x_bot+=m_bot*coord.x();
   xoff[2]=(x_top-x_bot)/(y_top-y_bot);
   xoff[1]=x_bot-y_bot*xoff[2];
   if (y_slope) {
      yoff[2]=(y_top-y_bot)/(x_top-x_bot);
      yoff[1]=y_top-x_top*yoff[2];
   }
   y_top-=2*m_top*coord.y(); // add hit in "negative" direction
   x_top-=2*m_top*coord.x();
   y_bot-=2*m_bot*coord.y();
   x_bot-=2*m_bot*coord.x();
   xoff[4]=(x_top-x_bot)/(y_top-y_bot);
   xoff[3]=x_bot-y_bot*xoff[4];
   if (y_slope) {
      yoff[4]=(y_top-y_bot)/(x_top-x_bot);
      yoff[3]=y_top-x_top*yoff[4];
   }

// add z-offsets
   zoff[1]=xoff[0]*plane.x()+yoff[0]*plane.y()+zoff[0]*plane.z();
   zoff[1]/=plane.z();
   zoff[2]=-plane.x()/plane.z();
   zoff[3]=-plane.y()/plane.z();
   

   m_isOT=true;
   m_ymax=m_ymin = yoff[0];
   if (wire.y()>0) {
       m_ymax+=wire.y()*wl;
       m_ymin-=wire.y()*wl;
   } else {
       m_ymax-=wire.y()*wl;
       m_ymin+=wire.y()*wl;
   }
   
   m_ymin -= SeedHit::Y_margin;
   m_ymax += SeedHit::Y_margin;
}
