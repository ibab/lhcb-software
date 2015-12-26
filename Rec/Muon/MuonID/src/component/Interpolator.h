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
class interpolator final {
public: 
  /// Standard constructor
  interpolator(double x1,double x2,double y1,double y2 ) {
    //get point and slope for the line
    b= (y2-y1)/(x2-x1);
    a= y2-x2*b;
  }

  //return corresponding value
  double value(double x) const {return a+b*x;}
  
private:

  double a;
  double b;  

};


class Uniformer final {
public: 
  /// Standard constructor
  Uniformer(const std::vector<double>& xpoints_in,const std::vector<double>& ypoints_in); 

  int findBin(double xval) const;
  double getYvalue(double xval) const;

private:
  std::vector<double> m_xpoints;
  std::vector<interpolator> m_uniformer;

};



#endif // INTERPOLATOR_H
