// $Id: OrderedTaggingTool.h,v 1.1.1.1 2002-05-23 23:25:51 gcorti Exp $
#ifndef ORDEREDTAGGINGTOOL_H 
#define ORDEREDTAGGINGTOOL_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

#include "FlavourTagging/IFlavourTaggingTool.h"

/** @class OrderedTaggingTool OrderedTaggingTool.h
 *  
 *
 *  @author Olivier Dormond
 *  @date   24/04/2002
 */
class OrderedTaggingTool : public AlgTool,
                           virtual public IFlavourTaggingTool {
public:
  /// Standard constructor
  OrderedTaggingTool( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~OrderedTaggingTool( ); ///< Destructor

  virtual StatusCode initialize();

  /// Implementation of the ITaggingTool Interface
  virtual void tagThisB( const Particle &theB,
                         const Particles &theEvent,
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

private:
  std::vector<std::string> m_taggingTools_names;
  std::vector<IFlavourTaggingTool *> m_taggingTools;
};
#endif // ORDEREDTAGGINGTOOL_H
