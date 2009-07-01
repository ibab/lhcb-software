// $Id: Interpolator.h,v 1.1 2009-07-01 18:27:11 polye Exp $
#ifndef INTERPOLATOR_H 
#define INTERPOLATOR_H 1

#include <vector>

// Include files

/** @class interpolator interpolator.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-10
 */
class interpolator {
public: 
  /// Standard constructor
  interpolator(const double x1,const double x2,const double y1,const double y2 ) {
    //get point and slope for the line
    a= y2-x2*(y2-y1)/(x2-x1);
    b=(y2-y1)/(x2-x1);
  }

  virtual ~interpolator( ) {}; ///< Destructor

  //return corresponding value
  double value(const double x) {return a+b*x;}
  
protected:

  double a;
  double b;  

private:

};


class Uniformer {
public: 
  /// Standard constructor
  Uniformer(const std::vector<double>& xpoints_in,const std::vector<double>& ypoints_in); 

  virtual ~Uniformer( ) {}; ///< Destructor
  int findBin(const double& xval);
  double getYvalue(const double& xval);

protected:
  std::vector<double> m_xpoints;
  std::vector<double> m_ypoints;
  std::vector<interpolator> m_uniformer;
  int m_npoints;
  int m_nbins;
  
private:

};



#endif // INTERPOLATOR_H
