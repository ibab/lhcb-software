/********************************************************************************/
//Author: L.Abadie
// version:v3.2
// wrapper to interface it with Python (can also be used a C++ wrapper)
//CONFDB class contains all the fcts
/********************************************************************************/


#include "../confDBpython/CONFDB.h"
#include "../confDBpython/CONFDBEXCEPTION.h"
/**********************************************************************************************/
/**
       * CONFDB Constructor .
       * @param dbname : name of the database.
       * @param login : user login.
       * @param passwd : user password.
       */
/**********************************************************************************************/


CONFDB::CONFDB(string dbname,string login,string passwd):_dbname(dbname),_login(login),_passwd(passwd){}
/**********************************************************************************************/
/**
       * CONFDB destructor .
       * no param
       */
/**********************************************************************************************/
CONFDB::~CONFDB(){}
/**********************************************************************************************/
/**
       * Connect to the database with the login,pwd and db as provided in the constructor .
       * @throw CONFDBEXCEPTION .
       * @return 0 if the connection is successful
       */
/**********************************************************************************************/
int CONFDB::PyDBConnexion()  
{
int c=0;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];
string ErrorMess_copy;

 c=DBConnexion((char*)_dbname.c_str(),(char*)_login.c_str(),(char*)_passwd.c_str(),ErrorMess);
 if(c!=0)
 {
	 ErrorMess_copy=ErrorMess;
	 delete [] ErrorMess;
	 throw CONFDBEXCEPTION(ErrorMess_copy);
 }
  ociEnv  = (OCIEnv*)get_OCIEnv();
  ociError = (OCIError*)get_ErrorHandle();
  ociHdbc = (OCISvcCtx*)get_ContxtHdl();

 delete [] ErrorMess;
return c;
}
/**********************************************************************************************/
/**
       * Disconnect from the database no input arguments and returning a int .
	   * @throw CONFDBEXCEPTION .
       * @return 0 if the disconnection is successful
       */
/**********************************************************************************************/
int CONFDB::PyDBDeconnexion()  
{
int c=0;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];
string ErrorMess_copy;

c=DBDeconnexion(ErrorMess);
if(c!=0)
{
	ErrorMess_copy=ErrorMess;
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(ErrorMess_copy);
}
delete [] ErrorMess;
return c;
}
/**************************************************************/
/**
       * Get the row of the given device type  returning a string value.
       * @param devitype : device type name: 
       * @return string if successfull formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */
/***************************************************************/
string  CONFDB::PyGetDeviceTypeRow(string devitype)  
{

int len_array=2000;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];

int len_array_copy=len_array;
char* devtype_result=new char[2000];
int rescode=0;
string dtype_result="";


rescode=GetDeviceTypeRow((char*)devitype.c_str(),len_array,devtype_result,ErrorMess);
 if(rescode==0 || strncmp(devtype_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(devtype_result,"NO_ROWS_SELECTED",16)==0)
	{
		dtype_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] devtype_result;
		throw CONFDBEXCEPTION(dtype_result);
	}
	else
		dtype_result=devtype_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete devtype_result;
		devtype_result=new char[len_array];
		rescode=GetDeviceTypeRow((char*)devitype.c_str(),len_array,devtype_result,ErrorMess);
		if(rescode==0|| strncmp(devtype_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(devtype_result,"NO_ROWS_SELECTED",16)==0)
			{
				dtype_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] devtype_result;
				throw CONFDBEXCEPTION(dtype_result);
			}
			else
				dtype_result=devtype_result;
		}
		else
		{
			dtype_result=ErrorMess;
			delete [] ErrorMess;
			delete [] devtype_result;
			throw CONFDBEXCEPTION(dtype_result);
		}

	}
	else
	{
		dtype_result=ErrorMess;
		delete [] ErrorMess;
		delete [] devtype_result;
		throw CONFDBEXCEPTION(dtype_result);
	}

}
 delete [] ErrorMess;
delete [] devtype_result;
return dtype_result;
}
/*********************************************************************/
/**
       * Get the row of the given device name ,  returning a string.
       * @param deviname : device name: 
       * @return if successful formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.
       */
/*********************************************************************/

string  CONFDB::PyGetDeviceRow_devicename(string deviname)  
{

int len_array=2000;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];

int len_array_copy=len_array;
char* device_result=new char[2000];
int rescode=0;
string dev_result="";
 rescode=GetDeviceRow_devicename((char*)deviname.c_str(),len_array,device_result,ErrorMess);
 if(rescode==0|| strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
	{
		dev_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
	else
	dev_result=device_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete device_result;
		device_result=new char[len_array];
		rescode=GetDeviceRow_devicename((char*)deviname.c_str(),len_array,device_result,ErrorMess);
		if(rescode==0 ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
			{
				dev_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] device_result;
				throw CONFDBEXCEPTION(dev_result);
			}
			else
				dev_result=device_result;
		}
		else
		{
			dev_result=ErrorMess;
			delete [] ErrorMess;
			delete [] device_result;
			throw CONFDBEXCEPTION(dev_result);
		}

	}
	else
	{
		dev_result=ErrorMess;
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
}
delete [] ErrorMess;
delete [] device_result;
return dev_result;
}

/*************************************************************************************/

/**
       * Get the row of the boot image information for a given device or device type name returning an integer value.
       * @param functionaldeviname : functional device name or functional device type name. it's  case sensitive.
	     * @return if successful formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.
       */


string CONFDB::PyGetBootImageRow_devicename(string functionaldeviname)
{


int len_array=2000;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];

int len_array_copy=len_array;
char* device_result=new char[2000];
int rescode=0;
string dev_result="";
rescode=GetBootImageRow_devicename((char*)functionaldeviname.c_str(),len_array,device_result,ErrorMess);
 if(rescode==0|| strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
	{
		dev_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
	else
	dev_result=device_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete device_result;
		device_result=new char[len_array];
		rescode=GetBootImageRow_devicename((char*)functionaldeviname.c_str(),len_array,device_result,ErrorMess);
		if(rescode==0 ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
			{
				dev_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] device_result;
				throw CONFDBEXCEPTION(dev_result);
			}
			else
				dev_result=device_result;
		}
		else
		{
			dev_result=ErrorMess;
			delete [] ErrorMess;
			delete [] device_result;
			throw CONFDBEXCEPTION(dev_result);
		}

	}
	else
	{
		dev_result=ErrorMess;
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
}
delete [] ErrorMess;
delete [] device_result;
return dev_result;
}
/*********************************************************************/
/**
       * Get the row of the given board cpnt name  returning a string.
       * @param cpntname : board cpnt name: 
       * @return if successful formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.
       */
/*********************************************************************/

string  CONFDB::PyGetBoardCpntRow_cpntname(string cpntname)  
{

int len_array=2000;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];

int len_array_copy=len_array;
char* device_result=new char[2000];
int rescode=0;
string dev_result="";
 rescode=GetBoardCpntRow_cpntname((char*)cpntname.c_str(),len_array,device_result,ErrorMess);
 if(rescode==0|| strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
	{
		dev_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
	else
	dev_result=device_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete device_result;
		device_result=new char[len_array];
		rescode=GetBoardCpntRow_cpntname((char*)cpntname.c_str(),len_array,device_result,ErrorMess);
		if(rescode==0 ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
			{
				dev_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] device_result;
				throw CONFDBEXCEPTION(dev_result);
			}
			else
				dev_result=device_result;
		}
		else
		{
			dev_result=ErrorMess;
			delete [] ErrorMess;
			delete [] device_result;
			throw CONFDBEXCEPTION(dev_result);
		}

	}
	else
	{
		dev_result=ErrorMess;
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
}
delete [] ErrorMess;
delete [] device_result;
return dev_result;
}
/*********************************************************************/
/**
       * Get the row of the given hw board cpnt serialnb  returning a string.
       * @param serialnb : serialnb of your board cpnt 
       * @return if successful formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.
       */
/*********************************************************************/

string  CONFDB::PyGetHWCpntRow_serialnb(string serialnb)  
{

int len_array=2000;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];

int len_array_copy=len_array;
char* device_result=new char[2000];
int rescode=0;
string dev_result="";
 rescode=GetHWCpntRow_serialnb((char*)serialnb.c_str(),len_array,device_result,ErrorMess);
 if(rescode==0|| strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
	{
		dev_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
	else
	dev_result=device_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete device_result;
		device_result=new char[len_array];
		rescode=GetHWCpntRow_serialnb((char*)serialnb.c_str(),len_array,device_result,ErrorMess);
		if(rescode==0 ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
			{
				dev_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] device_result;
				throw CONFDBEXCEPTION(dev_result);
			}
			else
				dev_result=device_result;
		}
		else
		{
			dev_result=ErrorMess;
			delete [] ErrorMess;
			delete [] device_result;
			throw CONFDBEXCEPTION(dev_result);
		}

	}
	else
	{
		dev_result=ErrorMess;
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
}
delete [] ErrorMess;
delete [] device_result;
return dev_result;
}
/***************************************************************************************/
/**
       * Get the row of the given deviceID of the device returning a string .
       * @param devid : deviceID of the device
	   * @return case of success formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.
       */
/******************************************************************************/

string  CONFDB::PyGetDeviceRow_deviceid(int devid)  
{

int len_array=2000;
int len_array_copy=len_array;
char* device_result=new char[2000];
int rescode=0;
string dev_result="";
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];
 rescode=GetDeviceRow_devid(devid,len_array,device_result,ErrorMess);
if(rescode==0 ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
	{
		dev_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
	else
	dev_result=device_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete device_result;
		device_result=new char[len_array];
		rescode=GetDeviceRow_devid( devid,len_array,device_result,ErrorMess);
		if(rescode==0  ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
			{
				dev_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] device_result;
				throw CONFDBEXCEPTION(dev_result);
			}
			else
			dev_result=device_result;
		}
		else
		{
			dev_result=ErrorMess;
			delete [] ErrorMess;
			delete [] device_result;
			throw CONFDBEXCEPTION(dev_result);
		}

	}
	else
	{
		dev_result=ErrorMess;
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
}
 delete [] ErrorMess;
delete [] device_result;
return dev_result;
}
/***************************************************************************************/
/**
       * Get the row of the given cpntID returning a string .
       * @param cpntid : cpntID of the board cpnt
	   * @return case of success formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.
       */
/******************************************************************************/

string  CONFDB::PyGetBoardCpntRow_cpntid(int cpntid)  
{

int len_array=2000;
int len_array_copy=len_array;
char* device_result=new char[2000];
int rescode=0;
string dev_result="";
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];
 rescode=GetBoardCpntRow_cpntid(cpntid,len_array,device_result,ErrorMess);
if(rescode==0 ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
	{
		dev_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
	else
	dev_result=device_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete device_result;
		device_result=new char[len_array];
		rescode=GetBoardCpntRow_cpntid(cpntid,len_array,device_result,ErrorMess);
		if(rescode==0  ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
			{
				dev_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] device_result;
				throw CONFDBEXCEPTION(dev_result);
			}
			else
			dev_result=device_result;
		}
		else
		{
			dev_result=ErrorMess;
			delete [] ErrorMess;
			delete [] device_result;
			throw CONFDBEXCEPTION(dev_result);
		}

	}
	else
	{
		dev_result=ErrorMess;
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
}
 delete [] ErrorMess;
delete [] device_result;
return dev_result;
}
/***************************************************************************************/
/**
       * Get the row of the given snbID returning a string .
       * @param snbid : snbID of the hw board cpnt (number given bu the DB to foresee the case when there is no serialnb)
	   * @return case of success formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.
       */
/******************************************************************************/

string  CONFDB::PyGetHWCpntRow_snbid(int snbid)  
{

int len_array=2000;
int len_array_copy=len_array;
char* device_result=new char[2000];
int rescode=0;
string dev_result="";
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];
 rescode=GetHWCpntRow_snbid(snbid,len_array,device_result,ErrorMess);
if(rescode==0 ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
	{
		dev_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
	else
	dev_result=device_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete device_result;
		device_result=new char[len_array];
		rescode=GetHWCpntRow_snbid(snbid,len_array,device_result,ErrorMess);
		if(rescode==0  ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
			{
				dev_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] device_result;
				throw CONFDBEXCEPTION(dev_result);
			}
			else
			dev_result=device_result;
		}
		else
		{
			dev_result=ErrorMess;
			delete [] ErrorMess;
			delete [] device_result;
			throw CONFDBEXCEPTION(dev_result);
		}

	}
	else
	{
		dev_result=ErrorMess;
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
}
 delete [] ErrorMess;
delete [] device_result;
return dev_result;
}
/**************************************************************************************/

/**
       * Get the port row  returning a string .
       * @param pID : portID of the device
       * @return case of success formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */


  
string CONFDB::PyGetPortRow_pid(int pID)  
{
	string port_result="";
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	int c=0;
	int c1=0;
	
	char* ErrorMess=new char[len_buffer];

	char* port_res=new char[len_buffer];
	c=GetPortRow_pid(pID,len_buffer, port_res,ErrorMess);
	if(c==0 ||  strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
		{
			port_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] port_res;
			throw CONFDBEXCEPTION(port_result);
		}
		else
			port_result=port_res; 		
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] port_res;
			port_res=new char[len_buffer];
			c1=GetPortRow_pid(pID,len_buffer, port_res,ErrorMess);
			if(c1==0 ||  strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
				{
					port_result="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] port_res;
					throw CONFDBEXCEPTION(port_result);
				}
				else
					port_result=port_res; 	
			}
			else
			{
				port_result=ErrorMess;
				delete [] ErrorMess;
				delete [] port_res;
				throw CONFDBEXCEPTION(port_result);
			}

		}
		else
		{
			port_result=ErrorMess;
			delete [] ErrorMess;
			delete [] port_res;
			throw CONFDBEXCEPTION(port_result);
		}
	}
	 delete [] ErrorMess;
	delete [] port_res;
	return port_result;
}
/**************************************************************************************/
/**
       * Get the port row   returning a string value.
       * @param devid : deviceID of the device
	   * @param port_nb : port nb of the device
	   * @param port_way : way 1 if it's an input, 2 if it' an output.
	   * @param port_type : type of the port Put "none" in lower case, if it's empty.
	   * @return  if it is successful formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return  if it is successful db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */

string CONFDB::PyGetPortRow_devid(int devid,string port_nb,int port_way,string port_type)  
{
	string port_result;
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	int c=0;
	int c1=0;
	
char* ErrorMess=new char[len_buffer];


	char* port_res=new char[len_buffer];

	
	c=GetPortRow_devid(devid,(char*)port_nb.c_str(),port_way,(char*)port_type.c_str(),len_buffer,port_res,ErrorMess);
	if(c==0 ||  strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
		{
			port_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] port_res;
			throw CONFDBEXCEPTION(port_result);
		}
		else
			port_result=port_res; 	
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] port_res;
			port_res=new char[len_buffer];
			c1=GetPortRow_devid(devid,(char*)port_nb.c_str(),port_way,(char*)port_type.c_str(),len_buffer,port_res,ErrorMess);
			if(c1==0 ||  strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
		{
			port_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] port_res;
			throw CONFDBEXCEPTION(port_result);
		}
		else
			port_result=port_res; 		
			}
			else
			{
				port_result=ErrorMess;
				delete [] ErrorMess;
				delete [] port_res;
				throw CONFDBEXCEPTION(port_result);
			}

		}
		else
		{
			port_result=ErrorMess;
			delete [] ErrorMess;
			delete [] port_res;
			throw CONFDBEXCEPTION(port_result);
		}
	}
	 delete [] ErrorMess;
	delete [] port_res;
	return port_result;
}

/**************************************************************************************/
/**
       * Get the port row   returning a string value.
       * @param serialnb : serialnb the spare device
	   * @param port_nb : port nb of the device
	   * @param port_way : way 1 if it's an input, 2 if it' an output.
	   * @param port_type : type of the port Put "none" in lower case, if it's empty.
	   * @return  if it is successful formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return  if it is successful db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */

string CONFDB::PyGetSparePortRow_snb(string serialnb,string port_nb,int port_way,string port_type)  
{
	string port_result;
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	int c=0;
	int c1=0;
	
char* ErrorMess=new char[len_buffer];


	char* port_res=new char[len_buffer];

	
	c=GetSparePortRow_snb((char*)serialnb.c_str(),(char*)port_nb.c_str(),port_way,(char*)port_type.c_str(),len_buffer,port_res,ErrorMess);
	if(c==0 ||  strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
		{
			port_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] port_res;
			throw CONFDBEXCEPTION(port_result);
		}
		else
			port_result=port_res; 	
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] port_res;
			port_res=new char[len_buffer];
			c1=GetSparePortRow_snb((char*)serialnb.c_str(),(char*)port_nb.c_str(),port_way,(char*)port_type.c_str(),len_buffer,port_res,ErrorMess);
			if(c1==0 ||  strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(port_res,"NO_ROWS_SELECTED",16)==0)
		{
			port_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] port_res;
			throw CONFDBEXCEPTION(port_result);
		}
		else
			port_result=port_res; 		
			}
			else
			{
				port_result=ErrorMess;
				delete [] ErrorMess;
				delete [] port_res;
				throw CONFDBEXCEPTION(port_result);
			}

		}
		else
		{
			port_result=ErrorMess;
			delete [] ErrorMess;
			delete [] port_res;
			throw CONFDBEXCEPTION(port_result);
		}
	}
	 delete [] ErrorMess;
	delete [] port_res;
	return port_result;
}

/**************************************************************************************/
/**
       * Get the macro link row  returning a string .
       * @param lkid : lkID of the link
       * @return formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */

string CONFDB::PyGetMacroConnectivityRow_lkid(int lkid)  
{
	string conn_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	char* ErrorMess=new char[len_buffer];

	char* conn_res=new char[len_buffer];
	c=GetMacroConnectivityRow_lkid(lkid,len_buffer,conn_res,ErrorMess);
	if(c==0 ||  strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
		{
			conn_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] conn_res;
			throw CONFDBEXCEPTION(conn_result);
		}
		else
			conn_result=conn_res;		
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] conn_res;
			conn_res=new char[len_buffer];
			c1=GetMacroConnectivityRow_lkid(lkid,len_buffer,conn_res,ErrorMess);
			if(c1==0 ||  strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
				{
					conn_result="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] conn_res;
					throw CONFDBEXCEPTION(conn_result);
				}
				else
					conn_result=conn_res;	
			}
			else
			{
				conn_result=ErrorMess;
				delete [] ErrorMess;
				delete [] conn_res;
				throw CONFDBEXCEPTION(conn_result);
			}

		}
		else
		{
			conn_result=ErrorMess;
			delete [] ErrorMess;
			delete [] conn_res;
			throw CONFDBEXCEPTION(conn_result);
		}
	}
	 delete [] ErrorMess;
	delete [] conn_res;
	return conn_result;
}
/**************************************************************************************/
/**
       * Get the micro link row returning a string .
       * @param lkid : lkID of the micro link
       * @return formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */

string CONFDB::PyGetMicroConnectivityRow_lkid(int lkid)  
{
	string conn_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	char* ErrorMess=new char[len_buffer];

	char* conn_res=new char[len_buffer];
	c=GetMicroConnectivityRow_lkid(lkid,len_buffer,conn_res,ErrorMess);
	if(c==0 ||  strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
		{
			conn_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] conn_res;
			throw CONFDBEXCEPTION(conn_result);
		}
		else
			conn_result=conn_res;		
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] conn_res;
			conn_res=new char[len_buffer];
			c1=GetMicroConnectivityRow_lkid(lkid,len_buffer,conn_res,ErrorMess);
			if(c1==0 ||  strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
				{
					conn_result="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] conn_res;
					throw CONFDBEXCEPTION(conn_result);
				}
				else
					conn_result=conn_res;	
			}
			else
			{
				conn_result=ErrorMess;
				delete [] ErrorMess;
				delete [] conn_res;
				throw CONFDBEXCEPTION(conn_result);
			}

		}
		else
		{
			conn_result=ErrorMess;
			delete [] ErrorMess;
			delete [] conn_res;
			throw CONFDBEXCEPTION(conn_result);
		}
	}
	 delete [] ErrorMess;
	delete [] conn_res;
	return conn_result;
}
//way=1 if it's node from else node to
/**************************************************************************************/
/**
       * Get the macro link row returning a string .
       * @param nodeid : deviceid of the device
	   * @param port_nb : port_nb of the device
	   * @param port_type : Put "none" in lower case if you have no type.
	   * @param way_given : 1 if the port_nb is an input (so it means that the link ends up to this port),2 if the port nb is an output (so the link starts from this port)
       * @return formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */

string CONFDB::PyGetMacroConnectivityRow_node(int nodeid,string port_nb,int way_given,string port_type)  
{
	string conn_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	char* conn_res=new char[len_buffer];
	char* ErrorMess=new char[len_buffer];

	c=GetMacroConnectivityRow_node(nodeid, (char*) port_nb.c_str(), way_given,(char*) port_type.c_str(),len_buffer,conn_res,ErrorMess);
	if(c==0 ||  strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
		{
			conn_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] conn_res;
			throw CONFDBEXCEPTION(conn_result);
		}
		else
			conn_result=conn_res;		
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] conn_res;
			conn_res=new char[len_buffer];
			c1=GetMacroConnectivityRow_node(nodeid, (char*) port_nb.c_str(), way_given,(char*) port_type.c_str(),len_buffer,conn_res,ErrorMess);
			if(c1==0 ||  strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
				{
					conn_result="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] conn_res;
					throw CONFDBEXCEPTION(conn_result);
				}
				else
					conn_result=conn_res;	

			}
			else
			{
				conn_result=ErrorMess;
				delete [] ErrorMess;
				delete [] conn_res;
				throw CONFDBEXCEPTION(conn_result);
			}

		}
		else
		{
			conn_result=ErrorMess;
			delete [] ErrorMess;
			delete [] conn_res;
			throw CONFDBEXCEPTION(conn_result);
		}
	}
	 delete [] ErrorMess;
	delete [] conn_res;
	return conn_result;
}
/**************************************************************************************/
/**
       * Get the micro link row ,returning a string .
	   * @param nodeid : cpntid of the logical board cpnt. Put -1 if it's the motherboard
	   * @param port_nb : port_nb of the cpnt. Put the portid if cpntID=-1
	   * @param way_given : 1 if the port_nb is an input (so it means that the link ends up to this port),2 if the port nb is an output (so the link starts from this port)
       * @return formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */

string CONFDB::PyGetMicroConnectivityRow_node(int nodeid,int port_nb,int way_given)  
{
	string conn_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	char* conn_res=new char[len_buffer];
	char* ErrorMess=new char[len_buffer];

	c=GetMicroConnectivityRow_node(nodeid,  port_nb, way_given,len_buffer,conn_res,ErrorMess);
	if(c==0 ||  strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
		{
			conn_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] conn_res;
			throw CONFDBEXCEPTION(conn_result);
		}
		else
			conn_result=conn_res;		
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] conn_res;
			conn_res=new char[len_buffer];
			c1=GetMicroConnectivityRow_node(nodeid, port_nb, way_given,len_buffer,conn_res,ErrorMess);
			if(c1==0 ||  strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(conn_res,"NO_ROWS_SELECTED",16)==0)
				{
					conn_result="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] conn_res;
					throw CONFDBEXCEPTION(conn_result);
				}
				else
					conn_result=conn_res;	

			}
			else
			{
				conn_result=ErrorMess;
				delete [] ErrorMess;
				delete [] conn_res;
				throw CONFDBEXCEPTION(conn_result);
			}

		}
		else
		{
			conn_result=ErrorMess;
			delete [] ErrorMess;
			delete [] conn_res;
			throw CONFDBEXCEPTION(conn_result);
		}
	}
	 delete [] ErrorMess;
	delete [] conn_res;
	return conn_result;
}
/**************************************************************************************/
/**
       * Get the ip row in a string .
       * @param IPadd : ip address about which you want to get some info
	   * @return formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */

string CONFDB::PyGetIPInfoRow(string IPadd)  
{
	string  mac_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	char* ErrorMess=new char[len_buffer];
	char* mac_res=new char[len_buffer];
	c=GetIPInfoRow((char*)IPadd.c_str(),len_buffer,mac_res,ErrorMess);
	if(c==0 ||  strncmp(mac_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(mac_res,"NO_ROWS_SELECTED",16)==0)
		{
			mac_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] mac_res;
			throw CONFDBEXCEPTION(mac_result);
		}
		else
			mac_result=mac_res;	  	
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] mac_res;
			mac_res=new char[len_buffer];
			c=GetIPInfoRow((char*)IPadd.c_str(),len_buffer,mac_res,ErrorMess);
			if(c1==0 ||  strncmp(mac_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(mac_res,"NO_ROWS_SELECTED",16)==0)
				{
					mac_result="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] mac_res;
					throw CONFDBEXCEPTION(mac_result);
				}
				else
				mac_result=mac_res;	  
			}
			else
			{
				mac_result=ErrorMess;
				delete [] ErrorMess;
				delete [] mac_res;
				throw CONFDBEXCEPTION(mac_result);
			}

		}
		else
		{
			mac_result=ErrorMess;
			delete [] ErrorMess;
			delete [] mac_res;
			throw CONFDBEXCEPTION(mac_result);
		}
	}
	 delete [] ErrorMess;
	delete [] mac_res;	
	return mac_result;
}

