/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/


#include "Demo/DemoAnalyzer/interface/RecoToFlatRootAnalyzer.h"
#include "Demo/DemoAnalyzer/interface/DemoAnalyzer.h"             // just for function: unsigned int siliconId(unsigned int detId);

RecoToFlatRootAnalyzer::RecoToFlatRootAnalyzer(const edm::ParameterSet& ps)
{
  // CREATING FLAT ROOT FILE WITH FIELDS:
  T->SetDirectory(f);

  T->Branch("recoID", &recoID, "recoID/I");
  T->Branch("eventID", &eventID, "eventID/I");
  T->Branch("armID", &armID, "armID/I");
  T->Branch("groupID", &groupID, "groupID/I");
  T->Branch("rpID", &rpID, "rpID/I");
  T->Branch("uLineSize", &uLineSize, "uLineSize/I");
  T->Branch("vLineSize", &vLineSize, "vLineSize/I");
  T->Branch("siliconID", &siliconID, "siliconID/I");
  T->Branch("position", &position, "position/F");
  T->Branch("sigma_", &sigma_, "sigma_/F");

  T->Branch("uv_line_a", &uv_line_a, "uv_line_a/F");
  T->Branch("uv_line_b", &uv_line_b, "uv_line_b/F");
  T->Branch("uv_line_w", &uv_line_w, "uv_line_w/F");

}


RecoToFlatRootAnalyzer::~RecoToFlatRootAnalyzer(){
  T->Print();
  f->Write();
  delete f;
}

// ------------ method called for each event  ------------
void
RecoToFlatRootAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  exportToFlatRoot(iEvent);

#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}

void 
RecoToFlatRootAnalyzer::exportHitsFromLineToFFlatRoot(std::vector<RPRecognizedPatterns::Line> &lines){
  for(auto& line: lines){

    // SET U/V LINE VALUES - LOOK FOR RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatterns.h
    uv_line_a = line.a; 
    uv_line_b = line.b; 
    uv_line_w = line.w; 

    for(auto& hit: line.hits){
      unsigned int rawDetId = hit.DetId();
      
      siliconID = siliconId(rawDetId);
      position  = hit.Position();
      sigma_    = hit.Sigma();
      T->Fill();
    }
  }
}

void 
RecoToFlatRootAnalyzer::exportToFlatRoot(const edm::Event &iEvent){
  using namespace edm;
  using namespace std;
  Handle <RPRecognizedPatternsCollection> input;
  iEvent.getByLabel("NonParallelTrackFinder", input);

  for (auto it : *input) {
    unsigned int rp = it.first;
    unsigned int rpIdx = rp % 10;

    eventID = iEvent.id().event();
    armID = rp/100; 
    rpID = rp;

    if (rpIdx == 0 || rpIdx == 4)
      groupID = 1;
    if (rpIdx == 1 || rpIdx == 5)
      groupID = 2;
    if (rpIdx == 2 || rpIdx == 3)
      groupID = 3;
    if (armID == 1)
      groupID += 3;

    uLineSize = it.second.uLines.size();
    vLineSize = it.second.vLines.size();

    exportHitsFromLineToFFlatRoot(it.second.uLines);
    exportHitsFromLineToFFlatRoot(it.second.vLines);
  }
}


// ------------ method called once each job just before starting event loop  ------------
void 
RecoToFlatRootAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
RecoToFlatRootAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
RecoToFlatRootAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
RecoToFlatRootAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
RecoToFlatRootAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
RecoToFlatRootAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RecoToFlatRootAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RecoToFlatRootAnalyzer);