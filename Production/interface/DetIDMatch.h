/* Extract related to matching based on DetID
*/
#pragma once



#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementSuperCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementGsfTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementBrem.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "DataFormats/DetId/interface/DetId.h"

// #include "DataFormats/Math/interface/deltaPhi.h"
#include "Math/Transform3D.h" // For the 'Point'



#include <vector>



namespace tau_analysis {


class DetIDMatcher {

public:
    typedef ROOT::Math::Transform3D::Point Point;

    DetIDMatcher()

// kuidas initsialiseerimine käib
private:
    void associateClusterToSimCluster(const vector<ElementWithIndex>& all_elements);
    void clearVariables();
    GlobalPoint getHitPosition(const DetId& id);
    // ----------member data ---------------------------

    edm::EDGetTokenT<std::vector<reco::GenParticle>> genParticles_;
    edm::EDGetTokenT<edm::View<TrackingParticle>> trackingParticles_;
    edm::EDGetTokenT<edm::View<CaloParticle>> caloParticles_;
    edm::EDGetTokenT<edm::View<reco::Track>> tracks_;
    edm::EDGetTokenT<std::vector<reco::PFBlock>> pfBlocks_;
    edm::EDGetTokenT<std::vector<reco::PFCandidate>> pfCandidates_;
    edm::EDGetTokenT<reco::RecoToSimCollection> tracks_recotosim_;


    vector<float> rechit_e_;
    vector<float> rechit_x_;
    vector<float> rechit_y_;
    vector<float> rechit_z_;
    vector<float> rechit_det_;
    vector<float> rechit_subdet_;
    vector<float> rechit_eta_;
    vector<float> rechit_phi_;
    vector<int> rechit_idx_element_;
    vector<uint64_t> rechit_detid_;


    vector<pair<int, int>> simcluster_to_element;
    vector<float> simcluster_to_element_cmp;
}





} // end of tau_analysis namespace