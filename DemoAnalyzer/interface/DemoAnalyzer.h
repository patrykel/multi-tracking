

/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatternsCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatterns.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "Demo/DemoAnalyzer/interface/GeometryUtility.h"
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>


class DemoAnalyzer : public edm::EDAnalyzer {
   public:
      explicit DemoAnalyzer(const edm::ParameterSet&);
      ~DemoAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);   // method fills 'descriptions' with the allowed parameters for the module

   private:
      virtual void beginJob() override;                                             // method called once each job just before starting event loop
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;     // method called for each event
      virtual void endJob() override;                                               // method called once each job just after ending the event loop
      
      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;    // method called when starting to processes a run
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;      // method called when ending the processing of a run
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;    // method called when starting to processes a luminosity block
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;      // method called when ending the processing of a luminosity block

      void hits_log(std::vector<RPRecoHit> &hits, std::ostringstream &oss, int eventId, unsigned int line_no, std::string &prefix);
      void lines_log(std::vector<RPRecognizedPatterns::Line> &lines, std::ostringstream &oss, std::string &prefix, int eventId);
      void hit_points_log(std::vector<GeometryUtility::PossibleHitPoint> &possibleHits, std::ostringstream &oss);
      void point_logs(std::vector<RPRecognizedPatterns::Line> &uLines, std::vector<RPRecognizedPatterns::Line> &vLines, std::ostringstream &oss);

      // ----------member data ---------------------------
      unsigned int minTracks_;
      unsigned int max_uLines;
      unsigned int max_vLines;

      // TMP VARIABLES
      int write_to_file;
      int rps_in_event[200001] = {0};


      GeometryUtility *geometryUtility;
      ofstream oneEventFile;  // to create csv file for event dump
};

unsigned int armId(unsigned int detId);       // single digit [0|1]
unsigned int stationId(unsigned int detId);   // single digit [0|2]
unsigned int rpId(unsigned int detId);        // single digit [0-5]
unsigned int siliconId(unsigned int detId);   // single digit [0-9]

unsigned int pureRpId(unsigned int detId);
unsigned int pureSiliconId(unsigned int detId);

void printGeometryUtilityData(GeometryUtility *geometryUtility);
