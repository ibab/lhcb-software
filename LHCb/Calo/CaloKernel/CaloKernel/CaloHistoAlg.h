// $Id: CaloHistoAlg.h,v 1.1 2004-02-17 11:55:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOKERNEL_CALOHISTOALG_H 
#define CALOKERNEL_CALOHISTOALG_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiHistoAlg.h"
// ============================================================================


/** @class CaloHistoAlg CaloHistoAlg.h CaloKernel/CaloHistoAlg.h
 *
 *  Small extention of class GaudiHistoAlg:
 *
 *   - defines 'standard' calorimeter properties 
 *   - set reasonable default values for some properties 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-16
 */
class CaloHistoAlg : public GaudiHistoAlg 
{
public:
  
  /// the actual type for container of 'addresses' 
  typedef std::vector<std::string> Inputs ;

public:
  
  /** standard initialization method
   *  @see  GaudiHistoAlg
   *  @see  GaudiAlgorithm
   *  @see       Algorithm
   *  @see      IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode initialize(); 
  
  /** standard finalization method
   *  @see  GaudiHistoAlg
   *  @see  GaudiAlgorithm
   *  @see       Algorithm
   *  @see      IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode finalize  ();
  
protected:
  
  /** Standard constructor
   *  @see GaudiHistoAlg 
   *  @see  GaudiAlgorithm
   *  @see       Algorithm
   *  @param name           name of the algorithm
   *  @param pSvcLocator    pointer to Service Locator 
   */
  CaloHistoAlg ( const std::string& name         , 
                 ISvcLocator*       pSvcLocator ) ;
  
  /// destructor 
  virtual ~CaloHistoAlg( ); 
  
private:
  
  // default constructor  is disabled 
  CaloHistoAlg();
  // copy    constructor  is disabled 
  CaloHistoAlg           ( const  CaloHistoAlg& ) ;  
  // assigenment operator is disabled 
  CaloHistoAlg& operator=( const  CaloHistoAlg& ) ;
  
protected: ///< "technical" methods
  
  /// address/location/name in Transient Store of input  data container 
  const std::string& inputData  () const { return m_inputData  ; }
  
  /// vector of input  data containers 
  const Inputs&       inputs    () const { return m_inputs     ; }
  
  /// address/location/name in Transient Store of output data container 
  const std::string& outputData () const { return m_outputData ; }
  
  /// address/location/name in Transient Store of detector data  
  const std::string& detData    () const { return m_detData    ; }
  
  /** set address/location/name in Transient Store of input  data container 
   *  @param  addr address/location/name in Transient Store of input data 
   */
  void setInputData ( const std::string& addr ) { m_inputData  = addr; }
  
  /** set input  data containers  
   *  @param  addr vector of  addresses in Transient Store of input data 
   */
  void setInputs    ( const Inputs&      addr ) { m_inputs     = addr  ; }
  
  /** add address/location/name in Transient Store of input  data container 
   *  @param  addr address/location/name in Transient Store of input data 
   */
  void addToInputs  ( const std::string& addr ) { m_inputs.push_back( addr ) ; }
  
  /** set address/location/name in Transient Store of output  data container 
   *  @param  addr address/location/name in Transient Store of output data 
   */
  void setOutputData( const std::string& addr ) { m_outputData = addr; }  
  
  /** set address/location/name in Transient Store of detector data  
   *  @param  addr address/location/name in Transient Store of detector 
   */
  void setDetData   ( const std::string& addr ) { m_detData  = addr; }
  
private:
  
  std::string      m_inputData  ;
  // vector of addresses in the case of 'few' inputs 
  Inputs           m_inputs     ;
  // address/location/name in Transient Store of output data container 
  std::string      m_outputData ;
  // address/location/name in Transient Store of detector data  
  std::string      m_detData    ;

};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOKERNEL_CALOHISTOALG_H
// ============================================================================
