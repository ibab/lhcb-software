// $Id: $
#ifndef NBB2HHTRIGGERTOOL_H 
#define NBB2HHTRIGGERTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// plots
#include "GaudiAlg/IHistoTool.h"


// NeuroBayes (only on linux with gcc for the moment)
#ifdef __GNUC__
#include "NeuroBayesExpert.hh"
#include "nb_param.hh"
#endif


/** @class NBB2HHTriggerTool NBB2HHTriggerTool.h
 *  
 *
 *  @author Ulrich Kerzel
 *  @date   2011-02-16
 */
class NBB2HHTriggerTool : public extends1<GaudiTool,IParticleFilter>{

  friend class ToolFactory<NBB2HHTriggerTool>; ///< friend factory

public: 

  /** initialize tool */
  virtual StatusCode initialize();

  /** finalize tool */
  virtual StatusCode finalize() {return GaudiTool::finalize();}

  /** performs the filtering based on the output of NeuroBayes
   *  @see IParticleFilter
   */
  virtual bool operator()(const LHCb::Particle* p) const ;


protected:

  /** standard constructor
   *  @param type the actual tool type (?)
   *  @param name the tool instance name
   *  @param parent the tool parent
   */
  NBB2HHTriggerTool(const std::string& type, const std::string& name,
                    const IInterface* parent);

  /// virtual & protected destructor
  virtual ~NBB2HHTriggerTool(){};


private:

  /// default constructor is disabled
  NBB2HHTriggerTool();

  /// copy constructor is disabled
  NBB2HHTriggerTool(const NBB2HHTriggerTool&);

  /// assignemet operator is disabled
  NBB2HHTriggerTool& operator=(const NBB2HHTriggerTool&);

  /// local
  bool getInputVar(const LHCb::Particle* particle) const;

  // properties
  std::vector<float>          m_Expertise;
  float                       m_NetworkCut;
  bool                        m_DoPrecuts;
  bool                        m_UsePID;
  bool                        m_DoPlot;

  // attributes
  const IDistanceCalculator*  m_DistCalc; ///< LoKi::DistanceCalculator
  int                         m_nVar;     ///< number of variables 
  IHistoTool*                 m_HistoTool;


  float*   m_inArray;

#ifdef __GNUC__
  Expert*  m_NBExpert;
#endif


};
#endif // NBB2HHTRIGGERTOOL_H
