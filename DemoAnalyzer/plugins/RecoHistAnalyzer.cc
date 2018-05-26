

/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/


#include "Demo/DemoAnalyzer/interface/RecoHistAnalyzer.h"

RecoHistAnalyzer::RecoHistAnalyzer(const edm::ParameterSet& ps) :
nbinsx_(ps.getUntrackedParameter<int>("nbinsx", 50)),
xlow_(ps.getUntrackedParameter<double>("xlow", 0.0)),
xup_(ps.getUntrackedParameter<double>("xup", 50.0))
{
    edm::Service<TFileService> fs;   

    vLinesHisto = fs->make<TH1D>("vLines" , "VLINES" , nbinsx_ , xlow_ , xup_ );   // TH1D (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup)
    uLinesHisto = fs->make<TH1D>("uLines" , "ULINES" , nbinsx_ , xlow_ , xup_ );

}

RecoHistAnalyzer::~RecoHistAnalyzer(){
}

// ------------ method called once each job just before starting event loop  ------------
void 
RecoHistAnalyzer::beginJob()
{
}

// ------------ method called for each event  ------------
void
RecoHistAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  Handle<RPRecognizedPatternsCollection> input;
  iEvent.getByLabel("NonParallelTrackFinder", input);

  for (auto it : *input)
  {
    // U V lines number
    unsigned int uLinesNumber = it.second.uLines.size();
    unsigned int vLinesNumber = it.second.vLines.size();

    // FILLING HISTOGRAMS
    vLinesHisto->Fill(vLinesNumber);
    uLinesHisto->Fill(uLinesNumber);
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

// ------------ method called once each job just after ending the event loop  ------------
void 
RecoHistAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
RecoHistAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
RecoHistAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
RecoHistAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
RecoHistAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RecoHistAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RecoHistAnalyzer);