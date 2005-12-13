// $Id: IBackgroundCategory.h,v 1.1 2005-12-13 16:18:25 pkoppenb Exp $
#ifndef DAVINCIMCTOOLS_IBACKGROUNDCATEGORY_H 
#define DAVINCIMCTOOLS_IBACKGROUNDCATEGORY_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
class Particle;
class MCParticle;

static const InterfaceID IID_IBackgroundCategory ( "IBackgroundCategory", 1, 0 );

/** @class IBackgroundCategory IBackgroundCategory.h pkoppenb/IBackgroundCategory.h
 *
 *  Returns the background category for a reconstructed candidate Particle  
 *
 *  The categories in the enum are:
 *    Undefined : an error occured 
 *    Signal    : this is the correct signal
 *    QuasiSignal : <some comment here>
 *    <...>
 *
 *  @author Vladimir Gligorov
 *  @date   2005-11-03
 */
class IBackgroundCategory : virtual public IAlgTool {
public: 

  /// enum defining categories. From 1000 on, specific tools can add codes.

  enum categories { Undefined = -1, 
                    Signal = 0,
                    QuasiSignal = 10,
                    FullyRecoPhysBkg = 20,
                    Reflection = 30,
                    PartRecoPhysBkg = 40,
                    LowMassBkg = 50,
                    Ghost = 60,
                    FromSameNonRecoPV = 70,
                    FromSamePV = 80,
                    FromDifferentNonRecoPV = 90,
                    FromDifferentPV = 100,
                    bbar = 110 ,
                    ccbar = 120 ,
                    uds = 130,
                    LastGlobal = 1000 } ;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBackgroundCategory; }

  /// Method that returns the category
  virtual IBackgroundCategory::categories category(const Particle*) = 0;
  
  /// Method that returns the MC mother particle associated to the background if any
  virtual const MCParticle* origin( const Particle* ) = 0 ;
                  

protected:

private:

};
#endif // DAVINCIMCTOOLS_IBACKGROUNDCATEGORY_H
