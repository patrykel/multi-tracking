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
  T->Branch("recoId", &recoId, "recoId/I");
  T->Branch("eventId", &eventId, "eventId/I");
  T->Branch("eventSize", &eventSize, "eventSize/I");
  T->Branch("rpID", &rpID, "rpID/I");
  T->Branch("arm", &arm, "arm/I");
  T->Branch("groupId", &groupId, "groupId/I");
  T->Branch("siliconID", &siliconID, "siliconID/I");
  T->Branch("position", &position, "position/F");
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
    for(auto& hit: line.hits){
      unsigned int rawDetId = hit.DetId();
      siliconID = siliconId(rawDetId);
      position = hit.Position();
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
    unsigned int group = 0;

    if (rpIdx == 0 || rpIdx == 4)
      group = 1;
    if (rpIdx == 1 || rpIdx == 5)
      group = 2;
    if (rpIdx == 2 || rpIdx == 3)
      group = 3;

    eventId = iEvent.id().event();
    eventSize = input->size();
    rpID = rp;
    arm  = rp/100;
    groupId = group;

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