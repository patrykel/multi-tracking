import FWCore.ParameterSet.Config as cms

demo = cms.EDAnalyzer('DemoAnalyzer')

recoHist = cms.EDAnalyzer('RecoHistAnalyzer')

recoToFlatRoot = cms.EDAnalyzer('RecoToFlatRootAnalyzer')
