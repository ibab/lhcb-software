// $Id: GiGaSensDetBase.h,v 1.12 2007-10-03 15:21:09 gcorti Exp $
#ifndef GIGA_GIGASENSDETBASE_H
#define GIGA_GIGASENSDETBASE_H 1 

// Include files
// from GiGa
#include "GiGa/IGiGaSensDet.h" 
#include "GiGa/GiGaBase.h" 

// forward declaration
class G4step;
class G4TouchableHistory;

/** @class GiGaSensDetBase GiGaSensDetBase.h GiGa/GiGaSensDetBase.h
 *
 *  Base class for implementation of concrete Sensitive Detector for GiGa
 *  
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaSensDetBase: virtual public IGiGaSensDet ,
                       public GiGaBase
{

public:
  
  /** standard constructor   
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaSensDetBase ( const std::string& type   , 
                    const std::string& name   , 
                    const IInterface*  parent );

  /// virtual destructor 
  virtual ~GiGaSensDetBase();

public:

  /** initialize the sensitive detector  
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize(); 

  /** finalize the sensitive detector  
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize();

  /** Method for being a member of a GiGaSensDetSequence
   *  Implemented by base class, does not need reimplementation!
   */ 
  virtual bool processStep( G4Step* step,
                            G4TouchableHistory* history );
  
  virtual unsigned long release() ;

protected:

  GiGaSensDetBase( const GiGaSensDetBase& ); /// no copy constructor!
  GiGaSensDetBase& operator=( const GiGaSensDetBase& ); /// no assignment! 

private:

  bool                m_active  ;  ///< Active Flag
  std::string         m_detPath ;
};

#endif   // GIGA_GIGASENSDET_H

