// $Id: CaloMoniAlg.h,v 1.3 2005-11-07 12:16:38 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/05/08 09:58:25  ibelyaev
//  remove associators, update options
//
// Revision 1.1.1.1  2004/10/25 09:00:25  ibelyaev
// // The new pckage: code is imported from Calo/CaloMonitor
//
// Revision 1.1  2004/03/17 16:35:20  ibelyaev
//  regular update: MCCaloMonitor and CaloPIDsMonitor
// 
// ============================================================================
#ifndef CALOMONITOR_CALOMONIALG_H 
#define CALOMONITOR_CALOMONIALG_H 1
// ============================================================================
// Include files
// ============================================================================
// Calo/CaloKernel
// ============================================================================
#include "CaloKernel/CaloHistoAlg.h"
// ============================================================================

typedef CaloHistoAlg CaloMoniAlg ;


namespace 
{
  template <class HISTO> 
  inline StatusCode hFill ( HISTO* histo ,
                            double value ) 
  {
    if ( 0 == histo ) { return StatusCode::FAILURE ; }
    histo -> fill( value ) ;
    return StatusCode::SUCCESS ;
  };  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOMONITOR_CALOMONIALG_H
// ============================================================================
