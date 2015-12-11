// ============================================================================
#ifndef   CALODET_DESUBCALORIMETER_H
#define   CALODET_DESUBCALORIMETER_H   1
#include "GaudiKernel/MsgStream.h"
#include "DetDesc/DetectorElement.h"
#include "CaloDet/CLIDDeSubCalorimeter.h"
#include "Kernel/CaloCellID.h"

class MsgStream;
class DeSubSubCalorimeter ;

/** @class DeSubCalorimeter DeSubCalorimeter.h "CaloDet/DeSubCalorimeter.h" 
 *
 *  specific detector element class (calo halves)
 *
 *  @author Olivier Deschamps odescham@in2p3.fr 
 * 
 */

class DeSubCalorimeter: public DetectorElement 
{
public:
  /// Constructors
  DeSubCalorimeter( const std::string& name    = ""  );
  /// Destructor
  ~DeSubCalorimeter() override = default;
  /// object identification 
  static  const CLID& classID()       { return CLID_DeSubCalorimeter ; }
  /// printout to standard STL stream 
  virtual std::ostream& printOut( std::ostream& os = std::cerr ) const override; 
  /// printout to standard Gaudi stream 
  virtual MsgStream&    printOut( MsgStream&                   ) const override;
  /// object identification 
  virtual const CLID& clID   () const override;
  /// standard initialization
  virtual StatusCode    initialize() override;
  /// get side
  inline int side    ()            const    { return m_side; };
  inline std::string sideName()       const    { 
    if( m_side == 1){return "A-side";}
    else if( m_side == -1){return "C-side";}
    else {return "??";}
  };
  /// set side (0=C=Right / 1=A=Left)
  inline void   setSide( const int s )     { m_side = s;    };
  ///
  typedef std::vector<const DeSubSubCalorimeter*>  SubSubCalos  ;
  const SubSubCalos& subSubCalos() const {    return m_subSubCalos;  };
  

private:
  int    m_side = 0;      ///< Calo side (0=C-side=Right , 1=A-side=Left)
  SubSubCalos   m_subSubCalos  ;
};
// ============================================================================
inline std::ostream& operator<<( std::ostream& os , const DeSubCalorimeter& de ){ 
  return de.printOut( os ); 
} 
inline std::ostream& operator<<( std::ostream& os , const DeSubCalorimeter* de ){ 
  return de ? (os<<*de) :(os<<" DeSubCalorimeter* points to NULL!"<<std::endl); 
}
inline MsgStream& operator<<( MsgStream&    os , const DeSubCalorimeter& de ) { 
  return de.printOut( os ); 
} 
inline MsgStream& operator<<( MsgStream&    os , const DeSubCalorimeter* de ){ 
  return de ? (os<<*de) :(os<<" DeSubCalorimeter* points to NULL!"<<endmsg   ); 
}
// ============================================================================
#endif    //   CALODET_DESUBCALORIMETER_H
