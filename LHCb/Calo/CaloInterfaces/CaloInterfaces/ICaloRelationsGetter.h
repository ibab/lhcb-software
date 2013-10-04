#ifndef ICALORELATIONSGETTER_H 
#define ICALORELATIONSGETTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "CaloUtils/Calo2Track.h"

static const InterfaceID IID_ICaloRelationsGetter ( "ICaloRelationsGetter", 1, 0 );

/** @class ICaloRelationsGetter ICaloRelationsGetter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2013-10-04
 */
class ICaloRelationsGetter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloRelationsGetter; }


  virtual LHCb::Calo2Track::ITrHypoTable2D* getTrHypoTable2D(std::string location)=0;
  virtual LHCb::Calo2Track::IHypoEvalTable* getHypoEvalTable(std::string location)=0;
  virtual LHCb::Calo2Track::IClusTrTable*   getClusTrTable  (std::string location)=0;


protected:

private:



};
#endif // ICALORELATIONSGETTER_H
