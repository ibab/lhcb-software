#ifndef L0CALOMONIT_H 
#define L0CALOMONIT_H 1

// Include files
// from Gaudi
#include "CaloUtils/Calo2Dview.h"
// from Event
#include "Event/L0DUBase.h"

// AIDA
#include "AIDA/IHistogram1D.h"

// Forward declarations
class DeCalorimeter ;
namespace LHCb {
  class L0CaloCandidate ;
}

/** @class L0CaloMonit L0CaloMonit.h
 *  Monitoring algorithm for the L0 Calorimeter trigger
 *  Scans the L0CaloCandidates and fills histograms.
 *  Looks also for hot cells if requested.
 *
 *  @author Olivier Callot
 *  @author Marie-Helene Schune
 *  @date   31/05/2001
 */

class L0CaloMonit: public Calo2Dview {

public:
  /// Standard constructor
  L0CaloMonit( const std::string& name , ISvcLocator* pSvcLocator ) ;

  /// Standard destructor
  virtual ~L0CaloMonit( ) ;

  /// Initialization: book histograms
  virtual StatusCode initialize() ;

  /// Main execution routine: fill histograms and find hot cells
  virtual StatusCode execute   () ;

  /// Finalization: print hot cells
  virtual StatusCode finalize  () ;

private:

  /** Automatic search for hot cells from L0 Calo trigger histograms and hits
   *  and print the information about the hot cells.
   *  @param[in] hist  Histogram of hit cells for a given type of L0Calo
   *                   candidate and a given area of ECAL or HCAL.
   *                   The histogram is reset once the function is executed
   *  @param[in] type  The type of candidate corresponding to the histogram
   */
  void SearchForHotCellsAndReset( AIDA::IHistogram1D * hist , const int type ) ;

  DeCalorimeter * m_ecal ; ///< Pointer to Ecal detector element
  DeCalorimeter * m_hcal ; ///< Pointer to Hcal detector element

  unsigned int m_nEvents ; ///< Counter of events with existing L0Calo data

  bool m_fullMonitoring ; ///< Flag to perform full monitoring, set by option

  /** Suffix to add to the default name of L0Calo data containers. Allows
   *  to select between different sources (data or emulation)
   *  Set by option.
   */
  std::string m_inputDataSuffix ;

  /** Frequency with which the search of hot cells is launched.
   *  Set by option.
   *  @sa SearchForHotCellsAndReset
   */
  int  m_updateFrequency ;

  /** Ratio to declare a cell hot.
   *  Set by option.
   *  @sa SearchForHotCellsAndReset
   */
  int m_alarmThresholdRatio ;

  /** Activate or desactivate hot cell automatic search.
   *  Set by option.
   *  @sa SearchForHotCellsAndReset
   */
  bool  m_lookForHotCells ;

  /// BCId histogram
  AIDA::IHistogram1D * m_bcidHist ;

  /// Et Spectra histogram. Index of vector is type L0DUBase::CaloType
  std::vector< AIDA::IHistogram1D *> m_etHist ;

  /** Et Spectra histogram for the full monitoring.
   *  Index of vector is type L0DUBase::CaloType
   */
  std::vector< AIDA::IHistogram1D *> m_etFullHist ;

  /** Frequency histogram. Index of vector is type L0DUBase::CaloType
   *  and area of detector.
   */
  std::vector< std::vector< AIDA::IHistogram1D *> > m_freqHist ;

  /** Crate histogram. Index of vector is type L0DUBase::CaloType
   *  and crate number (renumbering from 0).
   */
  std::vector< std::vector< AIDA::IHistogram1D *> > m_crateHist ;

  /// Name of 2D map histograms
  std::vector< std::string > m_mapName ;

  /// Title of the 2D map histograms
  std::vector< std::string > m_mapTitle ;

