// $Id: GiGaInputStream.h,v 1.1 2002-09-26 18:10:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:20:53  ibelyaev
//  Vanya: update for newer versions of Gaudi and Geant4
// 
// ============================================================================
#ifndef GIGA_GIGAINPUTSTREAM_H 
#define GIGA_GIGAINPUTSTREAM_H 1
// ============================================================================
// include
// local (GiGa) 
#include "GiGaStream.h"
// forward decclaartion 
template <class ALGORITHM> class AlgFactory ; 

/** @class GiGaInputStream GiGaInputStream.h component/GiGaInputStream.h
 *
 *  Input stream for GiGa 
 *
 *  @author Ivan Belyaev
 *  @date   15/01/2002
 */

class GiGaInputStream : virtual public GiGaStream
{
  ///
  friend class AlgFactory<GiGaInputStream>;
  ///
public:
 
  virtual StatusCode execute   ();    ///< Algorithm execution
  
protected:
  
  /** Standard constructor
   *  @param name stream(algorithm) name 
   *  @param pSvcLoc pointer to the Service Locator 
   */ 
  GiGaInputStream( const std::string& name     , 
                   ISvcLocator*       pSvcLoc  ) ;
  
  /** destructor 
   */
  virtual ~GiGaInputStream(); 
  
private:
  
  GiGaInputStream();
  GiGaInputStream           ( const GiGaInputStream& );
  GiGaInputStream& operator=( const GiGaInputStream& );

};

// ============================================================================
// The End 
// ============================================================================
#endif // GIGA_GIGAINPUTSTREAM_H
// ============================================================================
