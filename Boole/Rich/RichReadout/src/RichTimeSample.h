
#ifndef RICHREADOUT_RICHTIMESAMPLE_H
#define RICHREADOUT_RICHTIMESAMPLE_H 1

#include <vector>
#include "RichPixel.h"

class RichTimeSample {

public:

  RichTimeSample() : sz(0) {};

  RichTimeSample( const RichPixel* id, int isize, double value)
    : mypixel   ( id ),
      TimeFrame ( isize, value ),
      sz        ( isize ) { }

  ~RichTimeSample(){};

  const std::vector<double> & TimeSample() const
  {
    return TimeFrame;
  }

  void insert(unsigned int i, double value)
  {
    TimeFrame[i] = value;
  }

  unsigned int size() const
  {
    return sz;
  }

  double& operator[](unsigned int i)
  {
    return TimeFrame[i];
  }

  const double& operator[](unsigned int i) const
  {
    return TimeFrame[i];
  }

  RichTimeSample& operator *= ( double c )
  {
    for ( unsigned int i = 0; i < size(); ++i ) { TimeFrame[i] *= c; }
    return *this;
  }
  
  /*
  RichTimeSample& operator += ( const RichTimeSample & ts )
  {
    if ( size() == ts.size() ) {
      for ( unsigned int i=0; i < size(); ++i ) { 
        TimeFrame[i] += ts.TimeFrame[i];
      }
    }
    return *this;
  }
  */

  RichTimeSample operator << ( unsigned int n )
  {
    unsigned int i;
    RichTimeSample ts(*this);
    for ( i = n; i < size(); ++i ) { ts.TimeFrame[n] = ts.TimeFrame[i-n]; }
    if ( n > size() ) n = size();
    for ( i = 0; i < n; ++i ){ ts.TimeFrame[i] = 0.0; }
    return ts;
  }

  bool operator<(const RichTimeSample&d) const
  {
    return ( MyPixel() < d.MyPixel() );
  }

  bool operator==(const RichTimeSample&d) const
  {
    return ( MyPixel() == d.MyPixel() );
  }

  void reset()
  {
    for ( unsigned int i = 0; i < size(); ++i ) { TimeFrame[i] = 0; }
  }

  const RichPixel* MyPixel() const
  {
    return mypixel;
  }

private:

  const RichPixel * mypixel;
  std::vector<double> TimeFrame;
  unsigned int sz;

};

#endif // RICHREADOUT_RICHTIMESAMPLE_H
