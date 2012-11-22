#ifndef DECAYTREETUPLE_ITUPLETOOLDECAY_H 
#define DECAYTREETUPLE_ITUPLETOOLDECAY_H 1

// from STL
#include <string>

// Interfaces
#include "Kernel/IDecayFinder.h"
#include "MCInterfaces/IMCDecayFinder.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITupleToolDecay ( "ITupleToolDecay", 1, 0 );

/** @class ITupleToolDecay ITupleToolDecay.h DecayTreeTuple/ITupleToolDecay.h
 *  
 *  Interface for TupleToolDecay
 *
 *  @author Chris Jones
 *  @date   2012-11-22
 */

class ITupleToolDecay : virtual public IAlgTool 
{

public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITupleToolDecay; }

public:

  virtual std::string decay() const = 0;

  virtual std::string getInfo() const = 0;

  virtual void printInfo() const = 0;

  virtual StatusCode initializeDecay( const std::string&, bool ) = 0;

  virtual IDecayFinder* decayFinder() const = 0;

  virtual IMCDecayFinder* mcDecayFinder() const = 0;

  virtual const std::string& getName() const = 0;

  virtual const std::vector<std::string>& getStuffers() const = 0;

  virtual bool inheritTools() const = 0;

};

#endif // DECAYTREETUPLE_ITUPLETOOLDECAY_H
