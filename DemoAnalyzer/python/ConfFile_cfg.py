import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('Demo')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
        limit = cms.untracked.int32(-1)
        )
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(200) )

process.source = cms.Source("PoolSource",
                                # replace 'myfile.root' with the source file you want to use
                                fileNames = cms.untracked.vstring(
            # 'file:/afs/cern.ch/cms/Tutorials/TWIKI_DATA/TTJets_8TeV_53X.root'
            'root://eosuser.cern.ch//eos/user/p/plawski/totem/data/offline/2015/90m/Reco/version2_resynch/4510/9985/reco_0.root',
            # 'root://eosuser.cern.ch//eos/user/p/plawski/totem/data/offline/2015/90m/Reco/version2_resynch/4510/9985/reco_1.root'

                )
                            )

process.demo = cms.EDAnalyzer('DemoAnalyzer',

                                         minTracks=cms.untracked.uint32(0)
                              )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('uvLines.root')
                                   )


process.p = cms.Path(process.demo)