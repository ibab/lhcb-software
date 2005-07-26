// $Id: IJetMaker.h,v 1.1 2005-07-26 16:26:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef DAVINCIKERNEL_IJETMAKER_H 
#define DAVINCIKERNEL_IJETMAKER_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward decalrations 
// ============================================================================
class Particle ;
// ============================================================================

/** @class IJetMaker IJetMaker.h Kernel/IJetMaker.h
 *
 *  The abstract interface to Jet Maker 
 *
 *  @attention It is a responsibility of users (e.g. the algorithm) 
 *             to take care about the ownership of jets *AND* their 
 *             vertices). The tool is not intended to do it! 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr 
 *  @date   2005-07-26
 */
class IJetMaker : public virtual IAlgTool 
{
public: 
  /// the actual type of input data container 
  typedef std::vector<const Particle*>   Input  ;  
  /// the actual type of output container of jets 
  typedef std::vector<Particle*>         Jets   ;  
public:
  
  /** The main method: jet-finding procedure 
   * 
   *  @code 
   *
   *  // get the tool
   *  const IJetMaker* jetMaker = tool<IJetMaker> ( .... ) ;
   *
   *  IJetMaker::Jets  jets ;
   *  IJetMaker::Inputs input = ... 
   *
   *  // find the jets! 
   *  StatusCode sc = jetMaker -> makeJets ( input , jets ) ;
   *
   *  @endcode 
   *
   *  @attention It is a responsibility of users (e.g. the algorithm) 
   *             to take care about the ownership of jets *AND* their 
   *             vertices). The tool is not intended to do it! 
   *  
   * 
   *  @param input contaainer of input particles 
   *  @param jets  container of  output jets 
   *  @return status code 
   */
  virtual StatusCode makeJets 
  ( const Input& input , Jets& jets ) const = 0 ;
  
  /** perform jet-finding procedute getting the input data from
   *  arbitrary sequence of data, convertible to "const Particle*"
   *
   *  @code 
   *  
   *  // get the tool
   *  const IJetMaker* jetMaker = tool<IJetMaker> ( .... ) ;
   *
   *  IJetMaker::Jets  jets ;
   *
   *  // get input data 
   *  const Particles* ps = get<Particles> ( ... ) ;
   * 
   *  // find the jets! 
   *  StatusCode sc = jetMaker -> makeJets ( ps -> begin () , 
   *                                         ps -> end   () , jets ) ;
   *
   *  @endcode 
   *  
   *  @attention It is a responsibility of users (e.g. the algorithm) 
   *             to take care about the ownership of jets *AND* their 
   *             vertices). The tool is not intended to do it! 
   *  
   * 
   *  @param first 'begin' iterator to the sequence of particles 
   *  @param last  'end'   iterator to the sequence of particles 
   *  @param jets  container of outptu jets 
   *  @return status code 
   */
  template <class PARTICLE>
  inline StatusCode makeJets 
  ( PARTICLE first ,
    PARTICLE last  , 
    Jets&    jets  ) const 
  { return makeJets ( Input( first , last )  , jets ) ; }
  
public:
  
  /// Return the unique interface identifier 
  static const InterfaceID& interfaceID() ;
  
protected:
  /// virtual and protected destructor 
  virtual ~IJetMaker();
};

// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IJETMAKER_H
// ============================================================================

