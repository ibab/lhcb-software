// $Id: Algorithm2ID.h,v 1.1 2005-04-20 14:08:39 pkoppenb Exp $
#ifndef ALGORITHM2ID_H 
#define ALGORITHM2ID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IAlgorithm2ID.h"            // Interface


/** @class Algorithm2ID Algorithm2ID.h
 *  
 * Small tool that alows to assign an ID to an algorithm by options.
 *    ToolSvc.AlgorithmNames = { "DVAlg1", "DVAlg2" };
 * Since this tool is declared to DVAlgorithm, DVAlg1 will know
 * it is number 1, and DVAlg2 is 2. Essentially designed for HLT.   
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-04-15
 */
class AlgNamePair;
class Algorithm2ID : public GaudiTool, virtual public IAlgorithm2ID {
public: 
  /// Standard constructor
  Algorithm2ID( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~Algorithm2ID( ){}; ///< Destructor

  /// Return ID from name
  int idFromName( const std::string& );
 
  /// Return name from ID
  std::string nameFromId( const int& );

  StatusCode initialize();
 
protected:

private:

  class AlgNamePair{
  public:
    AlgNamePair(){};
    AlgNamePair(const int& id, const std::string name){
      set(id,name);
      return ;
    };
    std::string getAlgName()const {return m_algName;};
    int getID()const {return m_id;};
    void set(const int& id, const std::string name){
      m_algName = name ;
      m_id = id ;
      return ;
    };

  private:
    std::string m_algName;
    int m_id;
  };
  
  std::vector<AlgNamePair> m_pairs ;
  std::vector<std::string> m_names ;
  
    
};
#endif // ALGORITHM2ID_H
