/* Extract related to matching based on DetID
*/
#pragma once



#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementSuperCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementGsfTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementBrem.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"

#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

// #include "DataFormats/Math/interface/deltaPhi.h"
#include "Math/Transform3D.h" // For the 'Point'



#include <vector>



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

// kuidas initsialiseerimine käib
private:
    void fill(const edm::Event& iEvent, const edm::EventSetup& iSetup);
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

    edm::ESGetToken<CaloGeometry, CaloGeometryRecord> geometryToken_;

    vector<std::map<uint64_t, double>> simcluster_detids_;
};





} // end of tau_analysis namespace