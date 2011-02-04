// $Id: Jets2Jets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IJets2Jets.h"            // Interface
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Geometry.h"

#include "Relations/Relation1D.h"
#include "Relations/Relation2D.h"


#include "LoKi/ILoKiSvc.h"
#include "LoKi/Interface.h"


#include "LoKi/LoKiPhys.h"

namespace LoKi
{
  class Jets2Jets
    : public virtual IJets2Jets
    , public         GaudiTool
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class ToolFactory<LoKi::Jets2Jets> ;
  public: 

    // ========================================================================
    /// standard initialization of the tool
    StatusCode initialize() ;
    const IJets2Jets::Table makeRelation( const IJets2Jets::Jets& StdPrimaryJets  , const IJets2Jets::Jets& StdSecondaryJets ) const ;
  protected:
    // ========================================================================
    /// standard constructor
    Jets2Jets 
    ( const std::string& type   , ///< tool type ??? 
      const std::string& name   , ///< tool name 
      const IInterface*  parent ) 
      : GaudiTool ( type, name , parent )
    {
      //
      declareInterface<IJets2Jets>(this);
      //
    } 
    /// virtual protected destructor 
  virtual ~Jets2Jets() {} 
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled 
    Jets2Jets() ;
    // copy constructor is disabled 
    Jets2Jets( const Jets2Jets& ) ;
    // assignement operator is disabled 
    Jets2Jets& operator=( const Jets2Jets& ) ;
    // ========================================================================
  private:
    // ======================================================================== 
    std::string         m_output            ;///< Output relation table (1D) from partonics jet to StdHepMCJets to use as reference
    
    // ========================================================================
  };
}
 // end of namespace LoKi 


// ============================================================================
// standard initialization of the tool 
// ============================================================================
StatusCode LoKi::Jets2Jets::initialize() 
{
  // locate LoKi service (needed for some functions) 
  LoKi::ILoKiSvc* loki = svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;
  
  if ( 0 == loki ) { return StatusCode::FAILURE ; }
  return StatusCode::SUCCESS ; 
}


const IJets2Jets::Table LoKi::Jets2Jets::makeRelation( const IJets2Jets::Jets& StdPrimaryJets  , const IJets2Jets::Jets& StdSecondaryJets ) const 
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  typedef std::vector<const LHCb::Particle*> Parts;

  // create the relation table and register it into TES 
  IJets2Jets::Table* table = new IJets2Jets::Table() ;
  // select
  for( LHCb::Particles::const_iterator secjet =StdSecondaryJets.begin() ; StdSecondaryJets.end()!= secjet ; secjet++ )
  { 
    for(LHCb::Particles::const_iterator primjet =StdPrimaryJets.begin() ; StdPrimaryJets.end()!= primjet ; primjet++)
    {  
      double weight_jetsec_jetprim = 0.;
      Parts daug_secjet ;
      LoKi::Extract::getParticles (*secjet, std::back_inserter (daug_secjet), PID!="CELLjet");
      Parts daug_primjet ;
      LoKi::Extract::getParticles (*primjet, std::back_inserter (daug_primjet), PID!="CELLjet");
      for(Parts::iterator idaug_secjet = daug_secjet.begin() ; daug_secjet.end() != idaug_secjet   ; idaug_secjet++ )
      {
        for(Parts::iterator idaug_primjet = daug_primjet.begin() ; daug_primjet.end()!= idaug_primjet  ; idaug_primjet++ )
        { 
	  // both are charged and have track... compare tracking LHCbIDs
	  double sharedID = 0.;
	  const LHCb::Track* track_first = (*idaug_primjet)->proto()->track();
	  const LHCb::Track* track_second = (*idaug_secjet)->proto()->track();
	  if (track_first!= NULL && track_second!= NULL ){
	    const LHCb::Track::LHCbIDContainer idsFirst = track_first->lhcbIDs () ;
	    const LHCb::Track::LHCbIDContainer idsSec = track_second->lhcbIDs () ;
	    for (LHCb::Track::LHCbIDContainer::const_iterator i_idsFirst = idsFirst.begin()
		   ; idsFirst.end()!=i_idsFirst;i_idsFirst++){
	      for (LHCb::Track::LHCbIDContainer::const_iterator i_idsSec = idsSec.begin()
		   ; idsSec.end()!=i_idsSec;i_idsSec++){
		if ((*i_idsFirst)==(*i_idsSec)) sharedID += 1.;
	      }
	    }
	    if(idsFirst.size()<idsSec.size()) sharedID = sharedID/idsFirst.size();
	    else sharedID = sharedID/idsSec.size();
	  }
	  else if(track_first== NULL && track_second== NULL ){
	    const SmartRefVector< LHCb::CaloHypo > hypofirst = (*idaug_primjet)->proto()->calo();
	    if (hypofirst.size() == 0)continue;
	    const SmartRefVector< LHCb::CaloHypo > hyposecond = (*idaug_secjet)->proto()->calo();
	    if (hyposecond.size() == 0)continue;
	    const LHCb::CaloHypo::Digits  digitsfirst = (hypofirst[0]).data()->digits();
	    const LHCb::CaloHypo::Digits digitssecond = (hyposecond[0]).data()->digits();
	    for (LHCb::CaloHypo::Digits::const_iterator i_idsFirst = digitsfirst.begin()
		   ; digitsfirst.end()!=i_idsFirst;i_idsFirst++){
	      for (LHCb::CaloHypo::Digits::const_iterator i_idsSec = digitssecond.begin()
		   ; digitssecond.end()!=i_idsSec;i_idsSec++){        
          SmartRef<LHCb::CaloDigit> digit1= (*i_idsFirst);
          SmartRef<LHCb::CaloDigit> digit2= (*i_idsSec);
          if (digit1->cellID ()==digit2->cellID ()) sharedID += 1.;
	      }
	    }
	    if(digitsfirst.size()<digitssecond.size()) sharedID = sharedID/digitsfirst.size();
	    else sharedID = sharedID/digitssecond.size();
	  }
          // Check how many hits are shared for the tracks and how many calodigit are shared for the ...
          if(sharedID>0.9)
          {
            weight_jetsec_jetprim += E(*idaug_secjet)/E(*primjet);
            continue;
          }
        }
      }
      if(weight_jetsec_jetprim>0) table->relate(*primjet,*secjet,weight_jetsec_jetprim);
    }
  }
  
  return (*table) ;
}



// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,Jets2Jets);
// ============================================================================
// The END 
// ============================================================================
