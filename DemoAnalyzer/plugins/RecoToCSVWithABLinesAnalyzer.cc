
/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/


#include "Demo/DemoAnalyzer/interface/DemoAnalyzer.h"
#include "Demo/DemoAnalyzer/interface/RecoToCSVWithABLinesAnalyzer.h"


RecoToCSVWithABLinesAnalyzer::RecoToCSVWithABLinesAnalyzer(const edm::ParameterSet& ps)
{
  std::string fillFileName = ps.getParameter<std::string>("fill_name");
  
  row_counter = 0;
  recoId = ps.getParameter<std::string>("recoId");
  linesCSV.open(fillFileName + ".csv");
  linesCSV << "recoId,eventId,rpId,direction,a,b\n";  
}

RecoToCSVWithABLinesAnalyzer::~RecoToCSVWithABLinesAnalyzer(){
  linesCSV.close();
}

unsigned int 
RecoToCSVWithABLinesAnalyzer::rpIdOfLine(RPRecognizedPatterns::Line &line){
  RPRecoHit first_hit = line.hits[0];
  unsigned int rawDetId = first_hit.DetId();
  return pureRpId(rawDetId);
}

void 
RecoToCSVWithABLinesAnalyzer::lines_log(std::vector<RPRecognizedPatterns::Line> &lines, std::string &prefix, int eventId){
  unsigned int totalLineNumber = lines.size();
  RPRecognizedPatterns::Line curr_line;

  for(unsigned int line_no = 0; line_no < totalLineNumber; line_no++){
    curr_line = lines[line_no];
    
    linesCSV << recoId << "," 
              << eventId << ","
              << rpIdOfLine(curr_line) << ","
              << prefix << ","
              << curr_line.a << ","
              << curr_line.b << "\n";
  }
}

// ------------ method called for each event  ------------
void
RecoToCSVWithABLinesAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  Handle<RPRecognizedPatternsCollection> input;
  iEvent.getByLabel("NonParallelTrackFinder", input);

  int eventId = iEvent.id().event();

  for (auto it : *input)
  {
    unsigned int uLinesNumber = it.second.uLines.size();
    unsigned int vLinesNumber = it.second.vLines.size();
    
    if(uLinesNumber == 1 && vLinesNumber == 1){     // CONDITION ONLY SINGLE TRACKING
      // lines to CSV
      string prefix = "u";
      lines_log(it.second.uLines, prefix, eventId);
      prefix = "v";
      lines_log(it.second.vLines, prefix, eventId);  
    }
    
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
RecoToCSVWithABLinesAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
RecoToCSVWithABLinesAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
RecoToCSVWithABLinesAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
RecoToCSVWithABLinesAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
RecoToCSVWithABLinesAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
RecoToCSVWithABLinesAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RecoToCSVWithABLinesAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RecoToCSVWithABLinesAnalyzer);