// Class: Read4XPlanes
// Automatically generated by MethodBase::MakeClass
//

/* configuration options =====================================================

#GEN -*-*-*-*-*-*-*-*-*-*-*- general info -*-*-*-*-*-*-*-*-*-*-*-

Method         : MLP::4XPlanes
TMVA Release   : 4.2.1         [262657]
ROOT Release   : 6.04/14       [394254]
Creator        : mstahl
Date           : Fri Mar  4 08:43:06 2016
Host           : Linux kreplinpc 3.13.0-77-generic #121-Ubuntu SMP Wed Jan 20 10:50:42 UTC 2016 x86_64 x86_64 x86_64 GNU/Linux
Dir            : /work/MVATests
Training events: 40000
Analysis type  : [Classification]


#OPT -*-*-*-*-*-*-*-*-*-*-*-*- options -*-*-*-*-*-*-*-*-*-*-*-*-

# Set by User:
HiddenLayers: "16,10" [Specification of hidden layer architecture]
NeuronType: "ReLU" [Neuron activation function type]
RandomSeed: "0" [Random seed for initial synapse weights (0 means unique seed for each run; default value '1')]
EstimatorType: "CE" [MSE (Mean Square Estimator) for Gaussian Likelihood or CE(Cross-Entropy) for Bernoulli Likelihood]
VarTransform: "N" [List of variable transformations performed before training, e.g., "D_Background,P_Signal,G,N_AllClasses" for: "Decorrelation, PCA-transformation, Gaussianisation, Normalisation, each for the given class of events ('AllClasses' denotes all events of all classes, if no class indication is given, 'All' is assumed)"]
CreateMVAPdfs: "True" [Create PDFs for classifier outputs (signal and background)]
TestRate: "5" [Test for overtraining performed at each #th epochs]
# Default:
NCycles: "500" [Number of training cycles]
NeuronInputType: "sum" [Neuron input function type]
V: "False" [Verbose output (short form of "VerbosityLevel" below - overrides the latter one)]
VerbosityLevel: "Default" [Verbosity level]
H: "False" [Print method-specific help message]
IgnoreNegWeightsInTraining: "False" [Events with negative weights are ignored in the training (but are included for testing and performance evaluation)]
TrainingMethod: "BP" [Train with Back-Propagation (BP), BFGS Algorithm (BFGS), or Genetic Algorithm (GA - slower and worse)]
LearningRate: "2.000000e-02" [ANN learning rate parameter]
DecayRate: "1.000000e-02" [Decay rate for learning parameter]
EpochMonitoring: "False" [Provide epoch-wise monitoring plots according to TestRate (caution: causes big ROOT output file!)]
Sampling: "1.000000e+00" [Only 'Sampling' (randomly selected) events are trained each epoch]
SamplingEpoch: "1.000000e+00" [Sampling is used for the first 'SamplingEpoch' epochs, afterwards, all events are taken for training]
SamplingImportance: "1.000000e+00" [ The sampling weights of events in epochs which successful (worse estimator than before) are multiplied with SamplingImportance, else they are divided.]
SamplingTraining: "True" [The training sample is sampled]
SamplingTesting: "False" [The testing sample is sampled]
ResetStep: "50" [How often BFGS should reset history]
Tau: "3.000000e+00" [LineSearch "size step"]
BPMode: "sequential" [Back-propagation learning mode: sequential or batch]
BatchSize: "-1" [Batch size: number of events/batch, only set if in Batch Mode, -1 for BatchSize=number_of_events]
ConvergenceImprove: "1.000000e-30" [Minimum improvement which counts as improvement (<0 means automatic convergence check is turned off)]
ConvergenceTests: "-1" [Number of steps (without improvement) required for convergence (<0 means automatic convergence check is turned off)]
UseRegulator: "False" [Use regulator to avoid over-training]
UpdateLimit: "10000" [Maximum times of regulator update]
CalculateErrors: "False" [Calculates inverse Hessian matrix at the end of the training to be able to calculate the uncertainties of an MVA value]
WeightRange: "1.000000e+00" [Take the events for the estimator calculations from small deviations from the desired value to large deviations only over the weight range]
##


#VAR -*-*-*-*-*-*-*-*-*-*-*-* variables *-*-*-*-*-*-*-*-*-*-*-*-

NVar 9
mdx                           mdx                           mdx                           <#Delta x>                    mm                                'F'    [0.00867361109704,10.046962738]
sx                            sx                            sx                            #sigma_{Cluster}              mm                                'F'    [0.0104144280776,27.76912117]
spread                        spread                        spread                        x_{last} - x_{first}          mm                                'F'    [0.0260208323598,92.4787521362]
pp                            pp                            pp                            <x> - x_{Extrap}              mm                                'F'    [-5616.24755859,5762.14111328]
nwHits                        nwHits                        nwHits                        # of weighted hits                                              'I'    [4,20]
sx_pos                        sx_pos                        sx_pos                        #sigma_{position}             mm                                'F'    [0,875.192321777]
mx                            mx                            mx                            <x_{Cluster}>                 mm                                'F'    [-2842,2839.98754883]
tx                            tx                            tx                            XSlope                                                          'F'    [-0.436186283827,0.435859173536]
ty                            ty                            ty                            YSlope                                                          'F'    [-0.283686846495,0.283448517323]
NSpec 0


============================================================================ */

