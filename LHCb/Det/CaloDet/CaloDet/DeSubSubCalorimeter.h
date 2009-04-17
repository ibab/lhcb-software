#ifndef   CALODET_DESUBSUBCALORIMETER_H
#define   CALODET_DESUBSUBCALORIMETER_H   1
#include "GaudiKernel/MsgStream.h"
#include "DetDesc/DetectorElement.h"
#include "CaloDet/CLIDDeSubSubCalorimeter.h"

class MsgStream;

/** @class DeSubSubCalorimeter DeSubSubCalorimeter.h "CaloDet/DeSubSubCalorimeter.h" 
 *
 *  specific detector element class (calo areas)
 *
 *  @author Olivier Callot Olivier.Callot@cern.ch
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru 
 * 
 */

class DeSubSubCalorimeter: public DetectorElement 
{
public:
  /// Constructors
  DeSubSubCalorimeter( const std::string& name    = ""  );
  /// Destructor
  virtual ~DeSubSubCalorimeter();
  /// object identification 
  static  const CLID& classID()       { return CLID_DeSubSubCalorimeter ; }
  /// printout to standard STL stream 
  virtual std::ostream& printOut( std::ostream& os = std::cerr ) const ; 
  /// printout to standard Gaudi stream 
  virtual MsgStream&    printOut( MsgStream&                   ) const ;
  /// object identification 
  virtual const CLID& clID   () const ;
  /// standard initialization
  virtual StatusCode    initialize() ;
  /// get/set subSubcalo  size
  inline double xSize()const {return m_xSize;}
  inline double ySize()const {return m_ySize;}  
  inline void setXSize( const double x) {m_xSize = x;}
  inline void setYSize( const double y) {m_ySize = y;}
  /// get/set cell  size
  inline double cellSize   ()            const    { return m_cellSize; };
  inline void   setCellSize( const double s )     { m_cellSize = s;    };
  /// get/set area ID
  inline int area() const    { return m_area; };
  inline void   setArea( const int a)     { m_area = a;    };
  ///
private:
  double    m_cellSize;      ///< Cell dimension
  double    m_xSize;
  double    m_ySize;  
  int       m_area;      ///< Calo Area ID
};
// ============================================================================
inline std::ostream&  
operator<<( std::ostream& os , const DeSubSubCalorimeter& de )
{ return de.printOut( os ); } 
// ============================================================================
inline std::ostream&  
operator<<( std::ostream& os , const DeSubSubCalorimeter* de )
{ return de ? (os<<*de) : 
  (os<<" DeSubSubCalorimeter* points to NULL!"<<std::endl); }
// ============================================================================
inline MsgStream&     
operator<<( MsgStream&    os , const DeSubSubCalorimeter& de )
{ return de.printOut( os ); } 
// ============================================================================
inline MsgStream&     
operator<<( MsgStream&    os , const DeSubSubCalorimeter* de )
{ return de ? (os<<*de) : 
  (os<<" DeSubSubCalorimeter* points to NULL!"<<endmsg   ); }
// ============================================================================

// ============================================================================
#endif    //   CALODET_DESUSUBBCALORIMETER_H
