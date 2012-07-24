#ifndef GIGA_GDMLWRITERUNACTION_H 
#define GIGA_GDMLWRITERUNACTION_H 1

// GiGa
#include "GiGa/GiGaRunActionBase.h"

class GDMLRunAction: public virtual GiGaRunActionBase {

public:

  /// Constructor
  GDMLRunAction(const std::string& type,
                const std::string& name,
                const IInterface*  parent);
  /// Destructor
  virtual ~GDMLRunAction() {}

  /// Action to be performed at the begin of each run 
  virtual void BeginOfRunAction(const G4Run* run);
  
private:
  
  /// No default constructor
  GDMLRunAction();
  /// No copy constructor 
  GDMLRunAction(const GDMLRunAction&);  
  /// No assignement 
  GDMLRunAction& operator=(const GDMLRunAction&);

  bool m_done;
  /// Schema to be used for GDML
  std::string m_schema;
  /// Name of the file to write geometry           
  std::string m_outfile;          

};
#endif 
