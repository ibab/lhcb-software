#ifndef RICHDATAARRAYDEFINITIONS
#define RICHDATAARRAYDEFINITIONS

#include <vector>
#include <valarray>
#include <GaudiKernel/Bootstrap.h>

typedef std::vector<double> VD;
typedef std::vector<int> VI;
typedef std::vector<std::vector<double> > VVD;
typedef std::vector<std::vector<int> > VVI;
typedef std::vector<std::vector<std::vector<double> > >VVVD;
typedef std::vector<std::vector<std::vector<int> > >VVVI;
typedef std::vector<Gaudi::XYZPoint>  VC;
typedef std::vector<std::vector<Gaudi::XYZPoint> > VVC;
typedef const Gaudi::XYZPoint & CD;
typedef std::vector<std::string> VS;
typedef std::vector<std::vector<std::string> > VVS;
typedef std::vector<bool>  VB;
typedef std::vector<std::vector<bool> >  VVB;



typedef std::valarray<double> AD;
typedef std::valarray<int> AI;


typedef std::vector<double>::iterator VD_it;
typedef std::vector<int>::iterator VI_it;

#endif
