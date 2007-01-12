// $Id: GiGaOutputStream.h,v 1.2 2007-01-12 15:36:52 ranjard Exp $
#ifndef GIGA_GIGAOutputSTREAM_H 
#define GIGA_GIGAOutputSTREAM_H 1

// include
// local (GiGa) 
#include "GiGaStream.h"
// forward decclaartion 
//template <class ALGORITHM> class AlgFactory ; 

/** @class GiGaOutputStream GiGaOutputStream.h component/GiGaOutputStream.h
 *
 *  Output stream for GiGa 
 *
 *  @author Ivan Belyaev
 *  @date   15/01/2002
 */

class GiGaOutputStream : virtual public GiGaStream
{
  ///
  //  friend class AlgFactory<GiGaOutputStream>;
  ///
public:
 
  virtual StatusCode execute   ();    ///< Algorithm execution
  
  //protected:
  
  /** Standard constructor
   *  @param name stream(algorithm) name 
   *  @param pSvcLoc pointer to the Service Locator 
   */ 
  GiGaOutputStream( const std::string& name     , 
                   ISvcLocator*       pSvcLoc  ) ;
  
  /** destructor 
   */
  virtual ~GiGaOutputStream(); 
  
private:
  
  GiGaOutputStream();
  GiGaOutputStream           ( const GiGaOutputStream& );
  GiGaOutputStream& operator=( const GiGaOutputStream& );

};

// ============================================================================
// The End 
// ============================================================================
#endif // GIGA_GIGAOutputSTREAM_H
// ============================================================================
