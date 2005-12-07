// $Id: LHAPDFCommonBlocks.h,v 1.1 2005-12-07 23:01:32 robbep Exp $

#ifndef GENERATORS_LHAPDFCOMMONBLOCKS_H
#define GENERATORS_LHAPDFCOMMONBLOCKS_H 1

#include <string>

static const int s_lenlhaparm = 20 ;

/** @class Lhacontrol
 *
 *  Class to access LHACONTROL LhaPdf Common block from C++ code.
 * 
 *  @author Patrick Robbe
 *  @date   2005-12-06
 */

class Lhacontrol {
public:
  Lhacontrol();
  ~Lhacontrol();
  void setlhavalue( int n , double value) ;
  void setlhaparm( int n , const std::string & value ) ;

  void getlhavalue( int n , double & value) ;
  void getlhaparm( int n , std::string & value ) ;

  int lenlhaparm() const { return s_lenlhaparm ; }

private:
  char * m_dummystr ;
  double m_dummy;
};
#endif // GENERATORS_LHAPDFCOMMONBLOCKS_H
 
