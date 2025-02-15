#include "Celestial.hh"
#define HasMinmus 1
#define DrawOrbit 1
void EarthMoon(){
	double size_scale = 10;
	
	TEveManager::Create();

	TEveGeoShape* earth = new TEveGeoShape("Earth");
	earth->SetShape(new TGeoSphere(0, size_scale*6371.0e3));  // Earth's radius in m
	earth->SetMainColor(kBlue);                   // Set color
	
	TEveGeoShape* moon = new TEveGeoShape("Moon");
	moon->SetShape(new TGeoSphere(0, size_scale * 1737.1e3));  // Moon's radius in m
	moon->SetMainColor(kWhite);                   // Set color
	



	gEve->AddElement(earth);
	gEve->AddElement(moon);

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
	Moon->SetOrbit(MoonDistance,MoonEccentricity,MoonInclination,MoonArgPerigee,MoonLanAscNode,0);
	TVector3 pos_earth = Earth->GetPosition();
	TVector3 pos_moon = Moon->GetPosition();
	earth->RefMainTrans().SetPos(pos_earth.X(),pos_earth.Y(),pos_earth.Z());
	moon->RefMainTrans().SetPos(pos_moon.X(),pos_moon.Y(),pos_moon.Z());
	vector<TVector3> EarthPos;

	vector<TVector3> MoonPos;
	vector<TVector3> MoonCentroidPos;
	vector<TVector3> MoonCentroidVel;
	vector<TVector3> MoonOrbitalPos;
	vector<TVector3> MoonOrbitalVel;
	vector<vector<double>> MoonOrbitParameters;
	double par_moon[5];

	vector<TVector3> MinmusPos;
	vector<TVector3> MinmusCentroidPos;
	vector<TVector3> MinmusCentroidVel;
	vector<TVector3> MinmusOrbitalPos;
	vector<TVector3> MinmusOrbitalVel;
	vector<vector<double>> MinmusOrbitParameters;
	double par_minmus[5];
	Moon->GetOrbitParameters(par_moon);

	int OrbitPoints = 30;
#if DrawOrbit
	vector<TEveGeoShape*> MoonOrbit;
	Orbit* moonorbit = new Orbit(MoonMass,par_moon[0],par_moon[1],par_moon[2],par_moon[3],par_moon[4]);	
	for(int i=0;i<OrbitPoints;++i){
		double dnu = 2*acos(-1)/OrbitPoints;
		double nu0 = i*dnu;
		double nu1 = (i+1)*dnu;
		TVector3 pos_mother = Moon->GetCentroid();
		TVector3 pos_0 = moonorbit->GetPosition(nu0)+pos_mother;
		TVector3 pos_1 = moonorbit->GetPosition(nu1)+pos_mother;
		TVector3 pos = (pos_0+pos_1)* 0.5;
		TVector3 vel = moonorbit->GetVelocity(nu0+0.5*dnu);
		TVector3 dir = vel.Unit();
		TEveGeoShape* orbit_frag = new TEveGeoShape(Form("MoonOrbit_%d",i));
//		orbit_frag->SetShape(new TGeoTube(0,1e5,pos.Mag()/2));
		orbit_frag->SetShape(new TGeoSphere(0,3e6));
		orbit_frag->SetMainColor(kWhite);

		double theta = acos(dir.Z());
		double phi = atan2(dir.Y(),dir.X());
		TGeoRotation* rot = new TGeoRotation();
		rot->RotateZ(-phi*180/acos(-1));
		rot->RotateY(-theta*180/acos(-1));
		TGeoCombiTrans* trans = new TGeoCombiTrans((pos).X(), (pos).Y(), (pos).Z(), rot);	
		orbit_frag->SetTransMatrix(*trans);
		orbit_frag->RefMainTrans().SetPos(pos.X(),pos.Y(),pos.Z());	
		MoonOrbit.push_back(orbit_frag);
		gEve->AddElement(orbit_frag);
	}
#endif
#if HasMinmus	
	TEveGeoShape* minmus = new TEveGeoShape("Minmus");
	minmus->SetShape(new TGeoSphere(0, size_scale * 1000.1e3));  // Moon's radius in m
	minmus->SetMainColor(kGreen);                   // Set color
	gEve->AddElement(minmus);
	Celestial* Minmus = new Celestial(MoonMass/10,MoonPosition,MoonVelo);
	EM.AddPlanet(Minmus);
	Moon->AddMoon(Minmus);
	Minmus->SetOrbit(MoonDistance/4,MoonEccentricity*2,MoonInclination-15,MoonArgPerigee-44,MoonLanAscNode,0.5);
	Moon->GetOrbitParameters(par_moon);
	Minmus->GetOrbitParameters(par_minmus);
	TVector3 pos_minmus = Minmus->GetPosition();
	minmus->RefMainTrans().SetPos(pos_minmus.X(),pos_minmus.Y(),pos_minmus.Z());
#if DrawOrbit
	vector<TEveGeoShape*> MinmusOrbit;
	Orbit* minmusorbit = new Orbit(MoonMass/2,par_minmus[0],par_minmus[1],par_minmus[2],par_minmus[3],par_minmus[4]);	
	for(int i=0;i<OrbitPoints;++i){
		double dnu = 2*acos(-1)/OrbitPoints;
		double nu0 = i*dnu;
		double nu1 = (i+1)*dnu;
		TVector3 pos_mother = Minmus->GetCentroid();
		TVector3 pos_0 = minmusorbit->GetPosition(nu0)+pos_mother;
		TVector3 pos_1 = minmusorbit->GetPosition(nu1)+pos_mother;
		TVector3 pos = (pos_0+pos_1)* 0.5;
		TVector3 vel = minmusorbit->GetVelocity(nu0+0.5*dnu);
		TVector3 dir = vel.Unit();
		TEveGeoShape* orbit_frag = new TEveGeoShape(Form("MinmusOrbit_%d",i));
		orbit_frag->SetShape(new TGeoSphere(0,3e6));
		orbit_frag->SetMainColor(kGreen);
		double theta = acos(dir.Z());
		double phi = atan2(dir.Y(),dir.X());
		TGeoRotation* rot = new TGeoRotation();
		rot->RotateZ(-phi*180/acos(-1));
		rot->RotateY(-theta*180/acos(-1));
		TGeoCombiTrans* trans = new TGeoCombiTrans((pos).X(), (pos).Y(), (pos).Z(), rot);	
//		orbit_frag->SetTransMatrix(*trans);
		orbit_frag->RefMainTrans().SetPos(pos.X(),pos.Y(),pos.Z());	
		MinmusOrbit.push_back(orbit_frag);
		gEve->AddElement(orbit_frag);
	}

#endif
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
	double dt = 1;
	int np=0;

	for(int i=0;i<nstep;++i){
		EM.ProcessStep(dt);
		if( i%scale == 1){
		EarthPos.push_back(Earth->GetPosition());
		MoonPos.push_back(Moon->GetPosition());
		MoonCentroidPos.push_back(Moon->GetCentroid());
		double par_moon[5];
		Moon->GetOrbitParameters(par_moon);
		vector<double> par_moon_v;;
		for(int j=0;j<5;++j){
			par_moon_v.push_back(par_moon[j]);
		}
		MoonOrbitParameters.push_back(par_moon_v);
#if HasMinmus
		MinmusPos.push_back(Minmus->GetPosition());
		MinmusCentroidPos.push_back(Minmus->GetCentroid());
		double par_minmus[5];
		Minmus->GetOrbitParameters(par_minmus);
		vector<double> par_minmus_v;;
		for(int j=0;j<5;++j){
			par_minmus_v.push_back(par_minmus[j]);
		}
		MinmusOrbitParameters.push_back(par_minmus_v);
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
	cout<<"Calculations done. Waiting..."<<endl;
	for(int i=0;i<np;++i){
		TVector3 pos_earth = EarthPos[i];
		TVector3 pos_moon = MoonPos[i];
		earth->RefMainTrans().SetPos(pos_earth.X(),pos_earth.Y(),pos_earth.Z());
		moon->RefMainTrans().SetPos(pos_moon.X(),pos_moon.Y(),pos_moon.Z());
#if DrawOrbit
		for(int ip=0;ip<5;++ip){
			par_moon[ip] = MoonOrbitParameters[i][ip];
		}
		moonorbit->SetParameters(par_moon[0],par_moon[1],par_moon[2],par_moon[3],par_moon[4]);
		TVector3 pos_mother = MoonCentroidPos[i];
		for(int ip=0;ip<OrbitPoints;++ip){
			double dnu = 2*acos(-1)/OrbitPoints;
			double nu0 = ip*dnu;
			double nu1 = (ip+1)*dnu;
			TVector3 pos_mother = MoonCentroidPos[i];
			TVector3 pos_0 = moonorbit->GetPosition(nu0)+pos_mother;
			TVector3 pos_1 = moonorbit->GetPosition(nu1)+pos_mother;
			TVector3 pos = (pos_0+pos_1)* 0.5;
			TVector3 vel = moonorbit->GetVelocity(nu0+0.5*dnu);
			TVector3 dir = vel.Unit();

			double theta = acos(dir.Z());
			double phi = atan2(dir.Y(),dir.X());
			TGeoRotation* rot = new TGeoRotation();
			rot->RotateZ(-phi*180/acos(-1));
			rot->RotateY(-theta*180/acos(-1));
			TGeoCombiTrans* trans = new TGeoCombiTrans((pos).X(), (pos).Y(), (pos).Z(), rot);	
//			MoonOrbit[ip]->SetTransMatrix(*trans);	
			MoonOrbit[ip]->RefMainTrans().SetPos(pos.X(),pos.Y(),pos.Z());
		}
#endif
#if HasMinmus
		TVector3 pos_minmus = MinmusPos[i];
		minmus->RefMainTrans().SetPos(pos_minmus.X(),pos_minmus.Y(),pos_minmus.Z());
#if DrawOrbit
		for(int ip=0;ip<5;++ip){
			par_minmus[ip] = MinmusOrbitParameters[i][ip];
		}
		minmusorbit->SetParameters(par_minmus[0],par_minmus[1],par_minmus[2],par_minmus[3],par_minmus[4]);
		for(int ip=0;ip<OrbitPoints;++ip){
			double dnu = 2*acos(-1)/OrbitPoints;
			double nu0 = ip*dnu;
			double nu1 = (ip+1)*dnu;
			TVector3 pos_mother = MinmusCentroidPos[i];
			TVector3 pos_0 = minmusorbit->GetPosition(nu0)+pos_mother;
			TVector3 pos_1 = minmusorbit->GetPosition(nu1)+pos_mother;
			TVector3 vel = minmusorbit->GetVelocity(nu0+0.5*dnu);
			TVector3 dir = vel.Unit();
			TVector3 pos = (pos_0+pos_1)* 0.5;

			double theta = acos(dir.Z());
			double phi = atan2(dir.Y(),dir.X());
			TGeoRotation* rot = new TGeoRotation();
			rot->RotateZ(-phi*180/acos(-1));
			rot->RotateY(-theta*180/acos(-1));
			TGeoCombiTrans* trans = new TGeoCombiTrans((pos).X(), (pos).Y(), (pos).Z(), rot);	
//			MinmusOrbit[ip]->SetTransMatrix(*trans);
			MinmusOrbit[ip]->RefMainTrans().SetPos(pos.X(),pos.Y(),pos.Z());	
		}
#endif
#endif
		viewer->UpdateScene();
		
		
		gSystem->ProcessEvents();
        gSystem->Sleep(50);
		cout<<Form("Frame %d/%d",i,np)<<endl;
	}

}
