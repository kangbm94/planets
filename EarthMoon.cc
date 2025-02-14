#include "Celestial.hh"
#define HasMinmus 1
void EarthMoon(){
	double size_scale = 10;
	
	TEveManager::Create();

	TEveGeoShape* earth = new TEveGeoShape("Earth");
	earth->SetShape(new TGeoSphere(0, size_scale*6371.0e3));  // Earth's radius in m
	earth->SetMainColor(kBlue);                   // Set color
	
	TEveGeoShape* moon = new TEveGeoShape("Moon");
	moon->SetShape(new TGeoSphere(0, size_scale * 1737.1e3));  // Moon's radius in m
	moon->SetMainColor(kWhite);                   // Set color
	
	TEveGeoShape* Ref1 = new TEveGeoShape("Ref1");
	TEveGeoShape* Ref2 = new TEveGeoShape("Ref2");
	TEveGeoShape* Ref3 = new TEveGeoShape("Ref3");
	TEveGeoShape* Ref4 = new TEveGeoShape("Ref4");
	Ref1->SetShape(new TGeoSphere(0., 1.e1));
	Ref2->SetShape(new TGeoSphere(0., 1.e1));
	Ref3->SetShape(new TGeoSphere(0., 1.e1));
	Ref4->SetShape(new TGeoSphere(0., 1.e1));
	Ref1->SetMainColor(kRed);
	Ref2->SetMainColor(kRed);
	Ref3->SetMainColor(kRed);
	Ref4->SetMainColor(kRed);



	gEve->AddElement(earth);
	gEve->AddElement(moon);
	gEve->AddElement(Ref1);
	gEve->AddElement(Ref2);
	gEve->AddElement(Ref3);
	gEve->AddElement(Ref4);

	TVector3 stop(0,0,0);
	TVector3 center(0,0,0);
	TVector3 MoonPosition(0,0,0);
	TVector3 MoonVelo(0,0,0);
//	TVector3 EarthVelo = - (MoonMass/EarthMass) * MoonVelo;//to set CM momentum 0
	TVector3 EarthVelo = TVector3(0,0,0);//to set CM momentum 0
	Celestial* Earth = new Celestial(EarthMass,center,EarthVelo);
	Celestial* Moon = new Celestial(MoonMass,MoonPosition,MoonVelo);
	World EM;
	EM.AddPlanet(Earth);
	EM.AddPlanet(Moon);
	Earth->AddMoon(Moon);
	Moon->SetOrbit(MoonDistance,MoonEccentricity,180+MoonInclination,MoonArgPerigee,MoonLanAscNode,0);
	vector<TVector3> EarthPos;
	vector<TVector3> MoonPos;
	vector<TVector3> MinmusPos;
#if HasMinmus	
	TEveGeoShape* minmus = new TEveGeoShape("Minmus");
	minmus->SetShape(new TGeoSphere(0, size_scale * 1000.1e3));  // Moon's radius in m
	minmus->SetMainColor(kGreen);                   // Set color
	gEve->AddElement(minmus);
	Celestial* Minmus = new Celestial(MoonMass/2,MoonPosition,MoonVelo);
	EM.AddPlanet(Minmus);
	Moon->AddMoon(Minmus);
	Minmus->SetOrbit(MoonDistance/4,MoonEccentricity*2,MoonInclination-15,MoonArgPerigee-44,MoonLanAscNode,0.5);
#endif
	gEve->FullRedraw3D(kTRUE);
	TGLViewer* viewer = gEve->GetDefaultGLViewer();
	viewer->CurrentCamera().Reset();
	viewer->UpdateScene();
	gSystem->ProcessEvents();
 	int nstep = 86400*60.;
	int scale = 1000;
	vector<double> times;
	vector<double> EarthX;
	vector<double> EarthY;
	vector<double> EarthZ;
	vector<double> MoonX;
	vector<double> MoonY;
	vector<double> MoonZ;
	double dt = 10;
	int np=0;
	for(int i=0;i<nstep;++i){
		EM.ProcessStep(dt);
		if( i%scale == 1){
		EarthPos.push_back(Earth->GetPosition());
		MoonPos.push_back(Moon->GetPosition());
#if HasMinmus
		MinmusPos.push_back(Minmus->GetPosition());
#endif
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
	double half_width = 5e8;
//	np/=10;
	Ref1->RefMainTrans().SetPos(-half_width,0,-half_width);
	Ref2->RefMainTrans().SetPos(-half_width,0,half_width);
	Ref3->RefMainTrans().SetPos(half_width,0,half_width);
	Ref4->RefMainTrans().SetPos(half_width,0,-half_width);
	for(int i=0;i<np;++i){
		TVector3 pos_earth = EarthPos[i];
		TVector3 pos_moon = MoonPos[i];
		earth->RefMainTrans().SetPos(pos_earth.Z(),pos_earth.X(),pos_earth.Y());
		moon->RefMainTrans().SetPos(pos_moon.Z(),pos_moon.X(),pos_moon.Y());
#if HasMinmus
		TVector3 pos_minmus = MinmusPos[i];
		minmus->RefMainTrans().SetPos(pos_minmus.Z(),pos_minmus.X(),pos_minmus.Y());
#endif
		viewer->UpdateScene();
		
		
		gSystem->ProcessEvents();
        gSystem->Sleep(100);
		cout<<Form("Frame %d/%d",i,np)<<endl;
	}

}
