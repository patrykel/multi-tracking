
/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/


#include "Demo/DemoAnalyzer/interface/DemoAnalyzer.h"
#include "Demo/DemoAnalyzer/interface/RecoToCSVAnalyzer.h"


RecoToCSVAnalyzer::RecoToCSVAnalyzer(const edm::ParameterSet& ps)
{
  std::string fillFileName = ps.getParameter<std::string>("fill_name");
  eventsCSV.open(fillFileName + ".csv");
  eventsCSV << "EventId,RPId,SiliconId,position,u/v,line_no\n";  
}



RecoToCSVAnalyzer::~RecoToCSVAnalyzer(){
  eventsCSV.close();
}


void 
RecoToCSVAnalyzer::hits_log(std::vector<RPRecoHit> &hits, int eventId, unsigned int line_no, std::string &prefix){
  unsigned int totalHitNumber = hits.size();
  RPRecoHit curr_hit;

  for(unsigned int hit_no = 0; hit_no < totalHitNumber; hit_no++){
    curr_hit = hits[hit_no];
    unsigned int rawDetId = curr_hit.DetId();

    eventsCSV  << eventId << "," 
                  << armId(rawDetId) << stationId(rawDetId) << rpId(rawDetId) << ","
                  << armId(rawDetId) << stationId(rawDetId) << rpId(rawDetId) << siliconId(rawDetId) << ","
                  << curr_hit.Position() << ","
                  << prefix << ","
                  << line_no << "\n"; 
  }
}

void 
RecoToCSVAnalyzer::lines_log(std::vector<RPRecognizedPatterns::Line> &lines, std::string &prefix, int eventId){
  unsigned int totalLineNumber = lines.size();
  RPRecognizedPatterns::Line curr_line;

  for(unsigned int line_no = 0; line_no < totalLineNumber; line_no++){
    curr_line = lines[line_no];
    hits_log(curr_line.hits, eventId, line_no, prefix);
  }
}

// ------------ method called for each event  ------------
void
RecoToCSVAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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
    
    if(uLinesNumber == 3 && vLinesNumber == 3){     // CONDITION (JUST TO MAKE IT REASONABLE SIZE)
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
RecoToCSVAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
RecoToCSVAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
RecoToCSVAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
RecoToCSVAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
RecoToCSVAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
RecoToCSVAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RecoToCSVAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RecoToCSVAnalyzer);