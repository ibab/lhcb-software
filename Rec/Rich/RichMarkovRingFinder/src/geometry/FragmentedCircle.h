
#ifndef RICHMARKOV_FRAGMENTED_CIRCLE_H
#define RICHMARKOV_FRAGMENTED_CIRCLE_H

#include <list>
#include <cassert>
#include <iostream>
#include "CLHEP/Vector/TwoVector.h"
#include "MathsConstants/MathsConstants.h"

namespace RichMarkov {
  
  //fwd dec
  class Canvas;

  class FragmentedCircle {

  public:

      class ArcPart {
      private:
	const bool _whole; // true if arc-part is a "whole circle" 
	const double _thetaStart;
	const double _phi; 
      public:
	std::ostream & printMeTo(std::ostream & os) const;
      public:
	inline ArcPart(const ArcPart & ap) : 
	  _whole(ap._whole),
	  _thetaStart(ap._thetaStart),
	  _phi(ap._phi) {
	};
	inline ArcPart() :
	  // define a complete-circle arc-part
	  _whole(true),
	  _thetaStart(0),
	  _phi(0) {
	};
	inline ArcPart(const double thetaStart, const double phi) :
	  // define an incomplete-circle arc
	  _whole(false),
	  _thetaStart(regularisedAngle(thetaStart)),
	  _phi(regularisedAngle(phi)) {
	};
	inline bool isWhole() const { 
	  return _whole; 
	};
	inline double getThetaStart() const {
	  assert(!_whole);
	  return _thetaStart; 
	};
	inline double getPhi() const { 
	  assert(!_whole);
	  return _phi; 
	};
	inline double getTheta1() const { 
	  assert(!_whole);
	  return _thetaStart; 
	};
	inline double getTheta2() const { 
	  assert(!_whole);
	  return _thetaStart+_phi; 
	};
	inline double subtendedAngle() const {
	  if (_whole) {
	    return MathsConstants::twopi;
	  } else {
	    return getPhi();
	  };
	};
      public:
	static inline double regularisedAngle(const double a) {
	  double ans = a;
	  while (ans >= MathsConstants::twopi) {
	    ans -= MathsConstants::twopi;
	  };
	  while (ans < 0) {
	    ans += MathsConstants::twopi;
	  }; 
	  return ans;
	}; 
      }; // end of class ArcPart
      
