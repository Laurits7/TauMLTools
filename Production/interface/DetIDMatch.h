/* Extract related to matching based on DetID
*/
#pragma once

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/HcalRecHit/interface/HFRecHit.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementSuperCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementGsfTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementBrem.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"

#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "DataFormats/GeometrySurface/interface/PlaneBuilder.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "CommonTools/BaseParticlePropagator/interface/BaseParticlePropagator.h"
#include "Math/Transform3D.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/VolumeGeometry/interface/MagVolumeOutsideValidity.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "TH1F.h"
#include "TVector2.h"
#include "TTree.h"

#include <map>
#include <set>
#include <string>
#include <vector>
#include <set>



namespace tau_analysis {

class ElementWithIndex {
public:
  const reco::PFBlockElement& orig;
  size_t idx_block;
  size_t idx_elem;
  ElementWithIndex(const reco::PFBlockElement& _orig, size_t _idx_block, size_t _idx_elem)
      : orig(_orig), idx_block(_idx_block), idx_elem(_idx_elem){};
};



class DetIDMatcher {

public:
    typedef ROOT::Math::Transform3D::Point Point;

    DetIDMatcher();
    ~DetIDMatcher();

    std::vector<float> rechit_e_;
    std::vector<float> rechit_x_;
    std::vector<float> rechit_y_;
    std::vector<float> rechit_z_;
    std::vector<float> rechit_det_;
    std::vector<float> rechit_subdet_;
    std::vector<float> rechit_eta_;
    std::vector<float> rechit_phi_;
    std::vector<int> rechit_idx_element_;
    std::vector<uint64_t> rechit_detid_;


    std::vector<std::pair<int, int>> simcluster_to_element;
    std::vector<float> simcluster_to_element_cmp;
    void fill(const edm::Event& iEvent, const edm::EventSetup& iSetup);
    void associateClusterToSimCluster(const std::vector<ElementWithIndex>& all_elements);

// kuidas initsialiseerimine käib
private:
    std::pair<std::vector<ElementWithIndex>, std::vector<std::tuple<int, int, float>>> processBlocks(
        const std::vector<reco::PFBlock>& pfBlocks
    );
    void clearVariables();  // vaja defineerida src
    GlobalPoint getHitPosition(const DetId& id);
    // ----------member data ---------------------------

    edm::EDGetTokenT<std::vector<reco::GenParticle>> genParticles_;
    edm::EDGetTokenT<edm::View<TrackingParticle>> trackingParticles_;
    edm::EDGetTokenT<edm::View<CaloParticle>> caloParticles_;
    edm::EDGetTokenT<edm::View<reco::Track>> tracks_;
    edm::EDGetTokenT<std::vector<reco::PFBlock>> pfBlocks_;
    edm::EDGetTokenT<std::vector<reco::PFCandidate>> pfCandidates_;
    edm::EDGetTokenT<reco::RecoToSimCollection> tracks_recotosim_;
    CaloGeometry* geom;

    edm::ESGetToken<CaloGeometry, CaloGeometryRecord> geometryToken_;

    std::vector<std::map<uint64_t, double>> simcluster_detids_;
};





} // end of tau_analysis namespace