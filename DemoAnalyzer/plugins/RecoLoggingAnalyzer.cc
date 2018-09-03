
/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/

#include "Demo/DemoAnalyzer/interface/DemoAnalyzer.h"
#include "Demo/DemoAnalyzer/interface/RecoLoggingAnalyzer.h"

RecoLoggingAnalyzer::RecoLoggingAnalyzer(const edm::ParameterSet& ps)
{
  geometryUtility = new GeometryUtility();
  geometryUtility -> exportGeometryToRoot();
}


RecoLoggingAnalyzer::~RecoLoggingAnalyzer(){
  delete geometryUtility;
}

void 
RecoLoggingAnalyzer::hits_log(std::vector<RPRecoHit> &hits, std::ostringstream &oss, int eventId, unsigned int line_no, std::string &prefix){
  unsigned int totalHitNumber = hits.size();
  oss << "\t\t Related hits number: " << totalHitNumber << "\n";
  RPRecoHit curr_hit;

  for(unsigned int hit_no = 0; hit_no < totalHitNumber; hit_no++){
    curr_hit = hits[hit_no];
    unsigned int rawDetId = curr_hit.DetId();

    oss << "\t\t[ hit [" << hit_no << "] ] \tRPDetId: " << armId(rawDetId) << stationId(rawDetId) << rpId(rawDetId)
    << "\tSiliconId: " << siliconId(rawDetId) 
    << "\tposition (wrt to det center): " << curr_hit.Position() 
    << " [mm]\tsigma (uncertainty) " << curr_hit.Sigma() << "\n"; 
  }
}

void 
RecoLoggingAnalyzer::lines_log(std::vector<RPRecognizedPatterns::Line> &lines, std::ostringstream &oss, std::string &prefix, int eventId){
  unsigned int totalLineNumber = lines.size();
  oss << prefix << "Lines:\t" << totalLineNumber << "\n";
  RPRecognizedPatterns::Line curr_line;

  for(unsigned int line_no = 0; line_no < totalLineNumber; line_no++){
    curr_line = lines[line_no];
    oss << "\t[ "<< prefix << "line[" << line_no << "] ] \ta: " << curr_line.a << " [rad]\tb: " << curr_line.b << " [mm]\tw: " << curr_line.w << "\n" ;
    hits_log(curr_line.hits, oss, eventId, line_no, prefix);
  }
}

void 
RecoLoggingAnalyzer::point_logs(std::vector<RPRecognizedPatterns::Line> &uLines, std::vector<RPRecognizedPatterns::Line> &vLines, std::ostringstream &oss){
  // Container for possible hits (u/v intersection)
  std::vector<GeometryUtility::PossibleHitPoint> possibleHits;
  geometryUtility->getPossibleHitPoints(
    uLines,
    vLines,
    1,              // uSiliconId
    0,              // vSiliconId
    possibleHits,
    oss
  );
}


// ------------ method called for each event  ------------
void
RecoLoggingAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  Handle<RPRecognizedPatternsCollection> input;
  iEvent.getByLabel("NonParallelTrackFinder", input);

  int eventId = iEvent.id().event();

  for (auto it : *input)
  {
    unsigned int rp = it.first;
    unsigned int arm = rp / 100;
    unsigned char rpIdx = rp % 10;

    // U V lines
    unsigned int uLinesNumber = it.second.uLines.size();
    unsigned int vLinesNumber = it.second.vLines.size();

    if(uLinesNumber != 1 && vLinesNumber != 1){ 
      std::ostringstream oss;
      oss << "RP = " << rp << "\t arm = " << arm << "\t rpIdx = " << rpIdx << "\n";

      // logging crossing points
      point_logs(it.second.uLines, it.second.vLines, oss);

      // logging U/V lines
      string prefix = "u";
      lines_log(it.second.uLines, oss, prefix, eventId);
      prefix = "v";
      lines_log(it.second.vLines, oss, prefix, eventId);


      std::string text_log = oss.str();
      LogInfo("Demo") << text_log;
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
RecoLoggingAnalyzer::beginJob()
{
  geometryUtility -> printGeometryUtilityData();
}

// ------------ method called once each job just after ending the event loop  ------------
void 
RecoLoggingAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
RecoLoggingAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
RecoLoggingAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
RecoLoggingAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
RecoLoggingAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RecoLoggingAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RecoLoggingAnalyzer);