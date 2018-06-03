
/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatternsCollection.h"

#include "TTree.h"
#include "TFile.h"
#include "TSystemDirectory.h"


class RecoToFlatRootAnalyzer : public edm::EDAnalyzer {
   public:
      explicit RecoToFlatRootAnalyzer(const edm::ParameterSet&);
      ~RecoToFlatRootAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      virtual void exportToFlatRoot(const edm::Event &iEvent);
      virtual void exportHitsFromLineToFFlatRoot(std::vector<RPRecognizedPatterns::Line> &lines);

      // ----------member data ---------------------------
      
      // TO DO: UNHARDCODE IT
      TFile *f = new TFile("reco_0_flat.root", "recreate");
      TTree *T = new TTree("T", "reco_0_flat");
      
      Int_t recoID = 0;
      Int_t eventID, armID, groupID, rpID, uLineSize, vLineSize, siliconID;
      Float_t position, sigma_;
};