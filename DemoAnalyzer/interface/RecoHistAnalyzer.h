

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


// HISTOGRAMS
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h" 
#include "TH1.h"


class RecoHistAnalyzer : public edm::EDAnalyzer {
   public:
      explicit RecoHistAnalyzer(const edm::ParameterSet&);
      ~RecoHistAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);   // method fills 'descriptions' with the allowed parameters for the module

   private:
      virtual void beginJob() override;                                             // method called once each job just before starting event loop
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;     // method called for each event
      virtual void endJob() override;                                               // method called once each job just after ending the event loop

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;    // method called when starting to processes a run
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;      // method called when ending the processing of a run
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;    // method called when starting to processes a luminosity block
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;      // method called when ending the processing of a luminosity block      

      // member data
      TH1D *vLinesHisto, *uLinesHisto;
      int nbinsx_; 
      double xlow_; 
      double xup_;
};
