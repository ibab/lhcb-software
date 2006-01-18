// $Id: PrintEventAlg.h,v 1.3 2006-01-18 09:12:46 gcorti Exp $
#ifndef    GaussAlgs_PrintEventAlg_H 
#define    GaussAlgs_PrintEventAlg_H 1 

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declaration 
template <class ALGORITHM>
class AlgFactory            ; ///< GaudiKernel 

/** @class PrintEventAlg.h 
 *
 *  Almost empty/primitive algorithm to print debugging info for
 *  MC event
 *
 *  @author  Witold Pokorski, G.Corti
 *  @date    2002-12-09, mod. 2006-01-16
 */
class PrintEventAlg: 
  public GaudiAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<PrintEventAlg>;
  
public:

  /** the standard Algorithm initialization method 
   *  @see Algorithm
   *  @return status code
   */
  virtual StatusCode initialize ();

  /** the standard Algorithm execution method 
   *  @see Algorithm
   *  @return status code
   */
  virtual StatusCode execute    ();

  /** the standard Algorithm finalization method 
   *  @see Algorithm
   *  @return status code
   */
  virtual StatusCode finalize   ();
  
protected:
  
  /** standard constructor
   *  @param name name of teh algorithm 
   *  @param SvcLoc pointer to service locator 
   */
  PrintEventAlg( const std::string& name   , 
                       ISvcLocator*       SvcLoc ); 

  /** destructor 
   */
  virtual ~PrintEventAlg();
  
private:
  
  /** default constructor is private! 
   */
  PrintEventAlg();

  /** copy  constructor is private! 
   *  @param copy object to be copied  
   */
  PrintEventAlg
  ( const PrintEventAlg& copy );

  /** assignement operator is private! 
   *  @param copy object to be copied  
   */
  PrintEventAlg& operator= 
  ( const PrintEventAlg& copy );
  
private:

  std::string    m_particles;   ///< Location in TES of MCParticles
  std::string    m_vertices;    ///< Location in TES of MCVertices

  long m_depth;
  int  m_licznik;
  int  m_liczevent;

  /** recursive method to print a decay tree from an MCParticle
   * @param depth  depth of tree to reach in printout
   * @param prefix prefix to add to a given line for an MCParticle
   * @param part   pointer to MCParticle to print
   */   
  void printDecayTree(long depth, const std::string& prefix,
                      const LHCb::MCParticle*);
  
  IParticlePropertySvc* m_ppSvc; ///<< Reference to particle property service

};
#endif  // GaussAlgs_PrintEventAlg_H