    // class FragmentedCircle continues
  private:
    typedef std::list<ArcPart> ArcPartList;
    const Hep2Vector _centre;
    const double _radius;
    ArcPartList _arcPartList;
  public:
    inline const Hep2Vector & getCentre() const {
      return _centre;
    };
  public:
    void drawOn(Canvas & canvas) const;
  public:
    FragmentedCircle(const Hep2Vector & centre, const double radius) :
      _centre(centre), 
      _radius(radius),
      _arcPartList() {	  
      //std::cerr << "GGGGGGsdfsfdGGGSDYFSJDF " <<_centre.x() << " " << _centre.y() << std::endl;

      // initially defuine a "complete" circle:
      _arcPartList.push_back(ArcPart());
    };
  public:
    inline std::ostream & printMeTo(std::ostream & os) const;
  private:
    inline void privateNamelessMethod(const ArcPart & ap, 
				      ArcPartList::iterator curit, 
				      const double intersectionThetaStart,
				      const double phi) {
      if (ap.isWhole()) {
	// replacement arc is just as above
	_arcPartList.erase(curit);
	_arcPartList.push_front(ArcPart(intersectionThetaStart, phi));
      } else {
	// find intersection
	const double cut1 = intersectionThetaStart;
	//const double cut2 = cut1 + phi;
	const double present1 = ap.getTheta1();
	const double present2 = ap.getTheta2();
	assert(cut1+phi>=cut1);
	assert(present2>=present1);
	
	// rotate arcs by subtracting present1 from them so as to set rpresent1 to zero
	const double rcut1 = ArcPart::regularisedAngle(cut1-present1);
	const double rcut2 = rcut1 + phi; //cut2-present1;
	const double rpresent1 = 0; // == present1-present1; 
	const double rpresent2 = present2-present1;
	
	
	if (rcut2>=MathsConstants::twopi) {
	  //possibly two zones
	  _arcPartList.erase(curit);
	  
	  // top zone:
	  {
	    const double rnew1 = rcut1; // since it must be >= rpresent1 since rpresent1==0;
	    const double rnew2 = rpresent2;
	    const double size = rnew2-rnew1;
	    if (size>0) {
	      const double start = rnew1 + present1; // puts back the subtracted angle!
	      _arcPartList.push_front(ArcPart(start, size)); // must push to front to avoid clashes with subsequent iterations through the list.
	    };
	  };
	  
	  // bot zone:
	  {
	    const double rnew1 = rpresent1; // ==0
	    const double wrap = ArcPart::regularisedAngle(rcut2);
	    const double rnew2 = (wrap<rpresent2)?wrap:rpresent2;
	    const double size = rnew2-rnew1;
	    if (size>0) {
	      const double start = rnew1 + present1; // puts back the subtracted angle!
	      _arcPartList.push_front(ArcPart(start, size)); // must push to front to avoid clashes with subsequent iterations through the list.
	    };
	  };
	  
	} else {
	  // no more than one zone
	  const double rnew1 = rcut1; // since it must be >= rpresent1 since rpresent1==0;
	  const double rnew2 = (rcut2<rpresent2)?rcut2:rpresent2;
	  const double size = rnew2-rnew1;
	  if (size<=0) {
	    // the arc disappeared altogether
	    _arcPartList.erase(curit);
	  } else {
	    const double start = rnew1 + present1; // puts back the subtracted angle!
	    _arcPartList.erase(curit);
	    _arcPartList.push_front(ArcPart(start, size));
	  };
	};
      }; 
    };
  public:
    inline void leftClip(const double x) {
      // clips away all portions of arcs falling to the left of x, splitting arc-parts into more than one sub arc-part if necessary
      const double delta = x - _centre.x();
      if (delta <= -_radius) {
	// everything will remain
      } else if (delta >=_radius) {
	// everything will die
	_arcPartList.clear();
      } else {
	//Some things will die and some live ...
	for (ArcPartList::iterator it = _arcPartList.begin();
	     it != _arcPartList.end();
	     ) {
	  const ArcPartList::iterator curit = (it++);
	  const ArcPart & ap = *curit;
	  
	  const double cosPhiOnTwo = delta/_radius;
	  const double phiOnTwo = acos(cosPhiOnTwo);
	  assert(phiOnTwo>=0 && phiOnTwo<=MathsConstants::pi);
	  const double two = 2;
	  const double phi = phiOnTwo * two;
	  const double intersectionThetaStart = -phiOnTwo; // not regularised
	  
	  privateNamelessMethod(ap, curit, intersectionThetaStart, phi);
	};
      };
    }; 
    inline void rightClip(const double x) {
      // clips away all portions of arcs falling to the right of x, splitting arc-parts into more than one sub arc-part if necessary
      const double delta = x - _centre.x();
      if (delta >= _radius) {
	// everything will remain
      } else if (delta <= -_radius) {
	// everything will die
	_arcPartList.clear();
      } else {
	//Some things will die and some live ...
	for (ArcPartList::iterator it = _arcPartList.begin();
	     it != _arcPartList.end();
	     ) {
	  const ArcPartList::iterator curit = (it++);
	  const ArcPart & ap = *curit;
	  
	  const double cosPhiOnTwo = delta/_radius;
	  const double phiOnTwo = acos(cosPhiOnTwo);
	  assert(phiOnTwo>=0 && phiOnTwo<=MathsConstants::pi);
	  const double two = 2;
	  const double phi = phiOnTwo * two;
	  const double intersectionThetaStart = phiOnTwo; // not regularised
	  
	  privateNamelessMethod(ap, curit, intersectionThetaStart, MathsConstants::twopi - phi);
	};
      };
    }; 
    inline void bottomClip(const double y) {
      // clips away all portions of arcs falling below y, splitting arc-parts into more than one sub arc-part if necessary
      const double delta = y - _centre.y();
      if (delta <= -_radius) {
	// everything will remain
 	//std::cerr << "keep all Cos " << delta << " and "<< _radius << std::endl;
	// do nothing
     } else if (delta >=_radius) {
	// everything will die
	_arcPartList.clear();
	//std::cerr << "KILL ALL Cos " << delta << " and "<< _radius << std::endl;
      } else {
	//std::cerr << "Yummy Cos " << delta << " and "<< _radius << std::endl;
	//Some things will die and some live ...
	for (ArcPartList::iterator it = _arcPartList.begin();
	     it != _arcPartList.end();
	     ) {
	  const ArcPartList::iterator curit = (it++);
	  const ArcPart & ap = *curit;
	  
	  const double cosTopAngOnTwo = delta/_radius;
	  const double topAngOnTwo = acos(cosTopAngOnTwo);
	  const double intersectionThetaStart = MathsConstants::piontwo-topAngOnTwo; // not regularised
	  const double two = 2;
	  const double phi = two*topAngOnTwo;

	  assert(phi>=0);
	  
	  privateNamelessMethod(ap, curit, intersectionThetaStart, phi);
	};
      };
    }; 
    inline void topClip(const double y) {
      // clips away all portions of arcs falling below y, splitting arc-parts into more than one sub arc-part if necessary
      const double delta = y - _centre.y();
      if (delta >= _radius) {
	// everything will remain
      } else if (delta <= -_radius) {
	// everything will die
	_arcPartList.clear();
      } else {
	//Some things will die and some live ...
	for (ArcPartList::iterator it = _arcPartList.begin();
	     it != _arcPartList.end();
	     ) {
	  const ArcPartList::iterator curit = (it++);
	  const ArcPart & ap = *curit;
	  
	  const double cosTopAngOnTwo = delta/_radius;
	  const double topAngOnTwo = acos(cosTopAngOnTwo);
	  const double intersectionThetaStart = MathsConstants::piontwo+topAngOnTwo; // not regularised
	  const double two = 2;
	  const double phi = fabs(MathsConstants::twopi-two*topAngOnTwo); // the fabs is just there to protect against rounding errors .. the stuff inside SHOULD be positive already

	  assert(phi>=0);
	  
	  privateNamelessMethod(ap, curit, intersectionThetaStart, phi);
	};
      };
    }; 

    
    inline double totalSubtendedAngle() const {
      double ans = 0;
      for (ArcPartList::const_iterator it = _arcPartList.begin();
	   it != _arcPartList.end();
	   ++it) {
	ans += it->subtendedAngle();
      };
      return ans;
    };

    inline double totalArcLength() const {
      return totalSubtendedAngle()*_radius;
    };
    
  }; 
    
}

inline std::ostream & operator << (std::ostream & os, const RichMarkov::FragmentedCircle::ArcPart & ap) {
  return ap.printMeTo(os);
};

inline std::ostream & operator << (std::ostream & os, const RichMarkov::FragmentedCircle & fc) {
  return fc.printMeTo(os);
};

#endif
