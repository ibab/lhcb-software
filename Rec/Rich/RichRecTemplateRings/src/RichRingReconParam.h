// $Id: RichRingReconParam.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGRECONPARAM_H 
#define RICHRINGRECONPARAM_H 1

// Include files
// from Gaudi
#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichRingReconParam.h"            // Interface


/** @class RichRingReconParam RichRingReconParam.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-12
 */
class RichRingReconParam : public RichRingRecToolBase, 
                           virtual public IRichRingReconParam {
public: 
  /// Standard constructor
  RichRingReconParam( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~RichRingReconParam( ); ///< Destructor


  double YAgelShift()  {  return m_YAgelShift;}
  int MinRadiator()  {  return m_MinRadiator;}
  int MaxRadiator()  {  return m_MaxRadiator;}
  bool  ActivateSingleEvHisto(){  return  m_ActivateSingleEvHisto;}
  int MinTrackNumForDebug(){    return m_MinTrackNumForDebug;}
  int MaxTrackNumForDebug(){    return m_MaxTrackNumForDebug;}
  bool ActivateSingleTrackDebug()  {  return m_ActivateSingleTrackDebug;}
  std::string RingRecHistoPath()  {  return m_RingRecHistoPath;}
  bool ActivateRandomPhiLocalScaling()  {  return m_ActivateRandomPhiLocalScaling;}
  bool ActivateMCCompareMassPrint()  {  return m_ActivateMCCompareMassPrint;}
  
  
  

  //  bool WriteOutAuxiliaryDebugHisto()  {    return m_WriteOutAuxiliaryDebugHisto;}
  // std::string RichDebugAuxHistoFileName()  {    return m_RichDebugAuxHistoFileName;}
  
  StatusCode  initialize();
  
  
protected:

private:


  double m_YAgelShift;


  int  m_MinRadiator;
  int  m_MaxRadiator;
  bool m_ActivateSingleEvHisto;
  int m_MinTrackNumForDebug;
  int m_MaxTrackNumForDebug;
  bool m_ActivateSingleTrackDebug;
  std::string m_RingRecHistoPath;

  //  bool m_WriteOutAuxiliaryDebugHisto;
  // std::string m_RichDebugAuxHistoFileName;
  bool m_ActivateRandomPhiLocalScaling;
  bool m_ActivateMCCompareMassPrint;
  
  
  
  
  
};
#endif // RICHRINGRECONPARAM_H
