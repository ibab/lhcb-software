// $Id: CaloKernelTest.cpp,v 1.2 2002-03-20 15:49:09 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// ============================================================================
//
#include <iostream>
//
#include "CaloKernel/CaloCellCode.h"
#include "CaloKernel/CaloCellID.h"
#include "CaloKernel/CaloCollection.h"
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/InputStreamIterator.h"
#include "CaloKernel/OutputStreamIterator.h"
#include "CaloKernel/CaloMap.h"

// ============================================================================
/**  @file CaloGenTest.cpp 
 * 
 * small test program to test some includes from CaloGen package 
 *  
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date 23/06/2001 
 */
// ============================================================================

int main()
{

  ///
  unsigned short icell = 10970;
  CaloCellID cell1;
  CaloCellID cell2( icell) ;
  CaloCellID cell3( 0 , 2, 43 , 26 );
  
  ///
  std::cout 
    << " \tCellID " << cell1 
    << " \tCalo=  " << cell1.calo () 
    << " \tArea=  " << cell1.area () 
    << std::endl 
    << " \t\t" 
    << " \tRow=   " << cell1.row  () 
    << " \tCol=   " << cell1.col  () 
    << " \tRaw=   " << cell1.raw  ()
    << " \tBits=  " << cell1.bits ()
    << " \tSize of all/raw = " 
    << sizeof( cell1       ) 
    << "/" 
    << sizeof( cell1.raw() ) 
    << std::endl ;

  std::cout
    << " \tCellID " << cell2 
    << " \tCalo=  " << cell2.calo () 
    << " \tArea=  " << cell2.area () 
    << std::endl 
    << " \t\t" 
    << " \tRow=   " << cell2.row  () 
    << " \tCol=   " << cell2.col  () 
    << " \tRaw=   " << cell2.raw  ()
    << " \tBits=  " << cell2.bits () 
    << " \tSize of all/raw = " 
    << sizeof( cell2       ) 
    << "/" 
    << sizeof( cell2.raw() ) 
    << std::endl ;

  std::cout
    << " \tCellID " << cell3 
    << " \tCalo=  " << cell3.calo () 
    << " \tArea=  " << cell3.area () 
    << std::endl 
    << " \t\t" 
    << " \tRow=   " << cell3.row  () 
    << " \tCol=   " << cell3.col  () 
    << " \tRaw=   " << cell3.raw  ()
    << " \tBits=  " << cell3.bits () 
    << " \tSize of all/raw = " 
    << sizeof( cell3       ) 
    << "/" 
    << sizeof( cell3.raw() ) 
    << std::endl ;

  CaloMap<int,std::string> mm;
  
  mm[1]   = "value of 1st element" ;
  mm[100] = "value of 100th element" ;
  
    
  std::cout << " test of map "
            << "mm[1]   ='" << mm[1]   << "' " 
            << "mm[100] ='" << mm[100] << "' " 
            << "mm[50]  ='" << mm[50]  << "' " << std::endl;

  try
    {
      throw CaloException("This is an exception", StatusCode::FAILURE );
    }
  catch(...)
    {}
  ///
  return 0;
  ///
};

// ============================================================================

