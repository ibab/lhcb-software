/********************************************************************************/
//Author: L.Abadie
// version:v1.1
// wrapper to interface it with Python (can also be used a C++ wrapper)
/********************************************************************************/

#include <string>
#include <exception>

#include <iostream>
using namespace std;
/**********************************************************************************************/
/**
       * CONFDBEXCEPTION class .
       */
/**********************************************************************************************/

class CONFDBEXCEPTION : exception
{
private:
	string _errMessage;
	
public:
	CONFDBEXCEPTION();
	CONFDBEXCEPTION(string errMess);
	virtual ~CONFDBEXCEPTION() throw();
	string getconfDBErrMess();
	char const* what() throw();

};




