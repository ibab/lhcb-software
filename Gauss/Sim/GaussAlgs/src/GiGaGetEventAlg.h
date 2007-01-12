// $Id: GiGaGetEventAlg.h,v 1.2 2007-01-12 15:23:41 ranjard Exp $
#ifndef    GIGA_GIGAMISCGETEVENTALG_H 
#define    GIGA_GIGAMISCGETEVENTALG_H 1 

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class GiGaMiscGetEventAlg GiGaMiscGetEventAlg.h 
 *
 *  Almost empty/primitive algorithm to trigger GiGa 
 *
 *  @author  Vanya Belyaev
 *  @author  Gloria Corti
 *  @date    2001-02-21, last modified 2007-01-11
 */
class GiGaGetEventAlg : public GaudiAlgorithm 
{
public:
  
  /// Standard constructor
  GiGaGetEventAlg( const std::string& name, ISvcLocator* SvcLoc ); 
  
  virtual ~GiGaGetEventAlg();      ///< Destructor 
  
  virtual StatusCode execute();    ///< Algorithm execution
  
private:

  std::string    m_particles;    ///< location in TES of MCParticles
  std::string    m_vertices;     ///< location in TES of Vertices
};

#endif  // GIGA_GIGAMISCGETEVENTALG_H 
