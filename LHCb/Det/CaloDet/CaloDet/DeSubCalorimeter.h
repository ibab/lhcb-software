// $Id: DeSubCalorimeter.h,v 1.6 2006-06-15 09:27:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef   CALODET_DESUBCALORIMETER_H
#define   CALODET_DESUBCALORIMETER_H   1
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
// DetDesc 
#include "DetDesc/DetectorElement.h"
// CaloDet 
#include "CaloDet/CLIDDeSubCalorimeter.h"

class MsgStream;

/** @class DeSubCalorimeter DeSubCalorimeter.h "CaloDet/DeSubCalorimeter.h" 
 *
 *  specific detector element class
 *
 *  @author Olivier Callot Olivier.Callot@cern.ch
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru 
 * 
 */

class DeSubCalorimeter: public DetectorElement 
{
public:
  /// Constructors
  DeSubCalorimeter( const std::string& name    = ""  );
  /// Destructor
  virtual ~DeSubCalorimeter();
  /// object identification 
  static  const CLID& classID()       { return CLID_DeSubCalorimeter ; }
  /// printout to standard STL stream 
  virtual std::ostream& printOut( std::ostream& os = std::cerr ) const ; 
  /// printout to standard Gaudi stream 
  virtual MsgStream&    printOut( MsgStream&                   ) const ;
  /// object identification 
  virtual const CLID& clID   () const ;
  /// standard initialization
  virtual StatusCode    initialize() ;
  /// get cell  size
  inline double size   ()            const    { return m_size; };
  /// set cell  size
  inline void   setSize( const double s )     { m_size = s;    };
  ///
private:
  double    m_size;      ///< Cell dimension
};
// ============================================================================
inline std::ostream&  
operator<<( std::ostream& os , const DeSubCalorimeter& de )
{ return de.printOut( os ); } 
// ============================================================================
inline std::ostream&  
operator<<( std::ostream& os , const DeSubCalorimeter* de )
{ return de ? (os<<*de) : 
  (os<<" DeSubCalorimeter* points to NULL!"<<std::endl); }
// ============================================================================
inline MsgStream&     
operator<<( MsgStream&    os , const DeSubCalorimeter& de )
{ return de.printOut( os ); } 
// ============================================================================
inline MsgStream&     
operator<<( MsgStream&    os , const DeSubCalorimeter* de )
{ return de ? (os<<*de) : 
  (os<<" DeSubCalorimeter* points to NULL!"<<endreq   ); }
// ============================================================================

// ============================================================================
#endif    //   CALODET_DESUBCALORIMETER_H
// ============================================================================
// The END 
// ============================================================================
