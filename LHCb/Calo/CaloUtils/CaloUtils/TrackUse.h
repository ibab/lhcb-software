// $Id: TrackUse.h,v 1.3 2006-05-29 15:36:19 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/03/22 18:25:06  odescham
// Migration to new Event Model
//
// Revision 1.1  2005/11/07 11:57:13  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.1  2004/10/27 11:58:09  ibelyaev
//  ad dnew class TrackUse
// 
// ============================================================================
// ============================================================================
#ifndef CALOUTILS_TRACKUSE_H 
#define CALOUTILS_TRACKUSE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// forward declarations 
// ============================================================================
class Algorithm ;
class Service   ;
class AlgTool   ;
// ============================================================================

/** @class TrackUse TrackUse.h CaloUtils/TrackUse.h
 *
 *  the helper class to feed the algorithms/services/tools with 
 *  proper (and uniform) treatment of "Track properties" for 
 *  selection (or) rejection of tracks 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date   2004-10-27
 */
class TrackUse 
{
public:
  
  /** declare 'own' properties for the algorithm 
   * 
   *  @code 
   *
   *  // algorithm contructor 
   *  MyAlg::MyAlg  ( const std::string& name , 
   *                  ISvcLocator*       svc  )
   *   : Algorithm  ( name , svc ) 
   *   , m_trackUse ()
   *   {
   *      m_trackUse.declareProperties( this ) 
   *   };
   *
   *  @endcode 
   * 
   *  @param alg algorithm 
   *  @return status code 
   */
   StatusCode declareProperties ( Algorithm* alg ) ;

  /** declare 'own' properties for the service 
   * 
   *
   *  @code 
   *
   *  // service contructor 
   *  MySvc::MySvc  ( const std::string& name  , 
   *                  ISvcLocator*       isvc  )
   *   : Service    ( name , isvc ) 
   *   , m_trackUse ()
   *   {
   *      m_trackUse.declareProperties( this ) 
   *   };
   *
   *  @endcode 
   * 
   *  @param svc service 
   *  @return status code 
   */
  StatusCode declareProperties ( Service*  svc ) ;
  
  /** declare 'own' properties for the tool 
   * 
   *
   *  @code 
   *
   *  // tool contructor 
   *  MyTool::MyTool  ( const std::string& type , 
   *                    const std::string& name , 
   *                    const IInterface*  parent  )
   *   : AlgTool      ( type , name , parent ) 
   *   , m_trackUse ()
   *   {
   *      m_trackUse.declareProperties( this ) 
   *   };
   *
   *  @endcode 
   * 
   *  @param tool tool
   *  @return status code 
   */
  StatusCode declareProperties ( AlgTool*  tool ) ;
  
  /** the basic methdod which defines if track selected fro furtehr processing
   *  @param track pointer to TrStoiredTrack object 
   *  @return decsion
   */
  bool        use  ( LHCb::Track* track ) const ;
  
  /// 'bit'-representation of track flags/types/categories/algorithms
  std::string bits ( LHCb::Track* track ) const ;
  
  /// get the list of rejected track types 
  std::string  rejectedTypes      () const ;
  
  /// get the list of rejected track categories 
  std::string  rejectedCategories () const ;
  
  /// get the list of rejected track algorithms  
  std::string  rejectedAlgorithms () const ;
  
protected:
  
  /** the basic method for delegation of properties 
   *  @param object property manager 
   *  @return statsu code 
   */
  template <class TYPE>
  inline StatusCode i_declareProperties( TYPE* object ) ;
  
public:
  
  // by type 
  
  bool uniqueOnly   () const { return m_uniqueOnly   ; }
  bool error        () const { return m_error        ; }
  
  // by category 
  
  bool isLong       () const { return m_isLong       ; }
  bool isUpstream   () const { return m_isUpstream   ; }
  bool isDownstream () const { return m_isDownstream ; }
  bool isVelotrack  () const { return m_isVelotrack  ; }
  bool isBackward   () const { return m_isBackward   ; }
  bool isTtrack     () const { return m_isTtrack     ; }
  
  // by algorithm 
  
  bool PatVelo         () const { return m_patvelo         ; }
  bool PatVeloTT       () const { return m_patveloTT       ; }
  bool PatForward      () const { return m_patforward      ; }
  bool PatKShort       () const { return m_patkshort       ; }

  bool TrackSeeding        () const { return m_seed         ; }
  bool TrackMatching       () const { return m_match        ; }
  bool TrackVeloTT         () const { return m_veloTT       ; }
  bool TrackKShort         () const { return m_ksTrack      ; }
  bool TsaTrack            () const { return m_tsa          ; }
  bool TrgForward          () const { return m_trgforward   ; }
  
public: 
  
  /// Standard constructor
  TrackUse( );
  
  /// destructor 
  virtual ~TrackUse() ;
  
private:
  
  // track flags to be used  
  bool               m_uniqueOnly ;  
  bool               m_error      ;
  bool               m_isBackward   ;
  
  // by category 
  

  bool               m_isLong       ;
  bool               m_isUpstream   ;
  bool               m_isDownstream ;
  bool               m_isVelotrack  ;
  bool               m_isTtrack     ;
  
  // by algorithm

  bool               m_patvelo      ;
  bool               m_patforward   ;
  bool               m_patveloTT      ;
  bool               m_patkshort   ;

  bool               m_seed      ;
  bool               m_match     ;
  bool               m_veloTT    ;
  bool               m_ksTrack   ;
  bool               m_tsa   ;
  bool               m_trgforward   ;

};

// ============================================================================
// The END
// ============================================================================
#endif // CALOUTILS_TRACKUSE_H
// ============================================================================
