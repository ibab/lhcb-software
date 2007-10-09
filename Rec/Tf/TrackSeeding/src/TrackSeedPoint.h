#ifndef TrackSeedPoint_h
#define TrackSeedPoint_h
/* TrackSeedPoint: created 13-12-2005 by H.J. Bulten
   it is just a simple helper class to do calculations with 3d coordinates.
   probably there is a class that does it already but I do not want to look 
   for it right now.
*/
#include "math.h"

class TrackSeedPoint {
   public:
      friend class TrackSeedPlaneDef;
      TrackSeedPoint(const double& x=0, const double& y=0, const double& z=0) {
         xx=x;
         yy=y;
         zz=z;
      };
// default copy and assignment constructors, default destructor

      inline double lenght() const { return sqrt(xx*xx+yy*yy+zz*zz);}; // returnlenght

      void norm() { // make a unit vector 
	  double len = lenght();
          if (len>0) scale(1/len);
      }; 

      inline double x() const {return xx;};
      inline double y() const {return yy;};
      inline double z() const {return zz;};
      inline double dist(const TrackSeedPoint& p) const { 
// distance to point p
        return sqrt((xx-p.xx)*(xx-p.xx)+(yy-p.yy)*(yy-p.yy)+(zz-p.zz)*(zz-p.zz));
      };
      inline double inprod(const TrackSeedPoint& p) const { // inproduct
        return (xx*p.xx+yy*p.yy+zz*p.zz);
      };

      TrackSeedPoint direction(const TrackSeedPoint& p) const {
// returns the normalized vector that points from the current point in the 
// direction of point p
// returns 0,0,0 if the distance between the points is zero
         double d = this->dist(p);
	 if (d<1e-10) return TrackSeedPoint(0,0,0);
	 return TrackSeedPoint((p.xx-xx)/d,(p.yy-yy)/d,(p.zz-zz)/d);
      };

      TrackSeedPoint outprod(const TrackSeedPoint& p) const {
         return TrackSeedPoint((yy*p.zz-zz*p.yy),(zz*p.xx-xx*p.zz),(xx*p.yy-yy*p.xx));
      };

      inline void scale(const double& scal) {
         xx*=scal;
	 yy*=scal; 
         zz*=scal;
      };

      inline void translate(const TrackSeedPoint& p) {
	  xx+=p.xx;
	  yy+=p.yy;
	  zz+=p.zz;
      };
        

      private:
        double xx;
	double yy;
	double zz;
};
	    
#endif
