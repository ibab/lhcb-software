// $Id: GiGaFlushAlgorithm.h,v 1.4 2007-01-12 15:23:41 ranjard Exp $
#ifndef GIGA_GIGAFLUSHALGORITHM_H 
#define GIGA_GIGAFLUSHALGORITHM_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
class IGiGaSvc               ; ///< GiGa


/** @class GiGaFlushAlgorithm GiGaFlushAlgorithm.h 
 *  
 *  Very simple algorithm to tigger the event processing 
 *
 *  @author Vanya Belyaev
 *  @author Gloria Corti
 *  @date   2002-01-22, last modified 2007-01-11
 */
class GiGaFlushAlgorithm : public GaudiAlgorithm 
{
public:
  
  /// Standard constructor
  GiGaFlushAlgorithm( const std::string& Name, ISvcLocator* SvcLoc );
  
  virtual ~GiGaFlushAlgorithm();  ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  /** accessor to GiGa Service
   *  @return pointer to GiGa Service
   */
  inline IGiGaSvc* gigaSvc() const { return m_gigaSvc; }
  
private:  

  std::string     m_gigaSvcName ;
  IGiGaSvc*       m_gigaSvc     ;

};

#endif // GIGA_GIGAFLUSHALGORITHM_H
