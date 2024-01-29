
//C++
#include <vector>
#include <iostream>

//StRoot
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoEvent.h"

//local classes
#include "StUPCTrack.h"
#include "StUPCSelectV0.h"
#include "StUPCV0.h"

using namespace std;

//_____________________________________________________________________________
int StUPCSelectV0::selectTracks(vector<StUPCTrack>& tracks, std::vector<UChar_t>& sel, StPicoDst *pd) {

  //cout << "StUPCSelectV0::selectTracks" << endl;
  //cout << "field: " << pd->event()->bField() << endl;

  //run over global tracks and store all global tracks of interest.
  double massPion = 0.13957061;
  //double massKaon =  0.497611;
  double massProton = 0.93827;  

  TVector3 vertex(0,0,0);
  double beamline[4] = {0, 0, 0, 0}; 
  // beamline[0] = mUPCEvent->getBeamXPosition();
  // beamline[1] = mUPCEvent->getBeamXSlope();
  // beamline[2] = mUPCEvent->getBeamYPosition();
  // beamline[3] = mUPCEvent->getBeamYSlope();
  // add beamline as parameter or read it directly from mUPCEvent?
  // what about vertex?

  int nfound = 0;

  //outer tracks loop
  for(size_t itrk=0; itrk<tracks.size(); itrk++) {

    const StUPCTrack& track1 = tracks[itrk];

    if( track1.getNhits() < 15 ) continue;
    if( track1.getPt() < 0.15 ) continue;
    if( track1.getEta() > 1.1 ) continue;

    Bool_t matchTof1 =  track1.getFlag( StUPCTrack::kTof );

    //inner tracks loop
    for(size_t jtrk=itrk+1; jtrk<tracks.size(); jtrk++) {

      const StUPCTrack& track2 = tracks[jtrk];

      if( track2.getPt() < 0.15 ) continue;
      if( track2.getEta() > 1.1 ) continue;

      Bool_t matchTof2 =  track2.getFlag( StUPCTrack::kTof );

      if ( matchTof1 == kFALSE && matchTof2 == kFALSE ) 
        continue;
      if ( track1.getCharge()+track2.getCharge() != 0 ) 
        continue;
      if ( track2.getNhits()<15 ) 
        continue;

      //cout << "pair" << endl;

      StUPCV0 K0L(&track1,&track2, massPion, massPion, itrk, jtrk, vertex, beamline, pd->event()->bField(), true);

      if ( K0L.dcaDaughters() > 100. ) 
        continue;
      bool V0Candidate = false;
      StUPCV0 K0(&track1,&track2, massPion, massPion, itrk, jtrk, vertex, beamline, pd->event()->bField(), false);

      // the same cut for K0 and Lambda
      if ( !(K0.dcaDaughters() < 2.0 && K0.DCABeamLine() < 2.0 && (K0.pointingAngleHypo()>0.9 || K0.decayLengthHypo()<3.0) ) )
        continue;

      if ( abs(K0.m()-0.495) < 0.035 ) // it's a K0
        V0Candidate = true;

      // check if pair is Lambda/LambdaBar
      StUPCV0 L01(&track1,&track2, massPion, massProton, itrk, jtrk, vertex, beamline, pd->event()->bField(), false);
      if ( abs(L01.m()-1.115) < 0.015 ) // it's a Lambda/LambdaBar
        V0Candidate = true;

      // check if pair is LambdaBar/Lambda
      StUPCV0 L02(&track1,&track2, massProton, massPion, itrk, jtrk, vertex, beamline, pd->event()->bField(), false);
      if ( abs(L02.m()-1.115) < 0.015 ) // it's a LambdaBar/Lambda
        V0Candidate = true;

      if ( !V0Candidate ) // it is not a V0 canidate, discard the pair
        continue;

      //counter for found V0 candidates
      nfound++;

      //mark the tracks as used
      sel[itrk] |= (1 << StUPCTrack::kV0);
      sel[jtrk] |= (1 << StUPCTrack::kV0);
    }//inner tracks loop
  }//outer tracks loop

  return nfound;

}//selectTracks

