#ifndef StMixedD0Pion_hh
#define StMixedD0Pion_hh
#ifdef __ROOT__

#include <cmath>

#include "TObject.h"
#include "TClonesArray.h"
#include "StLorentzVectorF.hh"
class StMixedD0;
class StMixedSoftPion;
class StMixedD0Pion : public TObject
{
    public:
    StMixedD0Pion();
    StMixedD0Pion(StMixedD0 const& aD0, StMixedSoftPion const  & aPion);
    ~StMixedD0Pion() {}

    StLorentzVectorF const & lorentzVector() const { return mLorentzVector;}
//D_star
    float const deltaM() const { return mdeltaM;}
    float const m() const { return mLorentzVector.m();}
    float const pt() const { return mLorentzVector.perp();}
    float const eta() const { return mLorentzVector.pseudoRapidity();}
    float const phi() const { return mLorentzVector.phi();}
    StLorentzVectorF const & D0lorentzVector() const { return mD0LorentzVector;}
    StLorentzVectorF const & PilorentzVector() const { return mPiLorentzVector;}
    short const charge() const { return mCharge;}
    float D0toPion_pt() const{
        return mD0toPion_pt;
    }
    bool rightsign() const { return mRightsign;};

private:
    float mdeltaM;
    StLorentzVectorF mLorentzVector;
    short mCharge;
    bool mRightsign;
    float mD0toPion_pt;
    StLorentzVectorF mD0LorentzVector;
    StLorentzVectorF mPiLorentzVector;
    ClassDef(StMixedD0Pion,1)
};
#endif
#endif