#include <vector>
#include <cmath>
#include <string>
#include <iostream>

#ifndef IClassifierReader__def
#define IClassifierReader__def

class IClassifierReader {

 public:

   // constructor
   IClassifierReader() : fStatusIsClean( true ) {}
   virtual ~IClassifierReader() {}

   // return classifier response
   virtual double GetMvaValue( std::vector<double>& inputValues ) const = 0;

   // returns classifier status
   bool IsStatusClean() const { return fStatusIsClean; }

 protected:

   bool fStatusIsClean;
};

#endif

class Read4XPlanes : public IClassifierReader {

 public:

   // constructor
   Read4XPlanes( std::vector<std::string>& theInputVars )
      : IClassifierReader(),
        fClassName( "Read4XPlanes" ),
        fNvars( 9 ),
        fIsNormalised( false )
   {      
      // the training input variables
      const char* inputVars[] = { "mdx", "sx", "spread", "pp", "nwHits", "sx_pos", "mx", "tx", "ty" };

      // sanity checks
      if (theInputVars.size() <= 0) {
         std::cout << "Problem in class \"" << fClassName << "\": empty input vector" << std::endl;
         fStatusIsClean = false;
      }

      if (theInputVars.size() != fNvars) {
         std::cout << "Problem in class \"" << fClassName << "\": mismatch in number of input values: "
                   << theInputVars.size() << " != " << fNvars << std::endl;
         fStatusIsClean = false;
      }

      // validate input variables
      for (size_t ivar = 0; ivar < theInputVars.size(); ivar++) {
         if (theInputVars[ivar] != inputVars[ivar]) {
            std::cout << "Problem in class \"" << fClassName << "\": mismatch in input variable names" << std::endl
                      << " for variable [" << ivar << "]: " << theInputVars[ivar].c_str() << " != " << inputVars[ivar] << std::endl;
            fStatusIsClean = false;
         }
      }

      // initialize min and max vectors (for normalisation)
      fVmin[0] = -1;
      fVmax[0] = 1;
      fVmin[1] = -1;
      fVmax[1] = 1;
      fVmin[2] = -1;
      fVmax[2] = 1;
      fVmin[3] = -1;
      fVmax[3] = 1;
      fVmin[4] = -1;
      fVmax[4] = 1;
      fVmin[5] = -1;
      fVmax[5] = 1;
      fVmin[6] = -1;
      fVmax[6] = 1;
      fVmin[7] = -1;
      fVmax[7] = 1;
      fVmin[8] = -1;
      fVmax[8] = 1;

      // initialize input variable types
      fType[0] = 'F';
      fType[1] = 'F';
      fType[2] = 'F';
      fType[3] = 'F';
      fType[4] = 'I';
      fType[5] = 'F';
      fType[6] = 'F';
      fType[7] = 'F';
      fType[8] = 'F';

      // initialize constants
      Initialize();

      // initialize transformation
      InitTransform();
   }

   // destructor
   virtual ~Read4XPlanes() {
      Clear(); // method-specific
   }

   // the classifier response
   // "inputValues" is a vector of input values in the same order as the 
   // variables given to the constructor
   double GetMvaValue( std::vector<double>& inputValues ) const;

 private:

   // method-specific destructor
   void Clear();

   // input variable transformation

   double fMin_1[3][9];
   double fMax_1[3][9];
   void InitTransform_1();
   void Transform_1( std::vector<double> & iv, int sigOrBgd ) const;
   void InitTransform();
   void Transform( std::vector<double> & iv, int sigOrBgd ) const;

   // common member variables
   const char* fClassName;

   const size_t fNvars;
   size_t GetNvar()           const { return fNvars; }
   char   GetType( int ivar ) const { return fType[ivar]; }

   // normalisation of input variables
   const bool fIsNormalised;
   bool IsNormalised() const { return fIsNormalised; }
   double fVmin[9];
   double fVmax[9];
   double NormVariable( double x, double xmin, double xmax ) const {
      // normalise to output range: [-1, 1]
      return 2*(x - xmin)/(xmax - xmin) - 1.0;
   }

   // type of input variable: 'F' or 'I'
   char   fType[9];

   // initialize internal variables
   void Initialize();
   double GetMvaValue__( const std::vector<double>& inputValues ) const;

   // private members (method specific)

   double ActivationFnc(double x) const;
   double OutputActivationFnc(double x) const;

   int fLayers;
   int fLayerSize[4];
   double fWeightMatrix0to1[17][10];   // weight matrix from layer 0 to 1
   double fWeightMatrix1to2[11][17];   // weight matrix from layer 1 to 2
   double fWeightMatrix2to3[1][11];   // weight matrix from layer 2 to 3

   double * fWeights[4];
};

