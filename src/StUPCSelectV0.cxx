
//C++
#include <vector>
#include <iostream>

//StRoot
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoEvent/StPicoEvent.h"

//local classes
#include "StUPCSelectV0.h"
#include "StUPCV0.h"

using namespace std;

//_____________________________________________________________________________
int StUPCSelectV0::selectTracks(StPicoDst *pd, std::vector<bool>& sel) {

  //cout << "StUPCSelectV0::selectTracks" << endl;

  //run over global tracks and store all global tracks of interest.
  double massPion = 0.13957061;
  //double massKaon =  0.497611;
  double massProton = 0.93827;  

  TVector3 vertex(0,0,0);
  double beamline[4] = {0, 0, 0, 0}; // have to be loaded from star database and ideally store in StUpcEvent in the end
  //std::vector<int> globalList;

  int nfound = 0;

  for(UInt_t itrk=0; itrk < pd->numberOfTracks(); itrk++) {
      StPicoTrack *track1 = pd->track(itrk); 
      if ( !track1 ) 
        continue;
      if ( track1->nHits() < 15 ) 
        continue;
      if ( track1->gPt() < 0.15 ) 
        continue;
      TVector3 track1mom = track1->gMom();
      if ( abs(track1mom.Eta()) > 1.1 ) 
        continue;

      //TOF
      Bool_t matchTof1 = track1->isTofTrack();

      for(UInt_t jtrk=itrk+1; jtrk<pd->numberOfTracks(); jtrk++)
        {
          StPicoTrack *track2 = pd->track(jtrk); 

          if ( !track2 ) 
            continue;
          if ( track2->gPt()<0.15 ) 
            continue; 
          TVector3 track2mom = track2->gMom();
          if ( abs(track2mom.Eta()) > 1.1 ) 
            continue;

          //TOF
          Bool_t matchTof2 = track2->isTofTrack();

          if ( matchTof1 == kFALSE && matchTof2 == kFALSE ) 
            continue;
          if ( track1->charge()+track2->charge() != 0 ) 
            continue;
          if ( track2->nHits()<15 ) 
            continue;
          
          // check if pair is K0 
          StUPCV0 K0L(track1,track2, massPion, massPion, vertex, beamline, pd->event()->bField(), true); 
          
          if ( K0L.dcaDaughters() > 100. ) 
            continue;
          bool V0Candidate = false;
          StUPCV0 K0(track1,track2, massPion, massPion, vertex, beamline, pd->event()->bField(), false);

          // the same cut for K0 and Lambda
          if ( !(K0.dcaDaughters() < 2.0 && K0.DCABeamLine() < 2.0 && (K0.pointingAngleHypo()>0.9 || K0.decayLengthHypo()<3.0) ) )
            continue;

          if ( abs(K0.m()-0.495) < 0.035 ) // it's a K0
            V0Candidate = true;

          // check if pair is Lambda/LambdaBar
          StUPCV0 L01(track1,track2, massPion, massProton, vertex, beamline, pd->event()->bField(), false);
          if ( abs(L01.m()-1.115) < 0.015 ) // it's a Lambda/LambdaBar
            V0Candidate = true;

          // check if pair is LambdaBar/Lambda
          StUPCV0 L02(track1,track2, massProton, massPion, vertex, beamline, pd->event()->bField(), false);
          if ( abs(L02.m()-1.115) < 0.015 ) // it's a LambdaBar/Lambda
            V0Candidate = true;

          if ( !V0Candidate ) // it is not a V0 canidate, discard the pair
            continue;

          //counter for found V0 candidates
          nfound++;

          //mark the tracks as used
          sel[itrk] = true;
          sel[jtrk] = true;

        }

  }

  return nfound;

}//selectTracks

