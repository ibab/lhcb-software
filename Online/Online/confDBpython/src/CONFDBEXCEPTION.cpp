/********************************************************************************/
//Author: L.Abadie
// version:v1.1
// wrapper to interface it with Python (can also be used a C++ wrapper)
/********************************************************************************/
/**********************************************************************************************/
/**
       * CONFDBEXCEPTION class inherits from the std::exception.
       */
/**********************************************************************************************/
#include <string>
#include "../confDBpython/CONFDBEXCEPTION.h"
#include <iostream>
using namespace std;

/**********************************************************************************************/
/**
       * CONFDBEXCEPTION class constructor (default).
       */
/**********************************************************************************************/

	CONFDBEXCEPTION::CONFDBEXCEPTION():_errMessage("none"){}
/**********************************************************************************************/
/**
       * CONFDBEXCEPTION class constructor (initializes to the given string).
       */
/**********************************************************************************************/

	CONFDBEXCEPTION::CONFDBEXCEPTION(string errMess):_errMessage(errMess){}
	/**********************************************************************************************/
/**
       * CONFDBEXCEPTION class destructor (default).
       */
/**********************************************************************************************/

	CONFDBEXCEPTION::~CONFDBEXCEPTION() throw(){};
	/**********************************************************************************************/
/**
       * CONFDBEXCEPTION return the value of  _errMessage as a string .
       */
/**********************************************************************************************/

	string CONFDBEXCEPTION::getconfDBErrMess(){cout<<"Error value "<<_errMessage<<endl;return _errMessage;}
/**********************************************************************************************/
/**
       * CONFDBEXCEPTION return the value of  _errMessage as a char* .
       */
/**********************************************************************************************/

char const* CONFDBEXCEPTION::what() throw() { return _errMessage.c_str(); }
	

