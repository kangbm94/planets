#include "Celestial.hh"
void EarthMoon(){
	TVector3 stop(0,0,0);
	TVector3 center(0,0,0);
	TVector3 MoonPosition(0,3.844e8,0);
	TVector3 MoonVelo(1.022e3,0,0);
	TVector3 EarthVelo = - (MoonMass/EarthMass) * MoonVelo;//to set CM momentum 0
	Celestial* Earth = new Celestial(EarthMass,center,EarthVelo);
	Celestial* Moon = new Celestial(MoonMass,MoonPosition,MoonVelo);
	World EM;
	EM.AddPlanet(Earth);
	EM.AddPlanet(Moon);
	vector<TVector3> EarthPos;
	vector<TVector3> MoonPos;
 	int nstep = 86400*60.;
	int scale = 1000;
	vector<double> times;
	vector<double> EarthX;
	vector<double> EarthY;
	vector<double> EarthZ;
	vector<double> MoonX;
	vector<double> MoonY;
	vector<double> MoonZ;
	double dt = 1;
	int np=0;
	for(int i=0;i<nstep;++i){
		EM.ProcessStep(dt);
		if( i%scale == 1){
		EarthPos.push_back(Earth->GetPosition());
		MoonPos.push_back(Moon->GetPosition());
		times.push_back((i+1)*dt);
		np++;
		}
	}
	for(auto V:EarthPos){
		EarthX.push_back(V.X());
		EarthY.push_back(V.Y());
		EarthZ.push_back(V.Z());
	}
	for(auto V:MoonPos){
		MoonX.push_back(V.X());
		MoonY.push_back(V.Y());
		MoonZ.push_back(V.Z());
	}
	TGraph* grEarth = new TGraph();
	TGraph* grMoon = new TGraph();
	for(int i=0;i<np;++i){
//		grEarth->SetPoint(i,times[i],EarthY[i]);
		grEarth->SetPoint(i,EarthX[i],EarthY[i]);
//		grMoon->SetPoint(i,times[i],MoonY[i]);
//		grMoon->SetPoint(i,MoonX[i],MoonY[i]);
	}
  grEarth->SetMarkerSize(2);
	grEarth->SetMarkerStyle(22);
	grEarth->Draw("APL");
/*
	grMoon->SetMarkerSize(2);
	grMoon->SetMarkerStyle(22);
	grMoon->Draw("APL");
*/
}
