#ifndef     __CALO_DIGITISATION_CALODIGITISATION_H__ 


#include <functional>


/// exception class 
#include "CaloAlg/CaloException.h" 


#include "CaloEvent/MCCaloDigit.h" 
#include "CaloEvent/MCCaloSummedDeposit.h" 


//forward declaration 
class IMessageSvc; 


///
///   Collection of "real" digitisation algorithms 
///       in this version just a placeholder 
///
///  Author:Vanya Belyaev 
///  Date:  Nov 26 1999 
///

namespace CaloDigitisation
{
  ///
  typedef std::unary_function<MCCaloSummedDeposit*,MCCaloDigit*>   Digitisation;
  ///
  /// The simplest (most primitive) Digitisation - 
  ///   produces "energies" just by rescaling the 
  ///   "active energies" 
  ///
  class Digitisation_with_Constant : public Digitisation 
    {
    public:
      ///
      ///
      /// constructor 
      Digitisation_with_Constant( double        coefficient    = 1.0  , 
				  IMessageSvc*  messageService = 0    )
	: m_coefficient    ( coefficient    ) 
	, m_messageService ( messageService )
	{};
      ///
      /// the only essential method  
      /// 
      inline MCCaloDigit* operator() ( MCCaloSummedDeposit* mcdeposit ) const 
	{ 
	  ///
	  /// NULL is to be NULL !!!
          if( 0 == mcdeposit )             { return 0 ; }                           /// RETURN !!!
	  // create new digit  
          MCCaloDigit* newdigit = new(std::nothrow) MCCaloDigit( mcdeposit ) ;  
	  ///  
	  if( 0 != newdigit ) { newdigit->setE( mcdeposit->activeE() * m_coefficient ); }
	  ///
	  return newdigit; 
	};
      ///
    private:
      ///
      /// rescaling coefficient for energy 
      double       m_coefficient    ; 
      /// message service to report the problems 
      IMessageSvc* m_messageService ;
      ///
    };
  
  ///
  ///
  /// The more clever Digitisation - 
  ///   produces "raw energies" just by using the same function  
  ///   for all channels 
  ///
  /// in this context "Functor" is something which have the 
  ///  double operator() ( double )
  /// it can be real functor, or just an pointer to function, e.g 
  /// typedef double (*PF)(double) ; 
  /// The main aspect - the existence of 
  ///    double e1 = functor( double e2 ) method 
  /// 
  template< class Functor > 
    class Digitisation_with_Function : public Digitisation 
    {
    public:
      ///
      Digitisation_with_Function( Functor&      functor               , 
			     IMessageSvc*  messageService = 0    )
	: m_functor        ( functor        ) 
	, m_messageService ( messageService )
	{}; 
      ///
      /// the only one essentila method 
      inline MCCaloDigit* operator() ( MCCaloSummedDeposit* mcdigit ) const 
	{ 
	  ///
	  /// NULL is to be NULL !!!
          if( 0 == mcdeposit )             { return 0 ; }                           /// RETURN !!!
	  // create new digit  
          MCCaloDigit* newdigit = new(std::nothrow) MCCaloDigit( *mcdeposit ) ;  
	  ///  
	  if( 0 != newdigit ) { newdigit->setE( m_functor( mcdeposit->activeE() ) ) ; } 
	  ///
	  return newdigit; 
	};
    private:
      ///
      /// functor to be used for digitization  
      Functor&      m_functor        ; 
      /// message service to report the problems 
      IMessageSvc*  m_messageService ;
      ///
    };
  
  
  ///
  /// "the most clever" Digitisation 
  /// It require functor to be implemented as a 
  /// something having 
  ///   double (*f)( double ) operator() (const CaloCellID&)
  /// a "CaloCollection< double (*f)(double) >" is a nice 
  /// candidate for implementation of such functor! 
  ///
  template< class Functor > 
    class Digitisation_with_Function_per_Cell : public Digitisation 
    {
    public:
      ///
      ///
      /// constructor
      Digitisation_with_Function_per_Cell( Functor&      functor               , 
					   IMessageSvc*  messageService = 0    )
	: m_functor        ( functor        ) 
	, m_messageService ( messageService )
	{}; 
      ///
      ///
      /// the only essential method 
      inline MCCaloDigit* operator() ( MCCaloSummedDeposit* mcdeposit ) const 
	{ 
	  ///
	  /// NULL is to be NULL !!!
          if( 0 == mcdeposit )             { return 0 ; }                           /// RETURN !!!
	  // create new digit  
          MCCaloDigit* newdigit = new(std::nothrow) MCCaloDigit( *mcdeposit ) ;  
	  ///  
	  if( 0 != newdigit ) { newdigit->setE( m_functor( mcdeposit->cellID() )( mcdeposit->activeE() ) ); }
	  ///
	  return newdigit; 
	};
      ///
    private:
      ///
      /// functor to be used for digitization  
      Functor&      m_functor        ; 
      /// message service to report the problems 
      IMessageSvc*  m_messageService ;
      ///
    };
  
  ///
  ///
  ///
  
}; // end of namespace CaloDigitisation 


///
///
///


#endif  //  __CALO_DIGITISATION_CALODIGITISATION_H__ 