inline void Read4XPlanes::Initialize()
{
   // build network structure
   fLayers = 4;
   fLayerSize[0] = 10; fWeights[0] = new double[10]; 
   fLayerSize[1] = 17; fWeights[1] = new double[17]; 
   fLayerSize[2] = 11; fWeights[2] = new double[11]; 
   fLayerSize[3] = 1; fWeights[3] = new double[1]; 
   // weight matrix from layer 0 to 1
   fWeightMatrix0to1[0][0] = 3.86413114088581;
   fWeightMatrix0to1[1][0] = -0.594593585452409;
   fWeightMatrix0to1[2][0] = 0.364961561638081;
   fWeightMatrix0to1[3][0] = 0.315281780498077;
   fWeightMatrix0to1[4][0] = 0.445170541244067;
   fWeightMatrix0to1[5][0] = 2.4187435466836;
   fWeightMatrix0to1[6][0] = -2.14623084400937;
   fWeightMatrix0to1[7][0] = -2.11036086764159;
   fWeightMatrix0to1[8][0] = -4.16805976889433;
   fWeightMatrix0to1[9][0] = 0.676896827766244;
   fWeightMatrix0to1[10][0] = -1.61586979513878;
   fWeightMatrix0to1[11][0] = -1.43998354680442;
   fWeightMatrix0to1[12][0] = 1.14073139676159;
   fWeightMatrix0to1[13][0] = 3.8500540205695;
   fWeightMatrix0to1[14][0] = 5.72821974008214;
   fWeightMatrix0to1[15][0] = 2.38653928906527;
   fWeightMatrix0to1[0][1] = -3.69222396619516;
   fWeightMatrix0to1[1][1] = -3.08367221592457;
   fWeightMatrix0to1[2][1] = -5.27864559628458;
   fWeightMatrix0to1[3][1] = -2.32770535157104;
   fWeightMatrix0to1[4][1] = -1.60846117337629;
   fWeightMatrix0to1[5][1] = 0.451593766748252;
   fWeightMatrix0to1[6][1] = 3.11635911598355;
   fWeightMatrix0to1[7][1] = -3.44255017298465;
   fWeightMatrix0to1[8][1] = -6.11883130118042;
   fWeightMatrix0to1[9][1] = 1.58031593674058;
   fWeightMatrix0to1[10][1] = 2.22692706890895;
   fWeightMatrix0to1[11][1] = -3.43471461882084;
   fWeightMatrix0to1[12][1] = 3.92424697619743;
   fWeightMatrix0to1[13][1] = -2.00941550018593;
   fWeightMatrix0to1[14][1] = -1.29514506416555;
   fWeightMatrix0to1[15][1] = -0.726159049526721;
   fWeightMatrix0to1[0][2] = -5.43563833486622;
   fWeightMatrix0to1[1][2] = 2.6324380708822;
   fWeightMatrix0to1[2][2] = 3.69630791982848;
   fWeightMatrix0to1[3][2] = 1.4277648160001;
   fWeightMatrix0to1[4][2] = 0.605872665262387;
   fWeightMatrix0to1[5][2] = -0.3487606763965;
   fWeightMatrix0to1[6][2] = -1.62750077608682;
   fWeightMatrix0to1[7][2] = 2.19410573674901;
   fWeightMatrix0to1[8][2] = 2.68971236364546;
   fWeightMatrix0to1[9][2] = -0.22715864076564;
   fWeightMatrix0to1[10][2] = 0.442239317790377;
   fWeightMatrix0to1[11][2] = 2.55071281445994;
   fWeightMatrix0to1[12][2] = -0.988583850066812;
   fWeightMatrix0to1[13][2] = 1.80417344007211;
   fWeightMatrix0to1[14][2] = -1.83302129682753;
   fWeightMatrix0to1[15][2] = -1.2236748124365;
   fWeightMatrix0to1[0][3] = 2.80313032395704;
   fWeightMatrix0to1[1][3] = -0.697783498428631;
   fWeightMatrix0to1[2][3] = -13.0259864076955;
   fWeightMatrix0to1[3][3] = -0.0411131734099111;
   fWeightMatrix0to1[4][3] = 1.6227014543811;
   fWeightMatrix0to1[5][3] = -1.5954795135425;
   fWeightMatrix0to1[6][3] = -14.2691418870529;
   fWeightMatrix0to1[7][3] = -12.6881501918074;
   fWeightMatrix0to1[8][3] = -2.76914874381045;
   fWeightMatrix0to1[9][3] = -7.84993011076136;
   fWeightMatrix0to1[10][3] = 0.889010273606253;
   fWeightMatrix0to1[11][3] = 22.2719821992379;
   fWeightMatrix0to1[12][3] = 3.17881600386872;
   fWeightMatrix0to1[13][3] = -0.0470057735610003;
   fWeightMatrix0to1[14][3] = -4.74499761290379;
   fWeightMatrix0to1[15][3] = -7.65056381987299;
   fWeightMatrix0to1[0][4] = 1.49032331722656;
   fWeightMatrix0to1[1][4] = 0.268352021958121;
   fWeightMatrix0to1[2][4] = -0.0572944582790561;
   fWeightMatrix0to1[3][4] = 0.107498471563506;
   fWeightMatrix0to1[4][4] = 0.960305619888171;
   fWeightMatrix0to1[5][4] = -1.22089163548207;
   fWeightMatrix0to1[6][4] = -0.134917937154203;
   fWeightMatrix0to1[7][4] = 0.0391189387900255;
   fWeightMatrix0to1[8][4] = -0.609273173860336;
   fWeightMatrix0to1[9][4] = -0.517146416507249;
   fWeightMatrix0to1[10][4] = -0.819545299368246;
   fWeightMatrix0to1[11][4] = -0.138677137662426;
   fWeightMatrix0to1[12][4] = -1.07690795079521;
   fWeightMatrix0to1[13][4] = -3.6947961924071;
   fWeightMatrix0to1[14][4] = 1.28052521682342;
   fWeightMatrix0to1[15][4] = 0.716953684444617;
   fWeightMatrix0to1[0][5] = -1.71054921899014;
   fWeightMatrix0to1[1][5] = -0.223841580886121;
   fWeightMatrix0to1[2][5] = -0.738603936235009;
   fWeightMatrix0to1[3][5] = 0.248710279531894;
   fWeightMatrix0to1[4][5] = 1.2465256450903;
   fWeightMatrix0to1[5][5] = 1.62488727306594;
   fWeightMatrix0to1[6][5] = -0.162151439112302;
   fWeightMatrix0to1[7][5] = -6.6073548306424;
   fWeightMatrix0to1[8][5] = 0.633832139915441;
   fWeightMatrix0to1[9][5] = -2.5776153233552;
   fWeightMatrix0to1[10][5] = 0.307628068963733;
   fWeightMatrix0to1[11][5] = -4.95295739084634;
   fWeightMatrix0to1[12][5] = 0.0989491372012467;
   fWeightMatrix0to1[13][5] = -0.519356964680126;
   fWeightMatrix0to1[14][5] = -3.05770349478052;
   fWeightMatrix0to1[15][5] = 1.97391848381278;
   fWeightMatrix0to1[0][6] = 1.57257699114788;
   fWeightMatrix0to1[1][6] = 0.425043993396931;
   fWeightMatrix0to1[2][6] = 3.82381534485409;
   fWeightMatrix0to1[3][6] = 0.34587162341869;
   fWeightMatrix0to1[4][6] = -0.454560804170656;
   fWeightMatrix0to1[5][6] = 0.984768020692889;
   fWeightMatrix0to1[6][6] = 1.72117633518924;
   fWeightMatrix0to1[7][6] = 0.991022960956374;
   fWeightMatrix0to1[8][6] = 1.63639731661975;
   fWeightMatrix0to1[9][6] = 4.09732923616592;
   fWeightMatrix0to1[10][6] = -0.639230721700234;
   fWeightMatrix0to1[11][6] = -6.15230983461258;
   fWeightMatrix0to1[12][6] = 3.44997127294859;
   fWeightMatrix0to1[13][6] = -0.100854566240587;
   fWeightMatrix0to1[14][6] = 0.24505101425893;
   fWeightMatrix0to1[15][6] = 2.57696383537372;
   fWeightMatrix0to1[0][7] = -2.63797458884051;
   fWeightMatrix0to1[1][7] = -0.120209165327659;
   fWeightMatrix0to1[2][7] = -7.96356569653816;
   fWeightMatrix0to1[3][7] = -0.369767739227737;
   fWeightMatrix0to1[4][7] = 1.92084621858602;
   fWeightMatrix0to1[5][7] = -1.90896971856449;
   fWeightMatrix0to1[6][7] = -8.74076875598813;
   fWeightMatrix0to1[7][7] = -3.29237328797919;
   fWeightMatrix0to1[8][7] = -0.316005808761904;
   fWeightMatrix0to1[9][7] = -6.9465456340988;
   fWeightMatrix0to1[10][7] = 1.20595465763518;
   fWeightMatrix0to1[11][7] = 10.2567503279381;
   fWeightMatrix0to1[12][7] = 1.83554488136672;
   fWeightMatrix0to1[13][7] = -0.516097819697641;
   fWeightMatrix0to1[14][7] = -0.565805734699137;
   fWeightMatrix0to1[15][7] = -3.74396862599064;
   fWeightMatrix0to1[0][8] = 0.240710152842875;
   fWeightMatrix0to1[1][8] = -7.69495469975847;
   fWeightMatrix0to1[2][8] = -0.00204909079500627;
   fWeightMatrix0to1[3][8] = 7.99740325395691;
   fWeightMatrix0to1[4][8] = 0.983881658756474;
   fWeightMatrix0to1[5][8] = -2.01524950525847;
   fWeightMatrix0to1[6][8] = 0.172459943441461;
   fWeightMatrix0to1[7][8] = -0.044626222164295;
   fWeightMatrix0to1[8][8] = 4.06677526814806;
   fWeightMatrix0to1[9][8] = 0.0714274029308273;
   fWeightMatrix0to1[10][8] = 8.54535595298826;
   fWeightMatrix0to1[11][8] = 0.0632553134188451;
   fWeightMatrix0to1[12][8] = -0.0457398099901173;
   fWeightMatrix0to1[13][8] = 0.591745763250956;
   fWeightMatrix0to1[14][8] = 0.662875652769244;
   fWeightMatrix0to1[15][8] = -0.0194917971315217;
   fWeightMatrix0to1[0][9] = -6.76890984736248;
   fWeightMatrix0to1[1][9] = -7.74834057357453;
   fWeightMatrix0to1[2][9] = -2.2842545581443;
   fWeightMatrix0to1[3][9] = -7.10315493128592;
   fWeightMatrix0to1[4][9] = -0.930709110357693;
   fWeightMatrix0to1[5][9] = 0.259944976451661;
   fWeightMatrix0to1[6][9] = -5.04241484386281;
   fWeightMatrix0to1[7][9] = -9.42384329012765;
   fWeightMatrix0to1[8][9] = -3.88144296230364;
   fWeightMatrix0to1[9][9] = 0.746577557960971;
   fWeightMatrix0to1[10][9] = 0.496516420386681;
   fWeightMatrix0to1[11][9] = -6.58453907565042;
   fWeightMatrix0to1[12][9] = -1.91330227341966;
   fWeightMatrix0to1[13][9] = 0.997317773872567;
   fWeightMatrix0to1[14][9] = -0.834701142180007;
   fWeightMatrix0to1[15][9] = 2.01254454436325;
   // weight matrix from layer 1 to 2
   fWeightMatrix1to2[0][0] = 0.608291907509869;
   fWeightMatrix1to2[1][0] = -1.21126791483877;
   fWeightMatrix1to2[2][0] = -2.86016496915348;
   fWeightMatrix1to2[3][0] = 0.150587720796466;
   fWeightMatrix1to2[4][0] = 0.394083641143299;
   fWeightMatrix1to2[5][0] = 2.01080070844073;
   fWeightMatrix1to2[6][0] = -0.225216430086328;
   fWeightMatrix1to2[7][0] = -1.10051060267968;
   fWeightMatrix1to2[8][0] = 0.481410406796984;
   fWeightMatrix1to2[9][0] = -1.70509324877528;
   fWeightMatrix1to2[0][1] = 0.367627952588688;
   fWeightMatrix1to2[1][1] = -1.41416057809781;
   fWeightMatrix1to2[2][1] = -5.1430995523128;
   fWeightMatrix1to2[3][1] = -1.74451950961257;
   fWeightMatrix1to2[4][1] = -1.74339831805283;
   fWeightMatrix1to2[5][1] = -2.38467265812727;
   fWeightMatrix1to2[6][1] = 0.255320833736748;
   fWeightMatrix1to2[7][1] = -0.769551679574282;
   fWeightMatrix1to2[8][1] = -0.980712505057454;
   fWeightMatrix1to2[9][1] = 1.33001816725041;
   fWeightMatrix1to2[0][2] = 1.4764698732849;
   fWeightMatrix1to2[1][2] = -0.41537545466765;
   fWeightMatrix1to2[2][2] = -2.60576989260131;
   fWeightMatrix1to2[3][2] = -1.85361844433006;
   fWeightMatrix1to2[4][2] = -2.0774616574003;
   fWeightMatrix1to2[5][2] = 3.79984651289146;
   fWeightMatrix1to2[6][2] = -0.420098441539866;
   fWeightMatrix1to2[7][2] = 0.771457706839749;
   fWeightMatrix1to2[8][2] = -0.315277625726138;
   fWeightMatrix1to2[9][2] = -0.136923168086444;
   fWeightMatrix1to2[0][3] = 1.83122239057744;
   fWeightMatrix1to2[1][3] = -1.86921795058658;
   fWeightMatrix1to2[2][3] = -4.98669754308913;
   fWeightMatrix1to2[3][3] = 0.617335246092157;
   fWeightMatrix1to2[4][3] = -2.47464833867775;
   fWeightMatrix1to2[5][3] = 3.49049906238942;
   fWeightMatrix1to2[6][3] = -1.06323684109002;
   fWeightMatrix1to2[7][3] = -0.519753824298776;
   fWeightMatrix1to2[8][3] = 0.447913004826687;
   fWeightMatrix1to2[9][3] = 0.12649413349606;
   fWeightMatrix1to2[0][4] = -1.29005305764443;
   fWeightMatrix1to2[1][4] = 0.463940470060947;
   fWeightMatrix1to2[2][4] = 1.16530023826602;
   fWeightMatrix1to2[3][4] = 0.206442279431233;
   fWeightMatrix1to2[4][4] = 1.60918680359698;
   fWeightMatrix1to2[5][4] = -1.41955111399674;
   fWeightMatrix1to2[6][4] = 0.960374510772816;
   fWeightMatrix1to2[7][4] = 1.82365215514845;
   fWeightMatrix1to2[8][4] = 1.89328122740375;
   fWeightMatrix1to2[9][4] = -1.43394339743773;
   fWeightMatrix1to2[0][5] = -0.596645484331418;
   fWeightMatrix1to2[1][5] = 0.896545229931223;
   fWeightMatrix1to2[2][5] = 1.48052571726497;
   fWeightMatrix1to2[3][5] = -1.82115854472981;
   fWeightMatrix1to2[4][5] = -0.159416336646707;
   fWeightMatrix1to2[5][5] = -2.58026286396901;
   fWeightMatrix1to2[6][5] = 0.651502004413374;
   fWeightMatrix1to2[7][5] = 0.563731701583008;
   fWeightMatrix1to2[8][5] = -0.097313329577446;
   fWeightMatrix1to2[9][5] = 1.04410614002685;
   fWeightMatrix1to2[0][6] = -0.483945630584947;
   fWeightMatrix1to2[1][6] = -0.851283797437559;
   fWeightMatrix1to2[2][6] = -6.87748464970784;
   fWeightMatrix1to2[3][6] = 1.59946368049346;
   fWeightMatrix1to2[4][6] = 0.277955486903021;
   fWeightMatrix1to2[5][6] = 3.04319128713923;
   fWeightMatrix1to2[6][6] = -0.508056501864801;
   fWeightMatrix1to2[7][6] = -0.51489331437068;
   fWeightMatrix1to2[8][6] = -1.16297249495983;
   fWeightMatrix1to2[9][6] = -0.819653121649328;
   fWeightMatrix1to2[0][7] = -0.61386630033161;
   fWeightMatrix1to2[1][7] = -0.600773931881244;
   fWeightMatrix1to2[2][7] = -3.59916309305658;
   fWeightMatrix1to2[3][7] = -1.69514904078096;
   fWeightMatrix1to2[4][7] = -1.08481506612658;
   fWeightMatrix1to2[5][7] = -0.65578861917522;
   fWeightMatrix1to2[6][7] = 0.177791441200882;
   fWeightMatrix1to2[7][7] = -3.18829105152237;
   fWeightMatrix1to2[8][7] = -0.80112804314169;
   fWeightMatrix1to2[9][7] = -0.18331412873227;
   fWeightMatrix1to2[0][8] = -0.333368827081462;
   fWeightMatrix1to2[1][8] = -1.06420273435035;
   fWeightMatrix1to2[2][8] = -0.660545360577387;
   fWeightMatrix1to2[3][8] = 0.196634321497688;
   fWeightMatrix1to2[4][8] = -0.48957027821341;
   fWeightMatrix1to2[5][8] = 1.35324537014133;
   fWeightMatrix1to2[6][8] = -1.18913610000558;
   fWeightMatrix1to2[7][8] = 0.71502572902919;
   fWeightMatrix1to2[8][8] = 0.631808718200665;
   fWeightMatrix1to2[9][8] = -0.226584348946199;
   fWeightMatrix1to2[0][9] = -0.0733305239628279;
   fWeightMatrix1to2[1][9] = -1.24774344082799;
   fWeightMatrix1to2[2][9] = -0.4433056952281;
   fWeightMatrix1to2[3][9] = -1.70884209361273;
   fWeightMatrix1to2[4][9] = -2.60021663597513;
   fWeightMatrix1to2[5][9] = -1.30799169150411;
   fWeightMatrix1to2[6][9] = -2.41731561048632;
   fWeightMatrix1to2[7][9] = -1.2307395229519;
   fWeightMatrix1to2[8][9] = -0.987883806112236;
   fWeightMatrix1to2[9][9] = -2.24097834633684;
   fWeightMatrix1to2[0][10] = 0.164418455464218;
   fWeightMatrix1to2[1][10] = -0.453240232399855;
   fWeightMatrix1to2[2][10] = 0.540138264313415;
   fWeightMatrix1to2[3][10] = -0.549593924406241;
   fWeightMatrix1to2[4][10] = 0.407457620069982;
   fWeightMatrix1to2[5][10] = -0.54298212908047;
   fWeightMatrix1to2[6][10] = -0.77795588746292;
   fWeightMatrix1to2[7][10] = -5.87057501595778;
   fWeightMatrix1to2[8][10] = -0.694372106910885;
   fWeightMatrix1to2[9][10] = 0.415054550367752;
   fWeightMatrix1to2[0][11] = 3.21259905930109;
   fWeightMatrix1to2[1][11] = -0.532923090088685;
   fWeightMatrix1to2[2][11] = -4.16762404220634;
   fWeightMatrix1to2[3][11] = -0.948642726082605;
   fWeightMatrix1to2[4][11] = -0.0405327741981141;
   fWeightMatrix1to2[5][11] = 1.46236328640123;
   fWeightMatrix1to2[6][11] = 0.0462432564042095;
   fWeightMatrix1to2[7][11] = -1.25814286750971;
   fWeightMatrix1to2[8][11] = -1.41476548056567;
   fWeightMatrix1to2[9][11] = -1.12364727932834;
   fWeightMatrix1to2[0][12] = 2.73258700963906;
   fWeightMatrix1to2[1][12] = -0.621411890419558;
   fWeightMatrix1to2[2][12] = -1.97132530801345;
   fWeightMatrix1to2[3][12] = -0.241225419566035;
   fWeightMatrix1to2[4][12] = 1.75511887767327;
   fWeightMatrix1to2[5][12] = -1.96807681640864;
   fWeightMatrix1to2[6][12] = -1.15894913238375;
   fWeightMatrix1to2[7][12] = -0.751773440019891;
   fWeightMatrix1to2[8][12] = -0.77171925076071;
   fWeightMatrix1to2[9][12] = -1.90114637765475;
   fWeightMatrix1to2[0][13] = -0.448825415789959;
   fWeightMatrix1to2[1][13] = -0.869096598200462;
   fWeightMatrix1to2[2][13] = 1.13907417904331;
   fWeightMatrix1to2[3][13] = -0.532755209888969;
   fWeightMatrix1to2[4][13] = -0.0174417129927479;
   fWeightMatrix1to2[5][13] = -1.49737383009074;
   fWeightMatrix1to2[6][13] = -0.620010136682667;
   fWeightMatrix1to2[7][13] = 1.56978944158845;
   fWeightMatrix1to2[8][13] = -0.420449847744064;
   fWeightMatrix1to2[9][13] = -1.9330727539692;
   fWeightMatrix1to2[0][14] = -0.401397474144004;
   fWeightMatrix1to2[1][14] = 0.198803949860399;
   fWeightMatrix1to2[2][14] = -1.2646722604964;
   fWeightMatrix1to2[3][14] = -1.44923425782801;
   fWeightMatrix1to2[4][14] = -2.29915935084215;
   fWeightMatrix1to2[5][14] = -1.20218257003042;
   fWeightMatrix1to2[6][14] = 0.576940518042301;
   fWeightMatrix1to2[7][14] = -1.17067833400822;
   fWeightMatrix1to2[8][14] = 0.403826872243441;
   fWeightMatrix1to2[9][14] = -3.14686906594009;
   fWeightMatrix1to2[0][15] = -1.44097336877526;
   fWeightMatrix1to2[1][15] = -0.623714029193382;
   fWeightMatrix1to2[2][15] = -0.301256164735356;
   fWeightMatrix1to2[3][15] = -0.2660656191017;
   fWeightMatrix1to2[4][15] = -4.04488345657167;
   fWeightMatrix1to2[5][15] = -0.490034228971563;
   fWeightMatrix1to2[6][15] = -2.4704199875823;
   fWeightMatrix1to2[7][15] = -0.958584672175577;
   fWeightMatrix1to2[8][15] = -1.77799902383979;
   fWeightMatrix1to2[9][15] = 0.651883562752213;
   fWeightMatrix1to2[0][16] = -2.99819985098815;
   fWeightMatrix1to2[1][16] = 0.266312429092087;
   fWeightMatrix1to2[2][16] = 4.89533944284551;
   fWeightMatrix1to2[3][16] = -0.579385762867672;
   fWeightMatrix1to2[4][16] = 3.24732880155118;
   fWeightMatrix1to2[5][16] = -1.71851040411268;
   fWeightMatrix1to2[6][16] = -0.772231828025116;
   fWeightMatrix1to2[7][16] = -1.02874132489634;
   fWeightMatrix1to2[8][16] = -2.05076867336008;
   fWeightMatrix1to2[9][16] = -1.61917550046758;
   // weight matrix from layer 2 to 3
   fWeightMatrix2to3[0][0] = -1.21996002860025;
   fWeightMatrix2to3[0][1] = 1.20614500530673;
   fWeightMatrix2to3[0][2] = -1.14595639320647;
   fWeightMatrix2to3[0][3] = 0.0286838934294441;
   fWeightMatrix2to3[0][4] = 1.54997409937004;
   fWeightMatrix2to3[0][5] = 0.654253821149748;
   fWeightMatrix2to3[0][6] = 1.32576375035078;
   fWeightMatrix2to3[0][7] = 1.56826278472495;
   fWeightMatrix2to3[0][8] = 0.935307358051837;
   fWeightMatrix2to3[0][9] = -1.92116561866315;
   fWeightMatrix2to3[0][10] = 1.87253260865509;
}

