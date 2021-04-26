/* Extract related to matching based on DetID
*/

#include "TauMLTools/Production/interface/DetIDMatch.h"


namespace tau_analysis {


double detid_compare(
    const std::map<uint64_t,
    double>& rechits,
    const std::map<uint64_t,
    double>& simhits
){
  double ret = 0.0;
  for (const auto& rh : rechits) {
    for (const auto& sh : simhits) {
      if (rh.first == sh.first) {
        //rechit energy times simhit fraction
        ret += rh.second * sh.second;
        break;
      }
    }
  }
  return ret;
}


GlobalPoint DetIDMatcher::getHitPosition(const DetId& id) {
  GlobalPoint ret;
  bool present = false;
  if (((id.det() == DetId::Ecal &&
        (id.subdetId() == EcalBarrel || id.subdetId() == EcalEndcap || id.subdetId() == EcalPreshower)) ||
       (id.det() == DetId::Hcal && (id.subdetId() == HcalBarrel || id.subdetId() == HcalEndcap ||
                                    id.subdetId() == HcalForward || id.subdetId() == HcalOuter)))) {
    const CaloSubdetectorGeometry* geom_sd(geom->getSubdetectorGeometry(id.det(), id.subdetId()));
    present = geom_sd->present(id);
    if (present) {
      const auto& cell = geom_sd->getGeometry(id);
      ret = GlobalPoint(cell->getPosition());
    }
  }
  return ret;
}


//https://stackoverflow.com/questions/27086195/linear-index-upper-triangular-matrix/27088560
int get_index_triu_vector(int i, int j, int n) {
  int k = (n * (n - 1) / 2) - (n - i) * ((n - i) - 1) / 2 + j - i - 1;
  return k;
}

std::pair<int, int> get_triu_vector_index(int k, int n) {
  int i = n - 2 - floor(sqrt(-8 * k + 4 * n * (n - 1) - 7) / 2.0 - 0.5);
  int j = k + i + 1 - n * (n - 1) / 2 + (n - i) * ((n - i) - 1) / 2;
  return std::make_pair(i, j);
}

std::pair<std::vector<ElementWithIndex>, std::vector<std::tuple<int, int, float>>> DetIDMatcher::processBlocks(
    const std::vector<reco::PFBlock>& pfBlocks) {
  std::vector<ElementWithIndex> ret;
  std::vector<std::tuple<int, int, float>> distances;

  //Collect all the elements
  int iblock = 0;
  for (const auto& block : pfBlocks) {
    int ielem = 0;
    const auto& linkdata = block.linkData();

    //create a list of global element indices with distances
    for (const auto& link : linkdata) {
      const auto vecidx = link.first;
      const auto dist = link.second.distance;
      const auto& ij = get_triu_vector_index(vecidx, block.elements().size());
      auto globalindex_i = ij.first + ret.size();
      auto globalindex_j = ij.second + ret.size();
      distances.push_back(std::make_tuple(globalindex_i, globalindex_j, dist));
    }

    for (const auto& elem : block.elements()) {
      ElementWithIndex elem_index(elem, iblock, ielem);
      ret.push_back(elem_index);
      ielem += 1;
    }  //elements
    iblock += 1;
  }  //blocks
  return std::make_pair(ret, distances);

}  //processBlocks


void DetIDMatcher::fill(const edm::Event& iEvent, const edm::EventSetup& iSetup){
    edm::Handle<std::vector<reco::PFBlock>> pfBlocksHandle;
    iEvent.getByToken(pfBlocks_, pfBlocksHandle);
    std::vector<reco::PFBlock> pfBlocks = *pfBlocksHandle;

    edm::Handle<edm::View<CaloParticle>> caloParticlesHandle;
    iEvent.getByToken(caloParticles_, caloParticlesHandle);
    const edm::View<CaloParticle>& caloParticles = *caloParticlesHandle;

    //Collect all clusters, tracks and superclusters
    const auto& all_elements_distances = processBlocks(pfBlocks);
    const auto& all_elements = all_elements_distances.first;
    auto& pG = iSetup.getData(geometryToken_);
    geom = (CaloGeometry*)&pG;
    for (unsigned long ncaloparticle = 0; ncaloparticle < caloParticles.size();ncaloparticle++) {
        const auto& cp = caloParticles.at(ncaloparticle);
        edm::RefToBase<CaloParticle> cpref(caloParticlesHandle, ncaloparticle);
        for (const auto& simcluster : cp.simClusters()) {
            std::map<uint64_t, double> detid_energy;
            for (const auto& hf : simcluster->hits_and_fractions()) {
                DetId id(hf.first);
                if (id.det() == DetId::Hcal || id.det() == DetId::Ecal) {
                  detid_energy[id.rawId()] += hf.second;
                }
            }
            simcluster_detids_.push_back(detid_energy);
        }
        associateClusterToSimCluster(all_elements);
    }
}

// vt indentation
void DetIDMatcher::associateClusterToSimCluster(
    const std::vector<ElementWithIndex>& all_elements
){


  std::vector<std::map<uint64_t, double>> detids_elements;
  std::map<uint64_t, double> rechits_energy_all;

  int idx_element = 0;
  for (const auto& elem : all_elements) {
    std::map<uint64_t, double> detids;
    const auto& type = elem.orig.type();

    if (type == reco::PFBlockElement::ECAL || type == reco::PFBlockElement::HCAL || type == reco::PFBlockElement::PS1 ||
        type == reco::PFBlockElement::PS2 || type == reco::PFBlockElement::HO || type == reco::PFBlockElement::HFHAD ||
        type == reco::PFBlockElement::HFEM) {
      const auto& clref = elem.orig.clusterRef();
      assert(clref.isNonnull());
      const auto& cluster = *clref;

      //all rechits and the energy fractions in this cluster
      const std::vector<reco::PFRecHitFraction>& rechit_fracs = cluster.recHitFractions();
      for (const auto& rh : rechit_fracs) {
        const reco::PFRecHit pfrh = *rh.recHitRef();
        if (detids.find(pfrh.detId()) != detids.end()) {
          continue;
        }
        detids[pfrh.detId()] += pfrh.energy() * rh.fraction();
        const auto id = DetId(pfrh.detId());
        float x = 0;
        float y = 0;
        float z = 0;
        float eta = 0;
        float phi = 0;

        const auto& pos = getHitPosition(id);
        x = pos.x();
        y = pos.y();
        z = pos.z();
        eta = pos.eta();
        phi = pos.phi();

        rechit_x_.push_back(x);
        rechit_y_.push_back(y);
        rechit_z_.push_back(z);
        rechit_det_.push_back(id.det());
        rechit_subdet_.push_back(id.subdetId());
        rechit_eta_.push_back(eta);
        rechit_phi_.push_back(phi);
        rechit_e_.push_back(pfrh.energy() * rh.fraction());
        rechit_idx_element_.push_back(idx_element);
        rechit_detid_.push_back(id.rawId());
        rechits_energy_all[id.rawId()] += pfrh.energy() * rh.fraction();
      }  //rechit_fracs
    } else if (type == reco::PFBlockElement::SC) {
      const auto& clref = ((const reco::PFBlockElementSuperCluster*)&(elem.orig))->superClusterRef();
      assert(clref.isNonnull());
      const auto& cluster = *clref;

      //all rechits and the energy fractions in this cluster
      const auto& rechit_fracs = cluster.hitsAndFractions();
      for (const auto& rh : rechit_fracs) {
        if (detids.find(rh.first.rawId()) != detids.end()) {
          continue;
        }
        detids[rh.first.rawId()] += cluster.energy() * rh.second;
        const auto id = rh.first;
        float x = 0;
        float y = 0;
        float z = 0;
        float eta = 0;
        float phi = 0;

        const auto& pos = getHitPosition(id);
        x = pos.x();
        y = pos.y();
        z = pos.z();
        eta = pos.eta();
        phi = pos.phi();

        rechit_x_.push_back(x);
        rechit_y_.push_back(y);
        rechit_z_.push_back(z);
        rechit_det_.push_back(id.det());
        rechit_subdet_.push_back(id.subdetId());
        rechit_eta_.push_back(eta);
        rechit_phi_.push_back(phi);
        rechit_e_.push_back(rh.second);
        rechit_idx_element_.push_back(idx_element);
        rechit_detid_.push_back(id.rawId());
        rechits_energy_all[id.rawId()] += cluster.energy() * rh.second;
      }  //rechit_fracs
    }
    detids_elements.push_back(detids);
    idx_element += 1;
  }  //all_elements

  //associate elements (reco clusters) to simclusters
  int ielement = 0;
  for (const auto& detids : detids_elements) {
    int isimcluster = 0;
    if (!detids.empty()) {
      double sum_e_tot = 0.0;
      for (const auto& c : detids) {
        sum_e_tot += c.second;
      }

      for (const auto& simcluster_detids : simcluster_detids_) {
        double sum_e_tot_sc = 0.0;
        for (const auto& c : simcluster_detids) {
          sum_e_tot_sc += c.second;
        }

        //get the energy of the simcluster hits that matches detids of the rechits
        double cmp = detid_compare(detids, simcluster_detids);
        if (cmp > 0) {
          simcluster_to_element.push_back(std::make_pair(isimcluster, ielement));
          simcluster_to_element_cmp.push_back((float)cmp);
        }
        isimcluster += 1;
      }
    }  //element had rechits
    ielement += 1;
  }  //rechit clusters
}

void DetIDMatcher::clearVariables() {
    rechit_e_.clear();
    rechit_x_.clear();
    rechit_y_.clear();
    rechit_z_.clear();
    rechit_det_.clear();
    rechit_subdet_.clear();
    rechit_eta_.clear();
    rechit_phi_.clear();
    rechit_idx_element_.clear();
    rechit_detid_.clear();
}

}