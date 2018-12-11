
//_____________________________________________________________________________
void RunFilterMaker(string filelist="txt/sim_slight14e1x1.list",
                    Int_t nFiles=999999,
                    string outfile="/gpfs01/star/pwg/jaroslav/star-upcDst-data/test_productions/mc/StUPC_slight14e1x1_v2.root") {

  //load libraries to work with muDst
  gROOT->Macro("loadMuDst.C");

  // Load St_db_Maker and co
  gSystem->Load("StDbLib.so");
  gSystem->Load("StDbBroker.so");
  gSystem->Load("St_db_Maker");

  // Load Emc libraries
  gSystem->Load("StDaqLib");
  gSystem->Load("StEmcRawMaker");
  gSystem->Load("StEmcADCtoEMaker");
  gSystem->Load("StPreEclMaker");
  gSystem->Load("StEpcMaker");

  //load the analysis maker compiled before with cons
  gSystem->Load("StUPCFilterMaker.so");

  //create chain directory-like structure for maker
  //top level
  StChain *chain = new StChain;
  //maker to access muDST data
  StMuDstMaker *maker = new StMuDstMaker(0, 0, "", filelist.c_str(), "", nFiles);

  //St_db_Maker for Emc calibration
  St_db_Maker *db1 = new St_db_Maker("db","$HOME/StarDb","MySQL:StarDb","$STAR/StarDb");
  // Maker to apply calibration
  StEmcADCtoEMaker *adc_to_e = new StEmcADCtoEMaker();
  adc_to_e->setPrint(kFALSE);
  // Makers for clusterfinding
  StPreEclMaker *pre_ecl = new StPreEclMaker();
  pre_ecl->setPrint(kFALSE);
  StEpcMaker *epc = new StEpcMaker();
  epc->setPrint(kFALSE);
  //analysis maker
  StUPCFilterMaker *anaMaker = new StUPCFilterMaker(maker, outfile); //maker for muDst passed to the constructor

  //----------------------------------------------------------------------
  //Configure the analysis maker
  //
  //data/MC
  anaMaker->setIsMC(0); // 0 - data,  1 - starsim MC,  2 - embedding MC
  //
  //set trigger IDs
  anaMaker->addTriggerId(450705, 15084051, 15167014); // UPCJpsiB, Run14 AuAu, 1st id
  anaMaker->addTriggerId(450725, 15153036, 15167007); // UPCJpsiB, Run14 AuAu, 2nd id
  anaMaker->addTriggerId(450701, 15078073, 15167014); // UPC-main, Run14 AuAu, 1st id
  anaMaker->addTriggerId(450711, 15153036, 15167007); // UPC-main, Run14 AuAu, 2nd id
  anaMaker->addTriggerId(260750, 11039042, 11077018); // UPC_main, run 10 AuAu, 1st id
  anaMaker->addTriggerId(270601, 11089036, 11098056); // upc_main, run 10 AuAu, 2nd id
  anaMaker->addTriggerId(260750, 12122025, 12122025); // UPC_main, run 11 AuAu, 1st id
  anaMaker->addTriggerId(350007, 12146003, 12152016); // UPC_main, run 11 AuAu, 2nd id
  anaMaker->addTriggerId(350017, 12153002, 12179051); // UPC_main, run 11 AuAu, 3rd id
  //
  // use BEMC cluster conditions below if set to true
  Bool_t useClusterParam = kFALSE;
  Int_t sizeMax = 4;
  Float_t energySeed = 0.4;
  Float_t energyAdd = 0.001;
  Float_t energyThresholdAll = 0.4;
  //----------------------------------------------------------------------

  //no debug printouts
  StMuDebug::setLevel(0);

  Int_t nevt = maker->chain()->GetEntries();
  //nevt=1;
  cout << "Number of events: " << nevt << endl;

  //initialize the makers
  chain->Init();

  //apply BEMC clustering parameters if requested
  if( useClusterParam ) {
    //call need to happen after the StPreEclMaker has been initialized
    pre_ecl->SetClusterConditions("bemc", sizeMax, energySeed, energyAdd, energyThresholdAll, kFALSE);
  }
  cout << "-------------------------------------------" << endl;
  cout << "StEmcOldFinder cluster parameters for BEMC:" << endl;
  StEmcOldFinder *finder = dynamic_cast<StEmcOldFinder*>(pre_ecl->finder());
  //bemc has index 1, according to StRoot/StEmcUtil/others/emcDetectorName.h and StPreEclMaker.cxx
  cout << "  sizeMax: " << finder->sizeMax(1) << endl;
  cout << "  energySeed: " << finder->energySeed(1) << endl;
  cout << "  energyAdd: " << finder->energyAdd(1) << endl;
  cout << "  energyThresholdAll: " << finder->energyThresholdAll(1) << endl;
  cout << "-------------------------------------------" << endl;

  //loop over events
  chain->EventLoop(nevt);
  chain->Finish();

  //release allocated memory
  delete chain;







}//RunFilterMaker