/*****************************************************************************************/
/**
       * Get the link type row  returning a string .
       * @param lname :link type name about which you want to get some info
	   * @return formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */
/***********************************************************/
string CONFDB::PyGetLkTypeRow_lkname(string lname)  
{
	string  link_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_array_copy=len_buffer;
	char* link_res=new char[len_buffer];
	char* ErrorMess=new char[len_buffer];

	c=GetLkTypeRow_lkname((char*)lname.c_str(),len_buffer,link_res,ErrorMess);
	if(c==0 ||  strncmp(link_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(link_res,"NO_ROWS_SELECTED",16)==0)
		{
			link_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] link_res;
			throw CONFDBEXCEPTION(link_result);
		}
		else
			link_result=link_res; 		
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array_copy<len_buffer)
		{
			delete [] link_res;
			link_res=new char[len_buffer];
			c1=GetLkTypeRow_lkname((char*)lname.c_str(),len_buffer,link_res,ErrorMess);
			if(c1==0 ||  strncmp(link_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(link_res,"NO_ROWS_SELECTED",16)==0)
				{
					link_result="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] link_res;
					throw CONFDBEXCEPTION(link_result);
				}
				else
					link_result=link_res; 		 
			}
			else
			{
				link_result=ErrorMess;
				delete [] ErrorMess;
				delete [] link_res;
				throw CONFDBEXCEPTION(link_result);
			}

		}
		else
		{
			link_result=ErrorMess;
			delete [] ErrorMess;
			delete [] link_res;
			throw CONFDBEXCEPTION(link_result);
		}
	}
	 delete [] ErrorMess;
	delete [] link_res;
	return link_result;
}
/*****************************************************************************************/
/**
       * Get the link type row returning a string .
       * @param lknb :link type id  about which you want to get some info
	   * @return formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @return db_column_type is I for int and C for char*
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	   */
/**********************************************************************/
string CONFDB::PyGetLkTypeRow_lknb(int lknb)  
{
	string  link_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_buffer_copy=len_buffer;
	char* link_res=new char[len_buffer];
	char* ErrorMess=new char[len_buffer];

	c=GetLkTypeRow_lknb(lknb,len_buffer,link_res,ErrorMess);
	if(c==0 ||  strncmp(link_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(link_res,"NO_ROWS_SELECTED",16)==0)
		{
			link_result="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] link_res;
			throw CONFDBEXCEPTION(link_result);
		}
		else
			link_result=link_res; 				
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_buffer_copy<len_buffer)
		{
			delete [] link_res;
			link_res=new char[len_buffer];
			c=GetLkTypeRow_lknb(lknb,len_buffer,link_res,ErrorMess);
			if(c1==0  ||  strncmp(link_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(link_res,"NO_ROWS_SELECTED",16)==0)
				{
					link_result="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] link_res;
					throw CONFDBEXCEPTION(link_result);
				}
				else
					link_result=link_res; 	
			}
			else
			{
				link_result=ErrorMess;
				delete [] ErrorMess;
				delete [] link_res;
				throw CONFDBEXCEPTION(link_result);		
			}

		}
		else
		{
			link_result=ErrorMess;
			delete [] ErrorMess;
			delete [] link_res;
			throw CONFDBEXCEPTION(link_result);
		}
	}
	delete [] ErrorMess;
	delete [] link_res;
	return link_result;
}

