import FWCore.ParameterSet.Config as cms




def customize_step2(process):
    process.load('SimGeneral.MixingModule.caloTruthProducer_cfi')
    process.caloParticles.simHitCollections = cms.PSet(
        hcal = cms.VInputTag(cms.InputTag('g4SimHits','HcalHits')),
        ecal = cms.VInputTag(
            cms.InputTag('g4SimHits','EcalHitsEE'),
            cms.InputTag('g4SimHits','EcalHitsEB'),
            cms.InputTag('g4SimHits','EcalHitsES'),
        )
    )
    process.caloParticles.doHGCAL = False
    process.caloParticles.allowDifferentSimHitProcesses = True
    process.mix.digitizers.caloParticles = process.caloParticles
    process.mix.digitizers.mergedtruth.ignoreTracksOutsideVolume = True
    process.mix.digitizers.mergedtruth.allowDifferentSimHitProcesses = True
    process.mix.digitizers.mergedtruth.select.signalOnlyTP = False

    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_simSiStripDigis_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_simSiPixelDigis_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*_MergedCaloTruth_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*_MergedTrackTruth_*')
    return process


def customize_step3(process):
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_simSiStripDigis_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_simSiPixelDigis_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*_MergedCaloTruth_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*_MergedTrackTruth_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoPFRecTracks_*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoPFRecHits_*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoGsfPFRecTracks_*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_particleFlowBlock_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoTracks_standAloneMuons_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoTrackExtras_standAloneMuons_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoMuons_*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoTracks_*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoGsfTracks_*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep recoPFBlocks_*_*_*')
    # Additions
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedMET*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedPhotons*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedTaus*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedElectrons*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedJets*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedMuons*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedCaloJets*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedGenJets*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedMuonTrackExtras*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_slimmedHcalRecHits*_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_offlineSlimmedPrimaryVertices_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_packedPFCandidates_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_isolatedTracks_*_*')
    process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*Gen*_*_*')
    return process
