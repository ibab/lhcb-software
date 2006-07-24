// $Id: ITBoxOverlaps.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#ifndef _ITBoxOverlaps_H
#define _ITBoxOverlaps_H

/** @class ITBoxOverlaps ITBoxOverlaps.h
* Determines whether point is in overlap between IT boxes
* @author M. Needham
* @date 16.05.2004
*/ 

class DeSTDetector;
#include <vector>

class ITBoxOverlaps {

public:

  /// constructer
  ITBoxOverlaps(DeSTDetector* tracker , double tolerance = 0.0);

  // destructer
  virtual ~ITBoxOverlaps(){}

  bool isInside(const unsigned int iStation, 
                const double x,
                const double y) const;  
private:

  unsigned int m_firstIT;
  double tolerance;

  std::vector<double> m_vMinCont;
  std::vector<double> m_vMaxCont;
  std::vector<double> m_uMinCont;
  std::vector<double> m_uMaxCont;

};

inline bool ITBoxOverlaps::isInside(const unsigned int iStation, 
                                    const double x,
                                    const double y) const{

 return((fabs(x)>m_uMinCont[iStation-m_firstIT])&&(fabs(x)<m_uMaxCont[iStation-m_firstIT])
        &&(fabs(y)>m_vMinCont[iStation-m_firstIT])&&(fabs(y)<m_vMaxCont[iStation-m_firstIT]));
}

#endif // _ITBoxOverlaps_H
