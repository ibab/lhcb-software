#ifndef      __CALO_DIGITIZER_CALOFUNCTIONSCOLELCTION_H__
#define      __CALO_DIGITIZER_CALOFUNCTIONSCOLELCTION_H__


//
//
//
typedef   double *(Function)( double ); 

class CaloFunctionsColelction : public CaloCollection<Function>
{
};


#endif   //  __CALO_DIGITIZER_CALOFUNCTIONSCOLELCTION_H__
