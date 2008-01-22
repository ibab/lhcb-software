 void makePlots()
 {
 
   char file[200];
 
   TChain* theChain1 = new TChain("chain1"); 
   sprintf(file,"monitoring_refit_tuple.root/Forward/HitResiduals/ALL/");
   theChain1->Add(file);

   
   TChain* theChain2 = new TChain("chain2");
        
   sprintf(file,"misaligned_tuple.root/Forward/HitResiduals/ALL/");
   theChain2->Add(file);

   // make sure header is up to date
   theChain1->MakeClass("HitResiduals");
   gInterpreter->LoadMacro("HitResiduals.C+");
 
   // load file with the loop
   gInterpreter->LoadMacro("PlotHitResiduals.C+");


   int res[5] = {0,-1,-1,-1,-1};
   //velo residuals => res[0] = 0;
   //res[1]:  Phi=0; R=1; All=-1;
   //res[2]:  sensor number; All=-1;

   //TT residuals   => res[0] = 1;
   //res[1]: station number; All=-1;
   //res[2]: layer number;   All=-1;
   //res[3]: region number;  All=-1;
   //res[4]: sector number;  All=-1;

   //OT residuals   => res[0] = 2;
   //res[1]: station number;  All=-1;
   //res[2]: layer number;    All=-1;
   //res[3]: quarter number;  All=-1;
   //res[4]: module number;   All=-1;			      

   //IT residuals   => res[0] = 3
   //res[1]: station number;  All=-1;
   //res[2]: layer number;    All=-1;
   //res[3]: box number;      All=-1;
   //res[4]: sector number;   All=-1;			      


   // loop on the data
   loop(theChain1, theChain2, res);
 }
 
