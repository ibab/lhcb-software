
#ifndef LESTER_TESTFUNCTIONS_H
#define LESTER_TESTFUNCTIONS_H

//#include "CLHEP/Matrix/Vector.h"
//#include "Utils/NormalDistribution.h"
#include "LinAlg/RN.h"
#include "Utils/MathsConstants.h"

extern "C" {
#include <pam.h>
};



struct TestFunctions {


typedef enum {
    flat,
    mandel,
    blobs, 
    damaris, 
    waves1,
    waves2,
    waves3,
    waves4,
    mrDelaunay,
    mrDelaunayCyclic,
    integrableSingularity1,
    integrableSingularity2,
    integrableSingularity3
  } Type;

double function(double x, double y, Type type) {
  const double conversion=6;

  if (type==flat) {
    return 1;
  } else if (type==integrableSingularity1) {
    x=x*conversion-1;
    y=y*conversion-1;
    const double dx=x-2;
    const double dy=y-2;
    const double r = sqrt(dx*dx+dy*dy);
    const double ans = pow(r,-1.8); // close to the worst case!
    return ans;
  } else if (type==integrableSingularity2) {
    x=x*conversion-1;
    y=y*conversion-1;
    const double dx=x-2;
    const double dy=y-2;
    const double r = sqrt(dx*dx+dy*dy);
    const double ans = pow(r,-0.9);
    return ans;
  } else if (type==integrableSingularity3) {
    x=x*conversion-1;
    y=y*conversion-1;
    const double dx1=x-1;
    const double dy1=y-3;
    const double dx2=x-3;
    const double dy2=y-1;
    const double r1 = sqrt(dx1*dx1+dy1*dy1);
    const double r2 = sqrt(dx2*dx2+dy2*dy2);
    const double ans = pow(r1,-1.2)+ pow(r2,-1.2);
    return ans;
  } else if (type==mrDelaunay) {
    if (x<=0 || x>=1 || y<=0 || y>=1) {
      return 0;
    } else {
      return function(x,y,mrDelaunayCyclic);
    };
  } else if (type==mrDelaunayCyclic) {
    static struct pam inpam;
    static unsigned int width;
    static unsigned int height;
    static tuple ** arr;
    static bool first=true;
    if (first) {
      first=false;
      FILE * file = fopen("/usera/lester/proj/c++/Utils/images/Delaunay.pgm", "r");
      assert(file);
      //pnm_readpaminit(file, &inpam, sizeof(inpam));
      arr = pnm_readpam(file, &inpam, sizeof(inpam));//arr = pnm_allocpamarray(&inpam);
      height = inpam.height;
      width = inpam.width;
      //      arr = pnm_readpam(file, &inpam, sizeof(inpam));//arr = pnm_allocpamarray(&inpam);
      //pnm_writepam(&inpam, arr);
      
      assert(arr);
      //close(file);
    };
    
    unsigned int row = static_cast<int>((1.-y)*height+4*height)%height;
    unsigned int col = static_cast<int>(x*width)%width;
    if (row<=0 || row>=height || col<=0 || col>= width) {
      //std::cout << "K K K K K K  " << 0 <<" " << width << " " << height << std::endl;
      return 0;
    } else {
      const double ans = 256-arr[row][col][0];
      //std::cout << "KKKKKKKKKKK  " << ans <<" " <<width << " " << height << std::endl;
      return ans;
    };

  } else if (type==blobs) {
    x=x*conversion-1;
    y=y*conversion-1;
  static const double wid1=0.5;
  static const double wid2=0.5;
  static const double norm1=3;
  static const double norm2=3;
  //static const NormalDistribution xDist1(1,wid1);
  //static const NormalDistribution yDist1(1,wid1);
  //static const NormalDistribution xDist2(3,wid2);
  //static const NormalDistribution yDist2(4,wid2);
  const double delta1Sq=((x-1)*(x-1)+(y-1)*(y-1))/(wid1*wid1);
  const double delta2Sq=((x-3)*(x-3)+(y-4)*(y-4))/(wid2*wid2);
  const double ans1 = norm1/sqrt(MathsConstants::twoPi)/wid1*exp(-0.5*delta1Sq);
  const double ans2 = norm2/sqrt(MathsConstants::twoPi)/wid2*exp(-0.5*delta2Sq);
  const double ans=ans1+ans2;
  return ans;
  } else if (type==mandel) {
    x=x*conversion-1;
    y=y*conversion-1;
    const unsigned int maxSteps=100;
    unsigned int step=0;
    const double lx=-0.25*(x-3);
    const double ly=0.25*(y);
    double zr=0;
    double zi=0;
    while(step<maxSteps && zr*zr+zi*zi<4.) {
      const double tmp = zr*zr-zi*zi + lx;
      zi = 2.*zr*zi+ly;
      zr=tmp;
      step++;
    };
    //if (step==maxSteps) {
    //  return 0.1;
    //} else {
    //      return ((lx+0.3)*(lx+0.3)+ly*ly)*exp(step/10.);
      return exp(step/10.);
    //};
  } else if (type==damaris) {
    // A damaris D
    x=x*conversion-1;
    y=y*conversion-1;
    const double r0=3;
    const double w=0.3;
    const double cx=1;
    const double cy=3;
    const double dx=x-cx;
    const double dy=y-cy;
    const double theta=atan2(dy,dx);
    if (theta<3.14159/2&&theta>-3.14159/2) {
      const double r=sqrt(dx*dx+dy*dy);
      return 1./sqrt(2.*3.14159*w*w)*exp(-0.5*(r-r0)*(r-r0)/w/w);
    } else {
      if (fabs(dy)<r0) {
	return 1./sqrt(2.*3.14159*w*w)*exp(-0.5*dx*dx/w/w);
      } else {
	return 0.000000000000000001;
      };
    };
  } else if (type==waves1) {
    x=x*conversion-1;
    y=y*conversion-1;
    const double dx=x;
    const double dy=y;
    const double r=sqrt(dx*dx+dy*dy);
    const double c = cos(1.5*r);
    return pow(c,16);
  } else if (type==waves2) {
    x=x*conversion-1;
    y=y*conversion-1;
    const double dx=(x-4.)*2.;
    const double dy=(y-4.)*2.;
    const double r=sqrt(dx*dx+dy*dy);
    const double c = cos(1.5*r);
    return pow(c,32);
  } else if (type==waves3) {
    x=x*conversion-1;
    y=y*conversion-1;
    const double d1x=(x-4.)*2.;
    const double d1y=(y-4.)*2.;
    const double r=sqrt(d1x*d1x+d1y*d1y);
    const double c = cos(1.5*r);
    const double d2x=x;
    const double d2y=y;
    const double r2=sqrt(d2x*d2x+d2y*d2y)-4.*sqrt(2.);
    const double c2 = exp(-r2*r2/0.01);
    return pow(c,32)+c2;
  } else if (type==waves4) {
    x=x*conversion-1;
    y=y*conversion-1;
    const double d1x=(x-4.)*2.;
    const double d1y=(y-4.)*2.;
    const double r=sqrt(d1x*d1x+d1y*d1y);
    const double c = cos(1.5*r);
    const double d2x=x*2.;
    const double d2y=y*2.;
    const double r2=sqrt(d2x*d2x+d2y*d2y);
    const double c2 = cos(1.5*r2);;
    return pow(c,32)+pow(c2,32);
 };

 
 return 0;
};


};




#endif
