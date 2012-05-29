// $Id: $
#ifndef JETUTILS_H
#define JETUTILS_H 1
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "LoKi/Algo.h"
#include "LoKi/Vertices.h"
#include "LoKi/VertexCast.h"
#include "LoKi/Particles.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
//#include <Kernel/IPhysDesktop.h>
#include "LoKi/ParticleContextCuts.h"
#include "LoKi/LoKiAlgoMC.h"

#include "LoKi/GenTypes.h"
#include "LoKi/GenParticleCuts.h"

// ============================================================================
 // Include files
 // ============================================================================
 // Event
 // ============================================================================
 #include "Event/HepMCEvent.h"
 // ============================================================================
 // LHCbKernel
 // ============================================================================
 #include "Kernel/ParticleID.h"
 #include "Kernel/iNode.h"
 // ============================================================================
 // LoKi
 // ============================================================================
 #include "LoKi/Constants.h"
 #include "LoKi/Kinematics.h"
 #include "LoKi/BasicFunctors.h"
 #include "LoKi/Functions.h"
 #include "LoKi/iTree.h"


// ============================================================
// JetMaker.h
// ============================================================
//#include "LoKiFastJetMaker.h"
#include <Kernel/IJetMaker.h>
// Trigger Tool (Tistos)
#include "Kernel/ITriggerTisTos.h"
// ===========================================================================

// $Id: $  -*- C++ -*-                                                        
// ============================================================================
// Include files                                                               
#include <string>
#include <fstream>
#include <vector>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IAccept.h"
#include "Event/HltObjectSummary.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"

// =============================================================================
/// avoid long names:
//using namespace LoKi        ;
//using namespace LoKi::Types ;
//using namespace LoKi::Cuts  ;
//using namespace LoKi::Particles  ;
//using namespace LoKi::Vertices  ;
//using namespace LHCb;

namespace LoKi {
	namespace JetUtils {

		void getHltObjP4(const LHCb::HltObjectSummary *obj,Gaudi::LorentzVector &p4);
                void getHltObjSVPos(const LHCb::HltObjectSummary *obj, double &x, double &y, double &z);
		double getHltObjsAngle(const LHCb::HltObjectSummary *obj1, const LHCb::HltObjectSummary *obj2);
		double getHltObjsMass(const LHCb::HltObjectSummary *obj1, const LHCb::HltObjectSummary *obj2);
		bool addLHCbIDs(SmartRefVector< LHCb::HltObjectSummary >::const_iterator iter, std::vector<int> &ids);
		void getLHCbIDs(const LHCb::HltObjectSummary *obj, std::vector<int> &ids);
		bool doHltObjsOverlap(const LHCb::HltObjectSummary *obj1, const LHCb::HltObjectSummary *obj2);
		bool doLHCbIDsOverlap(const std::vector<int> &ids1, const std::vector<int> &ids2);

		class MCNEndVertices : public LoKi::Types::MCFunc 
		{
			public:
				virtual  MCNEndVertices* clone() const ;
				virtual ~MCNEndVertices() ;
				virtual  result_type operator() ( argument p ) const ;
				virtual  std::ostream& fillStream( std::ostream& s ) const ;
		};

		class GAUDI_API GNDaughters : public LoKi::GenTypes::GFunc 
		{
			public:
				virtual  GNDaughters* clone() const ;
				virtual ~GNDaughters() ;
				virtual  result_type operator() ( argument p ) const ;
				virtual  std::ostream& fillStream( std::ostream& s ) const ;
		};

		class Info4Jet : public LoKi::Types::Func 
		{
			public:
				virtual  Info4Jet* clone() const ;
				virtual ~Info4Jet() ;
				virtual  result_type operator() ( argument p ) const ;
				virtual  std::ostream& fillStream( std::ostream& s ) const ;
		};

		class CaloPositionX : public LoKi::Types::Func 
		{
			public:
				virtual  CaloPositionX* clone() const ;
				virtual ~CaloPositionX() ;
				virtual  result_type operator() ( argument p ) const ;
				virtual  std::ostream& fillStream( std::ostream& s ) const ;
		};

		class CaloPositionY : public LoKi::Types::Func 
		{
			public:
				virtual  CaloPositionY* clone() const ;
				virtual ~CaloPositionY() ;
				virtual  result_type operator() ( argument p ) const ;
				virtual  std::ostream& fillStream( std::ostream& s ) const ;
		};

		class CaloPositionZ : public LoKi::Types::Func 
		{
			public:
				virtual  CaloPositionZ* clone() const ;
				virtual ~CaloPositionZ() ;
				virtual  result_type operator() ( argument p ) const ;
				virtual  std::ostream& fillStream( std::ostream& s ) const ;
		};


		}
	}



namespace LoKi 
	{
	   namespace Cuts 
	   {
	       /** @var MYFUN 
	          * put some reasonable comment here and example of the code
	          *  @code
	          *  ...
	          *  @endcode
	          *  @see LoKi::XXX::MyFunctor
	          *  @date    2009-04-07
	          *  @author Xxx Yyyy  ZZZ@aaa.bbb   
	          */
	          const LoKi::JetUtils::Info4Jet    INFO4JET ;
	          const LoKi::JetUtils::GNDaughters    GNDAUGHTERS ;
	          const LoKi::JetUtils::CaloPositionX    CALOPOSITIONX ;
	          const LoKi::JetUtils::CaloPositionY    CALOPOSITIONY ;
	          const LoKi::JetUtils::CaloPositionZ    CALOPOSITIONZ ;
	          const LoKi::JetUtils::MCNEndVertices   MCNENDVERTICES;
	   }
	}

#endif /*JETUTILS_H*/ 