inline double Read4XPlanes::GetMvaValue__( const std::vector<double>& inputValues ) const
{
   if (inputValues.size() != (unsigned int)fLayerSize[0]-1) {
      std::cout << "Input vector needs to be of size " << fLayerSize[0]-1 << std::endl;
      return 0;
   }

   for (int l=0; l<fLayers; l++)
      for (int i=0; i<fLayerSize[l]; i++) fWeights[l][i]=0;

   for (int l=0; l<fLayers-1; l++)
      fWeights[l][fLayerSize[l]-1]=1;

   for (int i=0; i<fLayerSize[0]-1; i++)
      fWeights[0][i]=inputValues[i];

   // layer 0 to 1
   for (int o=0; o<fLayerSize[1]-1; o++) {
      for (int i=0; i<fLayerSize[0]; i++) {
         double inputVal = fWeightMatrix0to1[o][i] * fWeights[0][i];
         fWeights[1][o] += inputVal;
      }
      fWeights[1][o] = ActivationFnc(fWeights[1][o]);
   }
   // layer 1 to 2
   for (int o=0; o<fLayerSize[2]-1; o++) {
      for (int i=0; i<fLayerSize[1]; i++) {
         double inputVal = fWeightMatrix1to2[o][i] * fWeights[1][i];
         fWeights[2][o] += inputVal;
      }
      fWeights[2][o] = ActivationFnc(fWeights[2][o]);
   }
   // layer 2 to 3
   for (int o=0; o<fLayerSize[3]; o++) {
      for (int i=0; i<fLayerSize[2]; i++) {
         double inputVal = fWeightMatrix2to3[o][i] * fWeights[2][i];
         fWeights[3][o] += inputVal;
      }
      fWeights[3][o] = OutputActivationFnc(fWeights[3][o]);
   }

   return fWeights[3][0];
}

