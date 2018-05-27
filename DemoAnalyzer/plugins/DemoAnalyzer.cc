
/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/

#include "Demo/DemoAnalyzer/interface/DemoAnalyzer.h"

DemoAnalyzer::DemoAnalyzer(const edm::ParameterSet& ps){
}

DemoAnalyzer::~DemoAnalyzer(){
}

/*

FROM: DataFormats/TotemRPDetId/interface/TotRPDetId.h

1   - 15  - not used by me
16  - 19  - silicon id
20  - 22  - RP id
23  - 24  - station id
25        - arm id
26 - 31   - not used by me

*/

unsigned int armId(unsigned int rawDetId){
  return (rawDetId >> 24) & 0b0001;
}

unsigned int stationId(unsigned int rawDetId){
  return (rawDetId >> 22) & 0b0011;
}

unsigned int rpId(unsigned int rawDetId){
  return (rawDetId >> 19) & 0b0111;
}

unsigned int siliconId(unsigned int rawDetId){
  return (rawDetId >> 15) & 0b1111;
}

unsigned int pureRpId(unsigned int rawDetId){
  return armId(rawDetId) * 100 + stationId(rawDetId) * 10 + rpId(rawDetId);
}

unsigned int pureSiliconId(unsigned int rawDetId){
  return pureRpId(rawDetId) * 10 + siliconId(rawDetId);
}


// ------------ method called for each event  ------------
void
DemoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  Handle<RPRecognizedPatternsCollection> input;
  iEvent.getByLabel("NonParallelTrackFinder", input);

  for (auto it : *input)
  {
    unsigned int rp = it.first;
    unsigned int arm = rp / 100;
    unsigned char rpIdx = rp % 10;

    // U V lines number
    unsigned int uLinesNumber = it.second.uLines.size();
    unsigned int vLinesNumber = it.second.vLines.size();


    LogInfo("Demo") <<  "rp = " << rp << "\t"
                        "arm = " << arm << "\t"
                        "rpIdx = " << rpIdx << "\t"
                        "uLines = " << uLinesNumber << "\t"
                        "vLines = " << vLinesNumber;
  }

#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------

void 
DemoAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
DemoAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
DemoAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
DemoAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
DemoAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
DemoAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DemoAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DemoAnalyzer);