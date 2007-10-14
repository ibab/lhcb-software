/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtPartPropDb.hh,v 1.2 2007-10-14 18:50:07 robbep Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Light-weight implementation of particle properties database.
// Only one database, so all members are static.

#ifndef EVT_PART_PROP_DB_HH
#define EVT_PART_PROP_DB_HH

#include <vector>
#include <string>

class EvtPartPropDb {

public:

  EvtPartPropDb();
  EvtPartPropDb(const char* fname);
  ~EvtPartPropDb();

  static double getMass(const char* name);
  static double getWidth(const char* name);
  static int getStdHepID(const char* name);

  static void readPDT(const char* fname);
  static bool initialized() ;

private:

  static bool _init;
  static std::vector<std::string> _names;
  static std::vector<double>      _masses;
  static std::vector<double>      _widths;
  static std::vector<int>         _stdhepid;

};

#endif
