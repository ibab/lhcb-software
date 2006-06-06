// $Id: CaloTrackAlg.h,v 1.4 2006-06-06 11:59:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOUTILS_CALOTrackAlg_H 
#define CALOUTILS_CALOTrackAlg_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GausiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/TrackUse.h"
// ============================================================================

// ============================================================================
/** @class CaloTrackAlg CaloTrackAlg.h
 *  
 *  Helper base algorithm form implementation 'track'-related algorithms
 *
 *  It helps to select the appropriate tracks for further processing
 *  Track is 'selected' if it fullfills 
 *
 *   - general features
 *   - category 
 *   - "algorithm" 
 *
 *  @see TrackUse 
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date   2004-10-26
 */
// ============================================================================
class TrackUse;
class Track;

class CaloTrackAlg : public GaudiAlgorithm 
{
public:  
  /// standard algorithm initialization 
  virtual StatusCode initialize(); 
  /// standard algorithm finalization 
  virtual StatusCode finalize  ();
protected:
  /// Standard constructor
  CaloTrackAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  /// destructor (virtual and protected)
  virtual ~CaloTrackAlg();
private:
  /// default  construstor  is  private 
  CaloTrackAlg(); 
  /// copy     construstor  is  private 
  CaloTrackAlg
  ( const CaloTrackAlg& );
  /// assignement operator  is  private 
  CaloTrackAlg& operator=
  ( const CaloTrackAlg& );
protected:
  /// check if the track to be used @see TrackUse 
  inline bool use  ( const LHCb::Track* track ) const { return m_use( track) ; } 
  /// print the short infomration about track flags 
  inline MsgStream& print
  ( MsgStream&         stream , 
    const LHCb::Track* track  ) const ;
  /// print the short infomration about track flags 
  inline MsgStream& print 
  ( const LHCb::Track* track             , 
    const MSG::Level   level = MSG::INFO ) const ;
protected :
  // the actual object 
  TrackUse m_use ;
};
// ============================================================================
/// print the short infomration about track flags 
// ============================================================================
inline MsgStream& 
CaloTrackAlg::print ( MsgStream&         stream , 
                      const LHCb::Track* track  ) const 
{ return stream.isActive() ? m_use.print ( stream , track ) : stream ; }
// ============================================================================
/// print the short infomration about track flags 
// ============================================================================
inline MsgStream& 
CaloTrackAlg::print ( const LHCb::Track* track , 
                      const MSG::Level   level ) const 
{ return print ( msgStream ( level ) , track ) ; }
// ============================================================================

// ============================================================================
#endif // CALOTrackAlg_H
// ============================================================================
