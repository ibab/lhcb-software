// $Id: CaloTrack2IdAlg.h,v 1.1.1.1 2003-03-13 18:52:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/11/17 17:09:27  ibelyaev
//  new set of options and tools
// 
// ============================================================================
#ifndef CALOTRACK2IdALG_H 
#define CALOTRACK2IdALG_H 1
// Include files
// from STL
#include <string>
#include <vector>
#include <math.h>
// from Calo
#include "CaloKernel/CaloAlgorithm.h"
// forward declarations
template <class TYPE1, class TYPE2> class IAssociator ;
class TrStoredTrack ;
class IHistogram2D  ;

/** @class CaloTrack2IdAlg CaloTrack2IdAlg.h
 *  
 *  The generic algorithm to convert the relation table 
 *  from between track sand caloestimators to 
 *  the relation table between tracks and difference 
 *  in loglikelihoods.
 * 
 *  Each concrete instance requires the 
 *   2 histograms (for signal and background)
 * 
 *  For current implementation the histograms will be 
 *  created from options, in future better solution 
 *  need to be found 
 *  
 *  The obvious concrete implementation:
 *   - Prs  ID for e+/e-
 *   - Hcal ID for e+/e- 
 *   - Hcal ID for mu+/mu-
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-14
 */
class CaloTrack2IdAlg : public CaloAlgorithm 
{
  // friend factory for instantiation
  friend class AlgFactory<CaloTrack2IdAlg>;
public:

  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize  ();   
  
protected:
  
  /** Standard constructor
   *  @param name algorithm name 
   *  @param pSvc service locator 
   */
  CaloTrack2IdAlg( const std::string& name , 
                          ISvcLocator*       pSvc );
  
  // destructor 
  virtual ~CaloTrack2IdAlg() ; 

protected:
  
  /// transformation function for momentum 
  inline double pFunc( const double value ) const 
  { return tanh( value / m_pNorm ); };
  
  /// transformation function for estimator 
  inline double vFunc( const double value ) const 
  { return tanh( value / m_vNorm ); };
  
private:
  
  // the associator interface type 
  typedef  IAssociator<TrStoredTrack,float>    IAsct ;
  // actual type of associator to be used 
  std::string         m_associatorType ;
  // actual name of associatorto be used
  std::string         m_associatorName ;
  // associator  itself 
  IAsct*              m_associator     ;

  // use *ONLY* 'unique' tracks 
  bool                m_unique         ;  
  
  // use 'error'   tracks  
  bool                m_error          ;
  // use 'forward' tracks  
  bool                m_forward        ;
  // use  'match'  tracks
  bool                m_matched        ;
  // use  'seed'   tracks
  bool                m_seed           ;
  // use  'velo'   tracks
  bool                m_velo           ;
  // use  'veloTT' tracks
  bool                m_veloTT         ;
  // use  'veloBack' tracks
  bool                m_veloBack       ;
  // use  'upstream' tracks
  bool                m_upstream       ;

  unsigned long       m_vBins          ;
  unsigned long       m_pBins          ;
  
  double              m_vNorm          ;
  double              m_pNorm          ;
  
  typedef std::vector<double>  Data    ;
  Data                m_signalData     ;
  Data                m_backgrData     ;
  
  IHistogram2D*       m_signal         ;
  IHistogram2D*       m_backgr         ;  
  IHistogram2D*       m_signalN        ; // normalized signal 
  IHistogram2D*       m_backgrN        ; // normalized background  
  IHistogram2D*       m_deltaLL        ; // delta Log Likelihood 
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTRACK2IdALG_H
// ============================================================================

