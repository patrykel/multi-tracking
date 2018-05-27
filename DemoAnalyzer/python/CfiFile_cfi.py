import FWCore.ParameterSet.Config as cms

demo = cms.EDAnalyzer('DemoAnalyzer')

recoHist = cms.EDAnalyzer('RecoHistAnalyzer')

recoLogger = cms.EDAnalyzer('RecoLoggingAnalyzer')

recoToFlatRoot = cms.EDAnalyzer('RecoToFlatRootAnalyzer')

recoCSV = cms.EDAnalyzer('RecoToCSVAnalyzer')