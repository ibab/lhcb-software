// $Id: RichParameters.h,v 1.2 2001-10-26 13:02:12 rihill Exp $
#ifndef RICHREC_RICHPARAMETERS_H
#define RICHREC_RICHPARAMETERS_H 1

// Include files
#include <string>
#include <iostream>
#include <map>
#include <stdio.h>
using namespace std;

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Property.h"

/** @class RichParameters RichParameters.h RichDet/RichParameters.h
 *
 *
 *  @author Christopher Rob Jones
 *  @date   12/07/2001
 */

class RichParameters : public AlgTool {

public:    // Standard constructor
  RichParameters( const std::string& type, const std::string& name,
                  const IInterface* parent);

  virtual ~RichParameters();

  virtual int getValue (const std::string &key,
                        const int defValue) const;

  virtual double getValue (const std::string &key,
                           const double defValue) const;

  virtual std::string getValue (const std::string &key,
                                const std::string &defValue) const;

  virtual bool getValue (const std::string &key,
                         const bool defValue) const;

private:

  RichParameters();

  virtual void readStream (istream &parameterStream);

  virtual std::string nextLine (istream &ist);

  virtual void readCDF();

  virtual void updateParaMap();

private:
  // Data Members for Class Attributes

  std::string m_parameterFile;

  map<std::string,std::string> m_parameterMap;

  bool updateDone;

};

#endif // RICHREC_RICHPARAMETERS_H
