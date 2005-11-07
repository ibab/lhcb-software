// $Id: CaloData.h,v 1.6 2005-11-07 11:57:13 odescham Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/04/02 12:30:20  ocallot
// Fix CaloData.h reference to CaloEvent/CaloDigit.h
//
// Revision 1.4  2001/11/25 16:18:50  ibelyaev
//  bug fix
//
// Revision 1.3  2001/11/25 15:55:03  ibelyaev
//  bug fix
//
// Revision 1.2  2001/11/25 15:26:20  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.1.1.1  2001/11/02 14:39:53  ibelyaev
// New package: The first commit into CVS
//
// Revision 1.3  2001/07/17 20:00:48  ibelyaev
// modifications to improve Doxygen documentation
//
// Revision 1.2  2001/06/29 10:33:56  ibelyaev
// update due tomodifications in Event/CaloEvent
// 
// ===========================================================================
#ifndef     CALOALGS_CALODATA_H
#define     CALOALGS_CALODATA_H 1 
// ===========================================================================
// from STL 
#include <string>
// from Gaudi 
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h" 
#include "GaudiKernel/SmartDataPtr.h"
// from CaloKernel
#include "CaloKernel/CaloException.h"
// From CaloEvent 
#include "Event/CaloDigit.h"

class  IMessageSvc; 
class  IDataProviderSvc; 

/** @namespace CaloData   CaloData.h CaloAlgs/CaloData.h
 * 
 *  simple utility functions to get the calorimeter data in an easy way 
 *
 *  @author: Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date:   Dec 1,   1999
 */

namespace CaloData
{
  /** a simple utility to get digits
   *  @param  dataService    data provider        
   *  @param  address        address(path) in transient store 
   *  @param  output         output iterator    
   *  @param  messageService (obsolete) message service pointer 
   */
  template< class OUTPUTTYPE>
  StatusCode Digits( IDataProviderSvc*  dataService         ,  
                     const std::string& address             , 
                     OUTPUTTYPE         output              , 
                     IMessageSvc*       messageService  = 0 ) 
  {
    ///
    IMessageSvc* aux = 0  ; 
    aux  = messageService ; // trick to avoid comopilation warnings 
    /// check for data service 
    if( 0 == dataService ) 
      { 
        throw CaloException ("CaloData:: IDataProviderSvc* poins to NULL!"); 
        return StatusCode::FAILURE ; 
      }
    /// retrive data from the store 
    SmartDataPtr< ObjectVector<CaloDigit> > smart( dataService , address ); 
    /// success??
    if( !smart )           
      { throw CaloException("CaloData::  unable to retrieve "+address ); 
      return StatusCode::FAILURE;  }
    /// copy data using output iterator, remove nulls   
    std::remove_copy( smart->begin() , smart->end() , 
                      output , (CaloDigit *) 0 ); 
    return StatusCode::SUCCESS;                           /* RETURN! */  
    };
  ///
}; // end of namespace CaloData

// ===========================================================================
// The End 
// ===========================================================================
#endif  //  CALOUTILS_CALODATA_H
// ===========================================================================