  std::vector< std::string > m_histName ; ///< Names of the histograms
  std::vector< std::string > m_histTitle ; ///< Titles of the histograms
  std::vector< int >         m_maxScale ; ///< Maximum of the histograms

  /** Auxiliary function to book one Et spectra histogram
   *  @param[in]  i      type of candidate (L0DUBase::CaloType)
   *  @param[out] hist   vector of IHistogram1D to book
   *  @param[in]  suffix empty or "Full"
   */
  void bookEtHist( const unsigned int i , std::vector< AIDA::IHistogram1D *> & hist ,
                   const std::string& suffix ) ;

  /** Auxiliary function to book one frequency histogram
   *  @param[in]  i      type of candidate (L0DUBase::CaloType)
   *  @param[out] hist   vector of IHistogram1D to book
   */
  void bookFreqHist( const unsigned int i ,
                     std::vector< std::vector< AIDA::IHistogram1D *> > & hist ) ;

  /** Auxiliary function to book one crate histogram
   *  @param[in]  i      type of candidate (L0DUBase::CaloType)
   *  @param[out] hist   vector of IHistogram1D to book
   */
  void bookCrateHist( const unsigned int i ,
                      std::vector< std::vector< AIDA::IHistogram1D *> > & hist ) ;

  /** Auxiliary function to fill histograms for the default monitoring
   *  @param[in] cand  L0CaloCandidate to use to fill histograms
   */
  void defaultMonitoring( const LHCb::L0CaloCandidate * cand ) ;

  /** Auxiliary function to print the location of the hot cells for
   *  all candidate types.
   */
  void printHotCellSummary( ) ;

  /** Returns the detector element for a given type: Ecal for Electron,
   *  Photon, Local pi0 and Global pi0, and Hcal for hadron
   */
  inline DeCalorimeter * detector( const int type ) const {
    if ( L0DUBase::CaloType::Hadron == type ) return m_hcal ;
    return m_ecal ;
  }

  /// Returns the crate number counting from zero
  inline int crateOrder( const int type , const int crate ) const {
    if ( L0DUBase::CaloType::Hadron == type ) return ( crate - 22 ) ;
    return ( crate - 8 ) ;
  }

  /// Returns the crate number as a string
  inline std::string crateNumber( const int type , const int crate ) const {
    std::ostringstream theStr ;
    if ( L0DUBase::CaloType::Hadron == type ) theStr << ( crate + 22 ) ;
    else theStr << (crate + 8) ;
    return theStr.str() ;
  }

  /// Returns the name of the candidate type
  inline std::string fullName( const int type ) const {
    switch ( type ) {
    case L0DUBase::CaloType::Electron:  return "Electron"  ; break ;
    case L0DUBase::CaloType::Photon:    return "Photon"    ; break ;
    case L0DUBase::CaloType::Pi0Local:  return "Pi0Local"  ; break ;
    case L0DUBase::CaloType::Pi0Global: return "Pi0Global" ; break ;
    case L0DUBase::CaloType::Hadron:    return "Hadron"    ; break ;
    default: break ;
    }
    return "default" ;
  }

  /// Returns the abbreviation of the candidate type
  inline std::string abbrev( const int type ) const {
    switch ( type ) {
    case L0DUBase::CaloType::Electron:  return "Ele" ; break ;
    case L0DUBase::CaloType::Photon:    return "Pho" ; break ;
    case L0DUBase::CaloType::Pi0Local:  return "Pil" ; break ;
    case L0DUBase::CaloType::Pi0Global: return "Pig" ; break ;
    case L0DUBase::CaloType::Hadron:    return "Had" ; break ;
    default: break ;
    }
    return "default" ;
  }

  /// Name corresponding to the area
  inline std::string area( const int type ) const {
    switch ( type ) {
    case 0: return "Outer"  ; break ;
    case 1: return "Middle" ; break ;
    case 2: return "Inner"  ; break ;
    default: break ;
    }
    return "default" ;
  }
};
#endif // L0CALOMONIT_H
