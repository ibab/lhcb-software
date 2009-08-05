// $Id: CaloDataFunctor.h,v 1.4 2009-08-05 17:33:00 ibelyaev Exp $
// ============================================================================
#ifndef CALOUTILS_CALODATAFUNCTOR_H 
#define CALOUTILS_CALODATAFUNCTOR_H 1
// ============================================================================
// STD and STL 
// ============================================================================
#include <functional>
#include <cmath> 
// ============================================================================
// MathCore
// ============================================================================
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/Point3DTypes.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloClusterEntry.h"
#include "Kernel/CaloCellID.h"
#include "Event/CaloDigitStatus.h"
#include "Event/CaloDataFunctor.h"
// ============================================================================
// CaloDet
// ============================================================================
class DeCalorimeter ;
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  namespace CaloDataFunctor
  {
    // ========================================================================
    /** check if the digit is local maxima 
     *  @param digit the digit to be tested 
     *  @param det the detector ("neighbours provider")
     *  @param data dat avector 
     *  @return true for local maximum
     */
    template <class DETECTOR, class DATA>
    bool isLocalMax 
    ( const LHCb::CaloDigit* digit , 
      const DETECTOR*        det   ,
      const DATA&            data  ) 
    {
      // ======================================================================
      if ( 0 == digit || 0 == det || data.empty() ) { return false ; } // RETURN 
      // get all neighbours 
      const LHCb::CaloCellID::Vector& cells = 
        det->neighborCells ( digit->cellID() ) ;
      for ( LHCb::CaloCellID::Vector::const_iterator inei = cells.begin() ;
            cells.end() != inei ; ++inei )
      {
        const LHCb::CaloDigit* nei = data(*inei);
        if ( 0 == nei               ) { continue     ; }  // skip NULLS 
        if ( nei->e() >= digit->e() ) { return false ; }  // RETURN
      }
      return true ;                                      // RETURN 
      // ======================================================================
    }
    // =========================================================================
    /** check if the digit is local maxima 
     *  @param digit the digit to be tested 
     *  @param det the detector ("neighbours provider")
     *  @param data dat avector 
     *  @return true for local maximum
     */
    template <class DETECTOR, class DATA>
    bool isLocalMax 
    ( const LHCb::CaloDigit* digit , 
      const DETECTOR*        det   ,
      const DATA*            data  ) 
    {
      if ( 0 == data ) { return false ; }
      return isLocalMax ( digit , det , *data ) ;
    }
    // ========================================================================
    template <class DETECTOR, class DATA>
    struct IsLocalMax : public std::unary_function<const LHCb::CaloDigit*,bool>
    {
      // ======================================================================
      /// constructor from detector and data 
      IsLocalMax ( const DETECTOR* det  , 
                   const DATA*     data ) 
        : m_det ( det ) , m_data ( data ) 
      {}
      // ======================================================================
    public:
      // =====================================================================
      /// the only one essential method
      bool operator() ( const LHCb::CaloDigit* digit ) const 
      { return isLocalMax ( digit , m_det , m_data ) ; }
      // =====================================================================
    private:
      // =====================================================================
      /// the defalt constructor is disabled 
      IsLocalMax (); // the defalt constructor is disabled 
    private:
      // =====================================================================
      /// the detector 
      const DETECTOR* m_det ;                                  // the detector 
      /// data 
      const DATA*     m_data ;                                 //         data
      // =====================================================================  
    };
    // ========================================================================
  } // end of namespace CaloDataFunctor
  // ==========================================================================
}// end of namespace LHCb
// ============================================================================
// The End 
// ============================================================================
#endif // CALOUTILS_CALODATAFUNCTOR_H
// ============================================================================
