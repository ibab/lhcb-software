// $Id: SolidTrap.h,v 1.16 2007-03-16 15:57:09 cattanem Exp $
/// ===========================================================================
#ifndef     DETDESC_SOLIDTRAP_H
#define     DETDESC_SOLIDTRAP_H 1 

/** STD and STL  includes */
#include <cmath>
#include <iostream> 

/** Geometry Definitions */
#include "GaudiKernel/Point3DTypes.h" 
#include "GaudiKernel/Vector3DTypes.h" 
#include "GaudiKernel/Plane3DTypes.h" 

/** DetDesc includes */ 
#include "DetDesc/SolidBase.h" 
#include "DetDesc/SolidPolyHedronHelper.h" 

/** forward declarations */
template <class TYPE>
class SolidFactory;

/** @class SolidTrap SolidTrap.h "DetDesc/SolidTrap.h"
 *
 *  A simple implementation of TRAP
 *  
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */

class SolidTrap: public    virtual SolidBase            ,
                 protected virtual SolidPolyHedronHelper
{
  /// friend factory for instantiation  
  friend class SolidFactory<SolidTrap>;
  
public:

  /// typedef for vertices 
  typedef std::vector<Gaudi::XYZPoint> VERTICES;

public: 
  
  /** constructor 
   *  @param name                   name of trap solid 
   *  @param zHalfLength            half length in z 
   *  @param theta                  theta angle 
   *  @param phi                    phi angle 
   *  @param dyAtMinusZ             delta y at bottom face
   *  @param dxAtMinusZMinusY       delta x at bottom face , minus y
   *  @param dxAtMinusZPlusY        delta x at bottom face , plus y 
   *  @param alphaAtMinusZ          alpha angle at bottom face 
   *  @param dyAtPlusZ              delta y at top face 
   *  @param dxAtPlusZMinusY        delta x at top face, minus y 
   *  @param dxAtPlusZPlusY         delta x at top face, plus y     
   *  @param alphaAtPlusZ           alpha angle at top face 
   *  @exception SolidException wrong parameters range 
   */   
  SolidTrap( const std::string&  name             , 
             const double        zHalfLength      , 
             const double        theta            , 
             const double        phi              , 
             const double        dyAtMinusZ       ,  
             const double        dxAtMinusZMinusY , 
             const double        dxAtMinusZPlusY  ,
             const double        alphaAtMinusZ    ,
             const double        dyAtPlusZ        ,  
             const double        dxAtPlusZMinusY  , 
             const double        dxAtPlusZPlusY   ,
             const double        alphaAtPlusZ    );

  /// destructor 
  virtual ~SolidTrap();
  
  /** - retrieve the TRAP type 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid 
   *  return box type
   */
  inline std::string typeName () const { return "SolidTrap"  ; };
  
  /** - retrieve the pointer to "simplified" solid - "cover"
   *    -\# the cover for Trap is Trd 
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
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream 
   */
  virtual MsgStream&    printOut ( MsgStream&   os             ) const;
  
  /** half size in z  
   *  @return half size in z 
   */ 
  inline const double& zHalfLength      () const 
  { return m_trap_zHalfLength      ; };
  
  ///@{
  /** polar and asimutal angle of the trapezoid  */
  inline const double& theta            () const 
  { return m_trap_theta            ; };
  inline const double& phi              () const
  { return m_trap_phi              ; }; 
  ///@} 
  
  ///@{
  /** parameters of the bottom face of general trapezoid */
  inline const double& dyAtMinusZ       () const 
  { return m_trap_dyAtMinusZ       ; }; 
  inline const double& dxAtMinusZMinusY () const 
  { return m_trap_dxAtMinusZMinusY ; };
  inline const double& dxAtMinusZPlusY  () const 
  { return m_trap_dxAtMinusZPlusY  ; };
  inline const double& alphaAtMinusZ    () const 
  { return m_trap_alphaAtMinusZ    ; }; 
  ///@}
  
  ///@{
  /** parameters of the top face of general trapezoid */
  inline const double& dyAtPlusZ        () const 
  { return m_trap_dyAtPlusZ        ; }; 
  inline const double& dxAtPlusZMinusY  () const 
  { return m_trap_dxAtPlusZMinusY  ; };
  inline const double& dxAtPlusZPlusY   () const 
  { return m_trap_dxAtPlusZPlusY   ; };
  inline const double& alphaAtPlusZ     () const 
  { return m_trap_alphaAtPlusZ     ; }; 
  ///@}
  
protected:
  
  /** constructor 
   *  @param Name name of general trapezoid 
   */
  SolidTrap( const std::string& Name = "Anonymous Trap");

  /** initialize the polihedron base 
   *  @exception SolidException wrong parameters  
   */
  void        makeAll();
  
  /** acessor to vertices 
   *  @param indx  index of the vertex 
   *  @return  vertex 
   */
  inline const Gaudi::XYZPoint& point( const VERTICES::size_type& indx ) const 
  { return m_ph_vertices[indx]; } 
  
private: 
  
  SolidTrap           ( const SolidTrap & );  ///< no copy-constructor 
  SolidTrap& operator=( const SolidTrap & );  ///< no assignment
  
private:
  
  ///@{ 
  /** trapezoid parameters */
  double               m_trap_zHalfLength       ; 
  double               m_trap_theta             ; 
  double               m_trap_phi               ; 
  double               m_trap_dyAtMinusZ        ; 
  double               m_trap_dxAtMinusZMinusY  ; 
  double               m_trap_dxAtMinusZPlusY   ; 
  double               m_trap_alphaAtMinusZ     ; 
  double               m_trap_dyAtPlusZ         ; 
  double               m_trap_dxAtPlusZMinusY   ; 
  double               m_trap_dxAtPlusZPlusY    ; 
  double               m_trap_alphaAtPlusZ      ; 
  double               m_trap_sinAlphaAtPlusZ   ; 
  double               m_trap_cosAlphaAtPlusZ   ; 
  ///@}
};

/// ===========================================================================
#endif ///< DETDESC_SOLIDTRAP_H
/// ===========================================================================
