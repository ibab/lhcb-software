// $Id: CaloHypoAlg.h,v 1.6 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2009/08/21 16:48:11  odescham
// implement generic context-dependent TES I/O
//
// Revision 1.4  2008/06/30 15:36:33  odescham
// prepare for HLT processing
//
// Revision 1.3  2006/05/30 09:42:02  odescham
// first release of the CaloReco migration
//
// Revision 1.2  2005/11/07 12:12:42  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.1  2002/09/02 18:19:16  ibelyaev
//  new generic algorithm for applying CaloHypo tools to CaloHypo objects
//
// Revision 1.3  2002/06/15 12:53:13  ibelyaev
//  version update
//
// ============================================================================
#ifndef CaloReco_CaloHypoAlg_H 
#define CaloReco_CaloHypoAlg_H 1
// Include files
// from STL
#include <string>
#include <vector>

#include "GaudiAlg/GaudiAlgorithm.h"
// forward declaration
class ICaloHypoTool ;

/** @class CaloHypoAlg CaloHypoAlg.h
 *  
 *  Generic CaloHypo Algorithm.
 *  It is just applies a set of ICaloHypoTools 
 *  to a container of CaloHypo objects
 *  @see ICaloHypoTool
 *  @see  CaloHypo
 *  @see  CaloAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/09/2002
 */

class CaloHypoAlg : public GaudiAlgorithm 
{
  /// friend factory for instantiation
  friend struct AlgFactory<CaloHypoAlg>;

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
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloHypoAlg( const std::string& name   , 
              ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloHypoAlg();
  
private:

  /// default  construstor  is  private 
  CaloHypoAlg(); 
  /// copy     construstor  is  private 
  CaloHypoAlg
  ( const CaloHypoAlg& );
  /// assignement operator  is  private 
  CaloHypoAlg& operator=
  ( const CaloHypoAlg& );

private:
  
  typedef std::vector<std::string>    Names ;
  typedef std::vector<ICaloHypoTool*> Tools ;
  
  /// list of tool type/names 
  Names   m_names ;
  /// list of tools 
  Tools   m_tools ;
  std::string m_inputData;
  std::string m_type;
};
// ============================================================================
#endif // CaloHypoAlg_H
