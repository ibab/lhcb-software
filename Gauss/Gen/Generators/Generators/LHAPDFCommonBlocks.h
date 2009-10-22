// $Id: LHAPDFCommonBlocks.h,v 1.3 2009-10-22 16:43:05 robbep Exp $

#ifndef GENERATORS_LHAPDFCOMMONBLOCKS_H
#define GENERATORS_LHAPDFCOMMONBLOCKS_H 1

#include <string>

/** @class Lhacontrol LHAPDFCommonBlocks.h "Generators/LHAPDFCommonBlocks.h"
 *
 *  Class to access LHACONTROL LhaPdf Common block from C++ code.
 * 
 *  @author Patrick Robbe
 *  @date   2005-12-06
 */

class Lhacontrol {
 public:
  Lhacontrol(); ///< Constructor 

  ~Lhacontrol(); ///< Destructor

  /** Set value of LHAVALUE array in common LHACONTROL.
   *  @param[in] n     Index of the array to modify.
   *  @param[in] value Value to set to LHAVALUE(n).
   */
  void setlhavalue( const int n , const double value) ;

  /** Set value of LHAPARM array in common LHACONTROL.
   *  @param[in] n     Index of the array to modify.
   *  @param[in] value Value to set to LHAPARM(n).
   */
  void setlhaparm( const int n , const std::string & value ) ;

  /** Set value of LHAVALUE array in common LHACONTROL.
   *  @param[in]  n     Index of the array to modify.
   *  @param[out] value Value of LHAVALUE(n).
   */  
  void getlhavalue( int n , double & value) ;

  /** Set value of LHAPARM array in common LHACONTROL.
   *  @param[in]  n     Index of the array to modify.
   *  @param[out] value Value of LHAPARM(n).
   */  
  void getlhaparm( int n , std::string & value ) ;  
  
 private:
  /// Length of the LHAPARM and LHAVALUE arrays
  int lenlhaparm() const { return s_lenlhaparm ; }

  /// Size of the LHAPARM and LHAVALUE arrays
  static const int s_lenlhaparm = 20 ;

  /// Dummy string value to return in case of error.
  const char * m_dummystr ;

  /// Dummy value double value to return in case of error.
  double m_dummy;
};
#endif // GENERATORS_LHAPDFCOMMONBLOCKS_H
 