/****************************************************************************************/
/**
       * Get the list of portID  of the given deviceID,  returning a vector of int .
       * @param dID : deviceID
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/******************************************************************/
vector<int> CONFDB::PyGetPortIDPerDevID(int dID)  
{
	
	int c=0;
	int c1=0;
	int i=0;
	int len_buffer=1000;
	int len_array=len_buffer;
	char* ErrorMess=new char[len_buffer];
string ErrMess;
	// créer un tableau d'entiers vide
    vector<int> v;
	int* portID_list_res=new int[len_buffer];
	c=GetPortIDPerDevID(dID,len_array, portID_list_res,ErrorMess);
	v.clear();
	if(c==0 ||(portID_list_res[0]==-1 && portID_list_res[1]==-1))
	{
		if(portID_list_res[0]==-1 && portID_list_res[1]==-1)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] portID_list_res;	
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			for(i=0;i<len_array;i++)
				v.push_back(portID_list_res[i]); 
		}
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] portID_list_res;
			portID_list_res=new int[len_array];
			c1=GetPortIDPerDevID(dID,len_array, portID_list_res,ErrorMess);
			if(c1==0 ||(portID_list_res[0]==-1 && portID_list_res[1]==-1))
			{
				if(portID_list_res[0]==-1 && portID_list_res[1]==-1)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] portID_list_res;	
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					for(i=0;i<len_array;i++)
						v.push_back(portID_list_res[i]); 
				}
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] portID_list_res;	
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] portID_list_res;	
			throw CONFDBEXCEPTION(ErrMess);
		}
	}
	delete [] ErrorMess;
	delete [] portID_list_res;	
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of deviceID  of the given device type,  returning a vector of int .
       * @param dtype : name of the device type
	   * @return the list of deviceids
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<int> CONFDB::PyGetDeviceIDsPerType(string dtype)  
{
	
	int c=0;
	int c1=0;
	int i=0;
	int len_buffer=2000;
	int len_array=2000;
	vector<int> v;
	char* ErrorMess=new char[1000];
	string ErrMess;
	int* devID_list_res=new int[len_buffer];
	v.clear();
	c=GetDeviceIDsPerType((char*)dtype.c_str(),len_array, devID_list_res,ErrorMess);
	
	if(c==0 ||(devID_list_res[0]==-1 && devID_list_res[1]==-1))
	{
		if(devID_list_res[0]==-1 && devID_list_res[1]==-1)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] devID_list_res;	
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			for(i=0;i<len_array;i++)
				v.push_back(devID_list_res[i]); 
		}
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devID_list_res;
			devID_list_res=new int[len_array];
			c1=GetDeviceIDsPerType((char*)dtype.c_str(),len_array, devID_list_res,ErrorMess);
			if(c1==0 ||(devID_list_res[0]==-1 && devID_list_res[1]==-1))
			{
				if(devID_list_res[0]==-1 && devID_list_res[1]==-1)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] devID_list_res;	
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					for(i=0;i<len_array;i++)
						v.push_back(devID_list_res[i]); 
				}
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] devID_list_res;	
				throw CONFDBEXCEPTION(ErrMess);	
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] devID_list_res;	
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	delete [] ErrorMess;
	delete [] devID_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of devices  of the given device type,  returning a vector of strings.
       * @param dtype : name of the device type
	   * @return the list of devicenames concatenated with the deviceid  and presented as devicename|deviceid
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetDeviceNamesPerType(string dtype)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetDeviceNamesPerType((char*)dtype.c_str(),len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetDeviceNamesPerType((char*)dtype.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the decomposition of the link type  of the given linktypeID,  returning a vector of strings.
       * @param lktypeID : name of the device type
	   * @return the list of lknames concatenated with the lktypeid  and presented as lkname|lktypeID
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetLkTypeDecomposition_lknb(int lktypeID)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=5000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* LkNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	
	c=GetLkTypeDecomposition_lknb(lktypeID,len_array,LkNames_list_res,ErrorMess);
	if(c==0 ||  strncmp(LkNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(LkNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] LkNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(LkNames_list_res+pos2);
				strcpy(dto,LkNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] LkNames_list_res;
			LkNames_list_res=new char[len_array];
			c1=GetLkTypeDecomposition_lknb(lktypeID,len_array,LkNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(LkNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(LkNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] LkNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(LkNames_list_res+pos2);
						strcpy(dto,LkNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] LkNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] LkNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	delete [] ErrorMess;
	delete [] dto;
	delete [] LkNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of board cpnt  of the given cpnt type,  returning a vector of strings.
       * @param cpnttype : name of cpnt type 
	   * @return the list of cpnt names concatenated with the cpnt id. Presneted as follows cpntname|cpntid
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetBoardCpntPerType(string cpnttype)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetBoardCpntPerType((char*)cpnttype.c_str(),len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetBoardCpntPerType((char*)cpnttype.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
			
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of functional device names  located at the given location,  returning a vector of strings.
       * @param location : location (exact name or just a part (prefix)
	   * @return the list of devicenames
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetDeviceNamesPerLocation(string location)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	v.clear();
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	c=GetDeviceNamesPerLocation((char*)location.c_str(),len_array, devNames_list_res,ErrorMess);
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetDeviceNamesPerLocation((char*)location.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of functional board cpnt names  located at the given motherboard,  returning a vector of strings.
       * @param motherboardname : motherboadrname
	   * @return the list of board cpnt names
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetCpntNamesPerBoard(string motherboardname)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetCpntNamesPerBoard((char*)motherboardname.c_str(),len_array, devNames_list_res,ErrorMess);

	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetCpntNamesPerBoard((char*)motherboardname.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of spare hw serialnb concatenated with the hwname if it exists (equals to 'none' if it's null)  located at the given location,  returning a vector of strings.
       * @param location : location
	   * @return the list of serial nb and hwname. if hwname is null, we put "none". Presented as follows serialnb|hwname
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetSpareHWPerLocation(string location)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetSpareHWPerLocation((char*)location.c_str(),len_array, devNames_list_res,ErrorMess);

	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetSpareHWPerLocation((char*)location.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of port info of a spare hw serialnb  returning a vector of strings.
       * @param snb : snb of the spare
	   * @return the list of port_nb|port_type|port_way|macaddress|bia. if mac or bia or port_type are null, we put "none". Presented as follows serialnb|hwname
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetPortInfoPerSpare(string snb)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetPortInfoPerSpare((char*)snb.c_str(),len_array, devNames_list_res,ErrorMess);

	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetPortInfoPerSpare((char*)snb.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of spare hw snbid (automatic nb generated by the DB) concatenated with the serialnb and the hwname (functional name) if it exists (equals to 'none' if it's null)  located at the given location,  returning a vector of strings.
       * @param location : location
	   * @return the list of hw cpnt names presentend as follows snbid|serialnb|hwname
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetSpareHWCpntPerLocation(string location)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetSpareHWCpntPerLocation((char*)location.c_str(),len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetSpareHWCpntPerLocation((char*)location.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
			
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of spare hw serialnb concatenated with the hwname if it exists (equals to 'none' if it's null)  of the given hwtype,  returning a vector of strings.
       * @param hwtype : location
	   * @return the list of spare hw serialnb. Presented as follows serialnb|hwname. If hwname is null, we put 'none'
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetSpareHWPerType(string hwtype)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetSpareHWPerType((char*)hwtype.c_str(),len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetSpareHWPerType((char*)hwtype.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
			
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
				
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
				
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of spare hw snbid (automatic nb generated by the DB) concatenated with the serialnb and the hwname (functional name) if it exists (equals to 'none' if it's null)  located at the given location,  returning a vector of strings.
       * @param hwtype : name of the hwtype
	   * @return the list of cpnt name spresented as follows : snbid|serialnb|hwname
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetSpareHWCpntPerType(string hwtype)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetSpareHWCpntPerType((char*)hwtype.c_str(),len_array, devNames_list_res,ErrorMess);

	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetSpareHWCpntPerType((char*)hwtype.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of free devices  of the given device type, returning a vetcor of string .
       * @param dtype : device type name
	   * @return vector of strings (list of free devices)
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetFreeDeviceNamesPerType(string dtype)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	
	vector<string> v;
	v.clear();
	char* ErrorMess=new char[1000];
	string ErrMess;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	c=GetFreeDeviceNamesPerType((char*)dtype.c_str(),len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
			pos1=strlen(devNames_list_res+pos2);
			
			strcpy(dto,devNames_list_res+pos2);
			pos2+=pos1+1;
			
			v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetFreeDeviceNamesPerType((char*)dtype.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}


/*****************************************************************************************/
/**
       * Get the list of LkID  which start from the given deviceID,  returning a vector of int.
       * @param nfrom : deviceID
	   * @return  the list of linkIDs, 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*******************************************************/
vector<int> CONFDB::PyGetLkFromDevID(int nfrom)  
{
	vector<int> v;
	int c=0;
	int c1=0;
	int i=0;
	int len_buffer=2000;
	int len_array=len_buffer;
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	int* lkID_list_res=new int[len_buffer];
	c=GetLkFromDevID(nfrom,len_array, lkID_list_res,ErrorMess);
	
	if(c==0 || (lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
	{
		if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] lkID_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
		for(i=0;i<len_array;i++)
			v.push_back(lkID_list_res[i]); 	
		}	
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] lkID_list_res;
			lkID_list_res=new int[len_array];
			c1=GetLkFromDevID(nfrom,len_array, lkID_list_res,ErrorMess);
			if(c1==0 || (lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
			{
				if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] lkID_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					for(i=0;i<len_array;i++)
						v.push_back(lkID_list_res[i]);	
				}
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] lkID_list_res;
				throw CONFDBEXCEPTION(ErrMess);
				//delete [] lkID_list_res;

			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] lkID_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
	}
	 delete [] ErrorMess;
	delete [] lkID_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of micro LkID  which starts from the given cpntID,  returning a vector of int.
       * @param cpntid_from : cpntID which starts the link. Put -1 if you want to get the list of lkids which starts from your motherboard
	   * @param motherboard : id of motherboard (=deviceid of a logical device). Put -1 if you want to get the list of lkids which starts from a cpntboard
	   * @return  the list of micro linkIDs, 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*******************************************************/
vector<int> CONFDB::PyGetMicroLkFromCpntID(int cpntid_from, int motherboard)  
{
	vector<int> v;
	int c=0;
	int c1=0;
	int i=0;
	int len_buffer=2000;
	int len_array=len_buffer;
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	int* lkID_list_res=new int[len_buffer];
	c=GetMicroLkFromCpntID(cpntid_from,motherboard,len_array, lkID_list_res,ErrorMess);
	
	if(c==0 || (lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
	{
		if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] lkID_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
		for(i=0;i<len_array;i++)
			v.push_back(lkID_list_res[i]); 	
		}	
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] lkID_list_res;
			lkID_list_res=new int[len_array];
			c1=GetMicroLkFromCpntID(cpntid_from,motherboard,len_array, lkID_list_res,ErrorMess);
			if(c1==0 || (lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
			{
				if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] lkID_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					for(i=0;i<len_array;i++)
						v.push_back(lkID_list_res[i]);	
				}
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] lkID_list_res;
				throw CONFDBEXCEPTION(ErrMess);
				//delete [] lkID_list_res;

			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] lkID_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
	}
	 delete [] ErrorMess;
	delete [] lkID_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of micro LkID  which ends at the given cpntID,  returning a vector of int.
       * @param cpntid_to : cpntID which ends the link. Put -1 if you want to get the list of lkids which ends from your motherboard
	   * @param motherboard : id of motherboard (=deviceid of a logical device). Put -1 if you want to get the list of lkids which ends from a cpntboard
	   * @return  the list of micro linkIDs, 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*******************************************************/
vector<int> CONFDB::PyGetMicroLkToCpntID(int cpntid_to, int motherboard)  
{
	vector<int> v;
	int c=0;
	int c1=0;
	int i=0;
	int len_buffer=2000;
	int len_array=len_buffer;
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	int* lkID_list_res=new int[len_buffer];
	c=GetMicroLkToCpntID(cpntid_to,motherboard,len_array, lkID_list_res,ErrorMess);
	
	if(c==0 || (lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
	{
		if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] lkID_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
		for(i=0;i<len_array;i++)
			v.push_back(lkID_list_res[i]); 	
		}	
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] lkID_list_res;
			lkID_list_res=new int[len_array];
			c1=GetMicroLkToCpntID(cpntid_to,motherboard,len_array, lkID_list_res,ErrorMess);
			if(c1==0 || (lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
			{
				if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] lkID_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					for(i=0;i<len_array;i++)
						v.push_back(lkID_list_res[i]);	
				}
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] lkID_list_res;
				throw CONFDBEXCEPTION(ErrMess);
				//delete [] lkID_list_res;

			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] lkID_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
	}
	 delete [] ErrorMess;
	delete [] lkID_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of LkID  which ends at the given deviceID,  returning a vector of int
       * @param nto : deviceID
	   * @return  the list of linkIDs, 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.       
       */

/******************************************************/
vector<int> CONFDB::PyGetLkToDevID(int nto)  
{
	vector<int> v;
	int c=0;
	int c1=0;
	int i=0;
	int len_buffer=1000;
	int len_array=len_buffer;
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	int* lkID_list_res=new int[len_buffer];
	c=GetLkToDevID(nto,len_array, lkID_list_res,ErrorMess);
	
	if(c==0 || (lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
	{
		if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] lkID_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			for(i=0;i<len_array;i++)
				v.push_back(lkID_list_res[i]);	
		}
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_buffer<len_array)
		{
			delete [] lkID_list_res;
			lkID_list_res=new int[len_array];
			c1=GetLkToDevID(nto,len_array, lkID_list_res,ErrorMess);
			if(c1==0 || (lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
			{
				if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] lkID_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					for(i=0;i<len_array;i++)
						v.push_back(lkID_list_res[i]);	
				}
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] lkID_list_res;
				throw CONFDBEXCEPTION(ErrMess); 

			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] lkID_list_res;
			throw CONFDBEXCEPTION(ErrMess);	
		}
	}
	delete [] ErrorMess;
	delete [] lkID_list_res;	
	return v;
}
/****************************************************************************************/
/**
       * Get the list of ethernet and ip  of the given deviceID, returning a string .
       * @param dID : deviceID
	   * @return the different ethernet and IP adds . presented as follows |port_nbr?port_way?macaddress?ipaddress|. Each line is separated by '|'.
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.       
       */
/*************************************************************************************/
vector<string> CONFDB::PyGetMacIPAddPerDevID(int dID)  
{
	
	vector<string> v;
	int c=0;
	int c1=0;
	int pos1,pos2;
	int len_buffer=1000;
	int len_array=len_buffer;
	int len_devto=500;
	char* ErrorMess=new char[1000];
	char* Mac_list_res=new char[len_buffer];
	char* dto=new char[len_devto];
	v.clear();
	string ErrMess;
	c=GetMacIPAddPerDevID(dID,len_buffer,Mac_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(Mac_list_res,"NO_ROWS_SELECTED",16)==0)
	{
		
		if(strncmp(Mac_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] Mac_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_buffer)
			{
				pos1=strlen(Mac_list_res+pos2);
				strcpy(dto,Mac_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
	
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array<len_buffer)
		{
			delete [] Mac_list_res;
			Mac_list_res=new char[len_buffer];
			c1=GetMacIPAddPerDevID(dID,len_buffer,Mac_list_res,ErrorMess);
			if(c1==0 ||  strncmp(Mac_list_res,"NO_ROWS_SELECTED",16)==0)
			{
				if(strncmp(Mac_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] Mac_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_buffer)
					{
						pos1=strlen(Mac_list_res+pos2);
						strcpy(dto,Mac_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			}
			else
			{
				ErrMess="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] dto;
				delete [] Mac_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] Mac_list_res;	
			delete [] dto;
			throw CONFDBEXCEPTION(ErrMess);	
		}
	}
	 delete [] ErrorMess;
	 delete [] dto;
	delete [] Mac_list_res;	
	return v;
}
/****************************************/
/**
       * Insert a device type , returning a string .
       * @param sysname : list of the subsystem which use this device type separated with a coma, in upper case.
       * @param devtype : name of the devicetype
	   * @param nb_in : nb of input ports
	   * @param nb_out : nb of output ports
	   * @param rgbcolor : color of the device as represented in CDBVis
	   * @param devdescr :  some description about the component max size 500 characters.
	   * @param last : 1 if it's your  last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyInsertDeviceType(string sysname,string devtype,int nb_in,int nb_out,string  devdescr,string rgbcolor,int last)  
{
string  res_code;
int c=0;

if(devtype.length()<2)
{
res_code="Error:No Devtype given"; 
throw CONFDBEXCEPTION(res_code);	
}
if(last!=1 && last!=0)
{
res_code="last is either 0 or 1"; 
throw CONFDBEXCEPTION(res_code);	
}
char* ErrorMess=new char[1000];
	
if(devdescr.length()==0)
	devdescr=devtype;


c=InsertDeviceType((char*)sysname.c_str(),(char*)devtype.c_str(),nb_in,nb_out,(char*)devdescr.c_str(),(char*) rgbcolor.c_str(), last,ErrorMess);
if(c==0)
	res_code="Successful insertion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}
/*************************************************************************************************/
/**
       * Insert a logical device  returning a string .
       * @param sysname : list of the subsystems which use this logical device,separated by a coma (upper case): ex. VEL0_L1 board is used in the DAQ,TFC and VEL0
	   * @param devname : name of the device
       * @param devtype : name of the devicetype
	   * @param dnode : 1 if it's a host node (usually starts or ends a path such as L1FEs) , 0 otherwise.
	   * @param promismode : 1 if put in this mode (default mode), 0 otherwise
	   * @param serialnb :  serialnb of the device
	   * @param hwtype :  hardware type :put "" if nothing
	   * @param responsible :  for the device: :put "" if no responsible
	   * @param comments :  comments on the device (if there are some defaults but working). put "" if no comments
	   * @param location :  location of the device (same conv. if inserted in the equipment DB). Mandatory
	   * @param function_list : list of the functions of your devices (essentially for the DAQ, for instance DHCP_SERVER, DNS_SERVER...). Functions must be declared using insertSimpleDeviceFunction...If several, separate them with a coma. For ex. DHCP,DNS, NFS.	  
	   * @param last : 1 if it's your  last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/****************************************************************************************************/

string CONFDB::PyInsertFunctionalDevice(string sysname,string devname,string devtype,int dnode,int promismode,string serialnb,string hwtype,string responsible,string location,string comments,string function_list,int last)  
{
string  res_code;
int c=0;
char* ErrorMess=new char[1000];
if(devtype.length()<2 || devname.length()<2)
{
	res_code="Error:No Devtype or devicename given";
	throw CONFDBEXCEPTION(res_code);	
}

if(last!=1 && last!=0)
{
	res_code="Error:last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}

if(dnode!=1 && dnode!=0)
{
	res_code="Error:dnode is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(promismode!=1 && promismode!=0)
{
	res_code="Error:promismode is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}


c=InsertFunctionalDevice((char*)sysname.c_str(),(char*)devname.c_str(),(char*)devtype.c_str(),dnode,promismode,(char*)serialnb.c_str(),(char*)hwtype.c_str(),(char*) responsible.c_str(),(char*) comments.c_str(),(char*) location.c_str(),(char*) function_list.c_str(), last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
			

delete [] ErrorMess;	
return res_code;
}
/*************************************************************************************************/
/**
       * Insert a test logical device (used as test boards  for local tets)  returning a string . their name is automatically given TEST_BOARD_nbr
       * Put all the value which are not null first..
       * @param location : where these tests boards are sit
	   * @param ipaddList : list of the ip address : (same order to enable the association between mac, port nbr, port_type and port way
       * @param subnetList : list of the subnet (same order to enable the association between mac, port nbr, port_type and port way)
	   * @param ipnameList : list of the ipname list,  (same order to enable the association between mac, port nbr, port_type and port way);
	   * @param portnbList : list of portnb (same order to enable the association between mac, port nbr, port_type and port way)
	   * @param port_typeList : list of portype (same order to enable the association between mac, port nbr, port_type and port way)
	   * @param port_wayList :  list of port_way (1 if it's an input, 2 if it's an output)
	   * @param last : 1 if it's your  last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/****************************************************************************************************/

string CONFDB::PyInsertTestFunctionalDevice(string location,vector<string> ipaddList,vector<string> ipnameList,vector<string> subnetList,vector<string> portnbList,vector<string> port_typeList, vector<int> port_wayList,int last)  
{
string  res_code;
string ipline;
int c=0;
char* ErrorMess=new char[1000];
char* ipadd_array;
int ipadd_len=0;
char* ipname_array;
int ipname_len=0;
char* subnet_array;
int subnet_len=0;
char* ptype_array;
int ptype_len=0;
char* pnb_array;
int pnb_len=0;
int* pway_array;
int nb_of_element=0;
unsigned int i=0;
if(last!=1 && last!=0)
{
	res_code="Error:last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}

if(ipaddList.size()!=ipnameList.size() || ipnameList.size()!=subnetList.size())
{
	res_code="nb of ipadd, nb of subnet and nb of ipname should be equal"; 
	throw CONFDBEXCEPTION(res_code);
}
if(port_wayList.size()!=portnbList.size())
{
	res_code="nb of  nb of portnb and nb of port_way should be equal"; 
	throw CONFDBEXCEPTION(res_code);
}
for(i=0;i<ipaddList.size();i++)
{
	ipline=ipaddList[i];
	ipadd_len+=(ipline.length()+1);
	ipline=ipnameList[i];
	ipname_len+=(ipline.length()+1);
	ipline=subnetList[i];
	subnet_len+=(ipline.length()+1);
}
nb_of_element=port_wayList.size();
pway_array=new int[nb_of_element];
for(i=0;i<portnbList.size();i++)
{
	ipline=portnbList[i];
	pnb_len+=(ipline.length()+1);
	pway_array[i]=port_wayList[i];
}
for(i=0;i<port_typeList.size();i++)
{
ipline=port_typeList[i];
ptype_len+=(ipline.length()+1);
}
ptype_array=new char[ptype_len];
pnb_array=new char[pnb_len];
ipadd_array=new char[ipadd_len];
ipname_array=new char[ipname_len];
subnet_array=new char[subnet_len];
int pos1=0;
int pos2=0;
int pos3=0;
int pos4=0;
for(i=0;i<ipaddList.size();i++)
{
	ipline=ipaddList[i];
	pos4=(ipline.length()+1);
	memcpy(ipadd_array+pos1,(char*)ipline.c_str(),pos4);
	pos1+=pos4;

	ipline=ipnameList[i];
	pos4=(ipline.length()+1);
	memcpy(ipname_array+pos2,(char*)ipline.c_str(),pos4);
	pos2+=pos4;

	ipline=subnetList[i];
	pos4=(ipline.length()+1);
	memcpy(subnet_array+pos3,(char*)ipline.c_str(),pos4);
	pos3+=pos4;
}
pos1=0;
pos2=0;
pos3=0;
 pos4=0;
for(i=0;i<port_typeList.size();i++)
{
ipline=port_typeList[i];
pos4=(ipline.length()+1);
memcpy(ptype_array+pos2,(char*)ipline.c_str(),pos4);
pos2+=pos4;
}
for(i=0;i<portnbList.size();i++)
{
ipline=portnbList[i];
pos4=(ipline.length()+1);
memcpy(pnb_array+pos3,(char*)ipline.c_str(),pos4);
pos3+=pos4;
}
c=InsertTestFunctionalDevice((char*)location.c_str(),ipadd_array,ipadd_len,ipname_array,ipname_len,subnet_array,subnet_len,pnb_array,pnb_len, ptype_array,ptype_len,pway_array,nb_of_element, last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
			

delete [] ErrorMess;	
return res_code;
}
/*************************************************************************************************/
/**
       * Insert a spare device  returning a string .
       * @param hwname : name of the hw. Put a name it will be easier for to trace your spare hw. For ex. Spare_board1
       * @param hwtype : name of the hwtype,prefixed by the system name ifthe spare can only be used for the system. It will be used to count the nb of spare by hwtype. Useful for spare management.
	   * @param serialnb :  serialnb of the hw.
	   * @param responsible :  for the device 
	   * @param comments :  comments on the device 
	   * @param location :  location of the device (same conv. if inserted in the equipment DB)
	   * @param last : 1 if it's your  last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/****************************************************************************************************/

string CONFDB::PyInsertSpareDevice(string hwname,string hwtype,string serialnb,string responsible,string location,string comments,int last)  
{
string  res_code;
int c=0;
char* ErrorMess=new char[1000];
if(serialnb.length()<2 || serialnb.length()<2)
{
	res_code="Error: No serialnb given";
	throw CONFDBEXCEPTION(res_code);	
}

if(last!=1 && last!=0)
{
	res_code="Error:last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}



c=InsertSpareDevice((char*)hwname.c_str(),(char*)hwtype.c_str(),(char*)serialnb.c_str(),(char*) responsible.c_str(),(char*) comments.c_str(),(char*) location.c_str(), last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
			

delete [] ErrorMess;	
return res_code;
}
/*************************************************************************************************/
/**
       * Insert multiple spare devices  returning a string .
       * @param hwname : name of the hw. Put a name it will be easier for to trace your spare hw. For ex. Spare_board1
       * @param hwtype : name of the hwtype prefixed with the system if it can be used only for the system. It will be used to count the nb of spare by hwtype. Useful for spare management.
	   * @param serialnb :  serialnb of the hw.
	   * @param responsible :  for the device 
	   * @param comments :  comments on the device 
	   * @param location :  location of the device (same conv. if inserted in the equipment DB)
	   * @param first : 1 if it's your  first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your  last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/****************************************************************************************************/

string CONFDB::PyInsertMultipleSpareDevices(string hwname,string hwtype,string serialnb,string responsible,string location,string comments,int first,int last)  
{
string  res_code;
int c=0;
char* ErrorMess=new char[1000];
if(serialnb.length()<2 || serialnb.length()<2)
{
	res_code="Error: No serialnb given";
	throw CONFDBEXCEPTION(res_code);	
}

if(last!=1 && last!=0)
{
	res_code="Error:last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(first!=1 && first!=0)
{
	res_code="Error:first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}


c=InsertMultipleSpareDevices((char*)hwname.c_str(),(char*)hwtype.c_str(),(char*)serialnb.c_str(),(char*) responsible.c_str(),(char*) comments.c_str(),(char*) location.c_str(), first,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
			

delete [] ErrorMess;	
return res_code;
}
/*************************************************************************************************/
/**
       * Insert multiple board cpnts  returning a string .
       * @param cpntname : logical name of the cpnt (even if it's a spare cpnt, put its function name in the board)
       * @param cpnttype : name of the cpnttype
	   * @param replacable : 1 if it's replacable on its own , 0 otherwise.
	   * @param motherboardname : Put "none", if you insert a spare board cpnt. otherwise the name (logical device name) where the board cpnt sits
	   * @param serialnb :  serial_nb of the cpnt
	   * @param hwtype :hardware type : this one should be the same as you declared for your spare (to manage spare cpnt)
	   * @param responsible : guy responsible for this hw
	   * @param comments : some comments about the hw (max.1000 characters)
	   * @param location :  Serialnb nb of the board in which your board cpnt sits. If it's just a spare, which is not located on a board, put the location (building, shelves..)
	   * @param first : 1 if it's your first MultipleCpnt insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your MultipleCpnt last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
  	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/****************************************************************************************************/

string CONFDB::PyInsertMultipleBoardCpnts(string cpntname,string cpnttype,int replacable,string motherboardname,string serialnb,string hwtype,string responsible,string location,string comments,int first,int last)  
{
string  res_code;
int c=0;
char* ErrorMess=new char[1000];
if(replacable!=1 && replacable!=0)
{
	res_code="Error:replacable is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}

if(last!=1 && last!=0)
{
	res_code="Error:last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(first!=1 && first!=0)
{
	res_code="Error:first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(motherboardname=="none" && location.length()<1)
{
	res_code="Error: you should give a location"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(location=="none" && motherboardname.length()<1)
{
	res_code="Error: you should give the motherboardname"; 
	throw CONFDBEXCEPTION(res_code);	
}
c=InsertMultipleBoardCpnts((char*)cpntname.c_str(),(char*)cpnttype.c_str(),replacable,(char*)motherboardname.c_str(),(char*)serialnb.c_str(),(char*)hwtype.c_str(),(char*) responsible.c_str(),(char*) comments.c_str(),(char*) location.c_str(), first,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
			

delete [] ErrorMess;	
return res_code;
}
/****************************************************************/
/**
       * Insert a Simple Link Type,  returning a string .
	   * @param lktype : name of the link type
	   * @param last : 1 if it's your last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
/***********************************************************************************************/

string CONFDB::PyInsertSimpleLinkType(string lktype,int last)  
{
string  res_code;
int c=0;

if(lktype.length()<2)
{
	res_code="Error:No lktype given"; 
	throw CONFDBEXCEPTION(res_code);	
	
}	
if(last!=1 && last!=0)
{
	res_code="Error:last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrorMess=new char[1000];
c=InsertSimpleLinkType((char*)lktype.c_str(), last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	
delete [] ErrorMess;
return res_code;
}
/***********************/
//lktype_list is a list of link_name : you should put hte link names between single quote and separate then with a comma
/*******************************/
/*****************************************************************/
/**
       * Insert a Composite Link Type,  returning a string .
	   * @param lktype : name of the link type (such as "Mixed_Data")		 
	   * @param lktype_list_concan : name of the link types which compose the composite links, separated by "," 
	   * @param lktype_list_concan :ex. Mixed_Data is composed of HLT_Data and L1_Data, so simple_lk_list="HLT_Data,L1_Data"
	   * @param last : 1 if it's your last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
/*************************************************************************/

string CONFDB::PyInsertCompositeLinkType(string lktype,string lktype_list_concan,int last)  
{
string  res_code;
int c=0;

if(lktype.length()<1)
{
	res_code="Error:No lktype given"; 
	throw CONFDBEXCEPTION(res_code);	
	
}

if(last!=1 && last!=0)
{
	res_code="Error:last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(lktype_list_concan.length()<2)
{
	res_code="Error:list of link is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}

char* ErrorMess=new char[1000];			
c=InsertCompositeLinkType((char*)lktype.c_str(),(char*)lktype_list_concan.c_str(),last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
		
delete [] ErrorMess;

return res_code;
}

/************************************************************************************/
/**
	   * Insert  Ports, returning a string .
	   * @param devname : name of the device
	   * @param ptnb   : port nb of the device
	   * @param param_list: vector of int which must be filled as follows
	   * @param param_list : [0] corresponds to speed   : speed of the port 
	   * @param param_list : [1] corresponds to admin_status : 1 if the port status is OK, 0 otherwise
	   * @param param_list : [2] corresponds to pxi_booting : 1 if the port is the pxi_booting one, 0 otherwise
	   * @param port_type : type of the port: used in the DAQ (BMC,CONTROL). Put "" if none.
	   * @param port_way : 1 if the port is input, 2 otherwise. References: data flow from the detector (1); control flow (2).
	   * @param phy: possible values :"SX","T","SL" . Put "" if irrelevant (used in the DAQ)
	   * @param ipname : associated with this port. Put "" if irrelevant (used in the DAQ)
	   * @param ipadd : associated with this port. Put "" if irrelevant (used in the DAQ)
	   * @param subnet : associated with the IPadd. Put "" if irrelevant (used in the DAQ)
	   * @param macadd : associated with this port. Put "" if irrelevant (used in the DAQ)
	   * @param bia : mac address burnt associated with port. Put "" if irrelevant (used in the DAQ)
	   * @param last : 1 if it's your last Port insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 


/*************************************************************************************/

string CONFDB::PyInsertPort(string devname,string ptnb,string port_type,int port_way,string bia,string ipname,string ipadd,string subnet,string macadd,vector<int> param_list, string phy,int last)  
{
string  res_code;
int c=0;
int speed=param_list[0];
int admin_status=param_list[1];
int pxi_booting=param_list[2];

if(pxi_booting!=1 && pxi_booting!=0)
{
	res_code="pxi_bootingis equal to 1 or 0";
	throw CONFDBEXCEPTION(res_code);	
}
	

if(admin_status!=1 && admin_status!=0)
{
	res_code="admin_status is equal to 1 or 0";
	throw CONFDBEXCEPTION(res_code);	
}


if(port_way!=1 && port_way!=2)
{
	res_code="port_way is equal to 1 or 2";
	throw CONFDBEXCEPTION(res_code);	
}



if(devname.length()<2 )
{
	res_code="Error:devicename value is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last!=1 && last!=0 )
{
	res_code="Error:last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrorMess=new char[1000];
c=InsertPort((char*)devname.c_str(),(char*)ptnb.c_str(),admin_status,port_way,speed,pxi_booting,(char*) port_type.c_str(),(char*) bia.c_str(),(char*) ipadd.c_str(),(char*) ipname.c_str(),(char*) macadd.c_str(),(char*)subnet.c_str(),(char*) phy.c_str(),last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
delete [] ErrorMess;
return res_code;
}

/***************************************************************************/
/**
       * Insert  Link  returning a string .
       
	   * @param nfrom : name of the device starting the link
	   * @param nto : name of the device ending the link
	   * @param pfrom : port nb of the device from
	   * @param pto : port nb of the device to
	   * @param port_typefrom : Put "none" in lower case, if you didn't put anything when you insert this port
	   * @param port_typeto : Put "none" in lower case, if you didn't put anything when you insert this port
	   * @param lk_type : name of the link type
	   * @param bidirectional :  1 if it's a bidirectional link, 0 otherwise
	   * @param link_info :  some info about a link (max length 1000)
	   * @param last : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
/*****************************************************************************/

string CONFDB::PyInsertMacroLink(string nfrom,string nto,string pfrom,string pto,string port_typefrom,string port_typeto,string lk_type ,int bidirectional,string link_info,int last)  
{
string  res_code;

int c=0;


if(nfrom.length()==0)
{
	res_code="you must give a node from name";
	throw CONFDBEXCEPTION(res_code);	
}

if(nto.length()==0)
{
	res_code="you must give a node to name";
	throw CONFDBEXCEPTION(res_code);	
}
if(lk_type.length()==0)
{
	res_code="you must give a lk type name";
	throw CONFDBEXCEPTION(res_code);	
}

if(last!=1 && last!=0 )
{
	res_code="last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrorMess=new char[1000];

c=InsertMacroLink((char*)nfrom.c_str(), (char*)nto.c_str(),(char*)pfrom.c_str(),(char*)pto.c_str(),(char*) port_typefrom.c_str(),(char*) port_typeto.c_str(),(char*)lk_type.c_str(),(char*)link_info.c_str(),bidirectional,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	
delete [] ErrorMess;	
return res_code;
}
/****************************************/
/**
       * Insert a device type , returning a string .
       * @param sysname : listof the subsystem which use this device types, (upper case, separated by a coma )
       * @param devtype : name of the devicetype
	   * @param nb_in : nb of input ports
	   * @param nb_out : nb of output ports
	   * @param rgbcolor : color in RGB reference (for display)
	   * @param devdescr :  some description about the component max size 500 characters.
	   * @param first : 1 if it's your MultipleDeviceType first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your MultipleDeviceType last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyInsertMultipleDeviceTypes(string sysname,string devtype,int nb_in,int nb_out,string devdescr,string rgbcolor,int first,int last)  
{
string  res_code;
int c=0;

if(devtype.length()<2)
{
res_code="No Devtype given"; 
throw CONFDBEXCEPTION(res_code);	
}
if(last!=1 && last!=0)
{
res_code="last is either 0 or 1"; 
throw CONFDBEXCEPTION(res_code);	
}
if(first!=0 && first!=1)
{
	res_code="first is 0 or 1";
	throw CONFDBEXCEPTION(res_code);
}
char* ErrorMess=new char[1000];
if(devdescr.length()==0)
	devdescr=devtype;
	
c=InsertMultipleDeviceTypes((char*)sysname.c_str(),(char*)devtype.c_str(),nb_in,nb_out,(char*)devdescr.c_str(),(char*) rgbcolor.c_str(),first, last,ErrorMess);
if(c==0)
	res_code="Successful insertion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	

delete [] ErrorMess;	
return res_code;
}
/*************************************************************************************************/
/**
       * Insert a logical device  returning a string .
       * @param sysname : list of the subsystems which use this logical device,separeted by a coma (upper case): ex. VEL0_L1 board is used in the DAQ,TFC and VEL0
	   * @param devname : name of the device
       * @param devtype : name of the devicetype
	   * @param dnode : 1 if it's a host node (usually starts or ends a path such as L1FEs) , 0 otherwise.
	   * @param promismode : 1 if put in this mode (default mode), 0 otherwise
	   * @param serialnb :  serialnb of the device
	   * @param hwtype :  hardware type : 
	   * @param responsible :  for the device 
	   * @param comments :  comments on the device 
	   * @param location :  location of the device (same conv. if inserted in the equipment DB)
	   * @param function_list :  List of functions, separeted by a coma. Ex: DHCP_SERVER, DNS_SERVERPut "none", if there is not.
	   * @param first : 1 if it's your MultipleDeviceType first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your  last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/****************************************************************************************************/

string CONFDB::PyInsertMultipleFunctionalDevices(string sysname,string devname,string devtype,int dnode,int promismode,string serialnb,string hwtype,string responsible,string location,string comments,string function_list,int first,int last)  
{
string  res_code;
int c=0;

if(devtype.length()<2 || devname.length()<2)
{
	res_code="No Devtype or devicename given";
	throw CONFDBEXCEPTION(res_code);	
}

if(last!=1 && last!=0)
{
	res_code="last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}

if(dnode!=1 && dnode!=0)
{
	res_code="dnode is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(promismode!=1 && promismode!=0)
{
	res_code="promismode is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(first!=0 && first!=1)
{
	res_code="first is equal to 0 or 1";
	throw CONFDBEXCEPTION(res_code);
}


char* ErrorMess=new char[1000];

c=InsertMultipleFunctionalDevices((char*)sysname.c_str(),(char*)devname.c_str(),(char*)devtype.c_str(),dnode,promismode,(char*)serialnb.c_str(),(char*)hwtype.c_str(),(char*) responsible.c_str(),(char*) comments.c_str(),(char*) location.c_str(),(char*) function_list.c_str(),first, last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrorMess;
return res_code;
}
/****************************************************************/
/**
       * Insert a Simple Link Type, returning a string .
	   * @param lktype : name of the link type
	   * @param first: 1 if it's your SimpleLinkType first insert (save it into the database), 0 otherwise
	   * @param last: 1 if it's your SimpleLinkType last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION 
       */ 
/***********************************************************************************************/
string CONFDB::PyInsertMultipleSimpleLinkTypes(string lktype,int first,int last)  
{
string  res_code;
int c=0;
if(lktype.length()<2)
{
	res_code="No lktype given"; 
	throw CONFDBEXCEPTION(res_code);	
	
}	
if(last!=1 && last!=0)
{
	res_code="last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(first!=0 && first!=1)
{
	res_code="first is equal to 0 or 1";
	throw CONFDBEXCEPTION(res_code);
}
char* ErrorMess=new char[1000];

c=InsertMultipleSimpleLinkTypes((char*)lktype.c_str(), first,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	
delete [] ErrorMess;
return res_code;
}
/***********************/
//lktype_list is a list of link_name : you should put hte link names between single quote and separate then with a comma
/*******************************/
/*****************************************************************/
/**
       * Insert a Composite Link Type,  returning a string .
	   * @param lktype : name of the link type (such as "Mixed_Data")		 
	   * @param lktype_list_concan : name of the link types which compose the composite links, separated by "," 
	   * @param lktype_list_concan :ex. Mixed_Data is composed of HLT_Data and L1_Data, so simple_lk_list="HLT_Data,L1_Data"
	   * @param first : 1 if it's your CompositeLinkType first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your CompositeLinkType last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION 
       */ 
/*************************************************************************/
string CONFDB::PyInsertMultipleCompositeLinkTypes(string lktype,string lktype_list_concan,int first,int last)  
{
string  res_code;
int c=0;
if(lktype.length()<1)
{
	res_code="No lktype given"; 
	throw CONFDBEXCEPTION(res_code);	
	
}

if(last!=1 && last!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(lktype_list_concan.length()<2)
{
	res_code="list of link is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}

if(first!=0 && first!=1)
{
	res_code="first is equal to 0 or 1";
	throw CONFDBEXCEPTION(res_code);
}
char* ErrorMess=new char[1000];
c=InsertMultipleCompositeLinkTypes((char*)lktype.c_str(),(char*)lktype_list_concan.c_str(),first,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrorMess;
return res_code;
}
/************************************************************************************/
/**
	   * Insert  multiple Ports, returning a string .
	   * @param devname : name of the device
	   * @param ptnb   : port nb of the device
	   * @param param_list: vector of int which must be filled as follows
	   * @param param_list : [0] corresponds to speed   : speed of the port 
	   * @param param_list : [1] corresponds to admin_status : 1 if the port status is OK, 0 otherwise
	   * @param param_list : [2] corresponds to pxi_booting : 1 if the port is the pxi_booting one, 0 otherwise
	   * @param port_type : type of the port: used in the DAQ (BMC,CONTROL). Put ""  if none.
	   * @param port_way : 1 if the port is input, 2 otherwise. References: data flow from the detector (1); control flow (2).
	   * @param phy: possible values :"SX","T","SL" . Put "" if irrelevant (used in the DAQ)
	   * @param ipname : associated with this port. Put "" if irrelevant (used in the DAQ)
	   * @param ipadd : associated with this port. Put "" if irrelevant (used in the DAQ)
	   * @param subnet : associated with the IPadd. Put "" if irrelevant (used in the DAQ)
	   * @param macadd : associated with this port. Put "" if irrelevant (used in the DAQ)
	   * @param bia : mac address burnt associated with port. Put "" if irrelevant (used in the DAQ)
	   * @param first : 1 if it's your CompositeLinkType first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your last Port insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 


/*************************************************************************************/

string CONFDB::PyInsertMultiplePorts(string devname,string ptnb,int port_way,string port_type,string bia,string ipname,string ipadd,string subnet,string macadd,vector<int> param_list, string phy,int first,int last)  
{
string  res_code;
int c=0;
int speed=param_list[0];
int admin_status=param_list[1];
int pxi_booting=param_list[2];

if(pxi_booting!=1 && pxi_booting!=0)
{
	res_code="pxi_bootingis equal to 1 or 0";
	throw CONFDBEXCEPTION(res_code);	
}
	

if(admin_status!=1 && admin_status!=0)
{
	res_code="admin_status is equal to 1 or 0";
	throw CONFDBEXCEPTION(res_code);	
}


if(port_way!=1 && port_way!=2)
{
	res_code="port_way is equal to 1 or 2";
	throw CONFDBEXCEPTION(res_code);	
}



if(devname.length()<2 )
{
	res_code="Error:devicename value is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last!=1 && last!=0 )
{
	res_code="last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(first!=0 && first!=1)
{
	res_code="first is equal to 0 or 1";
	throw CONFDBEXCEPTION(res_code);
}
char* ErrorMess=new char[1000];

c=InsertMultiplePorts((char*)devname.c_str(),(char*)ptnb.c_str(),admin_status,port_way,speed,pxi_booting,(char*) port_type.c_str(),(char*) bia.c_str(),(char*) ipadd.c_str(),(char*) ipname.c_str(),(char*)subnet.c_str(),(char*) macadd.c_str(),(char*) phy.c_str(),first,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}

delete [] ErrorMess;
return res_code;
}
/************************************************************************************/
/**
	   * Insert  multiple Spare Ports to be used ONLY IF your spare ports have mac or bia address (for L0 electronics, no need), returning a string .
	   * @param serialnb : serialnb of the hw
	   * @param ptnb   : port nb of the device
	   * @param port_type : type of the port: used in the DAQ (BMC,CONTROL). Put "" if none.
	   * @param port_way : 1 if the port is input, 2 otherwise. References: data flow from the detector (1); control flow (2).
       * @param macadd : associated with this port. Put "" if irrelevant (used in the DAQ)
	   * @param bia : mac address burnt associated with port. Put "" if irrelevant (used in the DAQ)
	   * @param first : 1 if it's your CompositeLinkType first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your last Port insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 


/*************************************************************************************/

string CONFDB::PyInsertMultipleSparePorts(string serialnb,string ptnb,int port_way,string port_type,string bia,string macadd, int first,int last)  
{
string  res_code;
int c=0;





if(port_way!=1 && port_way!=2)
{
	res_code="port_way is equal to 1 or 2";
	throw CONFDBEXCEPTION(res_code);	
}



if(serialnb.length()<2 )
{
	res_code="Error:serialnb  value is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(bia.length()<2 )
{
	res_code="Error:bia  value is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(macadd.length()<2 )
{
	res_code="Error:mac  value is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last!=1 && last!=0 )
{
	res_code="last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(first!=0 && first!=1)
{
	res_code="first is equal to 0 or 1";
	throw CONFDBEXCEPTION(res_code);
}
char* ErrorMess=new char[1000];

c=InsertMultipleSparePorts((char*)serialnb.c_str(),(char*)ptnb.c_str(),port_way,(char*) port_type.c_str(),(char*) bia.c_str(),(char*) macadd.c_str(),first,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}

delete [] ErrorMess;
return res_code;
}
/************************************************************************************/
/**
	   * Insert  a  Spare Port to be used ONLY IF your spare ports have mac or bia address (for L0 electronics, no need), returning a string .
	   * @param serialnb : serialnb of the hw
	   * @param ptnb   : port nb of the device
	   * @param port_type : type of the port: used in the DAQ (BMC,CONTROL). Put "" if none.
	   * @param port_way : 1 if the port is input, 2 otherwise. References: data flow from the detector (1); control flow (2).
       * @param macadd : associated with this port. 
	   * @param bia : mac address burnt associated with port. 
	   * @param last : 1 if it's your last Port insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 


/*************************************************************************************/

string CONFDB::PyInsertSparePort(string serialnb,string ptnb,int port_way,string port_type,string bia,string macadd, int last)  
{
string  res_code;
int c=0;





if(port_way!=1 && port_way!=2)
{
	res_code="port_way is equal to 1 or 2";
	throw CONFDBEXCEPTION(res_code);	
}



if(serialnb.length()<2 )
{
	res_code="Error:serialnb  value is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(bia.length()<2 )
{
	res_code="Error:bia  value is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(macadd.length()<2 )
{
	res_code="Error:mac  value is empty"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last!=1 && last!=0 )
{
	res_code="last_rows is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}

char* ErrorMess=new char[1000];

c=InsertSparePort((char*)serialnb.c_str(),(char*)ptnb.c_str(),port_way,(char*) port_type.c_str(),(char*) bia.c_str(),(char*) macadd.c_str(),last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}

delete [] ErrorMess;
return res_code;
}
/***************************************************************************/
/**
       * Insert Multiple macro Link  returning a string .
       
	   * @param nfrom : name of the device starting the link
	   * @param nto : name of the device ending the link
	   * @param pfrom : port nb of the device from
	   * @param pto : port nb of the device to
	   * @param port_typefrom : type of the port from as declared in port_table. Put "none" in lower case if none
	   * @param port_typeto : type of the port to as declared in port_table. Put "none" in lower case if none
	   * @param lk_type : name of the link type
	   * @param bidirectional :  1 if it's a bidirectional link, 0 otherwise
	   * @param link_info : info about the link (max. size 1000)
	   * @param first : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful,
	   * @throw CONFDBEXCEPTION
       */ 
/*****************************************************************************/

string CONFDB::PyInsertMultipleMacroLinks(string nfrom,string nto,string pfrom,string pto,string port_typefrom,string port_typeto,string lk_type ,int bidirectional,string link_info,int first,int last)  
{
string  res_code;

int c=0;



if(nfrom.length()==0)
{
	res_code="you must give a node from name";
	throw CONFDBEXCEPTION(res_code);	
}

if(nto.length()==0)
{
	res_code="you must give a node to name";
	throw CONFDBEXCEPTION(res_code);	
}
if(lk_type.length()==0)
{
	res_code="you must give a lk type name";
	throw CONFDBEXCEPTION(res_code);	
}



char* ErrorMess=new char[1000];






if(last!=0 && last!=1)
{
	res_code="last is equal to 0 or 1";
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
if(first!=0 && first!=1)
{
	res_code="first is equal to 0 or 1";
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	
c=InsertMultipleMacroLinks((char*)nfrom.c_str(), (char*)nto.c_str(),(char*)pfrom.c_str(),(char*)pto.c_str(),(char*) port_typefrom.c_str(),(char*)port_typeto.c_str(),(char*)lk_type.c_str(),(char*)link_info.c_str(),bidirectional,first,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	

delete [] ErrorMess;
return res_code;
}


/***************************************************************************/
/**
       * Insert Multiple micro Links  returning a string .
       
	   * @param nfrom : name of the board cpnt starting the link : put "motherboard", if it's the board name in which the board cpnt sits.
	   * @param nto : name of the board cpnt ending the link : put "motherboard", if it's the board name in which the board cpnt sits.
	   * @param pfrom : port nb of the cpnt from. Put the portid if node_from="motherboard"
	   * @param pto : port nb of the cpnt to. Put the portid if node_to="motherboard"
	   * @param lk_type : name of the link type
	   * @param bidirectional :  1 if it's a bidirectional link, 0 otherwise
	   * @param first : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful,
	   * @throw CONFDBEXCEPTION
       */ 
/*****************************************************************************/

string CONFDB::PyInsertMultipleMicroLinks(string nfrom,string nto,int pfrom,int pto,string lk_type ,int bidirectional,int first,int last)  
{
string  res_code;

int c=0;



if(nfrom.length()==0)
{
	res_code="you must give a node from name";
	throw CONFDBEXCEPTION(res_code);	
}

if(nto.length()==0)
{
	res_code="you must give a node to name";
	throw CONFDBEXCEPTION(res_code);	
}
if(lk_type.length()==0)
{
	res_code="you must give a lk type name";
	throw CONFDBEXCEPTION(res_code);	
}



char* ErrorMess=new char[1000];






if(last!=0 && last!=1)
{
	res_code="last is equal to 0 or 1";
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
if(first!=0 && first!=1)
{
	res_code="first is equal to 0 or 1";
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	
c=InsertMultipleMicroLinks((char*)nfrom.c_str(), (char*)nto.c_str(),pfrom,pto,(char*)lk_type.c_str(),bidirectional,first,last,ErrorMess);
if(c==0)
	res_code="Successful insertion"; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	

delete [] ErrorMess;
return res_code;
}
/******************************************************************************************************/
/**
       * Get the port nbs out between dev_from and devtype,  returning a vector of int .
       * @param nfrom : name of the device 1 (ex. THOR_00)
       * @param nto : name of a devicetype  (ex. ECAL_L1FE) or the fixed part of a set of devices
       * @param nto_dtype : 1 if it's the devicetype you give, 0 if it's the fixed part.
	   * @param reload_connectivity : 1 if you want to load or reload the connectivity (we load only the connectivity of the subsystems in which dev_from is part of . for instance if dev_from belongs to HCAL, we load only the connectivity of HCAL.(To set to 1, after a change or if it is the first time or if the device is not in the same subsystem) otherwise 0. 
	   * @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0. N.B If ErrMess is like "Unsuccessful Malloc" or like NB of paths is too big,too many bidirectional_links, then you have to reload the connectivity table (it has been deleted).  If ErrMess looks like "NO_ROWS_SELECTED" or "DEVICE_NOT_CONNECTED", you don't need to reload the table	  
       * @return the list of ports (concatenation of port_nbr and port_type (separated with |)). The first entry of the vector is formatted as follows port_way|port_nb|port_type. The port_wway is not repeated as it is the same for the others.
       * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */


/****************************************************************************************************************************/
vector<string> CONFDB::PyGetConnOutBetweenDeviceDeviceType(string nfrom,string nto,int nto_dtype,int reload_connectivity,int delete_connectivity)  
{
int res_code=0;


char* ErrorMess=new char[1000];
string ErrMess;
int k,j,i,l,pnb;
char* port_temp=new char[100];
char *pfrom_list=new char[1000];
int len_array=1000;
int len_array_copy=len_array;
vector<string> v;
if(nto_dtype!=0 && nto_dtype!=1)
{
ErrMess="nto_dtype is 1 if nto is a device type, 0 if it's a fixed part";
delete [] ErrorMess;
delete [] pfrom_list;	
throw CONFDBEXCEPTION(ErrMess);
}
v.clear();
res_code=GetConnOutBetweenDeviceDeviceType((char*)nfrom.c_str(),(char*)nto.c_str(),nto_dtype, len_array, pfrom_list,reload_connectivity,delete_connectivity,ErrorMess);
if(res_code!=0)
{
	if(len_array>len_array_copy) 
	{
	
		delete [] pfrom_list;
		k=0;
		j=0;
		l=0;
		pnb=0;

		pfrom_list=new char[len_array];
		res_code=GetConnOutBetweenDeviceDeviceType((char*)nfrom.c_str(),(char*)nto.c_str(),nto_dtype,  len_array,pfrom_list,reload_connectivity,delete_connectivity,ErrorMess);
		if(res_code!=0)
		{
				ErrMess=ErrorMess; 
				delete [] ErrorMess;
				delete [] pfrom_list;
				delete [] port_temp;
				throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			i=0;
			while(i<len_array)
			{
				j=i;
				strcpy(port_temp,pfrom_list+i);
				v.push_back(port_temp); //need to look how stuff have separated
				i+=strlen(pfrom_list+j)+1;
			}
		}
	}
	else
	{
			ErrMess=ErrorMess; 
			delete [] ErrorMess;
			delete [] pfrom_list;	
			delete [] port_temp;
			throw CONFDBEXCEPTION(ErrMess);
		
	}
}
else
{
		i=0;
			while(i<len_array)
			{
				j=i;
				strcpy(port_temp,pfrom_list+i);
				v.push_back(port_temp); //need to look how stuff have separated
				i+=strlen(pfrom_list+j)+1;
			}
}
 delete [] ErrorMess;
	delete [] pfrom_list;
	delete [] port_temp;
	return v;

}
/***********************************/
//return a vector of strings, 
/**********************************************************************************************************/
/**
       * Get the path details  between nfrom and nto,  returning a vector of strings .
       * @param nfrom : name of the device 1 (ex. THOR_00)
	   * @param nto : name of the device 2 (ex. ECAL_L1FE06_00_00)
	   * @param reload_connectivity : 1 if you want to load or reload the connectivity (we load only the connectivity of the subsystems in which dev_from is part of . for instance if dev_from belongs to HCAL, we load only the connectivity of HCAL.(To set to 1, after a change or if it is the first time or if the device is not in the same subsystem) otherwise 0. 
	   * @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0. N.B If ErrMess is like "Unsuccessful Malloc" or like NB of paths is too big,too many bidirectional_links, then you have to reload the connectivity table (it has been deleted).  If ErrMess looks like "NO_ROWS_SELECTED" or "DEVICE_NOT_CONNECTED", you don't need to reload the table	  
	   * @return a vector of strings, one entry of the vector corresponds to the concetenation of node_from_name|node_to_name|port_nbfrom|port_typefrom|port_wayfrom|port_nbto|port_typeto|port_wayto|lk_pos|pathid|lkinfo
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	   */
/********************************************************************************************************************/
vector<string> CONFDB::PyGetDetailedConnectivityBetweenDevices(string nfrom,string nto,int reload_connectivity,int delete_connectivity)  
{
int res_code=0;
vector<string> v;

int k,j,i,l,pnb;
int len_array=2000;
int len_array_copy=len_array;
int pfrom_len=5000;
int pto_len=5000;
int nfrom_len=500000;
int nto_len=500000;
int lkinfo_len=5000000;
int pfrom_len1=pfrom_len;
int pto_len1=pto_len;
int nfrom_len1=nfrom_len;
int nto_len1=nto_len;
int nt1,pt1,nf1,pf1,pf0,pt0,nf0,nt0,lkin1,lkin0;
char *pfrom_list2=new char[pfrom_len];
char *pto_list2=new char[pto_len];
int m=0;
char *nfrom_list2=new char[nfrom_len];
char *nto_list2=new char[nto_len];
char *link_info_list=new char[lkinfo_len];
int lkinfo_len1=lkinfo_len;

int *pfrom_list1=new int[len_array];
int *pto_list1=new int[len_array];

int *pid_list1=new int[len_array];
int *link_pos_list=new int[len_array];
string resline;
char*  resline1=new char[1000];
char* ErrorMess=new char[1000];
v.clear();

res_code=GetDetailedConnectivityBetweenDevices((char*)nfrom.c_str(),(char*)nto.c_str(),nfrom_list2,len_array,nfrom_len,pfrom_len,nto_len,pto_len,lkinfo_len, pfrom_list2,pto_list2, pfrom_list1,pto_list1, nto_list2, pid_list1,link_pos_list,link_info_list,reload_connectivity,delete_connectivity,ErrorMess);
if(res_code==0)
{
	nf1=0;
	nt1=0;
	pt1=0;
	pf1=0;
	lkin1=0;
	for(i=0;i<len_array;i++)
	{
	  k=pfrom_list1[i];			
	  l=pto_list1[i];
	  j=pid_list1[i];
	  nf0=nf1;
	  pf0=pf1;
	  pt0=pt1;
	  nt0=nt1;
	  lkin0=lkin1;
	  m=link_pos_list[i];
	  sprintf(resline1,"%d|%d|%s|%s|%d|%s|%s|%d|%s",j,m,nfrom_list2+nf1,pfrom_list2+pf1,k,nto_list2+nt1,pto_list2+pt1,l,link_info_list+lkin1);
	  resline=resline1;
	  nf1+=strlen(nfrom_list2+nf0)+1;
	  nt1+=strlen(nto_list2+nt0)+1;
	  pf1+=strlen(pfrom_list2+pf0)+1;
	  pt1+=strlen(pto_list2+pt0)+1;
	  lkin1+=strlen(link_info_list+lkin0)+1;
	  v.push_back(resline);
	}
}
else
{
	if(len_array==len_array_copy && pfrom_len==pfrom_len1 &&pto_len==pto_len1 && nfrom_len==nfrom_len1 && nto_len==nto_len1 &&lkinfo_len1==lkinfo_len) //means there is an error not due to buffers too small
	{
		
		resline=ErrorMess; 
		delete [] ErrorMess;
		delete [] resline1;
		delete [] pfrom_list1;
		delete [] pto_list1;
		delete [] pfrom_list2;
		delete [] pto_list2;
			delete [] nfrom_list2;
		delete [] nto_list2;
		delete [] pid_list1;
		delete [] link_pos_list;
		delete [] link_info_list;
			throw CONFDBEXCEPTION(resline);
		
	}
	else
	{
		delete [] pfrom_list1;
		delete [] pto_list1;
			delete [] pfrom_list2;
		delete [] pto_list2;
			delete [] nfrom_list2;
		delete [] nto_list2;
		delete [] pid_list1;
		delete [] link_pos_list;
		delete [] link_info_list;
		k=0;
		j=0;
		l=0;
		pnb=0;
link_pos_list=new int[len_array];
		pfrom_list1=new int[len_array];
		pto_list1=new int[len_array];
		pid_list1=new int[len_array];
	pfrom_list2=new char[pfrom_len];
pto_list2=new char[pto_len];
nfrom_list2=new char[nfrom_len];
nto_list2=new char[nto_len];
link_info_list=new char[lkinfo_len];

res_code=GetDetailedConnectivityBetweenDevices((char*)nfrom.c_str(),(char*)nto.c_str(),nfrom_list2,len_array,nfrom_len,pfrom_len,nto_len,pto_len,lkinfo_len, pfrom_list2,pto_list2, pfrom_list1,pto_list1, nto_list2, pid_list1,link_pos_list,link_info_list,reload_connectivity,delete_connectivity,ErrorMess);
		if(res_code==0)
		{
				
			nf1=0;
	nt1=0;
	pt1=0;
	pf1=0;
	lkin1=0;
	for(i=0;i<len_array;i++)
	{
	  k=pfrom_list1[i];			
	  l=pto_list1[i];
	  j=pid_list1[i];
	  nf0=nf1;
	  pf0=pf1;
	  pt0=pt1;
	  nt0=nt1;
	  lkin0=lkin1;
	  m=link_pos_list[i];
	  sprintf(resline1,"%d|%d|%s|%s|%d|%s|%s|%d|%s",j,m,nfrom_list2+nf1,pfrom_list2+pf1,k,nto_list2+nt1,pto_list2+pt1,l,link_info_list+lkin1);
	  resline=resline1;
	  nf1+=strlen(nfrom_list2+nf0)+1;
	  nt1+=strlen(nto_list2+nt0)+1;
	  pf1+=strlen(pfrom_list2+pf0)+1;
	  pt1+=strlen(pto_list2+pt0)+1;
	  lkin1+=strlen(link_info_list+lkin0)+1;
	  v.push_back(resline);
	}
			
		}
		else
		{
			
				resline=ErrorMess; 
				delete [] ErrorMess;
				delete [] resline1;
				delete [] pfrom_list1;
		delete [] pto_list1;
			delete [] pfrom_list2;
		delete [] pto_list2;
			delete [] nfrom_list2;
		delete [] nto_list2;
		delete [] pid_list1;
		delete [] link_pos_list;
		delete [] link_info_list;
				throw CONFDBEXCEPTION(resline);
			

		}
	}
}
delete [] ErrorMess;
delete [] resline1;
	delete [] pfrom_list1;
		delete [] pto_list1;
			delete [] pfrom_list2;
		delete [] pto_list2;
			delete [] nfrom_list2;
		delete [] nto_list2;
		delete [] pid_list1;
		delete [] link_pos_list;
		delete [] link_info_list;
return v;
}
/***********************************/
//return a vector of strings, 
/******************************************************************************************************/
/**
       * Get the path (just the first and last link details)  between nfrom and nto,  returning a vector of strings .
       * @param nfrom : name of the device 1 (ex. THOR_00)
	   * @param nto : name of the device 2 (ex. ECAL_L1FE06_00_00)
	   * @param reload_connectivity : 1 if you want to load or reload the connectivity (we load only the connectivity of the subsystems in which dev_from is part of . for instance if dev_from belongs to HCAL, we load only the connectivity of HCAL.(To set to 1, after a change or if it is the first time or if the device is not in the same subsystem) otherwise 0. 
	   * @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0. N.B If ErrMess is like "Unsuccessful Malloc" or like NB of paths is too big,too many bidirectional_links, then you have to reload the connectivity table (it has been deleted).  If ErrMess looks like "NO_ROWS_SELECTED" or "DEVICE_NOT_CONNECTED", you don't need to reload the table	  
	   * @return a vector of strings, one entry of the vector corresponds to the concetenation of pathid|lkpos|node_from_name|port_nbfrom|port_typefrom|port_wayfrom|node_to_name|port_nbto|port_typeto|port_wayto|lkinfo
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	  */
/****************************************************************************************************************************/
vector<string> CONFDB::PyGetConnectivityBetweenDevices(string nfrom,string nto,int reload_connectivity,int delete_connectivity)  
{
int res_code=0;
vector<string> v;

int k,j,i,l,pnb;
int len_array=2000;
int len_array_copy=len_array;
int pfrom_len=5000;
int pto_len=5000;
int nfrom_len=500000;
int nto_len=500000;
int lkinfo_len=5000000;
int pfrom_len1=pfrom_len;
int pto_len1=pto_len;
//int lkinfo_len1=lkinfo_len;
int nfrom_len1=nfrom_len;
int nto_len1=nto_len;
int nt1,pt1,nf1,pf1,pf0,pt0,nf0,nt0,lkin0,lkin1;
char *pfrom_list2=new char[pfrom_len];
char *pto_list2=new char[pto_len];
int m=0;
char *nfrom_list2=new char[nfrom_len];
char *nto_list2=new char[nto_len];
char *link_info_list=new char[lkinfo_len];

int *pfrom_list1=new int[len_array];
int *pto_list1=new int[len_array];

int *pid_list1=new int[len_array];
int *link_pos_list=new int[len_array];
string resline;
char*  resline1=new char[1000];
char* ErrorMess=new char[1000];
v.clear();

res_code=GetConnectivityBetweenDevices((char*)nfrom.c_str(),(char*)nto.c_str(),len_array,pfrom_len,nfrom_len,pto_len,nto_len, lkinfo_len,nfrom_list2,nto_list2,pfrom_list2, pfrom_list1, pto_list2,pto_list1,  pid_list1,link_pos_list,link_info_list,reload_connectivity,delete_connectivity,ErrorMess);
if(res_code==0)
{
	nf1=0;
	nt1=0;
	pt1=0;
	pf1=0;
	lkin1=0;
	
	for(i=0;i<len_array;i++)
	{
	  k=pfrom_list1[i];			
	  l=pto_list1[i];
	  j=pid_list1[i];
	  nf0=nf1;
	  pf0=pf1;
	  pt0=pt1;
	  nt0=nt1;
	  lkin0=lkin1;
	  m=link_pos_list[i];
	  sprintf(resline1,"%d|%d|%s|%s|%d|%s|%s|%d|%s",j,m,nfrom_list2+nf1,pfrom_list2+pf1,k,nto_list2+nt1,pto_list2+pt1,l,link_info_list+lkin1);
	  resline=resline1;
	  nf1+=strlen(nfrom_list2+nf0)+1;
	  nt1+=strlen(nto_list2+nt0)+1;
	  pf1+=strlen(pfrom_list2+pf0)+1;
	  pt1+=strlen(pto_list2+pt0)+1;
	  lkin1+=strlen(link_info_list+lkin0)+1;
	  v.push_back(resline);
	}
}
else
{
	if(len_array==len_array_copy && pfrom_len==pfrom_len1 &&pto_len==pto_len1 && nfrom_len==nfrom_len1 && nto_len==nto_len1) //means there is an error not due to buffers too small
	{
		
			resline=ErrorMess; 
			delete [] ErrorMess;
			delete [] resline1;
				delete [] pfrom_list1;
		delete [] pto_list1;
			delete [] pfrom_list2;
		delete [] pto_list2;
			delete [] nfrom_list2;
		delete [] nto_list2;
		delete [] pid_list1;
		delete [] link_pos_list;
		delete [] link_info_list;
			throw CONFDBEXCEPTION(resline);
		
	}
	else
	{
		delete [] pfrom_list1;
		delete [] pto_list1;
			delete [] pfrom_list2;
		delete [] pto_list2;
			delete [] nfrom_list2;
		delete [] nto_list2;
		delete [] pid_list1;
		delete [] link_pos_list;
		delete [] link_info_list;
		k=0;
		j=0;
		l=0;
		pnb=0;
link_pos_list=new int[len_array];
		pfrom_list1=new int[len_array];
		pto_list1=new int[len_array];
		pid_list1=new int[len_array];
		pfrom_list2=new char[pfrom_len];
pto_list2=new char[pto_len];
nfrom_list2=new char[nfrom_len];
nto_list2=new char[nto_len];
link_info_list=new char[lkinfo_len];
res_code=GetConnectivityBetweenDevices((char*)nfrom.c_str(),(char*)nto.c_str(),len_array,pfrom_len,nfrom_len,pto_len,nto_len, lkinfo_len,nfrom_list2,nto_list2,pfrom_list2, pfrom_list1, pto_list2,pto_list1,  pid_list1,link_pos_list,link_info_list,reload_connectivity,delete_connectivity,ErrorMess);
		if(res_code==0)
		{
				
			nf1=0;
	nt1=0;
	pt1=0;
	pf1=0;
	lkin1=0;
	for(i=0;i<len_array;i++)
	{
	  k=pfrom_list1[i];			
	  l=pto_list1[i];
	  j=pid_list1[i];
	  nf0=nf1;
	  pf0=pf1;
	  pt0=pt1;
	  nt0=nt1;
	  lkin0=lkin1;
	  m=link_pos_list[i];
	  sprintf(resline1,"%d|%d|%s|%s|%d|%s|%s|%d|%s",j,m,nfrom_list2+nf1,pfrom_list2+pf1,k,nto_list2+nt1,pto_list2+pt1,l,link_info_list+lkin1);
	  resline=resline1;
	  nf1+=strlen(nfrom_list2+nf0)+1;
	  nt1+=strlen(nto_list2+nt0)+1;
	  pf1+=strlen(pfrom_list2+pf0)+1;
	  pt1+=strlen(pto_list2+pt0)+1;
	  lkin1+=strlen(link_info_list+lkin0)+1;
	  v.push_back(resline);
	}
			
		}
		else
		{
			
				resline=ErrorMess; 
				delete [] ErrorMess;
				delete [] resline1;
				delete [] pfrom_list1;
		delete [] pto_list1;
			delete [] pfrom_list2;
		delete [] pto_list2;
			delete [] nfrom_list2;
		delete [] nto_list2;
		delete [] pid_list1;
		delete [] link_pos_list;
		delete [] link_info_list;
				throw CONFDBEXCEPTION(resline);
			

		}
	}
}
delete [] ErrorMess;
delete [] resline1;
	delete [] pfrom_list1;
		delete [] pto_list1;
			delete [] pfrom_list2;
		delete [] pto_list2;
			delete [] nfrom_list2;
		delete [] nto_list2;
		delete [] pid_list1;
		delete [] link_pos_list;
		delete [] link_info_list;
return v;
}
/**************************************************************************************/
/**
       * create the tables, indexes and constraints in your DB schema,  returning a int .
       * @return 0 if it is successful
	   * @throw CONFDBEXCEPTION .
       */

/***********************************************************************************************************************/
int CONFDB::PyCreateTableSchema()  
{
int res_code=-1;
char* ErrorMess=new char[1000];
string ErrMess;
res_code=CreateTableSchema(ErrorMess);
if(res_code!=0)
{
 ErrMess=ErrorMess;
 delete [] ErrorMess;
throw CONFDBEXCEPTION(ErrMess);
}
 delete [] ErrorMess;
return res_code;
}

/****************************************************************************************************************************/
/**************************************************************************************/
/**
       *drop the tables, indexes and constraints in your DB schema, returning a int .
       * The user should use this function with caution no way to rollback!
       * @return 0 if it is successful
	   * @throw CONFDBEXCEPTION .
       */

/***********************************************************************************************************************/
int CONFDB::PyDropTableSchema()  
{
int res_code=-1;
char* ErrorMess=new char[1000];
string ErrMess;
 res_code=DropTableSchema(ErrorMess);
if(res_code!=0)
{
 ErrMess=ErrorMess;
 delete [] ErrorMess;
throw CONFDBEXCEPTION(ErrMess);
}
  delete [] ErrorMess;
return res_code;
}
/**********************************************************************************************************/
/**
       * Get the path details  between nfrom and nto, returning a vector of strings .
       * @param nfrom : name of the device 1 (ex. THOR_00)
	   * @param nto : name of the devicetype 2 (ex. ECAL_L1FE06)
	   * @param dtype_given : 1 if it's a device type, 0 if it should be like 'nto%'
	   * @param reload_connectivity : 1 if you want to load or reload the connectivity (we load only the connectivity of the subsystems in which dev_from is part of . for instance if dev_from belongs to HCAL, we load only the connectivity of HCAL.(To set to 1, after a change or if it is the first time or if the device is not in the same subsystem) otherwise 0. 
	   * @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0. N.B If ErrMess is like "Unsuccessful Malloc" or like NB of paths is too big,too many bidirectional_links, then you have to reload the connectivity table (it has been deleted).  If ErrMess looks like "NO_ROWS_SELECTED" or "DEVICE_NOT_CONNECTED", you don't need to reload the table	  
	   * @return a vector of strings, one entry of the vector corresponds to the concetenation of node_from_name|node_to_name|port_nbfrom|port_typefrom|port_wayfrom|port_nbto|port_typeto|port_wayto|lk_pos|pathid|lkinfo
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	   */
/********************************************************************************************************************/
vector<string> CONFDB::PyGetDetailedConnBetweenDeviceDevType(string nfrom,string nto,int dtype_given,int reload_connectivity,int delete_connectivity)  
{
vector<string> res_code;
int rescode=0;


int pfrom_arraylen=100000;
int pto_arraylen=100000;
int pfrom_arraylen1=pfrom_arraylen;
int pto_arraylen1=pto_arraylen;

int len_array=60000;
int nfrom_arraylen=1000000;
int nto_arraylen=1000000;
char* ErrorMess=new char[1000];

int n,p,o,q;
int k,j,i,l,pnb;
int lkinfo_len=1000000;
int pf0,pt0,pf1,pt1,lkin1,lkin0;
int len_array1=len_array;
int nfrom_arraylen1=nfrom_arraylen;
int nto_arraylen1=nto_arraylen;
int lkinfo_len1=lkinfo_len;
char* nfrom_list1=new char[nfrom_arraylen];
char* nto_list1=new char[nto_arraylen];
char* pfrom_list2=new char[pfrom_arraylen];
char* pto_list2=new char[pto_arraylen];
char* link_info_list=new char[lkinfo_len];
int *pfrom_list1=new int[len_array];
int *pto_list1=new int[len_array];
int *pid_list1=new int[len_array];
int *lkpos_list1=new int[len_array];
string resline;
char* resline1=new char[2000];
res_code.clear();



rescode=GetDetailedConnBetweenDeviceDevType((char*)nfrom.c_str(),(char*)nto.c_str(),dtype_given, nfrom_list1,pfrom_list2,pto_list2, len_array, nfrom_arraylen,pfrom_arraylen,nto_arraylen,pto_arraylen,lkinfo_len, pfrom_list1, pto_list1, nto_list1, pid_list1,lkpos_list1,link_info_list,reload_connectivity,delete_connectivity,ErrorMess);
if(rescode==0)
{
	k=0;
	j=0;
	l=0;
	pnb=0;
	pf1=0;
	pf0=0;
	pt1=0;
	pt0=0;
	lkin1=0;

	for(i=0;i<len_array;i++)
	{
	   k=j;
	   l=pnb;
	   pf0=pf1;
	   pt0=pt1;
	   lkin0=lkin1;
	   n=pfrom_list1[i];			
	   p=pto_list1[i];
	   o=pid_list1[i];
	   q=lkpos_list1[i];
	   sprintf(resline1,"%s|%s|%s|%s|%d|%d|%d|%d|%s",(nfrom_list1+j),(nto_list1+pnb),(pfrom_list2+pf1),(pto_list2+pt1),n,p,q,o,link_info_list+lkin1);
	   resline=resline1;
	   j+=strlen(nfrom_list1+k)+1;
	   pnb+=strlen(nto_list1+l)+1;
	   pf1+=strlen(pfrom_list2+pf0)+1;
	   pt1+=strlen(pto_list2+pt0)+1;
	   lkin1+=strlen(link_info_list+lkin0)+1;
	   res_code.push_back(resline);		
	}
	
	
}
else
{
	if(lkinfo_len==lkinfo_len1 && len_array==len_array1 && nfrom_arraylen==nfrom_arraylen1 && nto_arraylen==nto_arraylen1 && pto_arraylen==pto_arraylen1 && pfrom_arraylen==pfrom_arraylen1) //means there is an error not due to buffers too small
	{
		
			resline=ErrorMess; 
			delete [] ErrorMess;
			delete [] resline1;
			delete [] nfrom_list1;
			delete [] nto_list1;
			delete [] pfrom_list1;
			delete [] pto_list1;
			delete [] pfrom_list2;
			delete [] pto_list2;
			delete [] pid_list1;
			delete [] lkpos_list1;
			delete [] link_info_list;
			throw CONFDBEXCEPTION(resline);
		
	}
	else
	{
		delete [] nfrom_list1;
		delete [] nto_list1;
		delete [] pfrom_list1;
		delete [] pto_list1;
		delete [] pid_list1;
		delete [] lkpos_list1;
			delete [] pfrom_list2;
			delete [] pto_list2;
			delete [] link_info_list;
		
		k=0;
		j=0;
		l=0;
		pnb=0;

		nfrom_list1=new char[nfrom_arraylen];
		nto_list1=new char[nto_arraylen];
			pfrom_list2=new char[pfrom_arraylen];
		pto_list2=new char[pto_arraylen];
	
		pfrom_list1=new int[len_array];
		pto_list1=new int[len_array];
		pid_list1=new int[len_array];
		lkpos_list1=new int[len_array];
		link_info_list=new char[lkinfo_len];
rescode=GetDetailedConnBetweenDeviceDevType((char*)nfrom.c_str(),(char*)nto.c_str(),dtype_given, nfrom_list1,pfrom_list2,pto_list2, len_array, nfrom_arraylen,pfrom_arraylen,nto_arraylen,pto_arraylen,lkinfo_len, pfrom_list1, pto_list1, nto_list1, pid_list1,lkpos_list1,link_info_list,reload_connectivity,delete_connectivity,ErrorMess);
		if(rescode==0)
		{
		
			
			k=0;
			j=0;
			l=0;
			pnb=0;
			pf1=0;
			pf0=0;
			pt1=0;
			pt0=0;
			lkin1=0;
			for(i=0;i<len_array;i++)
			{
	   k=j;
	   l=pnb;
	   pf0=pf1;
	   pt0=pt1;
	   lkin0=lkin1;
	   n=pfrom_list1[i];			
	   p=pto_list1[i];
	   o=pid_list1[i];
	   q=lkpos_list1[i];
	   sprintf(resline1,"%s|%s|%s|%s|%d|%d|%d|%d|%s",(nfrom_list1+j),(nto_list1+pnb),(pfrom_list2+pf1),(pto_list2+pt1),n,p,q,o,link_info_list+lkin1);
	   resline=resline1;
	   j+=strlen(nfrom_list1+k)+1;
	   pnb+=strlen(nto_list1+l)+1;
	   pf1+=strlen(pfrom_list2+pf0)+1;
	   pt1+=strlen(pto_list2+pt0)+1;
	   lkin1+=strlen(link_info_list+lkin0)+1;
	   res_code.push_back(resline);		
	}
			
			
		}
		else
		{
			
				resline=ErrorMess; 
				delete [] ErrorMess;
				delete [] resline1;
				delete [] nfrom_list1;
				delete [] nto_list1;
				delete [] pfrom_list1;
				delete [] pto_list1;
				delete [] pid_list1;
				delete [] lkpos_list1;
					delete [] pfrom_list2;
			delete [] pto_list2;
		delete [] link_info_list;
				throw CONFDBEXCEPTION(resline);
		}
	}
}
 delete [] ErrorMess;
delete [] resline1;
delete [] nfrom_list1;
delete [] nto_list1;
delete [] pfrom_list1;
delete [] pto_list1;
delete [] pid_list1;
delete [] lkpos_list1;
	delete [] pfrom_list2;
			delete [] pto_list2;
		delete [] link_info_list;
return res_code;
}
/**********************************************************************************************************/
/**
       * Get the devicename corresponding to the deviceid
       * @param devID : deviceid
       * @return devicename 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	   */
/*******************************************************************/
string  CONFDB::PyGetDeviceName_deviceid(int devID)  
{
  string  device_result;
  int c=0;
  int len_buffer=1000;
  char* ErrorMess=new char[1000];
  char* device_res=new char[len_buffer];
  c=GetDeviceName_deviceid(devID,device_res,ErrorMess);
	
	if(c==0)
	{
		device_result=device_res; 		
	}
	else
	{
		device_result=ErrorMess; 
		delete [] ErrorMess;
		delete [] device_res;
		throw CONFDBEXCEPTION(device_result);
	}
	delete [] ErrorMess;
	delete [] device_res;
	return device_result;
}
/**********************************************************************************************************/
/**
       * Get the cpntname corresponding to the cpntid
	
       * @param cpntID : cpntid
       * @return cpntname 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	   */
/*******************************************************************/
string  CONFDB::PyGetCpntName_cpntid(int cpntID)  
{
  string  device_result;
  int c=0;
  int len_buffer=1000;
  char* ErrorMess=new char[1000];
  char* device_res=new char[len_buffer];
  c=GetCpntName_cpntid(cpntID,device_res,ErrorMess);
	
	if(c==0)
	{
		device_result=device_res; 		
	}
	else
	{
		device_result=ErrorMess; 
		delete [] ErrorMess;
		delete [] device_res;
		throw CONFDBEXCEPTION(device_result);
	}
	delete [] ErrorMess;
	delete [] device_res;
	return device_result;
}

/***************************************************/
/**********************************************************************************************************/
/**
       * Get the deviceid  corresponding to the devicename
       * @param devname : name of device 
       * @return deviceid 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	   */
/*******************************************************************/
int CONFDB::PyGetDeviceID_devicename(string  devname)  
{
  int c=0;
  
  
  
  char* ErrorMess=new char[1000];
  int devID=-1;
  string device_result;
  c=GetDeviceID_devicename((char*)devname.c_str(),devID,ErrorMess);
  if(c!=0)
  {
    device_result=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(device_result);
  }
  delete [] ErrorMess;
  return devID;
}

/***************************************************/
/**********************************************************************************************************/
/**
       * Get the portID  corresponding to the deviceid, port_nb,port_type and port_way
       * @param deviceid : deviceid
	   * @param port_nb : port nb of the device
	   * @param port_type : port type of the port
       * @param port_way : port way of the port
       * @return portid 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	   */
/*******************************************************************/
int CONFDB::PyGetPortID_portinfo(int deviceid, string port_nb, string port_type, int port_way)  
{
  int c=0;
  
  
  
  char* ErrorMess=new char[1000];
  int portID=-1;
  string device_result;
  c=GetPortID_portinfo(deviceid,(char*)port_nb.c_str(),(char*)port_type.c_str(),port_way,portID,ErrorMess);
  if(c!=0)
  {
    device_result=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(device_result);
  }
  delete [] ErrorMess;
  return portID;
}

/***************************************************/
/**********************************************************************************************************/
/**
       * Get the cpntid  corresponding to the cpntname
       * @param cpntname : name of the board cpnt
       * @return deviceid 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
	   */
/*******************************************************************/
int CONFDB::PyGetCpntID_cpntname(string  cpntname)  
{
  int c=0;
  
  
  
  char* ErrorMess=new char[1000];
  int devID=-1;
  string device_result;
  c=GetCpntID_cpntname((char*)cpntname.c_str(),devID,ErrorMess);
  if(c!=0)
  {
    device_result=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(device_result);
  }
  delete [] ErrorMess;
  return devID;
}
/*****************************************************************************************/
/**
       * Get the list of possible destination  reachable from this port   
       * @param devicename : name of the device
       * @param port_nb : port nb of the device
	   * @param port_type : port type of the device. Put "none" in lower case if it's null
       * @return  list of the devices
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/

/****************************************************************/
vector<string> CONFDB::PyGetDestinationNamePerDevPort(string devicename,string  port_nb,string port_type)  
{
	vector<string> devID_list_result;
	int c=0;
	int c1=0;
	
//	int actual_len=0;
	int pos1,pos2;
	int len_dto=500;
	int len_array=50000;
	int len_buffer=len_array;

	string ErrMess;
	devID_list_result.clear();
	char* devID_list_res=new char[len_buffer];
	char* dto=new char[len_dto];
	char* ErrorMess=new char[1000];
	c=GetDestinationNamePerDevPort((char*)devicename.c_str(),(char*)port_nb.c_str(),(char*) port_type.c_str(),len_array, devID_list_res,ErrorMess);
	
	if(c==0)
	{
		pos2=0;
		pos1=0;
		while(pos2<len_array )
		{	
			pos1=strlen(devID_list_res+pos2);
			strcpy(dto,devID_list_res+pos2);
			pos2+=pos1+1;
			devID_list_result.push_back(dto); 
		
		}	
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devID_list_res;
			devID_list_res=new char[len_array];
			c1=GetDestinationNamePerDevPort((char*)devicename.c_str(),(char*)port_nb.c_str(),(char*) port_type.c_str(),len_array, devID_list_res,ErrorMess);
			if(c1==0)
			{
				pos2=0;
				pos1=0;
				while(pos2<len_array )
				{	
					pos1=strlen(devID_list_res+pos2);
					strcpy(dto,devID_list_res+pos2);
					pos2+=pos1+1;
					devID_list_result.push_back(dto); 
		
				}	
			}
			else
			{
				
					ErrMess=ErrorMess; 
					delete [] ErrorMess;
					delete [] dto;
					delete [] devID_list_res;
					throw CONFDBEXCEPTION(ErrMess);

			}

		}
		else
		{
			
				ErrMess=ErrorMess; 
				delete [] ErrorMess;
				delete [] dto;
				delete [] devID_list_res;
				throw CONFDBEXCEPTION(ErrMess);
		}
	}
	delete [] ErrorMess;
	delete [] dto;
	delete [] devID_list_res;
	return devID_list_result;
}
/*****************************************************************************************/
/**
       * create the destination table of a given device and stored in the database, returning a int
       * @param dname : device name
       * @param roundtrip : maximum path length
       * @return  0 if successfully created
	   * @throw CONFDBEXCEPTION .
       */
/*****************************************************************************/
/***********************************************************************************************************************/
int  CONFDB::PyCreateDestinationTable(string dname,int roundtrip)  
{
int  res_code=0;

char* ErrorMess=new char[1000];
string ErrMess;
res_code=CreateDestinationTable((char*)dname.c_str(),roundtrip,ErrorMess);
if(res_code!=0)
{
ErrMess=ErrorMess;
delete [] ErrorMess;
throw CONFDBEXCEPTION(ErrMess);
}
delete [] ErrorMess;
return res_code;
}
/*****************************************************************************************/
/**
       * create the routing table of a given device and stored in the database, returning a int
       * @param devname : device name
       * @param roundtrip : maximum path length
       * @return  0 if successfully created
	   * @throw CONFDBEXCEPTION .
       */
/*****************************************************************************/
/***********************************************************************************************************************/
int  CONFDB::PyCreateRoutingTable(string  devname, int  roundtrip)  
{
int  res_code=0;
string ErrMess;
char* ErrorMess=new char[1000];

res_code=CreateRoutingTable((char*)devname.c_str(),roundtrip,ErrorMess);
if(res_code!=0)
{
ErrMess=ErrorMess;
delete [] ErrorMess;
throw CONFDBEXCEPTION(ErrMess);
}
 delete [] ErrorMess;
return res_code;
}
/*****************************************************************************************/
/**
       * get all the paths going through a given device , returning a vector of strings
       * @param systemname : paths which will be part of this system name of the subsystem (kind of filter)
       * @param devicename : device name
	   * @param reload_connectivity : 1 if you want to load or reload the connectivity (we load only the connectivity of the subsystems in which dev_from is part of . for instance if dev_from belongs to HCAL, we load only the connectivity of HCAL.(To set to 1, after a change or if it is the first time or if the device is not in the same subsystem) otherwise 0. 
	   * @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0. N.B If ErrMess is like "Unsuccessful Malloc" or like NB of paths is too big,too many bidirectional_links, then you have to reload the connectivity table (it has been deleted).  If ErrMess looks like "NO_ROWS_SELECTED" or "DEVICE_NOT_CONNECTED", you don't need to reload the table	  
       * @return  the list of paths formatted as follows: pathid|link_pos|linkid
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
/**************************************************************************/
vector<string>  CONFDB::PyGetAllPathsPerDevice(string systemname,string devicename,int reload_connectivity,int delete_connectivity)  
{
  vector<string> result_query;
  char* ErrorMess=new char[1000]; 
  int i=0;
  int len_array=200000;
  int len_array1=len_array;
  int res=0;
  int res1=0;
  int pos=0;
//  int pos2=0;
  int *lkid_list1=new int[len_array];
  int *pid_list1=new int[len_array];
  int *lkpos_list1=new int[len_array];
//  int len_array2=50000;
  
  char* resline=new char[500];
  string row_concat;
  res=GetAllPathsPerDevice((char*)systemname.c_str(),(char*)devicename.c_str(),len_array,lkid_list1,pid_list1,lkpos_list1,reload_connectivity,delete_connectivity,ErrorMess);
  result_query.clear();
  if(res==0)
   {
      for (i=0;i<len_array;i++)
	{
		  
	  sprintf(resline,"%d|%d|%d",pid_list1[i],lkpos_list1[i],lkid_list1[i]);
	  row_concat=resline;
	  result_query.push_back(row_concat);
	 
	}
   }
  else
    {
      if(len_array1<len_array )
	{
	  delete [] lkid_list1;
	  delete [] pid_list1;
	  delete [] lkpos_list1;
	  
	  lkid_list1=new int[len_array];
	  pid_list1=new int[len_array];
	  lkpos_list1=new int[len_array];
	  
	  res1=GetAllPathsPerDevice((char*)systemname.c_str(),(char*)devicename.c_str(),len_array,lkid_list1,pid_list1,lkpos_list1,reload_connectivity,delete_connectivity,ErrorMess);
	  if(res1==0)
	    {
		  pos=0;
	      for (i=0;i<len_array;i++)
		  {
			sprintf(resline,"%d|%d|%d",pid_list1[i],lkpos_list1[i],lkid_list1[i]);
			row_concat=resline;
			result_query.push_back(row_concat);
			
		  }

	    }
	  else
	    {
	     
				row_concat=ErrorMess; 
				delete [] ErrorMess;
				delete [] lkid_list1;
				delete [] pid_list1;
				delete [] lkpos_list1;
				delete [] resline;
				throw CONFDBEXCEPTION(row_concat);
	    }
	}
    else
	{
	  
		row_concat=ErrorMess; 
		delete [] ErrorMess;
		delete [] lkid_list1;
		delete [] pid_list1;
		delete [] lkpos_list1;
		delete [] resline;
		throw CONFDBEXCEPTION(row_concat);
	}

 }
  delete [] ErrorMess;
  delete [] lkid_list1;
  delete [] pid_list1;
  delete [] lkpos_list1;
  delete [] resline;
  return result_query;
}
/*****************************************************************************************/
/**
       * Load the connectivity table of a given system , returning a vector of strings
       * @param systemname : link which are part of the given list of the subsystem.  (upper case: one subsystem because it's big table)   
       * @return  the list of links formatted as follows:linkid|node_from|node_to|port_from|port_typefrom|port_to|port_typeto|bidirectional|link_used|linktype|lkinfo
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.       
       */
/*****************************************************************************/
/******************************************************************/

vector<string> CONFDB::PyLoadConnectivityTable (string systemname)  
{


  char* ErrorMess=new char[1000];
  vector<string> result_query;
  int i=0;
  int len_array=100000;
  int len_array1=len_array;
  int len_array2=len_array;
  int len_array3=len_array;
  int lkinfo_len=5000000;
  int lkinfo_len1=lkinfo_len;
  int pos=0;
  int pos1=0;
  int pos2=0;
  int pos3=0;
  int res=0;
  int lkinpos1=0;
  int lkinpos0=0;
  int res1=0;
  int *lkid_list=new int[len_array];
  int *node_from_list=new int[len_array];
  char *port_nbrfrom_list=new char[len_array];
  char *port_nbrto_list=new char[len_array];
    char *link_info_list=new char[lkinfo_len];
  int *node_to_list=new int[len_array];
  int *bidirectional_list=new int[len_array];
  int *lkused_list=new int[len_array];
  int *lktype_list=new int[len_array];
  char* resline=new char[500];
  char* pfrom=new char[100];
  char* pto=new char[100];
  char* lkinfo=new char[1000];
  string row_concat;
  res=LoadConnectivityTable((char*)systemname.c_str(),len_array1,lkid_list, node_from_list,node_to_list,len_array2,len_array3,port_nbrfrom_list, port_nbrto_list, bidirectional_list,lkused_list,lktype_list,link_info_list,lkinfo_len,ErrorMess);

  //std::cout<<"value of res="<<res<<std::endl;
//std::cout<<"value of ErrorMess="<<ErrorMess<<std::endl;
  result_query.clear();
  

  if(res==0)
  {
	  
      for (i=0;i<len_array1;i++)
	  {
		pos=strlen(port_nbrfrom_list+pos1);
		pos3=strlen(port_nbrto_list+pos2);
		strcpy(pfrom,port_nbrfrom_list+pos1);
		strcpy(pto,port_nbrto_list+pos2);
		lkinpos0=strlen(link_info_list+lkinpos1);
		strcpy(lkinfo,link_info_list+lkinpos1);
		pos1+=pos+1;
		pos2+=pos3+1;
		lkinpos1+=lkinpos0+1;
	    sprintf(resline,"%d|%d|%d|%s|%s|%d|%d|%d|%s",lkid_list[i],node_from_list[i],node_to_list[i],pfrom,pto,bidirectional_list[i],lkused_list[i],lktype_list[i],lkinfo);
	    row_concat=resline;
		result_query.push_back(row_concat);
	  }
  }
  else
  {
      if(lkinfo_len1<lkinfo_len || len_array<len_array1 ||len_array<len_array2||len_array<len_array3)
	  {
		  delete [] lkid_list;
		  delete [] node_from_list;
		  delete [] node_to_list;
		  delete [] port_nbrfrom_list;
		  delete [] port_nbrto_list;
		  delete [] bidirectional_list;
		  delete [] lkused_list;
		  delete [] lktype_list;
 delete [] link_info_list;

		lkid_list=new int[len_array1];
		node_from_list=new int[len_array1];
		port_nbrfrom_list=new char[len_array2];
		port_nbrto_list=new char[len_array3];
		node_to_list=new int[len_array1];
		bidirectional_list=new int[len_array1];
		lkused_list=new int[len_array1];
		lktype_list=new int[len_array1];
		link_info_list=new char[lkinfo_len];
		res1=LoadConnectivityTable((char*)systemname.c_str(),len_array1,lkid_list, node_from_list,node_to_list,len_array2,len_array3,port_nbrfrom_list, port_nbrto_list, bidirectional_list,lkused_list,lktype_list,link_info_list,lkinfo_len,ErrorMess);
		//std::cout<<"value of res1="<<res1<<std::endl;

	 

		if(res1==0)
		{
			pos=0;
			pos1=0;
			pos2=0;
			pos3=0;
			lkinpos0=0;
			lkinpos1=0;
			for (i=0;i<len_array1;i++)
			{
				pos=strlen(port_nbrfrom_list+pos1);
				pos3=strlen(port_nbrto_list+pos2);
				strcpy(pfrom,port_nbrfrom_list+pos1);
				strcpy(pto,port_nbrto_list+pos2);
				lkinpos0=strlen(link_info_list+lkinpos1);
				strcpy(lkinfo,link_info_list+lkinpos1);
				pos1+=pos+1;
				pos2+=pos3+1;
				lkinpos1+=lkinpos0+1;
				sprintf(resline,"%d|%d|%d|%s|%s|%d|%d|%d|%s",lkid_list[i],node_from_list[i],node_to_list[i],pfrom,pto,bidirectional_list[i],lkused_list[i],lktype_list[i], lkinfo);
				row_concat=resline;
				result_query.push_back(row_concat);
			}
		}
		else
		{
	      
		    row_concat=ErrorMess; 
		    delete [] ErrorMess;
			delete [] lkid_list;
			delete [] node_from_list;
			delete [] node_to_list;
			delete [] port_nbrfrom_list;
			delete [] port_nbrto_list;
			delete [] bidirectional_list;
			delete [] lkused_list;
			delete [] lktype_list;
			delete [] resline;
				delete [] pfrom;
				delete [] link_info_list;
		delete [] pto;
		    throw CONFDBEXCEPTION(row_concat);
	   }
	}
    else
	{
	  
		    row_concat=ErrorMess; 
		    delete [] ErrorMess;
			delete [] lkid_list;
			delete [] node_from_list;
			delete [] node_to_list;
			delete [] port_nbrfrom_list;
			delete [] port_nbrto_list;
			delete [] bidirectional_list;
			delete [] lkused_list;
			delete [] lktype_list;
			delete [] resline;
			delete [] link_info_list;
				delete [] pfrom;
		delete [] pto;
		    throw CONFDBEXCEPTION(row_concat);
	}

  }
	 delete [] ErrorMess;
  delete [] lkid_list;
  delete [] node_from_list;
  delete [] node_to_list;
  delete [] port_nbrfrom_list;
  delete [] port_nbrto_list;
  delete [] bidirectional_list;
  delete [] lkused_list;
  delete [] lktype_list;
  delete [] link_info_list;
	delete [] pfrom;
		delete [] pto;

  delete [] resline;
  return result_query;
}
/*****************************************************************************************/
/**
       * Load part of the device table of a given system , returning a vector of strings
       * @param systemname : device used in the given list of the subsystem.(upper case, separated with a coma) 
       * @return  the list of devicename/deviceid formatted as follows:deviceid|devicename
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
/******************************************************************/
/******************************************************************/

vector<string> CONFDB::PyMatchDeviceIDDeviceName (string systemname)  
{

  char* ErrorMess=new char[1000];
  vector<string> result_query;
  int i=0;
  int pos1=0;
  int pos2=0;
  int len_array=10000;
  int len_dname=50000;
  int len_array1=len_array;
  
  int res=0;
  int res1=0;
  int *devid_list=new int[len_array];
  char *devname_list=new char[len_dname];
  char* resline=new char[500];
  string row_concat;

  res=MatchDeviceIDDeviceName((char*)systemname.c_str(),len_array,devid_list,len_dname,devname_list,ErrorMess);

  result_query.clear();
  if(res==0)
    {
      for (i=0;i<len_array;i++)
		{
			pos1=strlen(devname_list+pos2);
			
			sprintf(resline,"%d|%s",devid_list[i],devname_list+pos2);
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2+=pos1+1;
		}
    }
  else
    {
      if(len_array1<len_array)
	{
	  delete [] devid_list;
	  delete [] devname_list;
	 
	  devid_list=new int[len_array];
	  devname_list=new char[len_dname];
	
  res1=MatchDeviceIDDeviceName((char*)systemname.c_str(),len_array,devid_list,len_dname,devname_list,ErrorMess);
pos2=0;
	 

	  if(res1==0)
	    {
	       for (i=0;i<len_array;i++)
			{
		   pos1=strlen(devname_list+pos2);
			
			sprintf(resline,"%d|%s",devid_list[i],devname_list+pos2);
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2+=pos1+1;
			}

	    }
	  else
	    {
	     
		    row_concat=ErrorMess; 
		    delete [] devid_list;
			delete []devname_list;
			delete [] ErrorMess;

			delete [] resline;
		    throw CONFDBEXCEPTION(row_concat);
	    }
	}
     else
	{
	  
		    row_concat=ErrorMess; 
		    delete [] devid_list;
			delete []devname_list;
			delete [] ErrorMess;

			delete [] resline;
		    throw CONFDBEXCEPTION(row_concat);
	}

   }
	delete [] devid_list;
	delete []devname_list;
	delete [] ErrorMess;

  delete [] resline;
  return result_query;
}
/*******************************************************************************************************/
/*****************************************************************************************/
/**
       * Get all the device types of a given system , returning a vector of strings
       * @param systemname : device type which are used by the list of the subsystems (upper case, separated by a coma) .    
       * @return  the list of device types and devtypeID : devicetypeID|devicetypename
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
/******************************************************************/
/******************************************************************/

vector<string> CONFDB::PyLoadDeviceTypeTable(string systemname)  
{

  char* ErrorMess=new char[1000];
  vector<string> result_query;

  int pos1=0;
  int pos2=0;
  int pos3=0;
  
  int len_dname=500000;
  int len_array=10000;
  int len_array1=len_array;
  int len_dname1=len_dname;
  int res=0;
  int res1=0;
  int i=0;
  char *devname_list=new char[len_dname];
  char* resline=new char[500];
  int* dtypeList=new int[len_array];
  string row_concat;

res=LoadDeviceTypeTable((char*) systemname.c_str(),len_dname,devname_list,len_array,dtypeList,ErrorMess);
result_query.clear();
 if(res==0)
 {
	 pos2=0;
     while (pos2<len_dname)
	 {
		pos1=strlen(devname_list+pos2);
		strcpy(resline,devname_list+pos2);
		sprintf(resline,"%d|%s",dtypeList[i],devname_list+pos2);
		row_concat=resline;
		result_query.push_back(row_concat);
		pos2+=pos1+1;
		i++;
	}
 }
 else
 {
    if(len_dname1<len_dname || len_array1<len_array)
	{
	  delete [] devname_list;
	  delete [] dtypeList;
	  devname_list=new char[len_dname];
	  dtypeList=new int[len_array];
	  res1=LoadDeviceTypeTable((char*) systemname.c_str(),len_dname,devname_list,len_array,dtypeList,ErrorMess);
	  pos2=0;
	  if(res1==0)
	  {
		pos1=0;
		pos2=0;
		pos3=0;
		i=0;
	    while (pos2<len_dname)
		 {
			pos1=strlen(devname_list+pos2);
			strcpy(resline,devname_list+pos2);
			sprintf(resline,"%d|%s",dtypeList[i],devname_list+pos2);
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2+=pos1+1;
			i++;
		}

	  }
	  else
	  {
	      
		 row_concat=ErrorMess; 
		 delete []devname_list;
		 delete [] ErrorMess;

		 delete [] resline;
		 throw CONFDBEXCEPTION(row_concat);
	   }
	}
    else
	{
	  
		    row_concat=ErrorMess; 
		    delete []devname_list;
			delete [] ErrorMess;

			delete [] resline;
		    throw CONFDBEXCEPTION(row_concat);
	      
	}

  }
  
  delete []devname_list;
 delete [] ErrorMess;

  delete [] resline;
  return result_query;
}
/*****************************************************************************************/
/**
       * Load the link type table  , returning a vector of strings
       * @return  the list of linkname/linkid formatted as follows:linktypeid|linkname
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
/******************************************************************/
/******************************************************************/

vector<string> CONFDB::PyLoadLinkTypeTable ()  
{

  char* ErrorMess=new char[1000];

  vector<string> result_query;
  int i=0;
  int pos1=0;
  int pos2=0;
  int len_array=10000;
  int len_dname=50000;
  int len_array1=len_array;
  
  int res=0;
  int res1=0;
  int *devid_list=new int[len_array];
  char *devname_list=new char[len_dname];
  char* resline=new char[500];
  string row_concat;

  res=LoadLinkTypeTable(len_array,devid_list,len_dname,devname_list,ErrorMess);

  result_query.clear();
  if(res==0)
    {
      for (i=0;i<len_array;i++)
		{
			pos1=strlen(devname_list+pos2);
			sprintf(resline,"%d|%s",devid_list[i],devname_list+pos2);
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2+=pos1+1;
		}
    }
  else
    {
      if(len_array1<len_array)
		{
	  delete [] devid_list;
	  delete [] devname_list;
	 
	  devid_list=new int[len_array];
	  devname_list=new char[len_dname];
	
  res1=LoadLinkTypeTable(len_array,devid_list,len_dname,devname_list,ErrorMess);
pos2=0;
	 

	  if(res1==0)
	    {
	       for (i=0;i<len_array;i++)
			{
		    pos1=strlen(devname_list+pos2);
			sprintf(resline,"%d|%s",devid_list[i],devname_list+pos2);
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2+=pos1+1;
			}

	    }
	  else
	    {
	     
		    row_concat=ErrorMess; 
		    delete [] devid_list;
			delete []devname_list;
			delete [] ErrorMess;

			delete [] resline;
		    throw CONFDBEXCEPTION(row_concat);
	    }
	}
     else
	{
	  
		    row_concat=ErrorMess; 
		    delete [] devid_list;
			delete []devname_list;
			delete [] ErrorMess;

			delete [] resline;
		    throw CONFDBEXCEPTION(row_concat);
	}

   }
	delete [] devid_list;
	delete []devname_list;
	delete [] ErrorMess;

  delete [] resline;
  return result_query;
}

/*******************************************************/
/**
	   * Upadte Multiple Ports, returning an integer value.
       * @param devicename : name of the device which will be updated
	   * @param port_nb   : port nb of the device to update
	   * @param port_way   : specify the way of the port to update :1 if the port is an input and 2 if it's an output 
	   * @param port_type   : specify the type of the port of the device to update. Put "" if there is none.
	   * @param speed   : new value of speed of the port: PUT -1 to keep the same value
	   * @param pxi_booting : new value of pxi_booting 1 if the port is the pxi_booting one, 0 otherwise. Put a value different from 1 or 0 to keep the same value.
	   * @param phy: new value of phy possible values :"SX","T","SL". if no changes put ""
	   * @param first_time1 : 1 if it's your first Port insert (save it into the database), 0 otherwise   
	   * @param last_rows1 : 1 if it's your last Port insert (save it into the database), 0 otherwise
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 

string CONFDB::PyUpdateMultiplePorts(string devicename, string port_nb, int port_way,string port_type,int speed,string phy, int pxi_booting,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultiplePorts((char*)devicename.c_str(),(char*) port_nb.c_str(),port_way,(char*) port_type.c_str(), speed,(char*) phy.c_str(),pxi_booting, first_time1,last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}
/**
	   * Update MAC entries,  returning an integer value.
	   * @param ip_add : key entry : you want to update the info about this IPaddress
	   * @param subnet_mask : new value : put "" or "none" if no changes
	   * @param ipname : new value : put "" or "none" if no changes
	   * @param first_time1 : 1 if it's your first MacIP insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your last MacIP insert (save it into the database), 0 otherwise
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 




string CONFDB::PyUpdateMultipleAttributeMacIPs(string ip_add,string subnet_mask,string ipname,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleAttributeMacIPs((char*)ip_add.c_str(),(char*)subnet_mask.c_str(),(char*) ipname.c_str(), first_time1, last_rows1,ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**
	   * Update MAC entries,  returning an integer value.
	   * @param oldip_add :  ip address entry you want to modify. 
	   * @param ip_add : new value of ip address entry you want to modify. 
	   * @param first_time1 : 1 if it's your first MacIP insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your last MacIP insert (save it into the database), 0 otherwise
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 


string CONFDB::PyUpdateMultipleIPAddresses(string ip_add,string oldip_add,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleIPAddresses((char*)ip_add.c_str(),(char*)oldip_add.c_str(), first_time1,last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}
/*****************************************************************/
/**
       * Update a Composite Link Type,  returning an integer value.Not to used...
	   * @param link_name :  the link type name you want to modify. 
	   * @param simple_lk_list : name of the link types which compose the composite link, separated by "," (all of them)
	   * @param simple_lk_list :ex. Mixed_Data is composed of HLT_Data and L1_Data, so simple_lk_list="HLT_Data,L1_Data"
	   * @param simple_lk_list : put none if the link type is no longer a composite link type
	   * @param first_time1 : 1 if it's your CompositeLinkType first insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your CompositeLinkType last insert (save it into the database), 0 otherwise
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
string CONFDB::PyUpdateMultipleCompositeLinkTypes(string link_name,string simple_lk_list,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleCompositeLinkTypes((char*)link_name.c_str(),(char *) simple_lk_list.c_str(),first_time1,last_rows1,ErrMess);

if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}
/**
       * Update a  Link Type Name (due to mistyping error), returning an integer value.
	   * @param old_link_name : value  of the  link type name you want to modify
	   * @param link_name : value  of the new link type
	   * @param first_time1 : 1 if it's your SimpleLinkType first insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your SimpleLinkType last insert (save it into the database), 0 otherwise
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 

string CONFDB::PyUpdateMultipleLinkTypeNames(string old_link_name, string link_name,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleLinkTypeNames((char *)old_link_name.c_str(), (char *)link_name.c_str(), first_time1, last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}
/**
       * Update the devicetype  returning an integer value.
	   * @param devicetype : name of the devicetype you want to update
       * @param description : new value of the description, if nothing you put "" or none (lower case)
	   * @param nbrofinput : new value of nbrofinput: put -1 if you don't want to change it
	   * @param nbrofoutput : new value of nbrofoutput: put -1 if you don't want to change it
	   * @param rgbcolor : new value of the rgbcolor of the devicetypefor display if nothing you put "" or none (lower case)
	   * @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/******************************************************************/
string CONFDB::PyUpdateMultipleDeviceTypeAttributes(string devicetype,string description,int nbrofinput,int nbrofoutput,string rgbcolor,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleDeviceTypeAttributes((char*)devicetype.c_str(),(char*) description.c_str(),nbrofinput, nbrofoutput,(char*)rgbcolor.c_str(),first_time1, last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**
       * Update the devicetype  returning an integer value.
	   * @param devicetype_old : name of the devicetype you want to update
	   * @param devicetype : new value of the devicetype, 
   	   * @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
  	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/******************************************************************/
string CONFDB::PyUpdateMultipleDeviceTypes(string devicetype_old,string devicetype,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleDeviceTypes((char*)devicetype_old.c_str(),(char*) devicetype.c_str(),first_time1, last_rows1, ErrMess);

if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**
       * Update a device  returning an integer value.
	   * @param devicename : new value of the devicename, if nothing you put  none (lower case)
	   * @param node : new value of node 1 if it's a host node (usually starts or ends a path such as L1FEs) , 0 otherwise.Put -1 not to change the value
	   * @param promiscuous_mode : new value of prom.mode 1 if put in this mode (default mode), 0 otherwise. Put -1 not to change the value
	   * @param location : new value of the uuid of the device if nothing you put none (lower case)
	   * @param function_list : new value of the functions of the device. If several, separate them with a coma. Put "none", if it doesn't have any.
	   * @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
  	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/******************************************************************/
string CONFDB::PyUpdateMultipleAttributesDevices(string devicename,int node,int promiscuous_mode,string location,string function_list,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleAttributesDevices((char*)devicename.c_str(), node, promiscuous_mode,(char*) location.c_str(), (char*) function_list.c_str(),first_time1,last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}
/**
       * Update the devicename field of a device (typically when there is a mistyping) returning an integer value.
	   * @param devicename_old : name of the device you want to update
	   * @param devicename : new value of the devicename. can't be NULL!
       * @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/******************************************************************/
string CONFDB::PyUpdateMultipleDevNamesDevices(string devicename_old,string devicename,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleDevNamesDevices((char*)devicename_old.c_str(),(char*) devicename.c_str(),first_time1,last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}
/**
       * Update bidirectional link field for a given link  returning an integer value.
	   * either you give node_from,pfrom or node_to,pto.
	   * @param node_from : name of the device starting the link: put "none" if you give node_to
	   * @param node_to : name of the device ending the link: put "none" if you give node_from
	   * @param port_nbrfrom : port nb of the device from
	   * @param port_nbrto : port nb of the device to
	   * @param port_typefrom : port nb of the device from: put "none" if it doesn't exist or if you provide about the node_to 
	   * @param port_typeto : port nb of the device to:put "none" if it doesn't exist or if you provide about the node_from
	   * @param bidirectional_link_used :  1 if it's a bidirectional link, 0 otherwise
	   * @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
string CONFDB::PyUpdateMultipleBidirectionalLinks(string node_from,string node_to,string port_nbrfrom,string port_typefrom,string port_nbrto,string port_typeto,int bidirectional_link_used,int first_time1, int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleBidirectionalLinks((char*)node_from.c_str(),(char*) node_to.c_str(),(char*) port_nbrfrom.c_str(),(char*) port_typefrom.c_str(),(char*) port_nbrto.c_str(),(char*) port_typeto.c_str(),bidirectional_link_used,first_time1, last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**
       * Update the lkused field of a Link  returning an integer value.
	   * you give either node_from,portnbrfrom or node_to,pto
	   * @param node_from : name of the device starting the link. Put "none" if you give node_to
	   * @param node_to : name of the device ending the link. Put "none" if you give node_to
	   * @param port_nbrfrom : port nb of the device from
	   * @param port_nbrto : port nb of the device to
	   * @param port_typefrom : port nb of the device from
	   * @param port_typeto : port nb of the device to
	   * @param lkused : 1 if the link is used in the configuration, 0 otherwise
	   * @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
string CONFDB::PyUpdateMultipleLkUsedLinks(string node_from,string node_to,string port_nbrfrom,string port_nbrto,string port_typefrom,string port_typeto,int lkused,int first_time1, int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleLkUsedLinks((char*) node_from.c_str(),(char*) node_to.c_str(),(char*) port_nbrfrom.c_str(),(char*) port_nbrto.c_str(),(char*) port_typefrom.c_str(),(char*) port_typeto.c_str(),lkused,first_time1, last_rows1,ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**
       * Update the link type of a Link  returning an integer value.
	   * give either nfrom,pfrom or nto,pto
	   * @param node_from : name of the device starting the link. Put "none" if giving node_to
	   * @param node_to : name of the device ending the link. Put "none" if giving node_from
	   * @param port_nbrfrom : port nb of the device from
	   * @param port_nbrto : port nb of the device to
	   * @param port_typefrom : port nb of the device from
	   * @param port_typeto : port nb of the device to
	   * @param link_type : name of the link type
	   * @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 

string CONFDB::PyUpdateMultipleLkTypeLinks(string node_from,string node_to,string port_nbrfrom,string port_nbrto,string port_typefrom,string port_typeto,string link_type,int first_time1, int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
	rescode=UpdateMultipleLkTypeLinks((char*)node_from.c_str(),(char*) node_to.c_str(),(char*) port_nbrfrom.c_str(),(char*) port_nbrto.c_str(),(char*) port_typefrom.c_str(),(char*) port_typeto.c_str(),(char*)link_type.c_str(), first_time1, last_rows1,ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}
/**
       * Update the systemlist of a given devicename  returning an integer value.
	   * @param devicename : name of the functional device 
	   * @param new_systemList : new list of the subsystems to which the device belongs to, elt separated by a coma
	   * @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
string CONFDB::PyUpdateMultipleDeviceSystemList(string devicename,string new_systemList,int first_time1, int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleDeviceSystemList((char*) devicename.c_str(),(char*) new_systemList.c_str(),first_time1, last_rows1,ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**********************************************************************************************************/
/**
       * Get the path details  between dev_from and dev_to, returning an integer value.
	   * you need grant execute if you have your own db account
       * @param cpntname_from : name of the board cpnt which wil start the micro path
	   * @param  mboard_portid : portid of the motherboard which wil end the micro path. Put 0 if you want for all of them.
	   * NB: the i-th entry of the pathid_list|link_pos_list|nfrom|port_nbrfrom|port_typefrom|nto|port_nbrto|port_typeto corresponds to one link of the path
	   * @return 0 if it is successful
       */


vector<string> CONFDB::PyGetMicroConnectBetweenBoardCpntAndMotherBoard(string cpntname_from,int mboard_portid)
{
	vector<string> result_query;
	int rescode=0;
	int len_pid=1000;
	int len_pid1=len_pid;
	int nfrom_len=10000;
	int nfrom_len1=nfrom_len;
	int last_ntolen=5000;
	int last_nfromlen=5000;
	int nto_len=10000;
	int nto_len1=nto_len;
	int i=0;
	int pos1=0;
	int pos2=0;
	int pos3=0;
	int pos=0;
	char* resline=new char[500];
    string row_concat;
	char* ErrMess=new char[len_pid];
	int* pathid_list=new int[len_pid];
	int* link_pos_list=new int[len_pid];
	char* nfrom_list=new char[nfrom_len];
	char* nto_list=new char[nto_len];
	char* last_nfrominfo=new char[last_nfromlen];
	char* last_ntoinfo=new char[last_ntolen];
	rescode=GetMicroConnectBetweenBoardCpntAndMotherBoard((char*)cpntname_from.c_str(), mboard_portid, pathid_list,  link_pos_list,len_pid, nfrom_list,nfrom_len, nto_list,nto_len,last_nfrominfo,last_nfromlen, last_ntoinfo,last_ntolen, ErrMess);
	result_query.clear();
	if(rescode==0)
	{
		for (i=0;i<len_pid;i++)
		{
		  
			sprintf(resline,"%d|%d|%s|%s",pathid_list[i],link_pos_list[i],(nfrom_list+pos1),(nto_list+pos));
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2=pos;
			pos+=strlen(nto_list+pos2)+1;
			pos3=pos1;
			pos1+=strlen(nfrom_list+pos3)+1;
		}
   }
   else
   {
      if(len_pid1<len_pid || nfrom_len1<nfrom_len ||nto_len1<nto_len)
	  {
		delete [] pathid_list;
		delete [] link_pos_list;
		delete [] nfrom_list;
		delete [] nto_list;
		pathid_list=new int[len_pid];
		link_pos_list=new int[len_pid];
		nfrom_list=new char[nfrom_len];
		nto_list=new char[nto_len];
		rescode=GetMicroConnectBetweenBoardCpntAndMotherBoard((char*)cpntname_from.c_str(), mboard_portid, pathid_list,  link_pos_list,len_pid, nfrom_list,nfrom_len, nto_list,nto_len,last_nfrominfo,last_nfromlen, last_ntoinfo,last_ntolen, ErrMess);
		if(rescode==0)
	    {
		  pos1=0;
		  pos2=0;
		  pos3=0;
		  pos=0;
	      for (i=0;i<len_pid;i++)
		  {
			sprintf(resline,"%d|%d|%s|%s",pathid_list[i],link_pos_list[i],(nfrom_list+pos1),(nto_list+pos));
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2=pos;
			pos+=strlen(nto_list+pos2)+1;
			pos3=pos1;
			pos1+=strlen(nfrom_list+pos3)+1;
		  }

	    }
		else
	    {
	     
				row_concat=ErrMess; 
				delete [] pathid_list;
				delete [] link_pos_list;
				delete [] nfrom_list;
				delete [] nto_list;
				delete [] last_nfrominfo;
				delete [] last_ntoinfo;
				delete [] resline;
				throw CONFDBEXCEPTION(row_concat);
	    }
	}
    else
	{
	  
		row_concat=ErrMess; 
		delete [] pathid_list;
		delete [] link_pos_list;
		delete [] nfrom_list;
		delete [] nto_list;
		delete [] last_nfrominfo;
		delete [] last_ntoinfo;
		delete [] resline;
		throw CONFDBEXCEPTION(row_concat);
	}

 }
  delete [] ErrMess;
  delete [] pathid_list;
  delete [] link_pos_list;
  delete [] nfrom_list;
  delete [] nto_list;
  delete [] last_nfrominfo;
  delete [] last_ntoinfo;
  delete [] resline;
  return result_query;
}

/**********************************************************************************************************/
/**
       * Get the path details  between dev_from and dev_to, returning an integer value.
	   * you need grant execute if you have your own db account
	   * @param mboardportid_from : portid_from of the motherboard. it will start the micro path
       * @param cpntname : put "none" if you provide a mboardportid_to
	   * @param mboardportid_to : portid of the motherboard which wil end the micro path. Put 0 if you provide a board cpnt name
	   * NB: the i-th entry of the pathid_list|link_pos_list|nfrom|port_nbrfrom|port_typefrom|nto|port_nbrto|port_typeto corresponds to one link of the path
	   * @return 0 if it is successful
       */


vector<string> CONFDB::PyGetMicroConnectFromPortid(int mboardportid_from,int mboardportid_to,string cpntname)
{
	vector<string> result_query;
	int rescode=0;
	int len_pid=1000;
	int len_pid1=len_pid;
	int nfrom_len=10000;
	int nfrom_len1=nfrom_len;
	int last_ntolen=5000;
	int last_nfromlen=5000;
	int nto_len=10000;
	int nto_len1=nto_len;
	int i=0;
	int pos1=0;
	int pos2=0;
	int pos3=0;
	int pos=0;
	char* resline=new char[500];
    string row_concat;
	char* ErrMess=new char[len_pid];
	int* pathid_list=new int[len_pid];
	int* link_pos_list=new int[len_pid];
	char* nfrom_list=new char[nfrom_len];
	char* nto_list=new char[nto_len];
	char* last_nfrominfo=new char[last_nfromlen];
	char* last_ntoinfo=new char[last_ntolen];
	rescode=GetMicroConnectFromPortid(mboardportid_from,  mboardportid_to, (char*) cpntname.c_str(), pathid_list,  link_pos_list, len_pid, nfrom_list,nfrom_len, nto_list,nto_len,last_nfrominfo,last_nfromlen, last_ntoinfo,last_ntolen, ErrMess);
	result_query.clear();
	if(rescode==0)
	{
		for (i=0;i<len_pid;i++)
		{
		  
			sprintf(resline,"%d|%d|%s|%s",pathid_list[i],link_pos_list[i],(nfrom_list+pos1),(nto_list+pos));
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2=pos;
			pos+=strlen(nto_list+pos2)+1;
			pos3=pos1;
			pos1+=strlen(nfrom_list+pos3)+1;
		}
   }
   else
   {
      if(len_pid1<len_pid || nfrom_len1<nfrom_len ||nto_len1<nto_len)
	  {
		delete [] pathid_list;
		delete [] link_pos_list;
		delete [] nfrom_list;
		delete [] nto_list;
		pathid_list=new int[len_pid];
		link_pos_list=new int[len_pid];
		nfrom_list=new char[nfrom_len];
		nto_list=new char[nto_len];
	    rescode=GetMicroConnectFromPortid(mboardportid_from,  mboardportid_to, (char*) cpntname.c_str(), pathid_list,  link_pos_list, len_pid, nfrom_list,nfrom_len, nto_list,nto_len,last_nfrominfo,last_nfromlen, last_ntoinfo,last_ntolen, ErrMess);
		if(rescode==0)
	    {
		  pos1=0;
		  pos2=0;
		  pos3=0;
		  pos=0;
	      for (i=0;i<len_pid;i++)
		  {
			sprintf(resline,"%d|%d|%s|%s",pathid_list[i],link_pos_list[i],(nfrom_list+pos1),(nto_list+pos));
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2=pos;
			pos+=strlen(nto_list+pos2)+1;
			pos3=pos1;
			pos1+=strlen(nfrom_list+pos3)+1;
		  }

	    }
		else
	    {
	     
				row_concat=ErrMess; 
				delete [] pathid_list;
				delete [] link_pos_list;
				delete [] nfrom_list;
				delete [] nto_list;
				delete [] last_nfrominfo;
				delete [] last_ntoinfo;
				delete [] resline;
				throw CONFDBEXCEPTION(row_concat);
	    }
	}
    else
	{
	  
		row_concat=ErrMess; 
		delete [] pathid_list;
		delete [] link_pos_list;
		delete [] nfrom_list;
		delete [] nto_list;
		delete [] last_nfrominfo;
		delete [] last_ntoinfo;
		delete [] resline;
		throw CONFDBEXCEPTION(row_concat);
	}

 }
  delete [] ErrMess;
  delete [] pathid_list;
  delete [] link_pos_list;
  delete [] nfrom_list;
  delete [] nto_list;
  delete [] last_nfrominfo;
  delete [] last_ntoinfo;
  delete [] resline;
  return result_query;
}

/**********************************************************************************************************/
/**
       * Get the path details  between dev_from and dev_to, returning an integer value.
	   * you need grant execute if you have your own db account
	   * @param mboardportid_from : portid from of the mboard start the micro path
       * @param cpnttype : cpnt type
	   * @param cpnttype_given : 1 if you have given a cpnt type, 0 if it should be consider as like 'cpnttype'
	   * NB: the i-th entry of the pathid_list|link_pos_list|nfrom|port_nbrfrom|port_typefrom|nto|port_nbrto|port_typeto corresponds to one link of the path
	   * @return 0 if it is successful
       */


vector<string> CONFDB::PyGetMicroConnectToCpntType(int mboardportid_from,int cpnttype_given,string cpnttype)
{
	vector<string> result_query;
	int rescode=0;
	int len_pid=1000;
	int len_pid1=len_pid;
	int nfrom_len=10000;
	int nfrom_len1=nfrom_len;
	int last_ntolen=5000;
	int last_nfromlen=5000;
	int nto_len=10000;
	int nto_len1=nto_len;
	int i=0;
	int pos1=0;
	int pos2=0;
	int pos3=0;
	int pos=0;
	char* resline=new char[500];
    string row_concat;
	char* ErrMess=new char[len_pid];
	int* pathid_list=new int[len_pid];
	int* link_pos_list=new int[len_pid];
	char* nfrom_list=new char[nfrom_len];
	char* nto_list=new char[nto_len];
	char* last_nfrominfo=new char[last_nfromlen];
	char* last_ntoinfo=new char[last_ntolen];
	char* nfrom_line=new char[2000];
	char* nto_line=new char[2000];
	rescode=GetMicroConnectToCpntType(mboardportid_from, cpnttype_given, (char*) cpnttype.c_str(), pathid_list,  link_pos_list, len_pid,nfrom_list,nfrom_len, nto_list,nto_len, last_nfrominfo,last_nfromlen, last_ntoinfo,last_ntolen,ErrMess);
    result_query.clear();
	
	if(rescode==0)
	{
		
		for (i=0;i<len_pid;i++)
		{
		    strcpy(nfrom_line,(nfrom_list+pos));
			strcpy(nto_line,(nto_list+pos1));
			
			sprintf(resline,"%d|%d|%s|%s",pathid_list[i],link_pos_list[i],nfrom_line+pos,nto_line+pos1);
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2=pos;
			
			pos+=strlen(nfrom_line+pos2)+1;
			pos3=pos1;
			pos1+=strlen(nto_line+pos3)+1;
			
		}
   }
   else
   {
      if(len_pid1<len_pid || nfrom_len1<nfrom_len ||nto_len1<nto_len)
	  {
		delete [] pathid_list;
		delete [] link_pos_list;
		delete [] nfrom_list;
		delete [] nto_list;
		pathid_list=new int[len_pid];
		link_pos_list=new int[len_pid];
		nfrom_list=new char[nfrom_len];
		nto_list=new char[nto_len];
	   rescode=GetMicroConnectToCpntType(mboardportid_from, cpnttype_given, (char*) cpnttype.c_str(), pathid_list,  link_pos_list, len_pid,nfrom_list,nfrom_len, nto_list,nto_len, last_nfrominfo,last_nfromlen, last_ntoinfo,last_ntolen,ErrMess);
	   if(rescode==0)
	    {
		  pos1=0;
		  pos2=0;
		  pos3=0;
		  pos=0;
	      for (i=0;i<len_pid;i++)
		  {
			
		     strcpy(nfrom_line,(nfrom_list+pos));
			strcpy(nto_line,(nto_list+pos1));
			//std::cout<<"value of nfrom_list "<<nfrom_line<<std::endl;
			sprintf(resline,"%d|%d|%s|%s",pathid_list[i],link_pos_list[i],nfrom_line+pos,nto_line+pos1);
			row_concat=resline;
			result_query.push_back(row_concat);
			pos2=pos;
			pos+=strlen(nfrom_line+pos2)+1;
			pos3=pos1;
			pos1+=strlen(nto_line+pos3)+1;
		
		  }

	    }
		else
	    {
	     
				row_concat=ErrMess; 
				delete [] pathid_list;
				delete [] link_pos_list;
				delete [] nfrom_list;
				delete [] nto_list;
				delete [] last_nfrominfo;
				delete [] last_ntoinfo;
				delete [] resline;
				delete [] nfrom_line;
				delete [] nto_line;
				throw CONFDBEXCEPTION(row_concat);
	    }
	}
    else
	{
	  
		row_concat=ErrMess; 
		delete [] pathid_list;
		delete [] link_pos_list;
		delete [] nfrom_list;
		delete [] nto_list;
		delete [] last_nfrominfo;
		delete [] last_ntoinfo;
		delete [] resline;
		delete [] nfrom_line;
		delete [] nto_line;
		throw CONFDBEXCEPTION(row_concat);
	}

	}
  delete [] ErrMess;
  delete [] pathid_list;
  delete [] link_pos_list;
  delete [] nfrom_list;
  delete [] nto_list;
  delete [] last_nfrominfo;
  delete [] last_ntoinfo;
  delete [] resline;
  delete [] nfrom_line;
  delete [] nto_line;
  return result_query;
}

/****************************************/
/**
       * delete a link returning a string .
       * @param linkid : linkid you want to delete
       * @param macro_link : 1 if it's a macro link you want to delete, 0 if it's a micro link
	   * @return "Successful deletion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyDeleteLinkRow(int linkid,int macro_link)  
{
string  res_code;
int c=0;


char* ErrorMess=new char[1000];
	


c=DeleteLinkRow(linkid,macro_link,ErrorMess);
if(c==0)
	res_code="Successful deletion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}

/***************************************/
/**
       * delete a port row returning a string .
       * @param portid : portid you want to delete. it will also delete all the links which start from or end at this port
	   * @return "Successful deletion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyDeletePortRow(int portid)  
{
string  res_code;
int c=0;


char* ErrorMess=new char[1000];
	


c=DeletePortRow(portid,ErrorMess);
if(c==0)
	res_code="Successful deletion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}

/**
        Update the devicetype field of a device :make sure that the devicetype exists 1  returning an integer value.
	   * @param devicename : name of the device you want to update
	   * @param devicetype : new value of the devicetype. can't be NULL! and should exist
       * @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

/******************************************************************/
string CONFDB::PyUpdateMultipleDTypeDevices(string devicename,string devicetype,int first_time1,int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleDTypeDevices((char*)devicename.c_str(),(char*) devicetype.c_str(),first_time1,last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**
       * Update bidirectional link field for a given link  returning an integer value.
	   * The user should manage the memory : there is no memory allocation.
	   * either you give node_from,pfrom or node_to,pto.
	   * @param node_from : name of the device starting the link: put "none" if you give node_to
	   * @param node_to : name of the device ending the link: put "none" if you give node_from
	   * @param port_nbrfrom : port nb of the device from
	   * @param port_nbrto : port nb of the device to
	   * @param port_typefrom : port nb of the device from: put "none" if it doesn't exist or if you provide about the node_to 
	   * @param port_typeto : port nb of the device to:put "none" if it doesn't exist or if you provide about the node_from
	   * @param link_info :  new info on the link
	   * @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
string CONFDB::PyUpdateMultipleLkInfoLinks(string node_from,string node_to,string port_nbrfrom,string port_typefrom,string port_nbrto,string port_typeto,string link_info,int first_time1, int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleLkInfoLinks((char*)node_from.c_str(),(char*) node_to.c_str(),(char*) port_nbrfrom.c_str(),(char*) port_typefrom.c_str(),(char*) port_nbrto.c_str(),(char*) port_typeto.c_str(),(char*)link_info.c_str(),first_time1, last_rows1, ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/************************************************************************/
// update the boot image of a functional device or a functional device type
/***********************************************************************/
/**
       * update the boot image information of a given device or device type returning an integer value.
       * @param devicename : name of the device or of the device type 
       * @param boot_image : will be used to configure the DHCP server. It is mandatory
	   * @param kernel_image_location : Put "" if absent.
	   * @param initrd_image_location : Put "" if absent.
	   * @param physical_location : Put "" if absent.
	   * @param boot_protocol : usually it;s BOOTP
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

string CONFDB::PyUpdateBootImage(string devicename,string boot_image,string kernel_image_location,string initrd_image_location,string physical_location,string  boot_protocol)
{
	int rescode=0;
	string res_code;

	char* ErrMess=new char[1000];
	rescode=UpdateBootImage((char*)devicename.c_str(),(char*) boot_image.c_str(),(char*) kernel_image_location.c_str(),(char*) initrd_image_location.c_str(),(char*) physical_location.c_str(),(char*) boot_protocol.c_str(), ErrMess);
	if(rescode==0)
		res_code="Successful update"; 
	else
	{
		res_code=ErrMess; 
		delete [] ErrMess;
		throw CONFDBEXCEPTION(res_code);
	}
		
delete [] ErrMess;
return res_code;
}

/************************************************************************/
// insert the boot image of a functional device or a functional device type
/***********************************************************************/
/**
       * insert the boot image information of a given device or device type returning an integer value.
       * @param devicename : name of the device or of the device type 
       * @param boot_image : will be used to configure the DHCP server. It is mandatory
	   * @param kernel_image_location : Put "" if absent.
	   * @param initrd_image_location : Put "" if absent.
	   * @param physical_location : Put "" if absent.
	   * @param boot_protocol : usually it;s BOOTP
	   * @param first_rows : 1 if it's your first MultipleDevice insert (save it into the database), 0 otherwise
	   * @param last_rows : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */

string CONFDB::PyInsertMultipleBootImages(string devicename,string boot_image,string kernel_image_location,string initrd_image_location,string physical_location,string  boot_protocol,int first_rows,int last_rows)
{
	int rescode=0;
	string res_code;

	char* ErrMess=new char[1000];
	if(last_rows!=0 && last_rows!=1)
	{
	res_code="last is equal to 0 or 1";
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
	}
if(first_rows!=0 && first_rows!=1)
{
	res_code="first is equal to 0 or 1";
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
	rescode=InsertMultipleBootImages((char*)devicename.c_str(),(char*) boot_image.c_str(),(char*) kernel_image_location.c_str(),(char*) initrd_image_location.c_str(),(char*) physical_location.c_str(),(char*) boot_protocol.c_str(), first_rows,last_rows,ErrMess);
	if(rescode==0)
		res_code="Successful insertion"; 
	else
	{
		res_code=ErrMess; 
		delete [] ErrMess;
		throw CONFDBEXCEPTION(res_code);
	}
		
delete [] ErrMess;
return res_code;
}

/************************************************************************/
// delete boot image information for a given device name or device type name
/***********************************************************************/
/**
       * delete boot image information returning an integer value.
       * @param devicename : devicename or device type for which you want to delete the boot information
       * @return the errMess
       */
string CONFDB::PyDeleteBootImage(string devicename)  
{
string  res_code;
int c=0;


char* ErrorMess=new char[1000];
	


c=DeleteBootImage((char*)devicename.c_str(),ErrorMess);
if(c==0)
	res_code="Successful deletion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}

/****************************************************************************************/
/**
       * Get the list of lkID  which have the link_info attributes similar to the given lkinfo,  returning a vector of int .
       * @param lkinfo : value of link_info (can be the exact value or contains this value)
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/******************************************************************/
vector<int> CONFDB::PyGetLkIDsPerLkInfo(string lkinfo)  
{
	
	int c=0;
	int c1=0;
	int i=0;
	int len_buffer=10000;
	int len_array=len_buffer;
	char* ErrorMess=new char[len_buffer];
string ErrMess;
	// créer un tableau d'entiers vide
    vector<int> v;
	int* lkID_list_res=new int[len_buffer];
	c=GetLkIDsPerLkInfo((char*) lkinfo.c_str(),len_array, lkID_list_res,ErrorMess);
	v.clear();
	if(c==0 ||(lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
	{
		if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] lkID_list_res;	
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			for(i=0;i<len_array;i++)
				v.push_back(lkID_list_res[i]); 
		}
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] lkID_list_res;
			lkID_list_res=new int[len_array];
			c=GetLkIDsPerLkInfo((char*) lkinfo.c_str(),len_array, lkID_list_res,ErrorMess);
			if(c1==0 ||(lkID_list_res[0]==-1 && lkID_list_res[1]==-1))
			{
				if(lkID_list_res[0]==-1 && lkID_list_res[1]==-1)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] lkID_list_res;	
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					for(i=0;i<len_array;i++)
						v.push_back(lkID_list_res[i]); 
				}
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] lkID_list_res;	
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] lkID_list_res;	
			throw CONFDBEXCEPTION(ErrMess);
		}
	}
	delete [] ErrorMess;
	delete [] lkID_list_res;	
	return v;
}

/**************************************************************/
/**
       * Get the row of the given IP alias  returning a string value.
       * @param ipalias : ip alias: 
       * @return string if successfull formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .
       */
/***************************************************************/
string  CONFDB::PyGetIPAliasRow(string ipalias)  
{

int len_array=2000;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];

int len_array_copy=len_array;
char* devtype_result=new char[2000];
int rescode=0;
string dtype_result="";


rescode=GetIPAliasRow((char*)ipalias.c_str(),len_array,devtype_result,ErrorMess);
 if(rescode==0 || strncmp(devtype_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(devtype_result,"NO_ROWS_SELECTED",16)==0)
	{
		dtype_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] devtype_result;
		throw CONFDBEXCEPTION(dtype_result);
	}
	else
		dtype_result=devtype_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete devtype_result;
		devtype_result=new char[len_array];
	rescode=GetIPAliasRow((char*)ipalias.c_str(),len_array,devtype_result,ErrorMess);
		if(rescode==0|| strncmp(devtype_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(devtype_result,"NO_ROWS_SELECTED",16)==0)
			{
				dtype_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] devtype_result;
				throw CONFDBEXCEPTION(dtype_result);
			}
			else
				dtype_result=devtype_result;
		}
		else
		{
			dtype_result=ErrorMess;
			delete [] ErrorMess;
			delete [] devtype_result;
			throw CONFDBEXCEPTION(dtype_result);
		}

	}
	else
	{
		dtype_result=ErrorMess;
		delete [] ErrorMess;
		delete [] devtype_result;
		throw CONFDBEXCEPTION(dtype_result);
	}

}
 delete [] ErrorMess;
delete [] devtype_result;
return dtype_result;
}

/*****************************************************************************************/
/**
       * Get the list of IP aliases  of the IP name,  returning a vector of strings.
       * @param IPName :IP name
	   * @return the list of IP aliases 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetIPAliasesPerIPName(string IPName)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetIPAliasesPerIPName((char*)IPName.c_str(),len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c=GetIPAliasesPerIPName((char*)IPName.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}

/**
	   * Update IP alias name,  returning an integer value.
	   * @param old_ipalias :  ip alias entry you want to modify. 
	   * @param new_ipalias : new value of ip address entry you want to modify. 
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 


string CONFDB::PyUpdateIPalias(string old_ipalias,string new_ipalias)
{
int rescode=0;
string res_code;

char* ErrMess=new char[1000];
rescode=UpdateIPalias((char*)old_ipalias.c_str(),(char*)new_ipalias.c_str(),  ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**
	   * Update serialnb due to a mistype,  returning an integer value.
	   * @param old_serialnb :  ip alias entry you want to modify. 
	   * @param new_serialnb : new value of ip address entry you want to modify. 
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 


string CONFDB::PyUpdateHWSerialNB(string old_serialnb,string new_serialnb)
{
int rescode=0;
string res_code;

char* ErrMess=new char[1000];
rescode=UpdateHWSerialNB((char*)old_serialnb.c_str(),(char*)new_serialnb.c_str(),  ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/**
	   * Update the function name due to a mistype,  returning an integer value.
	   * @param old_function_name :  ip alias entry you want to modify. 
	   * @param new_function_name : new value of ip address entry you want to modify. 
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 


string CONFDB::PyUpdateDeviceFunctionName(string old_function_name,string new_function_name)
{
int rescode=0;
string res_code;

char* ErrMess=new char[1000];
rescode=UpdateDeviceFunctionName((char*)old_function_name.c_str(),(char*)new_function_name.c_str(),  ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/*****************************************************************************************/
/**
       * Get the list of device names which have the given function,  returning a vector of strings.
       * @param function_name : name of the function. Put "none" if there is not.
	   * @return the list of device name 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetDeviceNamesPerFunction(string function_name)  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetDeviceNamesPerFunction((char*)function_name.c_str(),len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c=GetDeviceNamesPerFunction((char*)function_name.c_str(),len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}

/****************************************/
/**
       * Insert an IP alias , returning a string .
       * @param ip_address : ip address. Put "none", if you provide a IP name
       * @param ipname : ip name. Put "none", if you provide a IP @
	   * @param ipalias : ip alias to be associated to the given IP name or @.
	   * @param first : 1 if it's your MultipleDeviceType first insert (save it into the database), 0 otherwise
	   * @param last : 1 if it's your MultipleDeviceType last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyInsertMultipleIPAliases(string ip_address,string ipname,string ipalias,int first,int last)  
{
string  res_code;
int c=0;


if(last!=1 && last!=0)
{
res_code="last is either 0 or 1"; 
throw CONFDBEXCEPTION(res_code);	
}
if(first!=0 && first!=1)
{
	res_code="first is 0 or 1";
	throw CONFDBEXCEPTION(res_code);
}
char* ErrorMess=new char[1000];
	
c=InsertMultipleIPAliases((char*)ip_address.c_str(),(char*)ipname.c_str(),(char*)ipalias.c_str(),first, last,ErrorMess);
if(c==0)
	res_code="Successful insertion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	

delete [] ErrorMess;	
return res_code;
}


/****************************************/
/**
       * Insert a function , returning a string .
       * @param function : name of the function. 
       * @param last : 1 if it's your MultipleDeviceType last insert (save it into the database), 0 otherwise
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyInsertSimpleDeviceFunction(string function,int last)  
{
string  res_code;
int c=0;

if(function.length()<2)
{
res_code="No function given"; 
throw CONFDBEXCEPTION(res_code);	
}
if(last!=1 && last!=0)
{
res_code="last is either 0 or 1"; 
throw CONFDBEXCEPTION(res_code);	
}

char* ErrorMess=new char[1000];
	
c=InsertSimpleDeviceFunction((char*)function.c_str(), last,ErrorMess);
if(c==0)
	res_code="Successful insertion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	

delete [] ErrorMess;	
return res_code;
}

/****************************************/
/**
       * delete an ipalias .
       * @param ipalias : linkid you want to delete
	   * @return "Successful deletion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyDeleteIPAlias(string ipalias)  
{
string  res_code;
int c=0;


char* ErrorMess=new char[1000];
	


c=DeleteIPAlias((char*)ipalias.c_str(),ErrorMess);
if(c==0)
	res_code="Successful deletion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}

/**
       * Update the systemlist of a given devicetype  
	   * @param devicetype : name of the functional device type
	   * @param new_systemList : new list of the subsystems to which the device belongs to, elt separated by a coma
	   * @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	   * @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
string CONFDB::PyUpdateMultipleDeviceTypeSystemList(string devicetype,string new_systemList,int first_time1, int last_rows1)
{
int rescode=0;
string res_code;
if(first_time1!=1 && first_time1!=0)
{
	res_code="first is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
if(last_rows1!=1 && last_rows1!=0)
{
	res_code="last is 1 or 0"; 
	throw CONFDBEXCEPTION(res_code);	
}
char* ErrMess=new char[1000];
rescode=UpdateMultipleDeviceTypeSystemList((char*) devicetype.c_str(),(char*) new_systemList.c_str(),first_time1, last_rows1,ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}

/*****************************************************************************************/
/**
       * Get the list of available function,  returning a vector of strings.
	   * @return the list of available function 
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetAvailableFunctions()  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetAvailableFunctions(len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c=GetAvailableFunctions(len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}

/****************************************/
/**
       * delete a functional device. Ports of this device must be deleted before calling this fct. The device should not contain any microscopic cpnts.
       * @param deviceid : deviceid you want to delete
	   * @return "Successful deletion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyDeleteFunctionalDevice(int deviceid)  
{
string  res_code;
int c=0;


char* ErrorMess=new char[1000];
	


c=DeleteFunctionalDevice(deviceid,ErrorMess);
if(c==0)
	res_code="Successful deletion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}

/****************************************/
/**
       * delete a functional device type. Devices of this type must be deleted before calling this fct .
       * @param devicetype : device type name you want to delete
	   * @return "Successful deletion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyDeleteFunctionalDeviceType(string devicetype)  
{
string  res_code;
int c=0;


char* ErrorMess=new char[1000];
	


c=DeleteFunctionalDeviceType((char*)devicetype.c_str(),ErrorMess);
if(c==0)
	res_code="Successful deletion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}

/****************************************/
/**
       * delete a hw device. The status of the hw device should not be IN_USE and should not contain any microscopic cpnts.
       * @param serialnb : serialnb you want to delete
	   * @return "Successful deletion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyDeleteHWDevice(string serialnb)  
{
string  res_code;
int c=0;


char* ErrorMess=new char[1000];
	


c=DeleteHWDevice((char*)serialnb.c_str(),ErrorMess);
if(c==0)
	res_code="Successful deletion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}

/****************************************/
/**
       * delete a system name. there should be no device part of this system no longer.
       * @param system_name : system_name you want to delete
	   * @return "Successful deletion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyDeleteSystemName(string system_name)  
{
string  res_code;
int c=0;


char* ErrorMess=new char[1000];
	


c=DeleteSystemName((char*)system_name.c_str(),ErrorMess);
if(c==0)
	res_code="Successful deletion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);	
}
	

delete [] ErrorMess;
return res_code;
}

/*****************************************************************************************/
/**
       * Get the list of available system names,  returning a vector of strings.
	   * @return the list of available systems
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetListOfSubsystems()  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetListOfSubsystems(len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c=GetListOfSubsystems(len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}
/*****************************************************************************************/
/**
       * Get the list of available hwtypes,  returning a vector of strings.
	   * @return the list of available hwtypes
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetSpareHWTypeList()  
{
	
	int c=0;
	int c1=0;
	
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	string ErrMess;
	v.clear();
	c=GetSpareHWTypeList(len_array, devNames_list_res,ErrorMess);
	
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen(devNames_list_res+pos2);
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c=GetSpareHWTypeList(len_array, devNames_list_res,ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
					while(pos2<len_array)
					{
						pos1=strlen(devNames_list_res+pos2);
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}

/****************************************/
/**
       * Insert a system should follwo the ECS convention, returning a string .
       * @param system_name : system name. Ex : VELO, RICH
	   * @param parent_name : name of the parent system name if it exists. Put "none" if it doesn't. For instance VELO has no parent, but VELO_A has VELO. RICH1 has RICH. Should be the same as the FSM tree.
	   * @return "Successful insertion" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/****************************************************************************************************************************/
string CONFDB::PyInsertSubsystem(string system_name,string parent_name)  
{
string  res_code;
int c=0;




char* ErrorMess=new char[1000];
	
c=InsertSubsystem((char*)system_name.c_str(), (char*)parent_name.c_str(),ErrorMess);
if(c==0)
	res_code="Successful insertion "; 
else
{
	res_code=ErrorMess; 
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);
}
	

delete [] ErrorMess;	
return res_code;
}

/**
       * Update the name of a subsystem to be used in case of MYSTAPE ONLY  .
	   * @param old_sysname : name of the old system name
	   * @param new_sysname : name of the new system name
	   * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */ 
string CONFDB::PyUpdateSubsystemName(string old_sysname,string new_sysname)
{
int rescode=0;
string res_code;

char* ErrMess=new char[1000];
rescode=UpdateSubsystemName((char*) old_sysname.c_str(),(char*) new_sysname.c_str(),ErrMess);
if(rescode==0)
	res_code="Successful update"; 
else
{
	res_code=ErrMess; 
	delete [] ErrMess;
	throw CONFDBEXCEPTION(res_code);
}
		
delete [] ErrMess;
return res_code;
}


/************************************************************************************/
/****************************************************/
//there is one board with a serial nb XXX and has log_dev_1 as a functional name (its function)
//there is one board with a serial nb YYY and has log_dev_2 as a functional name (its function)
//Swapping two functional devices (log_dev_1 and log_dev_2) means:
//the board with the serial nb XXX has log_dev_2 as a functional name
//the board with the serial nb YYY has log_dev_1 as a functional name
//Connectivity is bound to the functional_device, so after the swapping the board XXX will be connected as the board YYY was connected and vice-versa
//For the DAQ, we also swap the mac addresses, but not the IP @ because they are bound to the functional device.
//we also report the swapping into the history table
/**
       * Swap two devices  returning an integer value.
	   * @param functional_devname1 : functional name of the device 1 
       * @param functional_devname2 :  functional name of the device 2
	   * @param comments : put "none" if you want to put the default comment which is "Swapping functional_devicename1 and functional_devicename1", otherwise, it will append your comment to the default.
       * @return "Successful update" if it is successful, 
	   * @throw CONFDBEXCEPTION
       */
/*****************************************************/
string CONFDB::PySwapTwoDevices(string functional_devname1,string functional_devname2,string comments)
{
string res_code;

int c=0;
char* ErrorMess=new char[1000];
c=SwapTwoDevices((char*)functional_devname1.c_str(),(char*)functional_devname2.c_str(),(char*)comments.c_str(),ErrorMess);
if(c==0)
	res_code="Successful update"; 
else
{
	res_code=ErrorMess;
	delete [] ErrorMess;
	throw CONFDBEXCEPTION(res_code);

}
			
		


delete [] ErrorMess;
return res_code;
}


/*********************************************************************/
/**
       * Get the status of the given functional device , 
       * @param dname : functional device name:  case sensitive.
       * @return the error message if not successful
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*********************************************************************/

string CONFDB::PyGetFunctionalDeviceStatus(string dname)
{
	string device_result;
	int c=0;
	
	int len_buffer=1000;

	
	char* ErrorMess=new char[len_buffer];
	
	char* device_res=new char[len_buffer];
	c=GetFunctionalDeviceStatus((char*)dname.c_str(),device_res,ErrorMess);
	
	if(c==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0 )
		device_result=device_res; 		
	else
	{
	
			device_result=ErrorMess;
			delete [] device_res;
		delete [] ErrorMess;
	throw CONFDBEXCEPTION(device_result);

			

	}
	delete [] ErrorMess;
	delete [] device_res;
	return device_result;
}

/*********************************************************************/
/**
       * Get the status of the given functional board cpnt , 
       * @param cpntname : functional board cpnt name:  case sensitive.
       * @return the error message if not successful
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*********************************************************************/

string CONFDB::PyGetFunctionalBoardCpntStatus(string cpntname)
{
	string device_result;
	int c=0;

	int len_buffer=1000;
	
	
	char* ErrorMess=new char[len_buffer];
	char* device_res=new char[len_buffer];
	c=GetFunctionalBoardCpntStatus((char*)cpntname.c_str(),device_res,ErrorMess);
	
	if(c==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0 )
		device_result=device_res; 		
	else
	{
		
			device_result=ErrorMess; 
			delete []ErrorMess;
			delete [] device_res;
			throw CONFDBEXCEPTION(device_result);

	}
	delete []ErrorMess;
	delete [] device_res;
	return device_result;
}
/*********************************************************************/
/**
       * Get the last functional device name which was occupied by the given hw serialnb , 
       * @param serialnb : hw serialnb :  case sensitive.
       * @return the error message if not successful
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*********************************************************************/

string CONFDB::PyGetHWLastFunctionalDevice(string serialnb)
{
string device_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_buffer_copy=len_buffer;
	
	char* ErrorMess=new char[len_buffer];
	char* device_res=new char[len_buffer];
	c=GetHWLastFunctionalDevice((char*)serialnb.c_str(),device_res,len_buffer,ErrorMess);
	
	if(c==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0 )
		device_result=device_res; 		
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_buffer_copy<len_buffer)
		{
			delete [] device_res;
			device_res=new char[len_buffer];
			c1=GetHWLastFunctionalDevice((char*)serialnb.c_str(),device_res,len_buffer,ErrorMess);
			if(c1==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0)
				device_result=device_res; 
			else
			{
				device_result=ErrorMess;
				delete [] device_res;
				delete []ErrorMess;
				throw CONFDBEXCEPTION(device_result);

			}
		}
		else
		{
			device_result=ErrorMess; 
			delete [] device_res;
			delete []ErrorMess;

			throw CONFDBEXCEPTION(device_result);

		}
	}
	delete []ErrorMess;
	delete [] device_res;
	return device_result;
}
/*********************************************************************/
/**
       * Get the last functional board cpnt name which was occupied by the given hw serialnb , 
       * @param hw_serialnb : serialnb of the board cpnt name:  case sensitive. Put "none" if there is not.
	   * @param hw_snbid : snbid of the board cpnt . case where there is no serialnb. Put -1 if you have provided a serialnb
       * @return the error message if not successful
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*********************************************************************/

string CONFDB::PyGetHWLastFunctionalBoardCpntName(string hw_serialnb,int hw_snbid)
{
	string device_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_buffer_copy=len_buffer;
	
	char* ErrorMess=new char[len_buffer];
	char* device_res=new char[len_buffer];
	c=GetHWLastFunctionalBoardCpntName((char*)hw_serialnb.c_str(),hw_snbid,device_res,len_buffer,ErrorMess);
	
	if(c==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0 )
		device_result=device_res; 		
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_buffer_copy<len_buffer)
		{
			delete [] device_res;
			device_res=new char[len_buffer];
			c1=GetHWLastFunctionalBoardCpntName((char*)hw_serialnb.c_str(),hw_snbid,device_res,len_buffer,ErrorMess);
			if(c1==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0)
				device_result=device_res; 
			else
			{
				device_result=ErrorMess; 
				delete [] device_res;
				delete []ErrorMess;

				throw CONFDBEXCEPTION(device_result);

			}		
		}
		else
		{
			device_result=ErrorMess; 
			delete [] device_res;
			delete []ErrorMess;

			throw CONFDBEXCEPTION(device_result);

		}
	}
	delete []ErrorMess;
	delete [] device_res;
	return device_result;
}

/*********************************************************************/
/**
       * Get the last hw serialnb  which  occupies the given functional name , 
       * @param functional_name : functional name of the device:  case sensitive. 
       * @return the error message if not successful
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*********************************************************************/

string CONFDB::PyGetFunctionalDeviceLastHW(string functional_name)
{
	string device_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_buffer_copy=len_buffer;
	
	char* ErrorMess=new char[len_buffer];
	char* device_res=new char[len_buffer];
	c=GetFunctionalDeviceLastHW((char*)functional_name.c_str(),device_res,len_buffer,ErrorMess);
	
	if(c==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0 )
		device_result=device_res; 		
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_buffer_copy<len_buffer)
		{
			delete [] device_res;
			device_res=new char[len_buffer];
			c1=GetFunctionalDeviceLastHW((char*)functional_name.c_str(),device_res,len_buffer,ErrorMess);
			if(c1==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0)
				device_result=device_res; 
			else
			{
				device_result=ErrorMess; 
				delete [] device_res;
				delete []ErrorMess;

				throw CONFDBEXCEPTION(device_result);
			}	
		}
		else
		{
			device_result=ErrorMess; 
			delete [] device_res;
			delete []ErrorMess;

			throw CONFDBEXCEPTION(device_result);

		}	
	}
	delete []ErrorMess;
	delete [] device_res;
	return device_result;
}

/*********************************************************************/
/**
       * Get the last cpnt hw serialnb  which  occupies the given functional name , 
       * @param functional_cpntname : functional name of the board cpnt:  case sensitive. 
       * @return the error message if not successful, If it's successful the resul is formatted at follows serialnb|snbid
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*********************************************************************/

string CONFDB::PyGetFunctionalBoardCpntNameLastHW(string functional_cpntname)
{
	string device_result;
	int c=0;
	int c1=0;
	int len_buffer=1000;
	int len_buffer_copy=len_buffer;
	
	char* ErrorMess=new char[len_buffer];
	char* device_res=new char[len_buffer];
	c=GetFunctionalBoardCpntNameLastHW((char*)functional_cpntname.c_str(),device_res,len_buffer,ErrorMess);
	
	if(c==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0 )
		device_result=device_res; 		
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_buffer_copy<len_buffer)
		{
			delete [] device_res;
			device_res=new char[len_buffer];
			c1=GetFunctionalDeviceLastHW((char*)functional_cpntname.c_str(),device_res,len_buffer,ErrorMess);
			if(c1==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0)
				device_result=device_res; 
			else
			{
				device_result=ErrorMess; 
				delete [] device_res;
				delete []ErrorMess;

				throw CONFDBEXCEPTION(device_result);

			}		
		}
		else
		{
			device_result=ErrorMess; 
			delete [] device_res;
			delete []ErrorMess;

			throw CONFDBEXCEPTION(device_result);

		}
	}
	delete []ErrorMess;
	delete [] device_res;
	return device_result;
}

/*********************************************************************/
/**
       * Get the status of the given hw device , 
       * @param dname : serialnb of the hw:  case sensitive.
       * @return the error message if not successful
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*********************************************************************/

string CONFDB::PyGetHWDeviceStatus(string dname)
{
	string device_result;
	int c=0;
	
	int len_buffer=1000;

	
	char* ErrorMess=new char[len_buffer];
	char* device_res=new char[len_buffer];
	c=GetHWDeviceStatus((char*)dname.c_str(),device_res,ErrorMess);
	
	if(c==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0 )
		device_result=device_res; 		
	else
	{
			device_result=ErrorMess; 
			delete [] device_res;
			delete []ErrorMess;

			throw CONFDBEXCEPTION(device_result);

	}
	delete []ErrorMess;
	delete [] device_res;
	return device_result;
}


/*********************************************************************/
/**
       * Get the status of the given hw serialnb , 
       * @param serialnb : serialnb of the board cpnt name:  case sensitive. Put "none" if there is not.
	   * @param snbid : snbid of the board cpnt . case where there is no serialnb. Put -1 if you have provided a serialnb
       * @return the error message if not successful
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*********************************************************************/

string CONFDB::PyGetHWBoardCpntStatus(string serialnb, int snbid)
{
	string device_result;
	int c=0;

	int len_buffer=1000;
	
	char* ErrorMess=new char[len_buffer];
	char* device_res=new char[len_buffer];
	c=GetHWBoardCpntStatus((char*)serialnb.c_str(),snbid,device_res,ErrorMess);
	
	if(c==0 || strncmp(device_res,"NO_ROWS_SELECTED",16)==0 )
		device_result=device_res; 		
	else
	{
			device_result=ErrorMess; 
			delete [] device_res;
			delete []ErrorMess;

			throw CONFDBEXCEPTION(device_result);

	}
	delete []ErrorMess;
	delete [] device_res;
	return device_result;
}

/*****************************************************************************************/
/**
       * Get the list of hw device names  given a status,  returning a vector of strings.
       * @param systemname : name of the subsystem
	   * @param device_status : status wanted
	   * @return the list of devicenames
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .       
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetHWDeviceByStatus(string systemname,string device_status)  
{
	
	int c=0;
	int c1=0;
	string ErrMess;
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	v.clear();
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	c=GetHWDeviceByStatus((char*)systemname.c_str(),(char*)device_status.c_str(),len_array, devNames_list_res,ErrorMess);
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess="NO_ROWS_SELECTED";
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		
		}
		else
		{
			pos1=0;
			pos2=0;
			
		
			while(pos2<len_array)
			{
				pos1=strlen((devNames_list_res+pos2));
				//devNames_list_res[pos1+pos2]='\0';
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetHWDeviceByStatus((char*)systemname.c_str(),(char*)device_status.c_str(),len_array, devNames_list_res,ErrorMess);			
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess="NO_ROWS_SELECTED";
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
				
				
					while(pos2<len_array+1)
					{
						pos1=strlen((devNames_list_res+pos2));
						//devNames_list_res[pos1+pos2]='\0';
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
			delete [] ErrorMess;
			delete [] dto;
			delete [] devNames_list_res;
			throw CONFDBEXCEPTION(ErrMess);
		
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}

/*****************************************************************************************/
/**
       * Get the list of hw board cpnt names  per status,  returning a vector of strings.
       * @param cpnt_status : location
	   * @return the list of devicenames
 	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .        
	   */
/*****************************************************************************/
vector<string> CONFDB::PyGetHWBoardCpntByStatus(string cpnt_status)  
{
	
	int c=0;
	int c1=0;
	string ErrMess;
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	v.clear();
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	
	c=GetHWBoardCpntByStatus((char*)cpnt_status.c_str(), devNames_list_res,len_array,ErrorMess);
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
		ErrMess=ErrorMess;
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
		
		}
		else
		{
			pos1=0;
			pos2=0;
		
			
			while(pos2<len_array)
			{
				pos1=strlen((devNames_list_res+pos2));
				//devNames_list_res[pos1+pos2]='\0';
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
			
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetHWBoardCpntByStatus((char*)cpnt_status.c_str(), devNames_list_res,len_array,ErrorMess);			
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
					ErrMess=ErrorMess;
					delete [] ErrorMess;
					delete [] dto;
					delete [] devNames_list_res;
					throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
			
					pos2=0;
				
					while(pos2<len_array)
					{
						pos1=strlen((devNames_list_res+pos2));
						//devNames_list_res[pos1+pos2]='\0';
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
			}

		}
		else
		{
			ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
		
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}


/*****************************************************************************************/
/**
       * Get the history of a hw serialnb  filtered by system name
	   * @param serial_nb : serialnb of the hw
       * @param min_date : min date formatted as follows YY/MM/DD/HH24/MM/SS
	   * @param max_date : max date formatted as follows YY/MM/DD/HH24/MM/SS
	   * @return the  history presented as follows devicename|serialnb|status|date of change|location
 	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .        
  */
/*****************************************************************************/
vector<string> CONFDB::PyGetHistoryOfHWDevice(string serial_nb,string min_date,string max_date)  
{
	
	int c=0;
	int c1=0;
	string ErrMess;
	int pos1,pos2;
	int len_buffer=500000;
	int len_devto=500;
	int len_array=len_buffer;
vector<string> v;
	v.clear();
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	c=GetHistoryOfHWDevice((char*)serial_nb.c_str(),devNames_list_res,len_array,(char*)min_date.c_str(),(char*)max_date.c_str(), ErrorMess);
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen((devNames_list_res+pos2));
				//devNames_list_res[pos1+pos2]='\0';
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetHistoryOfHWDevice((char*)serial_nb.c_str(),devNames_list_res,len_array,(char*)min_date.c_str(),(char*)max_date.c_str(), ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
						ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;
					pos2=0;
				
					while(pos2<len_array)
					{
						pos1=strlen((devNames_list_res+pos2));
						//devNames_list_res[pos1+pos2]='\0';
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
					
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
		ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);		}

		}
		else
		{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}


/*****************************************************************************************/
/**
       * Get the history of a hw serialnb  filtered by system name
	   * @param serial_nb : serialnb of the hw. Put "none" if you provide a hw_snbid
	   * @param hw_snbid : hw_snbid. Put -1 if you have provide a serialnb
       * @param min_date : min date formatted as follows YY/MM/DD/HH24/MM/SS. Put "none", if not
	   * @param max_date : max date formatted as follows YY/MM/DD/HH24/MM/SS. Put "none", if not
	   * @return the history presented as follows cpntname|serialnb|snbid|status|date of change|location
 	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .        
  */
/*****************************************************************************/
vector<string> CONFDB::PyGetHistoryOfHWBoardCpnt(string serial_nb,int hw_snbid,string min_date,string max_date)  
{
	
	int c=0;
	int c1=0;
	string ErrMess;
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	v.clear();
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	c=GetHistoryOfHWBoardCpnt((char*)serial_nb.c_str(),hw_snbid,devNames_list_res,len_array,(char*)min_date.c_str(),(char*)max_date.c_str(), ErrorMess);
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
	ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen((devNames_list_res+pos2));
				//devNames_list_res[pos1+pos2]='\0';
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetHistoryOfHWBoardCpnt((char*)serial_nb.c_str(),hw_snbid,devNames_list_res,len_array,(char*)min_date.c_str(),(char*)max_date.c_str(), ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);
				}
				else
				{
					pos1=0;

					pos2=0;
				
					while(pos2<len_array)
					{
						pos1=strlen((devNames_list_res+pos2));
						//devNames_list_res[pos1+pos2]='\0';
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);		}

		}
		else
		{
			ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);		
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}

/*****************************************************************************************/
/**
       * Get the history of a functional_name device  filtered by system name
	   * @param functional_name : functional_name of the device
       * @param min_date : min date formatted as follows YY/MM/DD/HH24/MM/SS
	   * @param max_date : max date formatted as follows YY/MM/DD/HH24/MM/SS
	   * @return the hsitory presented as follows devicename|serialnb|status|date of change|location
 	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .        
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetHistoryOfFunctionalDevice(string functional_name,string min_date,string max_date)  
{
	
	int c=0;
	int c1=0;
	string ErrMess;
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	v.clear();
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	c=GetHistoryOfFunctionalDevice((char*)functional_name.c_str(),devNames_list_res,len_array,(char*)min_date.c_str(),(char*)max_date.c_str(), ErrorMess);
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
		ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
		
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen((devNames_list_res+pos2));
				//devNames_list_res[pos1+pos2]='\0';
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
			
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetHistoryOfFunctionalDevice((char*)functional_name.c_str(),devNames_list_res,len_array,(char*)min_date.c_str(),(char*)max_date.c_str(), ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
				}
				else
				{
					pos1=0;
				
					pos2=0;
					
					while(pos2<len_array)
					{
						pos1=strlen((devNames_list_res+pos2));
						//devNames_list_res[pos1+pos2]='\0';
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
			}

		}
		else
		{
			ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
		
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}

/*****************************************************************************************/
/**
       * Get the history of a hw serialnb  filtered by system name
	   * @param functional_cpntname : functional_cpntname of the cpnt_name 
       * @param min_date : min date formatted as follows YY/MM/DD/HH24/MM/SS. Put "none", if not
	   * @param max_date : max date formatted as follows YY/MM/DD/HH24/MM/SS. Put "none", if not
	   * @return the history presented as follows cpntname|serialnb|snbid|status|date of change|location
 	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .        
 */
/*****************************************************************************/
vector<string> CONFDB::PyGetHistoryOfFunctionalBoardCpntName(string functional_cpntname,string min_date,string max_date)  
{
	
	int c=0;
	int c1=0;
	string ErrMess;
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	v.clear();
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	c=GetHistoryOfFunctionalBoardCpntName((char*)functional_cpntname.c_str(),devNames_list_res,len_array,(char*)min_date.c_str(),(char*)max_date.c_str(), ErrorMess);
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
			ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);		
		}
		else
		{
			pos1=0;
			pos2=0;
			
			while(pos2<len_array)
			{
				pos1=strlen((devNames_list_res+pos2));
				//devNames_list_res[pos1+pos2]='\0';
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetHistoryOfFunctionalBoardCpntName((char*)functional_cpntname.c_str(),devNames_list_res,len_array,(char*)min_date.c_str(),(char*)max_date.c_str(), ErrorMess);
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
				}
				else
				{
					pos1=0;
					pos2=0;
					
					while(pos2<len_array)
					{
						pos1=strlen((devNames_list_res+pos2));
						//devNames_list_res[pos1+pos2]='\0';
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
			ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
			}

		}
		else
		{
			ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
		
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}


/*****************************************************************************************/
/**
       * Get the list of functional device names  given a status,  returning a vector of strings.
       * @param systemname : name of the subsystem
	   * @param device_status : status wanted
	   * @return the list of devicenames
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found .        
       */
/*****************************************************************************/
vector<string> CONFDB::PyGetFunctionalDeviceByStatus(string systemname,string device_status)  
{
	
	int c=0;
	int c1=0;
	string ErrMess;
	int pos1,pos2;
	int len_buffer=50000;
	int len_devto=500;
	int len_array=len_buffer;
	vector<string> v;
	v.clear();
	char* dto=new char[len_devto];
	char* devNames_list_res=new char[len_buffer];
	char* ErrorMess=new char[1000];
	c=GetFunctionalDeviceByStatus((char*)systemname.c_str(),(char*)device_status.c_str(),len_array, devNames_list_res,ErrorMess);
	if(c==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
	{
		if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
		{
ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);			
		}
		else
		{
			pos1=0;
			pos2=0;
			while(pos2<len_array)
			{
				pos1=strlen((devNames_list_res+pos2));
				strcpy(dto,devNames_list_res+pos2);
				pos2+=pos1+1;
				v.push_back(dto); 
			}
		}
					
	}
	else
	{
		//check if the error is due to a buffer too small in that we retry with one bigger
		if(len_array>len_buffer)
		{
			delete [] devNames_list_res;
			devNames_list_res=new char[len_array];
			c1=GetFunctionalDeviceByStatus((char*)systemname.c_str(),(char*)device_status.c_str(),len_array, devNames_list_res,ErrorMess);			
			if(c1==0 ||  strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0 )
			{
				if(strncmp(devNames_list_res,"NO_ROWS_SELECTED",16)==0)
				{
				ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);				}
				else
				{
					pos1=0;
					pos2=0;
				
				
					while(pos2<len_array)
					{
						pos1=strlen((devNames_list_res+pos2));
						strcpy(dto,devNames_list_res+pos2);
						pos2+=pos1+1;
						
						v.push_back(dto); 
					}
				}
			
			}
			else
			{
			ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
			}

		}
		else
		{
		ErrMess=ErrorMess;
				delete [] ErrorMess;
				delete [] dto;
				delete [] devNames_list_res;
				throw CONFDBEXCEPTION(ErrMess);	
		}	
	}
	 delete [] ErrorMess;
	delete [] dto;
	delete [] devNames_list_res;
	return v;
}

/*********************************************************************/
/**
       * Get the row of the given spare serial nb ,  returning a string.
       * @param serialnb : serialnb: 
       * @return if successful formatted as follows |db_column_name (db_column_type):db_column_value|
	   * @throw CONFDBEXCEPTION : "NO_ROWS_SELECTED", if no matching entry in the table has been found.
       */
/*********************************************************************/

string  CONFDB::PyGetHWDeviceRow_serialnb (string serialnb)  
{

int len_array=2000;
int len_buffer=1000;
char* ErrorMess=new char[len_buffer];

int len_array_copy=len_array;
char* device_result=new char[2000];
int rescode=0;
string dev_result="";
 rescode=GetHWDeviceRow_serialnb ((char*)serialnb.c_str(),len_array,device_result,ErrorMess);
 if(rescode==0|| strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
{
	if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
	{
		dev_result="NO_ROWS_SELECTED";
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
	else
	dev_result=device_result;

}
else
{
	if(len_array>len_array_copy)
	{
		delete device_result;
		device_result=new char[len_array];
		rescode=GetHWDeviceRow_serialnb ((char*)serialnb.c_str(),len_array,device_result,ErrorMess);
		if(rescode==0 ||  strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
		{
			if(strncmp(device_result,"NO_ROWS_SELECTED",16)==0)
			{
				dev_result="NO_ROWS_SELECTED";
				delete [] ErrorMess;
				delete [] device_result;
				throw CONFDBEXCEPTION(dev_result);
			}
			else
				dev_result=device_result;
		}
		else
		{
			dev_result=ErrorMess;
			delete [] ErrorMess;
			delete [] device_result;
			throw CONFDBEXCEPTION(dev_result);
		}

	}
	else
	{
		dev_result=ErrorMess;
		delete [] ErrorMess;
		delete [] device_result;
		throw CONFDBEXCEPTION(dev_result);
	}
}
delete [] ErrorMess;
delete [] device_result;
return dev_result;
}
