/*
 * Class TrackCand
 * author H.J. Bulten
 * Feb 21, 2006
 *
 * helper class; basically just a structure, to keep track of intermediate
 * hits in the seeding
 *
*/ 
#include "TrackSeeding/TrackCand.h"

void TrackCand::reset() {
   resetX();
   resetY();
}

void TrackCand::resetX() {
 // clear X arrays
   xhits.clear();
   x.clear();
   x_error.clear();
   z_x.clear();
   xfitpars.clear();
   skipped_x.clear();
}
   
void TrackCand::resetY() {
 // clear Y arrays
   yhits.clear();
   y.clear();
   y_error.clear();
   z_y.clear();
   yfitpars.clear();
   skipped_y.clear();
}
   
void TrackCand::xHits(std::vector<TrackSeedHit>& vec) {
   // return non-skipped hits in vec
   vec.clear();
   for (unsigned int i=0; i<xhits.size(); ++i) {
       bool skip=false;
       for (unsigned int j=0; (j<skipped_x.size()) ; ++j) {
           skip=(skipped_x[j]==i);
           if (skip) break;
       }
       if (!skip) vec.push_back(xhits[i]);
   }
}

void TrackCand::yHits(std::vector<TrackSeedHit>& vec) {
   // return non-skipped hits in vec
   vec.clear();
   for (unsigned int i=0; i<yhits.size(); ++i) {
       bool skip=false;
       for (unsigned int j=0; (j<skipped_y.size()) ; ++j) {
          skip=(skipped_y[j]==i);
          if (skip) break;
       }
       if (!skip) vec.push_back(yhits[i]);
   }
}

void TrackCand::xParams(std::vector<double>& vec) {
   // return fit parameters
   vec.resize(xfitpars.size());
   for (unsigned int i=0; (i<xfitpars.size()); ++i) vec[i]=xfitpars[i];
}

void TrackCand::yParams(std::vector<double>& vec) {
   // return fit parameters
   vec.resize(yfitpars.size());
   for (unsigned int i=0; (i<yfitpars.size()); ++i) vec[i]=yfitpars[i];
}

void TrackCand::deleteYHit(unsigned int hnum) {
// the hit in the array with index hitnum should be deleted
// this is done by storing its index in the vectors of hits
// (y, yhits, yerror etc) in the vector skipped_y. 
// note, deleteYhit is called from SeedFind after a call to Ycands; hence the
// index give to it is not the index of the array, but the index taking into
// account the skipped numbers
   int hitnum = hnum; // needs to be signed int
   unsigned int skipped=skipped_y.size();
   if (skipped<1) {
      skipped_y.push_back(hnum);
      return;
   }
   if (skipped==1) {
      if (skipped_y[0]>hnum) skipped_y.push_back(hnum);
      else {
        skipped_y.push_back(++hnum);
      }
      return;
   }
   unsigned int index=0;
   while (hitnum > -1) {
      for (unsigned int k=0; k<skipped; ++k) {
         if (skipped_y[k]==index) {
            ++hitnum; // this is an array element that has already been skipped
            break;
         }
      }
      --hitnum; // lower hitnum for every correct index
      ++index; // check next index
   }
   --index; // last additional increment was one too many.  
   skipped_y.push_back(index);
}

void TrackCand::deleteXHit(unsigned int hnum) {
// the hit in the array with index hitnum should be deleted
// this is done by storing its index in the vectors of hits
// (y, yhits, yerror etc) in the vector skipped_x.
// note, deleteYhit is called from SeedFind after a call to Ycands; hence the
// index give to it is not the index of the array, but the index taking into
// account the skipped numbers
   int hitnum = hnum;
   unsigned int skipped=skipped_x.size();
   if (skipped<1) {
      skipped_x.push_back(hnum);
      return;
   }
   if (skipped==1) {
      if (skipped_x[0]>hnum) skipped_x.push_back(hnum);
      else skipped_x.push_back(++hnum);
      return;
   }
   unsigned int index=0;
   while (hitnum > -1) {
      for (unsigned int k=0; k<skipped; ++k) {
         if (skipped_x[k]==index) {
            ++hitnum;
            break;
         }
      }
      ++index;
      --hitnum;
   }
   skipped_x.push_back(--index);
}

/*
this implementation did not take care of the order in which the hits
are removed.

void TrackCand::deleteYHit(unsigned int hitnum) {
   for (unsigned int j = 0; j<skipped_y.size(); j++) 
      if (skipped_y[j] <= hitnum) hitnum++;
   if (yhits.size() > hitnum) skipped_y.push_back(hitnum);
}
*/

void TrackCand::addXHit(const TrackSeedHit& hit, double xpos, double zpos, double error) {
   xhits.push_back(hit);
   x.push_back(xpos);
   z_x.push_back(zpos);
   x_error.push_back(error);
}

void TrackCand::addYHit(const TrackSeedHit& hit, double ypos, double zpos, double error) {
   yhits.push_back(hit);
   y.push_back(ypos);
   z_y.push_back(zpos);
   y_error.push_back(error);
}

