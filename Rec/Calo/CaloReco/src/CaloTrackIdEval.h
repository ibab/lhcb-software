// $Id: CaloTrackIdEval.h,v 1.1.1.1 2002-11-13 20:46:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOTRACKIDEVAL_H 
#define CALOTRACKIDEVAL_H 1
// Include files
// CaloInterfaces 
#include "CaloInterfaces/ICaloTrackIdEval.h"
// CaloKernel 
#include "CaloKernel/CaloTool.h"

/** @class CaloTrackIdEval CaloTrackIdEval.h
 *  
 *
 *  @author Ivan Belyaev
 *  @date   2002-11-10
 */
class CaloTrackIdEval :
  public virtual ICaloTrackIdEval , 
  public          CaloTool        
{  
  
protected:
  
  /** Standard constructor
   *  @see   CaloTool
   *  @see    AlgTool 
   *  @param type    type of the tool  (?)
   *  @param name    name of the concrete instance 
   *  @param parent  pointer to parent object (algorithm, service xor tool)
   */
  CaloTrackIdEval
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  virtual ~CaloTrackIdEval(){}; ///< Destructor
  
private:
  
  /// the default constructor is private
  CaloTrackIdEval();
  /// the copy constructor is private 
  CaloTrackIdEval           ( const CaloTrackIdEval& );
  /// the assignement is private 
  CaloTrackIdEval& operator=( const CaloTrackIdEval& );

};

// ============================================================================
// The END
// ============================================================================
#endif // CALOTRACKIDEVAL_H
// ============================================================================