double Read4XPlanes::ActivationFnc(double x) const {
   // rectified linear unit
   return x>0 ? x : 0; 
}
double Read4XPlanes::OutputActivationFnc(double x) const {
   // sigmoid
   return 1.0/(1.0+exp(-x));
}
   
// Clean up
inline void Read4XPlanes::Clear()
{
   // clean up the arrays
   for (int lIdx = 0; lIdx < 4; lIdx++) {
      delete[] fWeights[lIdx];
   }
}
   inline double Read4XPlanes::GetMvaValue( std::vector<double>& inputValues ) const
   {
      // classifier response value
      double retval = 0;

      // classifier response, sanity check first
      if (!IsStatusClean()) {
         std::cout << "Problem in class \"" << fClassName << "\": cannot return classifier response"
                   << " because status is dirty" << std::endl;
         retval = 0;
      }
      else {
         if (IsNormalised()) {
            // normalise variables
            std::vector<double> iV;
            iV.reserve(inputValues.size());
            int ivar = 0;
            for (auto varIt = inputValues.begin();
                 varIt != inputValues.end(); varIt++, ivar++) {
               iV.push_back(NormVariable( *varIt, fVmin[ivar], fVmax[ivar] ));
            }
            Transform( iV, -1 );
            retval = GetMvaValue__( iV );
         }
         else {            
            Transform( inputValues, -1 );
            retval = GetMvaValue__( inputValues );
         }
      }

      return retval;
   }

