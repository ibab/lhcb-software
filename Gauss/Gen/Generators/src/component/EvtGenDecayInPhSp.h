// $Id: EvtGenDecayInPhSp.h,v 1.9 2009-03-16 16:53:26 robbep Exp $
#ifndef GENERATORS_EVTGENDECAYINPHSP_H 
#define GENERATORS_EVTGENDECAYINPHSP_H 1

#include "EvtGenDecay.h"

/** @class EvtGenDecayInPhSp EvtGenDecayInPhSp.h "EvtGenDecayInPhSp.h"
 *  
 *  Tool to interface to EvtGen generator. Concrete implementation of 
 *  a IDecayTool and inherits from EvtGenDecay
 *
 *  @author Alex Shires
 *  @date   2011-08-16
 */
class EvtGenDecayInPhSp : public EvtGenDecay, virtual public IDecayTool {
public:
  /// Standard constructor
  EvtGenDecayInPhSp( const std::string& type, const std::string& name,
              const IInterface* parent);

  virtual ~EvtGenDecayInPhSp( ) ; ///< Destructor

  /** Initialize method.
   *  In initialization:
   *  -# Call EvtGenDecay::initialize()
   *  -# Read the parameter file
   */
  virtual StatusCode initialize() ;

  /// Finalize method
  virtual StatusCode finalize() ;

  /// Implements IDecayTool::generateDecay
  virtual StatusCode generateDecay( HepMC::GenParticle * theMother ) const ;

  /// Implements IDecayTool::generateSignalDecay
  virtual StatusCode generateSignalDecay( HepMC::GenParticle * theMother , 
                                          bool & flip ) const ;

 private:

  StatusCode doEvtGenLoop( EvtParticle* &thePart , 
                           HepMC::GenParticle * &theHepMCParticle ,
                           const EvtId & eid ) const ;
  
  /// load qsquared map
  StatusCode loadMap(void ) ;
  
  /// File name of parameter file
  boost::filesystem::path m_q2mapfile ;

  std::string m_q2mapfilename ;

  // number of bins in q2
  int m_q2nbins ;
  int m_q2nmax ;

  //vector for q2 values
  std::vector<double> m_q2mapvec ;

  //statistics
  double m_tot_calls ;
  double m_num_evts ;
  double m_ave_eff ;

  int m_limit ;
 
};


#endif // GENERATORS_EVTGENDECAYINPHSP_H 1
