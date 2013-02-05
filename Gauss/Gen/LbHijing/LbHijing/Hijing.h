// $Id: Hijing.h,v 1.1.1.1 2006-07-03 17:23:36 gcorti Exp $
#ifndef LBHIJING_HIJING_H 
#define LBHIJING_HIJING_H 1

// Include files
#include <string>

/** @class Hijing Hijing.h LbHijing/Hijing.h
 * 
 *  Utility class to access Hijing Fortran Functions 
 *
 *  @author Daniele Gregori
 *  @date   2006-02-25
 */
class Hijing {
public:
  static void HijingInit(double l_efrm, const std::string l_frame,
                         const std::string l_proj, const std::string l_targ,
                         int l_iap, int l_izp, int l_iat, int l_izt) ;
  static void HijingEvnt(const std::string l_frame, double l_bmin, double l_bmax) ;
  static void LunHep( int mode ) ;

private:

};
#endif // LBHIJING_HIJING_H