//_______________________________________________________________________
inline void Read4XPlanes::InitTransform_1()
{
   // Normalization transformation, initialisation
   fMin_1[0][0] = 0.00867361109704;
   fMax_1[0][0] = 8.86680030823;
   fMin_1[1][0] = 0.0429715067148;
   fMax_1[1][0] = 10.046962738;
   fMin_1[2][0] = 0.00867361109704;
   fMax_1[2][0] = 10.046962738;
   fMin_1[0][1] = 0.0104144280776;
   fMax_1[0][1] = 20.7421417236;
   fMin_1[1][1] = 0.0688964873552;
   fMax_1[1][1] = 27.76912117;
   fMin_1[2][1] = 0.0104144280776;
   fMax_1[2][1] = 27.76912117;
   fMin_1[0][2] = 0.0260208323598;
   fMax_1[0][2] = 72.587852478;
   fMin_1[1][2] = 0.171886026859;
   fMax_1[1][2] = 92.4787521362;
   fMin_1[2][2] = 0.0260208323598;
   fMax_1[2][2] = 92.4787521362;
   fMin_1[0][3] = -5293.61669922;
   fMax_1[0][3] = 5453.54833984;
   fMin_1[1][3] = -5616.24755859;
   fMax_1[1][3] = 5762.14111328;
   fMin_1[2][3] = -5616.24755859;
   fMax_1[2][3] = 5762.14111328;
   fMin_1[0][4] = 4;
   fMax_1[0][4] = 20;
   fMin_1[1][4] = 4;
   fMax_1[1][4] = 20;
   fMin_1[2][4] = 4;
   fMax_1[2][4] = 20;
   fMin_1[0][5] = 0;
   fMax_1[0][5] = 630.205566406;
   fMin_1[1][5] = 0.196933358908;
   fMax_1[1][5] = 875.192321777;
   fMin_1[2][5] = 0;
   fMax_1[2][5] = 875.192321777;
   fMin_1[0][6] = -2842;
   fMax_1[0][6] = 2839.98754883;
   fMin_1[1][6] = -2720.55004883;
   fMax_1[1][6] = 2838.9375;
   fMin_1[2][6] = -2842;
   fMax_1[2][6] = 2839.98754883;
   fMin_1[0][7] = -0.409659594297;
   fMax_1[0][7] = 0.435859173536;
   fMin_1[1][7] = -0.436186283827;
   fMax_1[1][7] = 0.417666554451;
   fMin_1[2][7] = -0.436186283827;
   fMax_1[2][7] = 0.435859173536;
   fMin_1[0][8] = -0.283686846495;
   fMax_1[0][8] = 0.283448517323;
   fMin_1[1][8] = -0.280590295792;
   fMax_1[1][8] = 0.281541198492;
   fMin_1[2][8] = -0.283686846495;
   fMax_1[2][8] = 0.283448517323;
}

//_______________________________________________________________________
inline void Read4XPlanes::Transform_1( std::vector<double>& iv, int cls) const
{
   // Normalization transformation
   if (cls < 0 || cls > 2) {
     cls = 2;
   }

   for (int ivar=0;ivar<9;ivar++) {
      double offset = fMin_1[cls][ivar];
      double scale  = 1.0/(fMax_1[cls][ivar]-fMin_1[cls][ivar]);
      iv[ivar] = (iv[ivar]-offset)*scale * 2 - 1;
   }
}

//_______________________________________________________________________
inline void Read4XPlanes::InitTransform()
{
   InitTransform_1();
}

//_______________________________________________________________________
inline void Read4XPlanes::Transform( std::vector<double>& iv, int sigOrBgd ) const
{
   Transform_1( iv, sigOrBgd );
}
