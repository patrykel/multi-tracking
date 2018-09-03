
/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/


#include "Demo/DemoAnalyzer/interface/DemoAnalyzer.h"
#include "Demo/DemoAnalyzer/interface/RecoMultiToCSVAnalyzer.h"


RecoMultiToCSVAnalyzer::RecoMultiToCSVAnalyzer(const edm::ParameterSet& ps)
{
  std::string fillFileName = ps.getParameter<std::string>("fill_name");
  recoId = ps.getParameter<std::string>("recoId");

  hitsCSV.open(fillFileName + ".csv");
  hitsCSV << "recoID,eventID,armID,groupID,rpID,uLineNo,vLineNo,siliconID,direction,line_no,position\n";  
}

RecoMultiToCSVAnalyzer::~RecoMultiToCSVAnalyzer(){
  hitsCSV.close();
}

void 
RecoMultiToCSVAnalyzer::hits_log(std::vector<RPRecoHit> &hits, int eventId, unsigned int line_no, std::string &direction, unsigned int uLinesNumber,  unsigned int vLinesNumber){
  unsigned int totalHitNumber = hits.size();
  RPRecoHit curr_hit;

  for(unsigned int hit_no = 0; hit_no < totalHitNumber; hit_no++){
    curr_hit = hits[hit_no];
    unsigned int rawDetId = curr_hit.DetId();

    unsigned int armID = armId(rawDetId);
    unsigned int groupID = getGroupId(rawDetId);
    unsigned int rpID = rpId(rawDetId);
    unsigned int siliconID = siliconId(rawDetId);

    hitsCSV << recoId << "," 
              << eventId << ","
              << armID << ","
              << groupID << ","
              << rpID << ","
              << uLinesNumber << ","
              << vLinesNumber << ","
              << siliconID << ","
              << direction << ","
              << line_no << ","
              << curr_hit.Position() << "\n";
  }
}

void 
RecoMultiToCSVAnalyzer::lines_log(std::vector<RPRecognizedPatterns::Line> &lines, std::string &direction, int eventId, 
  unsigned int uLinesNumber,  unsigned int vLinesNumber){
  unsigned int totalLineNumber = lines.size();
  RPRecognizedPatterns::Line curr_line;

  for(unsigned int line_no = 0; line_no < totalLineNumber; line_no++){
    curr_line = lines[line_no];
    hits_log(curr_line.hits, eventId, line_no, direction, uLinesNumber, vLinesNumber);
  }
}

// ------------ method called for each event  ------------
void
RecoMultiToCSVAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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
    
    if( ( 1 < vLinesNumber && vLinesNumber < 5) && 
        ( 1 < uLinesNumber && uLinesNumber < 5) ) {     // CONDITION (JUST TO MAKE IT REASONABLE SIZE)
      // lines to CSV
      string direction = "u";
      lines_log(it.second.uLines, direction, eventId, uLinesNumber, vLinesNumber);
      direction = "v";
      lines_log(it.second.vLines, direction, eventId, uLinesNumber, vLinesNumber);  
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


/* detector_map = {
    1 : "L-TOP", 24, 20, 4, 0
    2 : "L-BOT", 23, 22, 3, 2
    3 : "L-HOR", 25, 21, 5, 1
    
    4 : "R-TOP", 100, 104, 120, 124
    5 : "R-BOT", 102, 103, 122, 123
    6 : "R-HOR"  101, 105, 121, 125
} */

unsigned int
getGroupId(unsigned int rawDetId) {

  unsigned int rp_id = pureRpId(rawDetId);
  unsigned int result = 0;

  if (rp_id % 10 == 0 || rp_id % 10 == 4) {
    result += 1;
  } else if (rp_id % 10 == 2 || rp_id % 10 == 3) {
    result += 2;
  } else {
    result += 3;
  }

  if (rp_id/100 == 1) {
    result += 3;
  }

  return result;
}


// ------------ method called once each job just before starting event loop  ------------

void 
RecoMultiToCSVAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
RecoMultiToCSVAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
RecoMultiToCSVAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
RecoMultiToCSVAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
RecoMultiToCSVAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
RecoMultiToCSVAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RecoMultiToCSVAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RecoMultiToCSVAnalyzer);