double TrackCand::refitXHits(bool linefit) {
// see what the x-z angle is from the previous fit; if any
   double ang=0;
   if (xfitpars.size()>3) ang=xfitpars[3];
   if (!x_linefit) ang=xfitpars[1]+xfitpars[2]*17000; // angle at 8500 mm from origin 
   ang=cos(ang);

// store what the fit parameters contain: parabola or line
   x_linefit=linefit;

// when y is known, X may be refitted
   if (yfitpars.size()>4) {
      double spread = yfitpars[2]+800*yfitpars[4]; // slight underestimate
   // y fit parameters are known. refit X with these parameters
   // since this may be the first time that the track is refitted, check if
   // the x-hit is compatible with the y at that z and update the errors.
      for (unsigned int i=0; i<xhits.size(); ++i) {
         bool skip=false;
         for (unsigned int j=0; j<skipped_x.size(); ++j) if (skipped_x[j]==i) skip=true;
	 if (skip) continue;
         double newy=yfitpars[1]+(z_x[i]-yfitpars[0])*yfitpars[3];
         if (newy<xhits[i].y_min()-spread) skipped_x.push_back(i);
         else if (newy>xhits[i].y_max()+spread) skipped_x.push_back(i);
// xhits[i].x(newy) is the position of the wire at height y
         x[i]=xhits[i].x(newy);
// correct for cosine of flight trajectory. The closest distance to the wire for an OT track does not occur at
// the z-position of the wire. there, the track is 1/cos(x-angle) further away than the measured drift time
// for the IT, there is no such correction since the cluster is in the sensor at fixed z
         if (xhits[i].isOT()) x[i]+=(xhits[i].x(xhits[i].ambiguity(),newy)-xhits[i].x(newy))/ang;
         z_x[i]=xhits[i].z(x[i],newy);
// reset errors. 
// the error may have been 1 mm to take into account the uncertainty in y
         x_error[i]=xhits[i].error(); 
      }
   }
   unsigned int skipped_size = skipped_x.size();
   if (linefit) {
      double chi2=TrackSeedHit::line_fit(z_x,x,x_error,xfitpars,skipped_x);
      while ((skipped_x.size() > skipped_size) && chi2>0) {
         skipped_size=skipped_x.size();
         chi2=TrackSeedHit::line_fit(z_x,x,x_error,xfitpars,skipped_x);
      }
      return chi2;
   }
   double chi2=TrackSeedHit::parabola_fit(z_x,x,x_error,xfitpars,skipped_x);
   while ((skipped_x.size() > skipped_size) && chi2>0) {
      skipped_size=skipped_x.size();
      chi2=TrackSeedHit::parabola_fit(z_x,x,x_error,xfitpars,skipped_x);
   }
   return chi2;
}

double TrackCand::refitYHits() {
   if (yhits.size() == 2) return TrackSeedHit::line_fit(z_y,y,y_error,yfitpars,skipped_y);
/* since the case of 2 hits in SeedFind is performed so often, I call it
  * without any tests. The line fit for the two trial points in Y1 and Y3 is
  * called millions of times per event. 
  * performing the usual tests and resizing the error etc would lead to a CPU
  * performance that is up to a factor 2 worse.
*/

   // extra Y-hits present.
   // since this may be the first time that the track is refitted, check if
   // the x-hit is compatible with the y at that z and update the positions and errors.

// first calculate the possibly large angle in the x-z plane. For the OT, the measured drift time where the track is
// closest to the wire is not at the z-position of the wire. at this z-position, the track is 1/cos(xang) further away
// For the IT, there is no such correction necessary, since the clusters lay in
// the center of the sensor (so the z of the cluster is "fixed").
   double xang=xfitpars[3];
   if (!x_linefit) xang=xfitpars[1]+17000*xfitpars[2]; // angle at z= 8500
   xang=cos(xang);
   for (unsigned int i=0; i<yhits.size(); ++i) {
      bool skip=false;
      for (unsigned int j=0; j<skipped_y.size(); ++j) if (skipped_y[j]==i) skip=true;
      if (skip) continue;
      double newx;
      if (x_linefit) newx=xfitpars[1]+(z_y[i]-xfitpars[0])*xfitpars[3];
      else newx=xfitpars[0]+z_y[i]*(xfitpars[1]+z_y[i]*xfitpars[2]);
      y[i]=yhits[i].y(newx);
      if (yhits[i].isOT()) y[i]+=(yhits[i].y(yhits[i].ambiguity(),newx)-yhits[i].y(newx))/xang;
      
      y_error[i]=yhits[i].error()*yhits[i].error();
      if (x_linefit) y_error[i]+= (xfitpars[2]*xfitpars[2]+xfitpars[4]*xfitpars[4]*(xfitpars[0]-z_y[i])*(xfitpars[0]-z_y[i]));
      else y_error[i] += xfitpars[3]*2; // factor of two is for the additional uncertainty in slope.
      y_error[i] = sqrt(y_error[i])/(fabs(yhits[i].tanY())+1e-8);
      z_y[i]=yhits[i].z(newx,y[i]);
   }
   unsigned int skipped_size = skipped_y.size();
   double chi2=TrackSeedHit::line_fit(z_y,y,y_error,yfitpars,skipped_y);
   while ((skipped_y.size() > skipped_size) && chi2>0) {
      skipped_size=skipped_y.size();
      chi2=TrackSeedHit::line_fit(z_y,y,y_error,yfitpars,skipped_y);
   }
   return chi2;
}
