// ============================================================================
#ifndef GIGA_GDMLRUNACTION_H 
#define GIGA_GDMLRUNACTIO_H 1
// ============================================================================
// GiGa
#include "GiGa/GiGaRunActionBase.h"
// forward declarations
template <class TYPE> class GiGaFactory;


class GDMLRunAction: public virtual GiGaRunActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<GDMLRunAction>;
  
public:

  
  /** performe the action at the begin of each run 
   *  @param run pointer to Geant4 run object 
   */
  virtual void BeginOfRunAction ( const G4Run* run );
  
  /** performe the action at the end  of each event 
   *  @param run pointer to Geant4 run object 
   */
  virtual void EndOfRunAction   ( const G4Run* run );
  
protected:
  
  /** standard constructor 
   *  @see GiGaPhysListBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GDMLRunAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  // destructor (virtual and protected)
  virtual ~GDMLRunAction( );
  
private:
  
  ///no default constructor
  GDMLRunAction();
  /// no copy constructor 
  GDMLRunAction( const GDMLRunAction& );  
  /// no assignement 
  GDMLRunAction& operator=( const GDMLRunAction& );

private:

  bool m_test;

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_GIGARUNACTIONCOMMAND_H
// ============================================================================
