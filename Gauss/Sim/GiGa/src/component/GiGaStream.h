// ============================================================================
// $Id: GiGaStream.h,v 1.6 2002-01-22 18:20:53 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/08/12 15:42:54  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.4  2001/07/23 13:12:28  ibelyaev
// the package restructurisation(II)
//
// ============================================================================
#ifndef          GIGA_GIGASTREAM_H
#define          GIGA_GIGASTREAM_H  1 
// ============================================================================
// from STL
#include <string>
#include <vector>
/// base class from Gaudi 
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataSelector.h" 
#include "GaudiKernel/DataStoreItem.h"
// forward declarations 
class IDataProviderSvc ;
class IDataManagerSvc  ;
class IServiceLocator  ;
class IConversionSvc   ; 
class IRegistry        ;

/** @class GiGaStream  GiGaStream.h 
 *
 *  simple utility to move data to and from GiGa
 *
 *  @author Vanya Belyaev
 */

class GiGaStream: public Algorithm
{
  ///  
public:
  ///
  typedef std::vector<DataStoreItem>  Items  ; 
  typedef std::vector<std::string>    Names  ; 
  typedef std::vector<IRegistry*>     Leaves ;
  ///
protected:
  
  /** standard constructor 
   *  @param StreamName name of the Stream 
   *  @param ServiceLocator pointer to Service Locator 
   */
  GiGaStream( const std::string& StreamName     , 
              ISvcLocator*       ServiceLocator ) ;

  /** virtual destructor 
   */
  virtual ~GiGaStream();
  
public: 
  
  /** standard initialization method 
   *  @return status code 
   */ 
  virtual StatusCode initialize() ; 
  
  /** standard finalization method 
   *  @return status code 
   */ 
  virtual StatusCode finalize  () ; 
  
protected: 
  
  /** Load objects pointed by Item  and put it into Selector 
   *  @param Item object tree 
   *  @param Selector data selector 
   *  @return statsu code 
   */
  StatusCode LoadObject( const DataStoreItem& Item     , 
                         IDataSelector*       Selector );
  
  /** Load objects pointed by Item  and put it into Selector 
   *  @param Object object directory 
   *  @param Level number of levels  
   *  @param Selector data selector 
   *  @return statsu code 
   */
  StatusCode LoadObject( const IRegistry* Object   , 
                         const int        Level , 
                         IDataSelector*   Selector );
  
  /** Miscellaneous function to simplify the typing 
   *  @param message message to be printed 
   *  @param status status code to be returned 
   */
  StatusCode Error( const std::string& message , 
                    const StatusCode& status  = StatusCode::FAILURE ); 
  
protected:
  
  bool                        m_executeOnce         ; 
  bool                        m_execute             ;  
  
  std::string                 m_nameOfCnvSvc        ; 
  IConversionSvc*             m_cnvSvc              ; 
  
  std::string                 m_nameOfDataSvc       ; 
  IDataProviderSvc*           m_dataSvc             ;
  
  Names                       m_namesOfItems        ;
  Items                       m_items               ;
  
  IDataSelector               m_dataSelector        ;  
  Leaves                      m_leaves              ;

};

// ============================================================================
#endif    ///< GIGA_GIGASTREAM_H
// ============================================================================






