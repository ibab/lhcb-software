// ============================================================================
#ifndef       Gauss_Target_Multiplicity_H
#define       Gauss_Target_Multiplicity_H 1 
// ============================================================================
// STL 
#include <string>
#include <vector>
//
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"
//#include "GaudiKernel/INTupleSvc.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/IHistoTool.h"
#include "AIDA/IHistogram1D.h"
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
// forward declarations 
//template <class TYPE> class GiGaFactory;
class G4Track;
class G4ParticleDefinition;

/** @class GaussPostTrackAction GaussPostTrackAction.h 
 *
 * Gauss tracking action deals with storing tracks
 * and handles track information objects
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  
 *  @date    23/01/2001
 */


class GaussTargetMultiplicity: virtual public GiGaTrackActionBase
{
	/// friend factory for instantiation 
	//  friend class GiGaFactory<GaussPostTrackAction>;

	public: 
		/// useful typedefs
		typedef  std::vector<std::string>                  TypeNames;
		typedef  std::vector<const G4ParticleDefinition*>  PartDefs;
		///
		//protected:

		/** standard constructor 
		 *  @see GiGaTrackActionBase 
		 *  @see GiGaBase 
		 *  @see AlgTool 
		 *  @param type type of the object (?)
		 *  @param name name of the object
		 *  @param parent  pointer to parent object
		 */
		GaussTargetMultiplicity
			( const std::string& type   ,
			  const std::string& name   ,
			  const IInterface*  parent ) ;

		/// destructor (virtual and protected)
		virtual ~GaussTargetMultiplicity();

	public: 


		/** initialize the track action  
		 *  @see GiGaTrackActionBase 
		 *  @see GiGaBase 
		 *  @see  AlgTool 
		 *  @see IAlgTool 
		 *  @return status code 
		 */
		virtual StatusCode initialize () ; 

		/** finalize the action object 
		 *  @see GiGaTrackActionBase 
		 *  @see GiGaBase 
		 *  @see  AlgTool 
		 *  @see IAlgTool 
		 *  @return status code
		 */
		virtual StatusCode finalize   () ;

		/** perform action 
		 *  @see G4UserTrackingAction
		 *  @param pointer to new track opbject 
		 */
		virtual void PreUserTrackingAction  ( const G4Track* ) ;

		/** perform action 
		 *  @see G4UserTrackingAction
		 *  @param pointer to new track opbject 
		 */
		virtual void PostUserTrackingAction ( const G4Track* ) ;

	protected:

		///

	private:


		GaussTargetMultiplicity() ; ///< no default constructor
		GaussTargetMultiplicity( const GaussTargetMultiplicity& ) ; ///< no copy 
		GaussTargetMultiplicity& operator=( const GaussTargetMultiplicity& ) ; 

		/// Utility function to check multiplicity
		//void checkIntRegion( const G4Track * track );
		int checkIntRegion( const G4Track* );

		//  Flags:
		/// bool to check the multiplicity
		std::vector<std::string> m_intVolumeName;
		std::vector<std::string> m_intVolumeString;
		std::vector<std::string> mp_targMat;
		std::vector<std::string> mp_physList;
		std::vector<int>         mp_projPdgID; 
		std::vector<double>      mp_projEng;
		std::vector<double>      mp_targThick;


		NTuple::Tuple*    m_ntuple;

		// track-level variables
		NTuple::Item< int >        m_projPdgID;
		NTuple::Item< double >       m_projEng;
		NTuple::Item< double >     m_targThick;
		NTuple::Item< char >       m_targMat;
		NTuple::Item< char >      m_physList;

		NTuple::Item< int >       m_trackid;
		NTuple::Item< int >      m_trackpdg;
		NTuple::Item< int >      m_trackpar;
		NTuple::Item< double >      m_prodx;
		NTuple::Item< double >      m_prody;
		NTuple::Item< double >      m_prodz;
		NTuple::Item< int >        m_cptype;
		NTuple::Item< int >       m_cpstype;
		NTuple::Item< double >       m_endx;
		NTuple::Item< double >       m_endy;
		NTuple::Item< double >       m_endz;
		NTuple::Item< double >       m_momx;
		NTuple::Item< double >       m_momy;
		NTuple::Item< double >       m_momz;
		NTuple::Item< double >       m_kine;
		NTuple::Item< double >     m_charge;
		//daughters variables
		NTuple::Item< int >         m_ndaugh;
		NTuple::Item< int >   m_ninelchdaugh;
		NTuple::Array< int >      m_isinside;
		NTuple::Array< int >     m_dirparent;
		NTuple::Array< int >      m_parentid;
		NTuple::Array< int >        m_daupdg;
		NTuple::Array< double >    m_daumomx;
		NTuple::Array< double >    m_daumomy;
		NTuple::Array< double >    m_daumomz;
		NTuple::Array< double >    m_daukine;
		NTuple::Array< double >  m_daucharge;
		NTuple::Array< int >        m_eptype;
		NTuple::Array< int >       m_epstype;



		AIDA::IHistogram1D* h_end_z;
		AIDA::IHistogram1D* h_end_z_inside;
		AIDA::IHistogram1D* h_endsubproc;
		AIDA::IHistogram1D* h_in_dau_mult;
		AIDA::IHistogram1D* h_el_dau_mult;
		AIDA::IHistogram1D* h_in_chdau_mult;
		AIDA::IHistogram1D* h_pdg_dau;
		AIDA::IHistogram1D* h_pdg_dau_high;
		AIDA::IHistogram1D* h_dau_gammas_kine;
		AIDA::IHistogram1D* h_dau_high_kine;
		AIDA::IHistogram1D* h_dau_kine;
		AIDA::IHistogram1D* h_end_proc;
		AIDA::IHistogram1D* h_in_nch_perc;
		AIDA::IHistogram1D* h_in_chplus_perc;
		AIDA::IHistogram1D* h_in_chminus_perc;
		AIDA::IHistogram1D* h_end_subproc;
};

// ============================================================================
//#include "GaussTargetMultiplicity.icpp"
// ============================================================================

// ============================================================================
#endif  //    Gauss_Target_Multiplicity_H
// ============================================================================
















