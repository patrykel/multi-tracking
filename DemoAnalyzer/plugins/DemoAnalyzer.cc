// -*- C++ -*-
//
// Package:    Demo/DemoAnalyzer
// Class:      DemoAnalyzer
// 
/**\class DemoAnalyzer DemoAnalyzer.cc Demo/DemoAnalyzer/plugins/DemoAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Juan Eduardo Ramirez Vargas
//         Created:  Wed, 30 Dec 2015 22:36:50 GMT
//
//


#include "Demo/DemoAnalyzer/interface/DemoAnalyzer.h"


DemoAnalyzer::DemoAnalyzer(const edm::ParameterSet& iConfig) :
minTracks_(iConfig.getUntrackedParameter<unsigned int>("minTracks",0))
{
    //now do what ever initialization is needed
    max_uLines = max_vLines = 0;
   
    edm::Service<TFileService> fs;
   
    // just for testing:
    vLinesHisto = fs->make<TH1D>("vLines" , "VLINES" , 50 , 0 , 50 ); // what should be the first parameter????
    uLinesHisto = fs->make<TH1D>("uLines" , "ULINES" , 50 , 0 , 50 ); // what should be the first parameter????

    geometryUtility = new GeometryUtility();
}


DemoAnalyzer::~DemoAnalyzer(){
}

/*

FROM: DataFormats/TotemRPDetId/interface/TotRPDetId.h

1   - 15  - shit 00000
16  - 19  - krzemowy wafelek
20  - 22  - RP id
23  - 24  - station id
25      - arm id
26 - 31   - jakiś shit...

*/

unsigned int armId(unsigned int detId){
  return (detId >> 24) & 0b0001;
}

unsigned int stationId(unsigned int detId){
  return (detId >> 22) & 0b0011;
}

unsigned int rpId(unsigned int detId){
  return (detId >> 19) & 0b0111;
}

unsigned int siliconId(unsigned int detId){
  return (detId >> 15) & 0b1111;
}


