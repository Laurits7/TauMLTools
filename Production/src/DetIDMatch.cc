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



// vt indentation
void DetIDMatcher::associateClusterToSimCluster(const std::vector<ElementWithIndex>& all_elements) {

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

}