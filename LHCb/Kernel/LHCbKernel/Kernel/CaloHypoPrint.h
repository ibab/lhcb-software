// $Id: CaloHypoPrint.h,v 1.2 2005-01-17 13:00:10 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/03/18 17:21:47  ibelyaev
//  add new stuff from CaloKernel package
//
// Revision 1.1  2001/10/28 18:52:46  ibelyaev
// Major redesign, new CaloHypo class and removal of 'totalE'
// 
// ============================================================================
#ifndef KERNEL_CALOHYPOPRINT_H 
#define KERNEL_CALOHYPOPRINT_H 1
// Include files 
// Kernel 
#include "Kernel/CaloHypotheses.h"

/** Simple templated function for printout of CaloHypo 
 *
 *  @param os reference to output stream 
 *  @param Hypo hypo 
 *  @return reference to output stream 
 *
 *  @author Ivan Belyaev
 *  @date   27/10/2001
 */
template<class OSTREAM> 
OSTREAM& CaloHypoPrint( OSTREAM& os , const long Hypo ) 
{
  /// dispatch 
  switch( Hypo )
    {    
    case CaloHypotheses::Undefined            : 
      os << "'Undefined'/"                 << Hypo  ; break ; 
    case CaloHypotheses::Mip                  : 
      os << "'Mip'/"                       << Hypo  ; break ; 
    case CaloHypotheses::MipPositive          : 
      os << "'MipPositive'/"               << Hypo  ; break ; 
    case CaloHypotheses::MipNegative          : 
      os << "'MipNegative'/"               << Hypo  ; break ; 
    case CaloHypotheses::Photon               :
      os << "'Photon'/"                    << Hypo  ; break ;
    case CaloHypotheses::PhotonFromMergedPi0  :
      os << "'PhotonFromMergedPi0'"        << Hypo  ; break ;
    case CaloHypotheses::BremmstrahlungPhoton :
      os << "'BremmstrahlungPhoton'"       << Hypo  ; break ;
    case CaloHypotheses::Pi0Resolved          :
      os << "'Pi0Resolved'"                << Hypo  ; break ;
    case CaloHypotheses::Pi0Overlapped        :
      os << "'Pi0Overlapped'"              << Hypo  ; break ;
    case CaloHypotheses::Pi0Merged            :
      os << "'Pi0Merged'"                  << Hypo  ; break ;
    case CaloHypotheses::EmCharged            :
      os << "'EmCharged'"                  << Hypo  ; break ;
    case CaloHypotheses::Positron             :
      os << "'Positron'"                   << Hypo  ; break ;
    case CaloHypotheses::Electron             :       
      os << "'Electron'"                   << Hypo  ; break ;
    case CaloHypotheses::EmChargedSeed        :
      os << "'EmChargedSeed'"              << Hypo  ; break ;
    case CaloHypotheses::PositronSeed         :
      os << "'PositronSeed'"               << Hypo  ; break ;
    case CaloHypotheses::ElectronSeed         :       
      os << "'ElectronSeed'"               << Hypo  ; break ;
    case CaloHypotheses::NeutralHadron        :
      os << "'NeutralHadron'/"             << Hypo  ; break ;
    case CaloHypotheses::ChargedHadron        :
      os << "'ChargedHadron'/"             << Hypo  ; break ;
    case CaloHypotheses::PositiveHadron       :
      os << "'ChargedHadron'/"             << Hypo  ; break ;
    case CaloHypotheses::NegativeHadron       :
      os << "'ChargedHadron'/"             << Hypo  ; break ;
    case CaloHypotheses::Jet                  :
      os << "'Jet'/"                       << Hypo  ; break ;
    case CaloHypotheses::Other                :
      os << "'Other'/"                     << Hypo  ; break ;
    default                             :
      os << "'?'/"                         << Hypo  ; break ;  
    };
  ///
  return os ;
};

// ============================================================================
// The End 
// ============================================================================
#endif // KERNEL_CALOHYPOPRINT_H
// ============================================================================
