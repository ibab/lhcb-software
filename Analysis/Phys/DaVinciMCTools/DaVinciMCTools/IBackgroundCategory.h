// $Id: IBackgroundCategory.h,v 1.3 2005-12-15 10:19:17 pkoppenb Exp $
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
 *  - Undefined : an error occured 
 *  - Signal    : this is the correct signal
 *  - QuasiSignal : a signal decay with some added intermediate resonance. 
 *		    This is a gray area and not currently implemented, hence
 *		    no distinction ismade between signal and quasi-signal
 *  - FullyRecoPhysBkg : A background which shares the final state particles 
 *			 of the signal decay, but has either a different mother
 *			 or a different topology. At present the topology check 
 *			 is not implemented.
 *  - Reflection : A background in which one or more final state particles are 
 *		   mis-identified.
 *  - PartRecoPhysBkg : A background in which the MC-true decay has an extra 
 *			particle when compared to the reconstructed background.
 *  - LowMassBkg : A sub-category of PartRecoPhysBkg, which has a systematically
 *			low mass as compared to the average of the head of the
 *			required signal decay.
 *  - Ghost : One or more final state particles has no MC-truth partner
 *  - FromSameNonRecoPV : An event where the reconstructed B is in fact a primary
 *			  vertex which was not reconstructed by the primary
 *			  vertex fitter. Not implemented yet.
 *  - FromSamePV : An event where at least one final state particle comes from 
 *		   a primary vertex (if more than one, they come from the same PV).
 *		   Not yet implemented.
 *  - FromDifferentNonRecoPV : A pileup where at least one final state particle 
 *				comes from a primary vertex which was not 
 *				reconstructed by the primary vertex fitter. Not 
 *				implemented yet.
 * -  FromDifferentPV : Bog standard pileup.
 * -  bbar : combinatorics where at least one final state particle has a mother 
 *		  which is a b-hadron.
 * -  ccbar : combinatorics where at least one final state particle has a mother
 *	  	which is a c-hadron.
 * -  light-flavour : any other combinatorics.
 *   
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
  /// For a signal it returns the true signal particle, and for any background with a 
  /// category < 60 it returns the mc_mother of the final state particles
  virtual const MCParticle* origin( const Particle* ) = 0 ;
                  

protected:

private:

};
#endif // DAVINCIMCTOOLS_IBACKGROUNDCATEGORY_H
