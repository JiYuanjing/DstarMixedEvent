/* **************************************************
 *  
 *  Modified from : Guannan Xie <guannanxie@lbl.gov>
 *           Mustafa Mustafa <mmustafa@lbl.gov>
 *
 * **************************************************
 */

#include <cmath>

#include "St_base/StMessMgr.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TString.h"
#include "StPicoDstMaker/StPicoEvent.h"
#include "StPicoDstMaker/StPicoTrack.h"
#include "StPicoPrescales/StPicoPrescales.h"
#include "StPicoCharmContainers/StKaonPion.h"
#include "StPicoCharmContainers/StD0Pion.h"
#include "StAnaCuts.h"
#include "TNtuple.h"
#include "THnSparse.h"
#include "THn.h"
#include "StDstarMixedEvent.h"
#include "StMixedD0.h"
#include "StPicoDstarMixedHists.h"
#include "StMixedD0Pion.h"

//-----------------------------------------------------------------------
StPicoDstarMixedHists::StPicoDstarMixedHists(TString fileBaseName, bool reconstructD, bool fillQaHists, bool fillBackgroundTrees, bool fillSoftPionEff, bool softpionQa) : mFillQaHists(fillQaHists), mFillBackgroundTrees(fillBackgroundTrees),mFillSoftPionEff(fillSoftPionEff),mReconstructD(reconstructD),mSoftPionQa(softpionQa),
   mPrescales(NULL), mOutFile(NULL), mh2InvariantMassVsPt(NULL), mh2InvariantMassVsPtLike(NULL), mh2InvariantMassVsPtTof(NULL), mh2InvariantMassVsPtTofLike(NULL),
   mh1Cent(NULL), mh1CentWg(NULL), mh1gRefmultCor(NULL), mh1gRefmultCorWg(NULL), mh2CentVz(NULL), mh2CentVzWg(NULL), mh3InvariantMassVsPtVsCent(NULL), mh3InvariantMassVsPtVsCentLike(NULL), mh3InvariantMassVsPtVsCentTof(NULL), mh3InvariantMassVsPtVsCentTofLike(NULL),mh2InvariantMassVsPtDstar(NULL), mh2InvariantMassVsPtLikeDstar(NULL),mh3InvariantMassVsPtVsCentDstar(NULL), mh3InvariantMassVsPtVsCentLikeDstar(NULL), mh3InvariantMassVsPtVsCentSBDstar(NULL),mh2InvariantMassVsPtSBDstar(NULL),mh2InvariantMassVsPtDstarD0(NULL), mh2InvariantMassVsPtSBD0(NULL),mh3SoftPionDcaVsPtVsCent(NULL), mh3SoftPionPtVsdiffInvBetaVsCent(NULL), mh3SoftPionPtVsBetaVsCent(NULL), mh3SoftPionPtVsnSigmaVsCent(NULL),
   mh2InvariantMassVsPtDstarMixed(NULL), mh3InvariantMassVsPtVsCentDstarMixed(NULL), mh3MDstarVsD0PtVsCentDstarMixed(NULL), mh3MDstarVsD0PtVsCent(NULL), mh3MDstarVsD0PtVsCentLike(NULL), buffer(NULL),mhnDstarD0PiMassCentCharge(NULL),mhnDstarD0PiMassCentChargeLK(NULL),mhnDstarD0PiMassCentChargeMix(NULL),mhnDstarD0PiMassCentChargeMixLK(NULL),mhnDstarD0PiMassCentChargeSB(NULL)

   // mh2Tpc1PtCent(NULL),  mh2Tpc1PhiVz(NULL), mh2HFT1PtCent(NULL),  mh2HFT1PhiVz(NULL),  mh3DcaXyPtCent(NULL), mh3DcaZPtCent(NULL),
{
   mPrescales = new StPicoPrescales(anaCuts::prescalesFilesDirectoryName);

   mOutFile = new TFile(Form("%s.hists.root", fileBaseName.Data()), "RECREATE");

   for (int iParticle = 0; iParticle < anaCuts::nParticles; iParticle++)
   {
      for (int iEta = 0; iEta < anaCuts::nEtasDca; iEta++)
      {
         for (int iVz = 0; iVz < anaCuts::nVzsDca; iVz++)
         {
            for (int iCent = 0; iCent < anaCuts::nCentsDca; iCent++)
            {
               mh3DcaXyZPtCentPartEtaVzPhi[iParticle][iEta][iVz][iCent] = NULL;
            }
         }
      }
   }

   for (int iParticle = 0; iParticle < anaCuts::nParticles; iParticle++)
   {
      for (int iEta = 0; iEta < anaCuts::nEtasRatio; iEta++)
      {
         for (int iVz = 0; iVz < anaCuts::nVzsRatio; iVz++)
         {
            for (int iPhi = 0; iPhi < anaCuts::nPhisRatio; iPhi++)
            {
               mh2Tpc1PtCentPartEtaVzPhi[iParticle][iEta][iVz][iPhi] = NULL;
               mh2HFT1PtCentPartEtaVzPhi[iParticle][iEta][iVz][iPhi] = NULL;
            }
         }
      }
   }

   for (int icent=0;icent<9;icent++)
   {
     for (int chg=0;chg<2;chg++)
     {
     mh3SoftPionPtVsEtaVsPhiTPC[chg][icent]= NULL;
     mh3SoftPionPtVsEtaVsPhiTof[chg][icent] = NULL;
     }
   }

   for (int icent=0;icent<9;icent++)
   {
     for (int chg=0;chg<2;chg++)
     {
       mh3SoftPionPtVsEtaVsPhiQa[chg][icent]= NULL;
     }
   }
   for (int icent=0;icent<9;icent++)
   {
      mh3InvariantMassVsPtVsrapidityD0[icent] = NULL;    
      mh3InvariantMassVsPtVsrapidityD0like[icent] = NULL;          
   }
   int nRuns = mPrescales->numberOfRuns();
   TH1::SetDefaultSumw2();
   mh1TotalEventsInRun         = new TH1F("mh1TotalEventsInRun", "totalEventsInRun;runIndex;totalEventsInRun", nRuns + 1, 0, nRuns + 1);
   mh1TotalEventsInRunBeforeCut = new TH1F("mh1TotalEventsInRunBeforeCut", "totalEventsInRun;runIndex;totalEventsInRun", nRuns + 1, 0, nRuns + 1);

   //add centrality
   mh1Cent         = new TH1F("mh1Cent", "EventsVsCentrality;cent;Counts", 10, -1.5, 8.5);
   mh1CentWg         = new TH1F("mh1CentWg", "EventsVsCentrality;cent;Counts", 10, -1.5, 8.5);
   mh1gRefmultCor  = new TH1F("mh1gRefmultCor", "gRefmultCor;gRefmult;Counts", 700, 0, 700);
   mh1gRefmultCorWg  = new TH1F("mh1gRefmultCorWg", "gRefmultCorWg;gRefmultCorWg;Counts", 700, 0, 700);
   mh2CentVz         = new TH2F("mh2CentVz", "CentralityVsVz;cent;Vz", 10, -1.5, 8.5, 200, -10, 10);
   mh2CentVzWg         = new TH2F("mh2CentVzWg", "CentralityVsVzWg;cent;Vz", 10, -1.5, 8.5, 200, -10, 10);

  if (mReconstructD){
    //D0 histogram
    // mh2InvariantMassVsPt        = new TH2F("mh2InvariantMassVsPt", "invariantMassVsPt;p_{T}(K#pi)(GeV/c);m_{K#pi}(GeV/c^{2})", 120, 0, 12, 50, 1.6, 2.1);
    // mh2InvariantMassVsPtLike    = new TH2F("mh2InvariantMassVsPtLike", "invariantMassVsPtLike;p_{T}(K#pi)(GeV/c);m_{K#pi}(GeV/c^{2})", 120, 0, 12, 50, 1.6, 2.1);
    // mh2InvariantMassVsPtTof     = new TH2F("mh2InvariantMassVsPtTof", "invariantMassVsPtTof;p_{T}(K#pi)(GeV/c);m_{K#pi}(GeV/c^{2})", 120, 0, 12, 50, 1.6, 2.1);
    // mh2InvariantMassVsPtTofLike = new TH2F("mh2InvariantMassVsPtTofLike", "invariantMassVsPtTofLike;p_{T}(K#pi)(GeV/c);m_{K#pi}(GeV/c^{2})", 120, 0, 12, 50, 1.6, 2.1);
    mh3InvariantMassVsPtVsCent        = new TH3F("mh3InvariantMassVsPtVsCent", "invariantMassVsPtVsCent;p_{T}(K#pi)(GeV/c);Cent;m_{K#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 50, 1.6, 2.1);
    mh3InvariantMassVsPtVsCentLike    = new TH3F("mh3InvariantMassVsPtVsCentLike", "invariantMassVsPtVsCentLike;p_{T}(K#pi)(GeV/c);Cent;m_{K#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 50, 1.6, 2.1);
    mh3InvariantMassVsPtVsCentTof     = new TH3F("mh3InvariantMassVsPtVsCentTof", "invariantMassVsPtVsCentTof;p_{T}(K#pi)(GeV/c);Cent;m_{K#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 50, 1.6, 2.1);
    mh3InvariantMassVsPtVsCentTofLike = new TH3F("mh3InvariantMassVsPtVsCentTofLike", "invariantMassVsPtVsCentTofLike;p_{T}(K#pi)(GeV/c);Cent;m_{K#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 50, 1.6, 2.1);

   // for (int icent=0;icent<9;icent++)
   // {
   //   mh3InvariantMassVsPtVsrapidityD0[icent]= new TH3F(Form("mh3InvariantMassVsPtVsrapiditycent%d",icent),"InvariantMassVsPtVsrapidity;p_{T}(K#pi)(GeV/c);m_{K#pi}(GeV/c^{2});rapidity",120, 0, 12, 100, -1, 1, 50, 1.6, 2.1);
   //   mh3InvariantMassVsPtVsrapidityD0like[icent]= new TH3F(Form("mh3InvariantMassVsPtVsrapiditycent%dlike",icent),"InvariantMassVsPtVsrapidity;p_{T}(K#pi)(GeV/c);m_{K#pi}(GeV/c^{2});rapidity",120, 0, 12, 100, -1, 1, 50, 1.6, 2.1);
   // }

   //Dstar histogram
    // mh2InvariantMassVsPtDstar        = new TH2F("mh2InvariantMassVsPtDstar", "invariantMassVsPt;p_{T}(K#pi#pi)(GeV/c);m_{K#pi#pi}-m_{K#pi}(GeV/c^{2})", 120, 0, 12, 90, 0.135, 0.18);
    // mh2InvariantMassVsPtLikeDstar    = new TH2F("mh2InvariantMassVsPtLikeDstar", "invariantMassVsPtLike;p_{T}(K#pi#pi)(GeV/c);m_{K#pi#pi}-m_{K#pi}(GeV/c^{2})", 120, 0, 12, 90, 0.135, 0.18);
    // mh3InvariantMassVsPtVsCentDstar        = new TH3F("mh3InvariantMassVsPtVsCentDstar", "invariantMassVsPtVsCent;p_{T}(K#pi#pi)(GeV/c);Cent;m_{K#pi}-m_{K#pi#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 90, 0.135, 0.18);
    // mh3InvariantMassVsPtVsCentLikeDstar    = new TH3F("mh3InvariantMassVsPtVsCentLikeDstar", "invariantMassVsPtVsCentLike;p_{T}(K#pi#pi)(GeV/c);Cent;m_{K#pi}-m_{K#pi#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 90, 0.135, 0.18);
    // mh3MDstarVsD0PtVsCent    = new TH3F("mh3MDstarVsD0PtVsCent", "invariantMassVsD0PtVsCent;p_{T}(D^{0})(GeV/c);Cent;m_{K#pi}-m_{K#pi#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 90, 0.135, 0.18);
    // mh3MDstarVsD0PtVsCentLike    = new TH3F("mh3MDstarVsD0PtVsCentLike", "invariantMassVsD0PtVsCentLike;p_{T}(D^{0})(GeV/c);Cent;m_{K#pi}-m_{K#pi#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 90, 0.135, 0.18);
    // mh2InvariantMassVsPtSBDstar    = new TH2F("mh2InvariantMassVsPtSBDstar", "invariantMassVsPtSB;p_{T}(K#pi#pi)(GeV/c);m_{K#pi#pi}-m_{K#pi}(GeV/c^{2})", 120, 0, 12, 90, 0.135, 0.18);
    // mh3InvariantMassVsPtVsCentSBDstar    = new TH3F("mh3InvariantMassVsPtVsCentSBDstar", "invariantMassVsPtVsCentSB;p_{T}(K#pi#pi)(GeV/c);Cent;m_{K#pi}-m_{K#pi#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 90, 0.135, 0.18);

    // mh2InvariantMassVsPtDstarD0  = new TH2F("mh2InvariantMassVsPtDstarD0", "invariantMassVsPt;p_{T}(K#pi)(GeV/c);m_{K#pi}(GeV/c^{2})", 120, 0, 12, 50, 1.6, 2.1);
    // mh2InvariantMassVsPtSBD0  = new TH2F("mh2InvariantMassVsPtSBD0", "invariantMassVsPt;p_{T}(K#pi)(GeV/c);m_{K#pi}(GeV/c^{2})", 120, 0, 12, 50, 1.6, 2.1);
  
     //Dstar mixed event
    // mh2InvariantMassVsPtDstarMixed = new TH2F("mh2InvariantMassVsPtDstarMixed", "invariantMassVsPtMixedEvent;p_{T}(K#pi#pi)(GeV/c);m_{K#pi#pi}-m_{K#pi}(GeV/c^{2})", 120, 0, 12, 90, 0.135, 0.18);
    // mh3InvariantMassVsPtVsCentDstarMixed = new TH3F("mh3InvariantMassVsPtVsCentDstarMixedEvent", "invariantMassVsPtVsCentDstarMixedEvent;p_{T}(K#pi#pi)(GeV/c);Cent;m_{K#pi}-m_{K#pi#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 90, 0.135, 0.18);
    // mh3MDstarVsD0PtVsCentDstarMixed = new TH3F("mh3MDstarVsD0PtVsCentDstarMixedEvent", "invariantMassVsD0PtVsCentDstarMixedEvent;p_{T}(D^{0})(GeV/c);Cent;m_{K#pi}-m_{K#pi#pi}(GeV/c^{2})", 120, 0, 12, 10, -1.5, 8.5, 90, 0.135, 0.18);
    buffer = new TH3F("mh3eventinbufferVsVzVsCent", "eventinbufferVsPtVsCent;Vzbin;Cent;Counts", 12, -1, 11, 11, -1, 10, 6, 0, 6);
    
  //THn histograms
  //D* pt, pi pt, centrality, D*-D0 mass, charge
    int const dim = 6;
    int nbins[dim] = {120,120,80,10,90,2};
    double xmin[dim] = {0.,0.,0.,-1.5, 0.135,-1.5};
    double xmax[dim] = {12.,12.,4., 8.5, 0.18,1.5};
    mhnDstarD0PiMassCentCharge = new THnSparseF("mhnDstarD0PiMassCentCharge","mhnDstarD0PiMassCentCharge",dim,nbins,xmin,xmax);
    mhnDstarD0PiMassCentChargeLK = new THnSparseF("mhnDstarD0PiMassCentChargeLK","mhnDstarD0PiMassCentChargeLK",dim,nbins,xmin,xmax);
    mhnDstarD0PiMassCentChargeSB = new THnSparseF("mhnDstarD0PiMassCentChargeSB","mhnDstarD0PiMassCentChargeSB",dim,nbins,xmin,xmax);
    mhnDstarD0PiMassCentChargeMix = new THnSparseF("mhnDstarD0PiMassCentChargeMix","mhnDstarD0PiMassCentChargeMix",dim,nbins,xmin,xmax);
    mhnDstarD0PiMassCentChargeMixLK = new THnSparseF("mhnDstarD0PiMassCentChargeMixLK","mhnDstarD0PiMassCentChargeMixLK",dim,nbins,xmin,xmax);
    mhnDstarD0PiMassCentCharge->Sumw2();
    mhnDstarD0PiMassCentChargeLK->Sumw2();
    mhnDstarD0PiMassCentChargeMix->Sumw2();
    mhnDstarD0PiMassCentChargeMixLK->Sumw2();
    mhnDstarD0PiMassCentChargeSB->Sumw2();
  }  //reconstructD

   if (mFillSoftPionEff)
    {
      /*
      for (int icent=0;icent<9;icent++)
      {
        mh3SoftPionPtVsEtaVsPhiTPC[0][icent]= new TH3F(Form("mh3SoftPionPtVsEtaVsPhiTPC_minus_cent%d",icent),"SoftPionPtVsEtaVsPhiTPC;p_{T}(#pi)(GeV/c);Eta;Phi",500,0,10,50,-1,1,50,-3.14159,3.14159);
        mh3SoftPionPtVsEtaVsPhiTof[0][icent] = new TH3F(Form("mh3SoftPionPtVsEtaVsPhiTof_minus_cent%d",icent),"mh3SoftPionPtVsEtaVsPhiTof;p_{T}(#pi)(GeV/c);Eta;Phi",500,0,10,50,-1,1,50,-3.14159,3.14159);
        
        mh3SoftPionPtVsEtaVsPhiTPC[1][icent]= new TH3F(Form("mh3SoftPionPtVsEtaVsPhiTPC_plus_cent%d",icent),"SoftPionPtVsEtaVsPhiTPC;p_{T}(#pi)(GeV/c);Eta;Phi",500,0,10,50,-1,1,50,-3.14159,3.14159);
        mh3SoftPionPtVsEtaVsPhiTof[1][icent] = new TH3F(Form("mh3SoftPionPtVsEtaVsPhiTof_plus_cent%d",icent),"mh3SoftPionPtVsEtaVsPhiTof;p_{T}(#pi)(GeV/c);Eta;Phi",500,0,10,50,-1,1,50,-3.14159,3.14159);
      }
*/
      mh3SoftPionPtVsdiffInvBetaVsCent = new TH3F("mh3SoftPionPtVsdiffInvBetaVsEta","SoftPionPtVsdiffInvBetaVsEta;p_{T}(GeV/c);diffInvBeta;Eta",200,0,2,200,-0.1,0.1,10,-1,1);
      mh3SoftPionPtVsBetaVsCent = new TH3F("mh3SoftPionPtVsBetaVsEta","SoftPionPtVsBetaVsCent;p_{T}(GeV/c);InvBeta;Eta",200,0,2,600,0.5,5.5,10,-1,1);
      mh3SoftPionPtVsnSigmaVsCent = new TH3F("mh3SoftPionPtVsnSigmaVsCent","SoftPionPtVsnSigmaVsCent;p_{T}(GeV/c);nSigma;Cent",200,0,2,50,-5,5,10,-0.5,9.5);
    }  //softpioneff

    if(mSoftPionQa){
      for (int icent=0;icent<9;icent++)
    {
      mh3SoftPionPtVsEtaVsPhiQa[1][icent]= new TH3F(Form("mh3SoftPionPtVsEtaVsPhiQa_plus_cent%d",icent),"mh3SoftPionPtVsEtaVsPhiQa;p_{T}(#pi)(GeV/c);Eta;Phi",500,0,10,10,-1,1,10,-3.14159,3.14159); 
      mh3SoftPionPtVsEtaVsPhiQa[0][icent]= new TH3F(Form("mh3SoftPionPtVsEtaVsPhiQa_minus_cent%d",icent),"mh3SoftPionPtVsEtaVsPhiQa;p_{T}(#pi)(GeV/c);Eta;Phi",500,0,10,10,-1,1,10,-3.14159,3.14159); 
    }
    mh3SoftPionDcaVsPtVsCent = new TH3F("mh3SoftPionDcaVsPtVsCent","SoftPionCountsDcaVsPtVscent;p_{T}(#pi)(GeV/c);Cent;Dca(mm)",120,0,12,10,-1.5,8.5,350,-3.5,3.5);
  }

   if(mFillBackgroundTrees)
   {
     TString var = "m:pt:decayLength:dca12:dcaV0ToPv:ptKaon:dcaKaon:ptPion:dcaPion";

     for(unsigned int iNt=0; iNt<anaCuts::nPtBins; ++iNt)
     {
       TString nameSS = Form("ntpDstarBackgroundSameSignPt%i%i",(int)anaCuts::PtBinsEdge[iNt],(int)anaCuts::PtBinsEdge[iNt+1]);
       TString nameSB = Form("ntpDstarBackgroundSideBandPt%i%i",(int)anaCuts::PtBinsEdge[iNt],(int)anaCuts::PtBinsEdge[iNt+1]);
       mNtDstarBackgroungSameSign[iNt] = new TNtuple(nameSS.Data(),"",var.Data());
       mNtDstarBackgroungSideBand[iNt] = new TNtuple(nameSB.Data(),"",var.Data());
     }
   }

   /******************************************************************************************/
   /*             NOTE: All histograms below will not be defined if mFillQaHists is not true */
   /******************************************************************************************/

   if (!mFillQaHists) return;

   //Add some HFT ratio plots
   mh2Tpc1PtCent  = new TH2F("mh2Tpc1PtCent", "Tpc tacks;p_{T}(GeV/c);cent", 120, 0, 12, 10, -1.5, 8.5); //Dca 1.5cm
   mh2HFT1PtCent  = new TH2F("mh2HFT1PtCent", "HFT tacks;p_{T}(GeV/c);cent", 120, 0, 12, 10, -1.5, 8.5); //Dca 1.5cm
   mh2Tpc1PhiVz  = new TH2F("mh2Tpc1PhiVz", "Tpc tacks;#Phi;Vz", 100, -3.1415, 3.1415, 20, -10, 10); //Dca 1.5cm
   mh2HFT1PhiVz  = new TH2F("mh2HFT1PhiVz", "HFT tacks;#Phi;Vz", 100, -3.1415, 3.1415, 20, -10, 10); //Dca 1.5cm

   for (int iParticle = 0; iParticle < anaCuts::nParticles; iParticle++)
   {
      for (int iEta = 0; iEta < anaCuts::nEtasRatio; iEta++)
      {
         for (int iVz = 0; iVz < anaCuts::nVzsRatio; iVz++)
         {
            for (int iPhi = 0; iPhi < anaCuts::nPhisRatio; iPhi++)
            {
               mh2Tpc1PtCentPartEtaVzPhi[iParticle][iEta][iVz][iPhi]  = new TH2F(Form("mh2Tpc1PtCentPartEtaVzPhi_%d_%d_%d_%d", iParticle, iEta, iVz, iPhi), Form("mh2Tpc1PtCent_%s_Eta%2.1f_Vz%2.1f_Phi%2.1f;p_{T}(GeV/c);cent", anaCuts::ParticleName[iParticle], anaCuts::EtaEdgeRatio[iEta], anaCuts::VzEdgeRatio[iVz], anaCuts::PhiEdgeRatio[iPhi]), anaCuts::nPtsRatio, anaCuts::PtEdgeRatio, anaCuts::nCentsRatio, anaCuts::CentEdgeRatio); //Dca 1.cm
               mh2HFT1PtCentPartEtaVzPhi[iParticle][iEta][iVz][iPhi]  = new TH2F(Form("mh2HFT1PtCentPartEtaVzPhi_%d_%d_%d_%d", iParticle, iEta, iVz, iPhi), Form("mh2HFT1PtCent_%s_Eta%2.1f_Vz%2.1f_Phi%2.1f;p_{T}(GeV/c);cent", anaCuts::ParticleName[iParticle], anaCuts::EtaEdgeRatio[iEta], anaCuts::VzEdgeRatio[iVz], anaCuts::PhiEdgeRatio[iPhi]), anaCuts::nPtsRatio, anaCuts::PtEdgeRatio, anaCuts::nCentsRatio, anaCuts::CentEdgeRatio); //Dca 1.cm
            }
         }
      }
   }

   // Add some Dca, resolution
   for (int iParticle = 0; iParticle < anaCuts::nParticles; iParticle++)
   {
      for (int iEta = 0; iEta < anaCuts::nEtasDca; iEta++)
      {
         for (int iVz = 0; iVz < anaCuts::nVzsDca; iVz++)
         {
            for (int iCent = 0; iCent < anaCuts::nCentsDca; iCent++)
            {
               mh3DcaXyZPtCentPartEtaVzPhi[iParticle][iEta][iVz][iCent]  = new TH3F(Form("mh3DcaXyZPtCentPartEtaVzPhi_%d_%d_%d_%d", iParticle, iEta, iVz, iCent), Form("mh3DcaXyZPt_%s_Eta%2.1f_Vz%2.1f_Cent%2.1f;p_{T}(GeV/c);DcaXy(cm);DcaZ(cm)", anaCuts::ParticleName[iParticle], anaCuts::EtaEdgeDca[iEta], anaCuts::VzEdgeDca[iVz], anaCuts::CentEdgeDca[iCent]), anaCuts::nPtsDca, anaCuts::PtEdgeDca, anaCuts::nDcasDca, anaCuts::DcaEdgeDca, anaCuts::nDcasDca, anaCuts::DcaEdgeDca); //Dca 1.cm
            }
         }
      }
   }

   mh3DcaPtCent  = new TH3F("mh3DcaPtCent", "mh3DcaPtCent;p_{T}(GeV/c);cent;Dca(cm)", 120, 0, 12, 10, -1.5, 8.5, 1000, -1, 1); //Dca 1.cm
   mh3DcaXyPtCent  = new TH3F("mh3DcaXyPtCent", "mh3DcaXyPtCent;p_{T}(GeV/c);cent;DcaXy(cm)", 120, 0, 12, 10, -1.5, 8.5, 1000, -1, 1); //Dca 1.cm
   mh3DcaZPtCent  = new TH3F("mh3DcaZPtCent", "mh3DcaZPtCent;p_{T}(GeV/c);cent;DcaZ(cm)", 120, 0, 12, 10, -1.5, 8.5, 1000, -1, 1); //Dca 1.cm

//  nt = new TNtuple("nt","nt","runnumber:dca:vz:pt:eta:phi:centrality:grefmultCor:zdcCoincidance:tofMatchFlag:hftMatchFlag");

}

StPicoDstarMixedHists::~StPicoDstarMixedHists()
{
   delete mPrescales;
   // note that histograms are owned by mOutFile. They will be destructed
   // when the file is closed.
}
//-----------------------------------------------------------------------
void StPicoDstarMixedHists::addEvent(StPicoEvent const* const picoEvent)
{
   int runIndex = mPrescales->runIndex(picoEvent->runId());
   mh1TotalEventsInRun->Fill(runIndex);
}
//-----------------------------------------------------------------------
void StPicoDstarMixedHists::addEventBeforeCut(StPicoEvent const* const picoEvent)
{
   int runIndex = mPrescales->runIndex(picoEvent->runId());
   mh1TotalEventsInRunBeforeCut->Fill(runIndex);
}
//-----------------------------------------------------------------------
void StPicoDstarMixedHists::addCent(const double refmultCor, int centrality, const double reweight, const float vz)
{
   mh1gRefmultCor->Fill(refmultCor);
   mh1gRefmultCorWg->Fill(refmultCor, reweight);
   mh1Cent->Fill(centrality);
   mh1CentWg->Fill(centrality, reweight);
   mh2CentVz->Fill(centrality, vz);
   mh2CentVzWg->Fill(centrality, vz, reweight);
}
//-----------------------------------------------------------------------
void StPicoDstarMixedHists::addTpcDenom1(bool IsPion, bool IsKaon, bool IsProton, float pt, int centrality, float Eta, float Phi, float Vz, float ZdcX)
{
   if (!mFillQaHists)
   {
      LOG_ERROR << " You are trying to fill QA histograms but StPicoDstarMixedHists::mFillQaHists is false -- ignoring attemp! " << endm;
   }

   int EtaIndex = getEtaIndexRatio(Eta);
   int PhiIndex = getPhiIndexRatio(Phi);
   int VzIndex = getVzIndexRatio(Vz);
   if (IsPion)
   {
      mh2Tpc1PtCentPartEtaVzPhi[0][EtaIndex][VzIndex][PhiIndex]->Fill(pt, centrality);
   }
   if (IsKaon)
   {
      mh2Tpc1PtCentPartEtaVzPhi[1][EtaIndex][VzIndex][PhiIndex]->Fill(pt, centrality);
   }
   if (IsProton)
   {
      mh2Tpc1PtCentPartEtaVzPhi[2][EtaIndex][VzIndex][PhiIndex]->Fill(pt, centrality);
   }
   mh2Tpc1PtCent->Fill(pt, centrality);
   if (fabs(Eta) < 0.1 && pt > 3.0) mh2Tpc1PhiVz->Fill(Phi, Vz);

}
//-----------------------------------------------------------------------
void StPicoDstarMixedHists::addHFTNumer1(bool IsPion, bool IsKaon, bool IsProton, float pt, int centrality, float Eta, float Phi, float Vz, float ZdcX)
{
   if (!mFillQaHists)
   {
      LOG_ERROR << " You are trying to fill QA histograms but StPicoDstarMixedHists::mFillQaHists is false -- ignoring attemp! " << endm;
   }

   int EtaIndex = getEtaIndexRatio(Eta);
   int PhiIndex = getPhiIndexRatio(Phi);
   int VzIndex = getVzIndexRatio(Vz);
   if (IsPion)
   {
      mh2HFT1PtCentPartEtaVzPhi[0][EtaIndex][VzIndex][PhiIndex]->Fill(pt, centrality);
   }
   if (IsKaon)
   {
      mh2HFT1PtCentPartEtaVzPhi[1][EtaIndex][VzIndex][PhiIndex]->Fill(pt, centrality);
   }
   if (IsProton)
   {
      mh2HFT1PtCentPartEtaVzPhi[2][EtaIndex][VzIndex][PhiIndex]->Fill(pt, centrality);
   }
   mh2HFT1PtCent->Fill(pt, centrality);
   if (fabs(Eta) < 0.1 && pt > 3.0) mh2HFT1PhiVz->Fill(Phi, Vz);
}
//-----------------------------------------------------------------------
void StPicoDstarMixedHists::addKaonPion(StKaonPion const* const kp, bool unlike, bool tpc, bool tof, int centrality, const double reweight)
{
   if (unlike)
   {
      // if (tpc) mh2InvariantMassVsPt->Fill(kp->pt(), kp->m(), reweight);
      // if (tof) mh2InvariantMassVsPtTof->Fill(kp->pt(), kp->m(), reweight);
      if (tpc) mh3InvariantMassVsPtVsCent->Fill(kp->pt(), centrality, kp->m(), reweight);
      if (tof) mh3InvariantMassVsPtVsCentTof->Fill(kp->pt(), centrality, kp->m(), reweight);
  //    if (tof) mh3InvariantMassVsPtVsrapidityD0[centrality]->Fill(kp->pt(),kp->lorentzVector().rapidity(),kp->m(), reweight) ;
   }
   else
   {
      // if (tpc) mh2InvariantMassVsPtLike->Fill(kp->pt(), kp->m(), reweight);
      // if (tof) mh2InvariantMassVsPtTofLike->Fill(kp->pt(), kp->m(), reweight);
      if (tpc) mh3InvariantMassVsPtVsCentLike->Fill(kp->pt(), centrality, kp->m(), reweight);
      if (tof) mh3InvariantMassVsPtVsCentTofLike->Fill(kp->pt(), centrality, kp->m(), reweight);
     
//     if (tof) mh3InvariantMassVsPtVsrapidityD0like[centrality]->Fill(kp->pt(),kp->lorentzVector().rapidity(), kp->m(),reweight) ;
   }
}
//-----------------------------------------------------------------------
void StPicoDstarMixedHists::addBackground(StKaonPion const* const kp, StPicoTrack const* const kaon, StPicoTrack const* const pion, int const ptBin, bool const SB)
{
     if(mFillBackgroundTrees)
     {
       TNtuple* const nt = SB ? mNtDstarBackgroungSideBand[ptBin]: mNtDstarBackgroungSameSign[ptBin];
       nt->Fill(kp->m(), kp->pt(), kp->decayLength(), kp->dcaDaughters(), kp->perpDcaToVtx(), kaon->gPt(), kp->kaonDca(), pion->gPt(), kp->pionDca());
     }
}
//---------------------------------------------------------------------
void StPicoDstarMixedHists::addDcaPtCent(float dca, float dcaXy, float dcaZ, bool IsPion, bool IsKaon, bool IsProton, float pt,  int centrality, float Eta, float Phi, float Vz, float ZdcX)
{
   if (!mFillQaHists)
   {
      LOG_ERROR << " You are trying to fill QA histograms but StPicoDstarMixedHists::mFillQaHists is false -- ignoring attemp! " << endm;
   }

   int EtaIndex = getEtaIndexDca(Eta);
   // int PhiIndex = getPhiIndexDca(Phi);
   int VzIndex = getVzIndexDca(Vz);

   if (centrality < 0) return; // remove bad centrality, only keep 9 centralities

   if (IsPion)
   {
      mh3DcaXyZPtCentPartEtaVzPhi[0][EtaIndex][VzIndex][centrality]->Fill(pt, dcaXy, dcaZ);
   }
   if (IsKaon)
   {
      mh3DcaXyZPtCentPartEtaVzPhi[1][EtaIndex][VzIndex][centrality]->Fill(pt, dcaXy, dcaZ);
   }
   if (IsProton)
   {
      mh3DcaXyZPtCentPartEtaVzPhi[2][EtaIndex][VzIndex][centrality]->Fill(pt, dcaXy, dcaZ);
   }
   mh3DcaPtCent->Fill(pt, centrality, dca);
   mh3DcaXyPtCent->Fill(pt, centrality, dcaXy);
   mh3DcaZPtCent->Fill(pt, centrality, dcaZ);
}
//---------------------------------------------------------------------
int StPicoDstarMixedHists::getEtaIndexDca(float Eta)
{
   for (int i = 0; i < anaCuts::nEtasDca; i++)
   {
      if ((Eta >= anaCuts::EtaEdgeDca[i]) && (Eta < anaCuts::EtaEdgeDca[i + 1]))
         return i;
   }
   return anaCuts::nEtasDca - 1;
}
//---------------------------------------------------------------------
int StPicoDstarMixedHists::getPhiIndexDca(float Phi)
{
   for (int i = 0; i < anaCuts::nPhisDca; i++)
   {
      if ((Phi >= anaCuts::PhiEdgeDca[i]) && (Phi < anaCuts::PhiEdgeDca[i + 1]))
         return i;
   }
   return anaCuts::nPhisDca - 1;
}
//---------------------------------------------------------------------
int StPicoDstarMixedHists::getVzIndexDca(float Vz)
{
   for (int i = 0; i < anaCuts::nVzsDca; i++)
   {
      if ((Vz >= anaCuts::VzEdgeDca[i]) && (Vz < anaCuts::VzEdgeDca[i + 1]))
         return i;
   }
   return anaCuts::nVzsDca - 1;
}
//---------------------------------------------------------------------
// int StPicoDstarMixedHists::getZdcxIndex(float ZdcX)
// {
//    for (int i = 0; i < anaCuts::nZdcxs; i++)
//    {
//       if ((ZdcX >= anaCuts::ZdcxEdge[i]) && (ZdcX < anaCuts::ZdcxEdge[i + 1]))
//          return i;
//    }
//    return anaCuts::nZdcxs - 1;
// }
//---------------------------------------------------------------------
int StPicoDstarMixedHists::getEtaIndexRatio(float Eta)
{
   for (int i = 0; i < anaCuts::nEtasRatio; i++)
   {
      if ((Eta >= anaCuts::EtaEdgeRatio[i]) && (Eta < anaCuts::EtaEdgeRatio[i + 1]))
         return i;
   }
   return anaCuts::nEtasRatio - 1;
}
//---------------------------------------------------------------------
int StPicoDstarMixedHists::getPhiIndexRatio(float Phi)
{
   for (int i = 0; i < anaCuts::nPhisRatio; i++)
   {
      if ((Phi >= anaCuts::PhiEdgeRatio[i]) && (Phi < anaCuts::PhiEdgeRatio[i + 1]))
         return i;
   }
   return anaCuts::nPhisRatio - 1;
}
//---------------------------------------------------------------------
int StPicoDstarMixedHists::getVzIndexRatio(float Vz)
{
   for (int i = 0; i < anaCuts::nVzsRatio; i++)
   {
      if ((Vz >= anaCuts::VzEdgeRatio[i]) && (Vz < anaCuts::VzEdgeRatio[i + 1]))
         return i;
   }
   return anaCuts::nVzsRatio - 1;
}
//---------------------------------------------------------------------
void StPicoDstarMixedHists::addQaNtuple(int runnumber, float dca, float vz, float pt, float eta, float phi, int centrality, const double refmultCor, float zdcx, int tofMatchFlag, int hftMatchFlag)
{
//  nt->Fill(runnumber, dca, vz, pt, eta, phi, centrality, refmultCor, zdcx, tofMatchFlag, hftMatchFlag);
}
//---------------------------------------------------------------------
void StPicoDstarMixedHists::addSoftPionEff(StThreeVectorF const& spMom, bool tpcpion, bool tofavailable, bool tofPion, double diffBeta, double nSigmaPion, double beta_pi, int centrality, const double reweight, short const charge)
{
  /*
  //tof match efficiency
  if (tpcpion) {
    if (charge<0)  mh3SoftPionPtVsEtaVsPhiTPC[0][centrality]->Fill(spMom.perp(),spMom.pseudoRapidity(), spMom.phi(), reweight);
    if (charge>0) mh3SoftPionPtVsEtaVsPhiTPC[1][centrality]->Fill(spMom.perp(),spMom.pseudoRapidity(), spMom.phi(), reweight);
  
  if (tofavailable) {
    if (charge<0)  mh3SoftPionPtVsEtaVsPhiTof[0][centrality]->Fill(spMom.perp(),spMom.pseudoRapidity(), spMom.phi(), reweight);
    if (charge>0) mh3SoftPionPtVsEtaVsPhiTof[1][centrality]->Fill(spMom.perp(),spMom.pseudoRapidity(), spMom.phi(), reweight);
    }
  }
  */
  //tpc pid efficiency
  if (tofPion){
  mh3SoftPionPtVsnSigmaVsCent->Fill(spMom.perp(), nSigmaPion, centrality, reweight);
  }

  //tof pid efficiency
  if (tpcpion && tofavailable && fabs(diffBeta)<0.1){
    mh3SoftPionPtVsdiffInvBetaVsCent->Fill(spMom.perp(), diffBeta,spMom.pseudoRapidity(), reweight);
      //
  }
  mh3SoftPionPtVsBetaVsCent->Fill(spMom.mag(), beta_pi, spMom.pseudoRapidity(), reweight);
}
//---------------------------------------------------------------------
void StPicoDstarMixedHists::addSoftPionQa(StThreeVectorF const& spMom,float spdca, int centrality, const double reweight, short const charge)
{
  if (charge<0)  mh3SoftPionPtVsEtaVsPhiQa[0][centrality]->Fill(spMom.perp(),spMom.pseudoRapidity(), spMom.phi());
  if (charge>0) mh3SoftPionPtVsEtaVsPhiQa[1][centrality]->Fill(spMom.perp(),spMom.pseudoRapidity(), spMom.phi());
  mh3SoftPionDcaVsPtVsCent->Fill(spdca, spMom.perp(), centrality, reweight);
}
//---------------------------------------------------------------------
void StPicoDstarMixedHists::addD0SoftPion(StD0Pion const* const d0p, StKaonPion const* const kp, bool unlike, int centrality, const double reweight)
{
   double y[6]={d0p->pt(),kp->pt() ,d0p->PilorentzVector().perp(),centrality,d0p->m()-kp->m(),d0p->charge()}; 
    if (unlike)
   {
      // mh2InvariantMassVsPtDstar->Fill(d0p->pt(), d0p->m()-kp->m(), reweight);
      // mh3InvariantMassVsPtVsCentDstar->Fill(d0p->pt(), centrality, d0p->m()-kp->m(), reweight);
//to check if the D0 mass range is right;
      // mh2InvariantMassVsPtDstarD0->Fill(kp->pt(), kp->m(), reweight);
      // mh3MDstarVsD0PtVsCent->Fill(kp->pt(), centrality , d0p->m()-kp->m(),reweight);
      mhnDstarD0PiMassCentCharge->Fill(y,reweight);
   }
   else
   {
      // mh2InvariantMassVsPtLikeDstar->Fill(d0p->pt(), d0p->m()-kp->m(), reweight);
      // mh3InvariantMassVsPtVsCentLikeDstar->Fill(d0p->pt(), centrality, d0p->m()-kp->m(), reweight);
      // mh3MDstarVsD0PtVsCentLike->Fill(kp->pt(), centrality , d0p->m()-kp->m(),reweight);
      mhnDstarD0PiMassCentChargeLK->Fill(y,reweight);
   }
 
}
//---------------------------------------------------------------------------
void StPicoDstarMixedHists::addDstarHists(StD0Pion const* const d0p, StKaonPion const* const kp, float const spi_pt, bool unlike, int centrality, const double reweight)
{
//left to add ... 
}
//
void StPicoDstarMixedHists::addSideBandBackground(StD0Pion const* const d0p, StKaonPion const* const kp,bool unlike, int centrality, const double reweight)
{
   double y[6]={d0p->pt(),d0p->D0lorentzVector().perp(),d0p->PilorentzVector().perp(),centrality,d0p->m()-kp->m(),d0p->charge()}; 
   if (unlike){
     // mh2InvariantMassVsPtSBD0->Fill(kp->pt(), kp->m(), reweight);
     // mh2InvariantMassVsPtSBDstar->Fill(d0p->pt(), d0p->m()-kp->m(), reweight);
     // mh3InvariantMassVsPtVsCentSBDstar->Fill(d0p->pt(), centrality, d0p->m()-kp->m(), reweight);
     mhnDstarD0PiMassCentChargeSB->Fill(y,reweight);
   }
}

void StPicoDstarMixedHists::addMixedEventBackground(StMixedD0Pion const & mixD0Pion,bool unlike,int centrality,const double reweight)
{
    double y[6]={mixD0Pion.pt(), mixD0Pion.D0lorentzVector().perp(), mixD0Pion.PilorentzVector().perp(),centrality,mixD0Pion.deltaM(),mixD0Pion.charge()};
    if (unlike && mixD0Pion.deltaM()<0.2 && mixD0Pion.deltaM()>0.1){
      // mh2InvariantMassVsPtDstarMixed->Fill(mixD0Pion.pt(),mixD0Pion.deltaM(),reweight);
      // mh3InvariantMassVsPtVsCentDstarMixed->Fill(mixD0Pion.pt(),centrality, mixD0Pion.deltaM(),reweight);
      // mh3MDstarVsD0PtVsCentDstarMixed->Fill(mixD0Pion.D0lorentzVector().perp(), centrality, mixD0Pion.deltaM(),reweight);
      mhnDstarD0PiMassCentChargeMix->Fill(y, reweight);
    }
    if ((!unlike)&&mixD0Pion.deltaM()<0.2&&mixD0Pion.deltaM()>0.1){
      mhnDstarD0PiMassCentChargeMixLK->Fill(y,reweight); 
    }
}

void StPicoDstarMixedHists::addeventsinbuffer(const int vzbin,const int centrality,const int size)
{
      buffer->Fill(vzbin,centrality, size);
}

void StPicoDstarMixedHists::closeFile()
{
   mOutFile->cd();

   mh1TotalEventsInRun->Write();
   mh1TotalEventsInRunBeforeCut->Write();
  
   //centrality
   mh1Cent->Write();
   mh1CentWg->Write();
   mh1gRefmultCor->Write();
   mh1gRefmultCorWg->Write();
   mh2CentVz->Write();
   mh2CentVzWg->Write();

   if (mReconstructD){
   //D0
   // mh2InvariantMassVsPt->Write();
   // mh2InvariantMassVsPtLike->Write();
   // mh2InvariantMassVsPtTof->Write();
   // mh2InvariantMassVsPtTofLike->Write();
   mh3InvariantMassVsPtVsCent->Write();
   mh3InvariantMassVsPtVsCentLike->Write();
   mh3InvariantMassVsPtVsCentTof->Write();
   mh3InvariantMassVsPtVsCentTofLike->Write();
 
   // for (int icent=0;icent<9;icent++)
   // {
   //   mh3InvariantMassVsPtVsrapidityD0[icent]->Write();
   //   mh3InvariantMassVsPtVsrapidityD0like[icent]->Write();
   // }
   //Dstar
   // mh2InvariantMassVsPtDstar->Write();
   // mh2InvariantMassVsPtSBDstar->Write();
   // mh2InvariantMassVsPtLikeDstar->Write();
   // mh3InvariantMassVsPtVsCentDstar->Write();
   // mh3InvariantMassVsPtVsCentSBDstar->Write();
   // mh3InvariantMassVsPtVsCentLikeDstar->Write();
   // mh2InvariantMassVsPtSBD0->Write();
   // mh2InvariantMassVsPtDstarD0->Write();
   // mh3MDstarVsD0PtVsCent->Write();
   // mh3MDstarVsD0PtVsCentLike->Write();
   //Dstar mixed event
   // mh2InvariantMassVsPtDstarMixed->Write();
   // mh3InvariantMassVsPtVsCentDstarMixed->Write();
   // mh3MDstarVsD0PtVsCentDstarMixed->Write();
   buffer->Write();
   
   //THn histograms
   mhnDstarD0PiMassCentCharge->Write();
   mhnDstarD0PiMassCentChargeLK->Write();
   mhnDstarD0PiMassCentChargeMix->Write();
   mhnDstarD0PiMassCentChargeMixLK->Write();
   mhnDstarD0PiMassCentChargeSB->Write();
   }  //resconstructD
      
//spion Qa
    if (mSoftPionQa)
  {
   for (int icent=0;icent<9;icent++)
   {
     for (int chg=0;chg<2;chg++)
     {
      mh3SoftPionPtVsEtaVsPhiQa[chg][icent]->Write();
     }
   }
   mh3SoftPionDcaVsPtVsCent->Write();
  }

   //spion eff
   if (mFillSoftPionEff)
   {
     /*
      for (int icent=0;icent<9;icent++)
      {
        for (int chg=0;chg<2;chg++)
        {
         mh3SoftPionPtVsEtaVsPhiTof[chg][icent]->Write();
         mh3SoftPionPtVsEtaVsPhiTPC[chg][icent]->Write();
        }
      }
*/
      mh3SoftPionPtVsdiffInvBetaVsCent->Write();
      mh3SoftPionPtVsBetaVsCent->Write();
      mh3SoftPionPtVsnSigmaVsCent->Write();
   }

   if(mFillBackgroundTrees)
   {
     for(unsigned int iNt=0; iNt<anaCuts::nPtBins; ++iNt)
     {
       mNtDstarBackgroungSameSign[iNt]->Write();
       mNtDstarBackgroungSideBand[iNt]->Write();
     }
   }

   if (!mFillQaHists)
   {
      mOutFile->Close();
      mOutFile->Delete("*;*");
      return;
   }

   //HFT ratio QA
   mh2Tpc1PtCent->Write();
   mh2Tpc1PhiVz->Write();
   mh2HFT1PhiVz->Write();
   mh2HFT1PtCent->Write();

   //HFT DCA Ratio
   for (int iParticle = 0; iParticle < anaCuts::nParticles; iParticle++)
   {
      for (int iEta = 0; iEta < anaCuts::nEtasDca; iEta++)
      {
         for (int iVz = 0; iVz < anaCuts::nVzsDca; iVz++)
         {
            for (int iCent = 0; iCent < anaCuts::nCentsDca; iCent++)
            {
               mh3DcaXyZPtCentPartEtaVzPhi[iParticle][iEta][iVz][iCent]->Write();
            }
         }
      }
   }

   for (int iParticle = 0; iParticle < anaCuts::nParticles; iParticle++)
   {
      for (int iEta = 0; iEta < anaCuts::nEtasRatio; iEta++)
      {
         for (int iVz = 0; iVz < anaCuts::nVzsRatio; iVz++)
         {
            for (int iPhi = 0; iPhi < anaCuts::nPhisRatio; iPhi++)
            {
               mh2Tpc1PtCentPartEtaVzPhi[iParticle][iEta][iVz][iPhi]->Write();
               mh2HFT1PtCentPartEtaVzPhi[iParticle][iEta][iVz][iPhi]->Write();
            }
         }
      }

   }

   mh3DcaPtCent->Write();
   mh3DcaXyPtCent->Write();
   mh3DcaZPtCent->Write();

   // nt->Write();

   mOutFile->Close();
   mOutFile->Delete();
}
