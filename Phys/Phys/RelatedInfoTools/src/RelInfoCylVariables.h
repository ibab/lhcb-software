#ifndef RELINFOCYLVARIABLES_H 
#define RELINFOCYLVARIABLES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedInfoTool.h"
#include "Kernel/IDistanceCalculator.h"

class IDVAlgorithm;
typedef std::map <std::string,double> MapStringDouble;
typedef std::map <LHCb::Track*,MapStringDouble> MapTrackList;
typedef std::pair <double,LHCb::VertexBase*> PairDoubleVertex;

/** @class RelInfoCylVariables RelInfoCylVariables.h
 *
 *  @author Xabier Cid Vidal
 *  @date   2014-06-30
 */

class RelInfoCylVariables : public GaudiTool, virtual public IRelatedInfoTool 
{

public: 

  /// Standard constructor
  RelInfoCylVariables( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual StatusCode initialize();

  /// Loop over different cyl and fill the variables
  virtual StatusCode calculateRelatedInfo( const LHCb::Particle*
                                           , const LHCb::Particle*);

  virtual LHCb::RelatedInfoMap* getInfo();  

  virtual ~RelInfoCylVariables( ); ///< Destructor]

private:
  
  double m_cyl_size; // relinfocylinder size in units of docachi2
  bool m_useVelo; // use velo tracks
  bool m_trackQuality; // apply quality cuts on tracks
  bool m_samePV; // force for tracks same PV as the signal

  // track quality cuts (from stripping and Verle tool)
  // chi2cut [0, 3]
  double m_min_chi2 = 0;
  double m_max_chi2 = 3;
  // clonedistcut [5000, 9e+99 ]
  double m_min_clone = 5000;
  double m_max_clone = 9e+99;
  // MaxNVeloHoles 1
  double m_max_nveloholes = 1;
  
  // global mass definitions
  double m_mPi = 139.6;
  double m_mK = 493.7;
  double m_mKs = 497.6;
  double m_mB = 5000;
  double m_mD0 = 2000;

  MapStringDouble m_properties;
  LHCb::RelatedInfoMap m_map;
  
  // maps for all the related info for the cyl
  std::map <std::string,double> m_cyl;
  
  
  std::vector<std::string> m_variables;
  std::vector<short int> m_keys; 

  void getBestPV(const LHCb::Track* track, LHCb::VertexBase* & pv, double & ipchi2);

  void varSorted(const MapTrackList mycyl, const std::string vartype,
                 LHCb::Track* & out_track, 
                 double & out_min, double & out_max, double & out_av);
  
  void varSortedB(const MapTrackList mycyl, LHCb::Track* & out_track, double & out_val);

  void tracksInsideCyl(const LHCb::Track*& mytrack, const int mypvkey, 
                       MapTrackList& out_list);
  
  double invariantMass(const LHCb::Track* tr1, const LHCb::Track* tr2,
                       const double m1, const double m2);
  
  void addToParList(const LHCb::Track* tr1,const LHCb::Track* tr2,
                    const double m1, const double m2, const bool isKs,
                    std::vector< std::pair<double,double> > & myl);
  
  void getBestD0Ks(const MapTrackList & mycyl);

  MapStringDouble fillVar(MapTrackList mycyl,std::string vartype); // fill a var from the cylinder

  void fillProperties(MapTrackList mycyl); //fill all the vars from the cylinder
  void emptyProperties(void); // initialize and empty cylinder

  void fillMap(); // fill the output map
  
};
  
#endif // RELINFOCYLVARIABLES_H
