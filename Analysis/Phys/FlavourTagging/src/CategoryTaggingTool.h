// $Id: CategoryTaggingTool.h,v 1.1 2003-06-13 08:34:34 odie Exp $
#ifndef CATEGORYTAGGINGTOOL_H 
#define CATEGORYTAGGINGTOOL_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <map>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

#include "FlavourTagging/IFlavourTaggingTool.h"

class IDataProviderSvc;

/** @class CategoryTaggingTool CategoryTaggingTool.h
 *  
 *
 *  @author Olivier Dormond
 *  @date   24/04/2002
 */
class CategoryTaggingTool : public AlgTool,
                           virtual public IFlavourTaggingTool {
public:
  /// Standard constructor
  CategoryTaggingTool( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~CategoryTaggingTool( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /// Implementation of the ITaggingTool Interface
  virtual void tagThisB( const Particle &theB,
                         const Particles &theEvent,
                         const Vertex &thePrimVtx,
                         FlavourTag &theTag );
  virtual void tagThisB( const Particle &theB,
                         const ParticleVector &theEvent,
                         const Vertex &thePrimVtx,
                         FlavourTag &theTag );
  virtual void tagFromList( const Particle &theB,
                            const ParticleVector &theCandidates,
                            const Vertex &thePrimVtx,
                            FlavourTag &theTag );
  virtual void tagExcludingFromList( const Particle &theB,
                                     const Particles &theEvent,
                                     const ParticleVector &theExcluded,
                                     const Vertex &thePrimVtx,
                                     FlavourTag &theTag );
  virtual void tagExcludingFromList( const Particle &theB,
                                     const ParticleVector &theEvent,
                                     const ParticleVector &theExcluded,
                                     const Vertex &thePrimVtx,
                                     FlavourTag &theTag );
private:
	IFlavourTaggingTool *getTT(std::string name);
	void combine( const FlavourTag &mu_tag,
		 						const FlavourTag &e_tag,
								const FlavourTag &kos_tag,
								const FlavourTag &kss_tag,
								//const	FlavourTag &vtx_tag,
								FlavourTag &theTag );
	void fillCategory(std::string categ, const FlavourTag &theTag);
	
	class Category {
		public:
			Category( void )
			{
			 	counts[FlavourTag::b]    = 0;
				counts[FlavourTag::bbar] = 0;
				counts[FlavourTag::none] = 0;
			};
			void add( FlavourTag::TagResult tag ) { counts[tag]++; };
			unsigned int count( FlavourTag::TagResult tag ) { return counts[tag]; };
			void results( unsigned int nevents, double &epsilon, double &sepsilon,
									    									  double &omega,   double &somega,
											    							  double &ed2,     double &sed2 )
			{
				double b    = double(counts[FlavourTag::b]);
				double bbar = double(counts[FlavourTag::bbar]);
				epsilon = (b+bbar)/nevents;
				omega = b/(b+bbar);
				ed2 = epsilon*pow(1-2*omega, 2);
				sepsilon = sqrt(epsilon*(1-epsilon)/nevents);
				somega = sqrt(omega*(1-omega)/(b+bbar));
				sed2 = sqrt(ed2/nevents*(4-ed2*(1+3/ed2)));
			};
		private:
			std::map<FlavourTag::TagResult,unsigned int> counts;
	};
	
	IDataProviderSvc *m_eventSvc;
	unsigned int m_nUntrigEvents;
  unsigned int m_nL0Events;
  unsigned int m_nL0L1Events;
  std::string m_muonTTN;
  std::string m_electronTTN;
  std::string m_kaonOSTTN;
  std::string m_kaonSSTTN;
  std::string m_vtxChargeTTN;
  IFlavourTaggingTool *m_muonTT;
  IFlavourTaggingTool *m_electronTT;
  IFlavourTaggingTool *m_kaonOSTT;
  IFlavourTaggingTool *m_kaonSSTT;
  IFlavourTaggingTool *m_vtxChargeTT;
	std::map<std::string,Category> m_untrigCateg;
	std::map<std::string,Category> m_L0Categ;
	std::map<std::string,Category> m_L0L1Categ;
};
#endif // CATEGORYTAGGINGTOOL_H
