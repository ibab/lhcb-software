// $Id: SolidTrd.h,v 1.10 2005-12-07 13:19:07 cattanem Exp $ 
// ===========================================================================
// CVS $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef     DETDESC_SOLIDTRD_H
#define     DETDESC_SOLIDTRD_H 1 
/// STD and STL 
#include <cmath>
#include <iostream> 
/// DetDesc 
#include "DetDesc/SolidBase.h" 
#include "DetDesc/SolidPolyHedronHelper.h" 
/// forward declarations 
template <class TYPE> 
class SolidFactory;


/** @class SolidTrd SolidTrd.h  "DetDesc/SolidTrd.h"
 *
 *   A simple implementation of TRD
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */

class SolidTrd: public    virtual SolidBase             ,
                protected virtual SolidPolyHedronHelper 
{
  /// friend factory for instantiation
  friend class SolidFactory<SolidTrd>;
  
 public:
  
  /** constructor 
   *  @param name name of the solid object
   *  @param zHalfLength half-length in z-direction 
   *  @param XHalfLength1 half-length in x-direction at z1  
   *  @param YHalfLength1 half-length in y-direction at z1  
   *  @param XHalfLength2 half-length in x-direction at z2  
   *  @param YHalfLength2 half-length in y-direction at z2
   *  @exception SolidException wrong parameter range 
   */ 
  SolidTrd( const std::string& name         ,
            const double       zHalfLength  , 
            const double       XHalfLength1 , 
            const double       YHalfLength1 , 
            const double       XHalfLength2 , 
            const double       YHalfLength2 );
  
  /// destructor 
  virtual ~SolidTrd();

  /** - retrieve the TRD type 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid 
   *  return box type
   */
  inline std::string typeName () const { return "SolidTrd"; };
  
  /** - retrieve the pointer to "simplified" solid - "cover"
   *    -# The cover for general TRD is "symmetric" TRD 
   *    -# the cover for symmetric TRD is BOX 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid 
   *  @return pointer to "simplified" solid - "cover"
   */
  const ISolid* cover () const;
  
  /** - printout to STD/STL stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( std::ostream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param os STD/STL stream
   *  @return reference to the stream 
   */
  virtual std::ostream& printOut ( std::ostream& os = std::cout ) const;
  
  /** - printout to Gaudi MsgStream stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( MsgStream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param os  Gaudi MsgStream stream
   *  @return reference to the stream 
   */
  virtual MsgStream&    printOut   ( MsgStream&  os             ) const;
  
  /**  half size in x at point 1 
   *  @return half size in x at point 1 
   */
  inline       double                xHalfLength1() const 
  { return m_trd_xHalfLength1; };

  /**  half size in x at point 2 
   *  @return  half size in x at point 2 
   */
  inline       double                xHalfLength2() const 
  { return m_trd_xHalfLength2; };

  /**  half size in y at point 1 
   *  @return half size in y at point 1 
   */
  inline       double                yHalfLength1() const 
  { return m_trd_yHalfLength1; };
  
  /**  half size in y at point 2 
   *  @return half size in y at point 2 
   */
  inline       double                yHalfLength2() const 
  { return m_trd_yHalfLength2; };
  
  /**  half size in z  
   *  @return  half size in z  
   */
  inline       double                zHalfLength () const 
  { return m_trd_zHalfLength; };

  /**  full size in x at point 1 
   *  @return full size in x at point 1 
   */
  inline       double                xLength1    () const 
  { return m_trd_xHalfLength1 * 2 ; };
  
  /**  full size in x at point 2 
   *  @return full size in x at point 2
   */ 
  inline       double                xLength2    () const 
  { return m_trd_xHalfLength2 * 2 ; };
  
  /**  full size in y at point 1 
   *  @return full size in y at point 1 
   */
  inline       double                yLength1    () const 
  { return m_trd_yHalfLength1 * 2 ; };

  /**  full size in y at point 2 
   *  @return full size in y at point 2 
   */
  inline       double                yLength2    () const 
  { return m_trd_yHalfLength2 * 2 ; };

  /**  full size in z  
   *  @return full size in z  
   */
  inline       double                zLength     () const 
  { return m_trd_zHalfLength  * 2 ; };
  
  //
protected:
  
  /** constructor
   *  @param name name of trd object
   */
  SolidTrd( const std::string& name = "Anonymous TRD");
  ///
private:
  
  /** initialize the polihedron base 
   *  @exception SolidException wrong parameters  
   */
  void makeAll() ; 
  
private:
  

  SolidTrd( const SolidTrd & );  // no copy-constructor 
  SolidTrd& operator=( const SolidTrd & );  // no assignment 

private:
  
  ///@{
  /** data members - TRD parameters  */
  double                 m_trd_zHalfLength  ;
  double                 m_trd_xHalfLength1 ;
  double                 m_trd_xHalfLength2 ;
  double                 m_trd_yHalfLength1 ;
  double                 m_trd_yHalfLength2 ;
  ///@}
};

/// ===========================================================================
#endif ///<   DETDESC_SOLIDTRD_H
/// ===========================================================================