void 
DemoAnalyzer::hits_log(std::vector<RPRecoHit> &hits, std::ostringstream &oss){
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
DemoAnalyzer::lines_log(std::vector<RPRecognizedPatterns::Line> &lines, std::ostringstream &oss, std::string &prefix){
  unsigned int totalLineNumber = lines.size();
  oss << prefix << "Lines:\t" << totalLineNumber << "\n";
  RPRecognizedPatterns::Line curr_line;

  for(unsigned int line_no = 0; line_no < totalLineNumber; line_no++){
    curr_line = lines[line_no];
    oss << "\t[ "<< prefix << "line[" << line_no << "] ] \ta: " << curr_line.a << " [rad]\tb: " << curr_line.b << " [mm]\tw: " << curr_line.w << "\n" ;
    hits_log(curr_line.hits, oss);
  }
}


/* 
Ta funckja ma za zadanie wypisać wszystkie punkty przeciecia linii
vline:
- wiele hitow
- ten koncept mi się pierniczy trochę...
  - może
    - jest wiele pasków 
    - nachodzą na siebie
    - fajne rozrysować paski --> potrzeba:
      - rozmiar detektora
      - kierunek u/v
      - rzut środka detektora na linie u/v
- najprostsza rzecz...
  - wyznacz punkt przeciecia pierwszego uLine i pierwszego vLine ;) --> tylko ze mają wiele hitów

*/


void 
DemoAnalyzer::hit_points_log(std::vector<GeometryUtility::PossibleHitPoint> &possibleHits, std::ostringstream &oss){
  oss << "Hits (detector centers): " << "\n";

  GeometryUtility::PossibleHitPoint point;
  for(unsigned int i = 0; i < possibleHits.size(); i++){
    point = possibleHits[i];
    oss << "\tPoint[" << i << "]: \tx = " << point.x << "\ty = " << point.y <<  "\tz = " << point.z << "\n";
  }
}

void 
DemoAnalyzer::point_logs(std::vector<RPRecognizedPatterns::Line> &uLines, std::vector<RPRecognizedPatterns::Line> &vLines, std::ostringstream &oss){
  //int uLinesNumber = uLines.size();
  //int vLinesNumber = vLines.size();

  // kontenerek na possible hits --> wypelniamy go dzieki geometryUtility
  std::vector<GeometryUtility::PossibleHitPoint> possibleHits;
  geometryUtility->getPossibleHitPoint(uLines[0], vLines[0], possibleHits, oss);

  hit_points_log(possibleHits, oss);



  // for(int u_i = 0; u_i < uLinesNumber; u_i++){
  //   for(int v_i = 0; v_i < vLinesNumber; v_i++){
  //     // po perwsze tutaj bedzie wiele possible hit points....
  //     // bo kazda uLine ma wiele hitow...
  //     // wiec pasowaloby sie przeiterować po wszystkich mozliwych :)

  //     for(int h_v_no = 0; h_v_no < vLines[v_i].hits.size(); h_v_no++){
  //       // tutaj winno sie dodawać do wektorka ;)
  //     }


  //     GeometryUtility::PossibleHitPoint geometryUtility->get_possible_hit_point(uLines[u_i], vLines[v_i]){
        
  //     }
  //   }
  // } 
}


// ------------ method called for each event  ------------
void
DemoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;

   // Handle<reco::TrackCollection> tracks;
   // iEvent.getByLabel("generalTracks", tracks); 
   // LogInfo("Demo") << "number of tracks "<<tracks->size();
   // vLinesHisto->Fill(tracks->size());

   // if( minTracks_ <= tracks->size() ) {
   //    LogInfo("Demo") << "number of tracks "<<tracks->size();
   // }

   // FROM ANNA
  bool combineVerticalHorizontalRPs = false;

  Handle<RPRecognizedPatternsCollection> input;
  iEvent.getByLabel("NonParallelTrackFinder", input);
  map<unsigned int, RPRecognizedPatternsCollection> inputGroups;

  int input_count = 0;

  for (auto it : *input)
  {
    unsigned int rp = it.first;
    unsigned int arm = rp / 100;
    unsigned char rpIdx = rp % 10;
  
    unsigned int group = 0;

    // Log RP id and number of uLines and vLines
    // LogInfo("Demo") << "RP id: " << it.first;
    // LogInfo("Demo") << "uLines:\t" << it.second.uLines.size() << "\tvLines:\t" << it.second.vLines.size();

    // U V lines number handled
    unsigned int uLinesNumber = it.second.uLines.size();
    unsigned int vLinesNumber = it.second.vLines.size();

    // LOGGING
    if(uLinesNumber > 1 && vLinesNumber > 1){
      std::ostringstream oss;

      oss << "RP = " << rp << "\t arm = " << arm << "\t rpIdx = " << rpIdx << "\n";

      // logging crossing points
      point_logs(it.second.uLines, it.second.vLines, oss);

      string prefix = "u";
      lines_log(it.second.uLines, oss, prefix);

      prefix = "v";
      lines_log(it.second.vLines, oss, prefix);

      oss << "\nInput count:\t" << input_count;
      std::string text_log = oss.str();
      LogInfo("Demo") << text_log;
    }

    // SETTING MAX VLINES AND ULINES
    if(uLinesNumber > max_uLines){
      max_uLines = uLinesNumber;
    }
    if(vLinesNumber > max_vLines){
      max_vLines = vLinesNumber;
    }
    
    // FILLING HISTOGRAMS
    vLinesHisto->Fill(vLinesNumber);
    uLinesHisto->Fill(uLinesNumber);
  

    if (!combineVerticalHorizontalRPs)
    {
      if (rpIdx == 0 || rpIdx == 4)
        group = 1;
      if (rpIdx == 1 || rpIdx == 5)
        group = 2;
      if (rpIdx == 2 || rpIdx == 3)
        group = 3;
    }
  
    // FILLING MAP
    unsigned int idx = 10*arm + group;
    inputGroups[idx][rp] = it.second;

    input_count++;
  }
  // LogInfo("Demo") << "Max uLines\t" << max_uLines << "\tMax vLines\t" << max_vLines;
  


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
    // TUTAJ ZROBIE C++ teścik na to czy dobrze działa geometryUtility

    std::ostringstream oss;

    oss << "ID\tx\ty\tz\tdx\tdy\n";
    for(int i = 0; i < 240; i++){
      oss << i << "\t" << geometryUtility -> x[i] << "\t"; 
      oss << geometryUtility -> y[i] << "\t";
      oss << geometryUtility -> z[i] << "\t";
      oss << geometryUtility -> dx[i] << "\t";
      oss << geometryUtility -> dy[i] << "\n";
    }

    oss << "\n";

    oss << "arm\tsta\trp\tdet\tidx\t\tx\ty\tz\n";

    for(int a_i = 0; a_i < 2; a_i++){
      for(int s_i = 0; s_i < 3; s_i += 2){
        for(int r_i = 0; r_i < 6; r_i++){
          for(int d_i = 0; d_i < 10; d_i++){
            oss << a_i << "\t";
            oss << s_i << "\t";
            oss << r_i << "\t";
            oss << d_i << "\t";
            oss << geometryUtility -> getIdx(a_i, s_i, r_i, d_i) << "\t\t";
            oss << geometryUtility -> getX(a_i, s_i, r_i, d_i) << "\t";
            oss << geometryUtility -> getY(a_i, s_i, r_i, d_i) << "\t";
            oss << geometryUtility -> getZ(a_i, s_i, r_i, d_i) << "\n";
          }
        }
      }
    }



    std::string text_log = oss.str();
    edm::LogInfo("Demo") << text_log;

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