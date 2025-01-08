#include "Celestial.hh"
void EarthOrbit(){
    TVector3 EarthCenter(0,0,0);
    TVector3 Stop(0,0,0);
    double SateliteMass = 1;
    Celestial* Earth = new Celestial(EarthMass,EarthCenter,Stop);
 //   Spaceship* Satelite = new Spaceship(SateliteMass,TVector3(0,7e6,0),TVector3(7.9e3,0,0));
    Spaceship* Satelite = new Spaceship(TVector3(0,7e6,0),TVector3(7.9e3,0,0));
    World EO;
    EO.AddPlanet(Earth);
    EO.AddPlanet(Satelite);

    int nstep = 10000;
    vector<double> timestep;
    vector<TVector3> EarthPos;
    vector<TVector3> SatelitePos;
    double dt = 1;
    int sample = 10/dt;
    for(int i=0;i<nstep*sample;++i){
        EO.ProcessStep(dt);
        if(i%sample==0){
            EarthPos.push_back(Earth->GetPosition());
            SatelitePos.push_back(Satelite->GetPosition());
            timestep.push_back((i+1)*dt);
        }
        if(i%10000==0){
            cout<<Form("Step %d",i+1)<<endl;
        }
    }
    vector<double> EarthX;  
    vector<double> EarthY;
    vector<double> EarthZ;
    vector<double> SateliteX;
    vector<double> SateliteY;
    vector<double> SateliteZ;
    for(auto V:EarthPos){
        EarthX.push_back(V.X());
        EarthY.push_back(V.Y());
        EarthZ.push_back(V.Z());
    }
    for(auto V:SatelitePos){
        SateliteX.push_back(V.X());
        SateliteY.push_back(V.Y());
        SateliteZ.push_back(V.Z());
    }
    TGraph* grEarth = new TGraph();
    TGraph* grSatelite = new TGraph();
    for(int i=0;i<nstep;++i){
        grEarth->SetPoint(i,EarthX[i],EarthY[i]);
        grSatelite->SetPoint(i,SateliteX[i],SateliteY[i]);
    }
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    grSatelite->SetMarkerSize(2);
    grSatelite->SetMarkerStyle(22);
    grSatelite->Draw("AL");
    grEarth->SetMarkerSize(2);
    grEarth->SetMarkerStyle(22);
    grEarth->SetLineColor(kRed);
    grEarth->SetMarkerColor(kRed);
    grEarth->Draw("PLSAME");
}