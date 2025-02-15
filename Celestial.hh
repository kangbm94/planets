#include "Field.hh"
#include "Orbit.hh"
static int planet_id = 0;
class Celestial{
	protected:
		double Mass;
		TVector3 Pos;
		TVector3 Vel;
		int id;
		Field MyField;
		Celestial* Mother;
		vector<Celestial*> Moons;
		Orbit MyOrbit0;
		bool ConicApproximation = 0;
	public:
		Celestial(){
		};
		Celestial(double M, TVector3 X, TVector3 V){
			id = planet_id;
			planet_id++;
			Mass = M;
			Pos = X;
			Vel = V;
			Update();
		}
		virtual ~Celestial(){
		}
		double GetMass(){
			return Mass;
		}
		void SetMother(Celestial* M){
			Mother = M;//Not recommended to use...
		}
		void AddMoon(Celestial* M){
			Moons.push_back(M);
			M->SetMother(this);
		}
		void AddVelocity(TVector3 V){
			Vel += V;
		}
		void AddPosition(TVector3 X){
			Pos += X;
			Update();
		}
		void SetVelocity(TVector3 V){
			Vel = V;
		}
		void SetPosition(TVector3 X){
			Pos = X;
			Update();
		}
		Field GetField(){
			return MyField;
		}
		TVector3 GetPosition(){
			return Pos;
		}
		TVector3 GetVelocity(){
			return Vel;
		}
		TVector3 GetCentroid(){
			return (Mass/(Mother->GetMass()+Mass))*Pos + (Mother->GetMass()/(Mother->GetMass()+Mass))*Mother->GetPosition();	
		}
		TVector3 GetCentroidVelocity(){
			return (Mass/(Mother->GetMass()+Mass))*Vel + (Mother->GetMass()/(Mother->GetMass()+Mass))*Mother->GetVelocity();
		}
		TVector3 GetOrbitalPosition(){
			return Pos - GetCentroid();
		}
		TVector3 GetOrbitalVelocity(){
			return Vel - GetCentroidVelocity();
		}
		int GetID(){
			return id;
		}
		Celestial* GetMother(){
			return Mother;
		}
		Celestial* GetMoon(int i){
			return Moons[i];
		}
		void SetOrbit(double r, double e, double i, double a, double l,double nu0){
			MyOrbit0 = Orbit(Mother->GetMass(),r,e,i,a,l);
			TVector3 pos_orbit = MyOrbit0.GetPosition(nu0);
			TVector3 vel_orbit = MyOrbit0.GetVelocity(nu0);
			TVector3 centroid = (Mass/(Mother->GetMass()+Mass))*pos_orbit;
			Pos = Mother->GetPosition() + pos_orbit;
			TVector3 dv_mother = -1 * (Mass/(Mother->GetMass()+Mass))*vel_orbit;
			Mother->AddVelocity(dv_mother);
			Vel = Mother->GetVelocity() + vel_orbit;
			Update();
		}
		void GetOrbitParameters0(double* pars){
			pars[0] = MyOrbit0.GetRadi();
			pars[1] = MyOrbit0.GetEcc();
			pars[2] = MyOrbit0.GetInc();
			pars[3] = MyOrbit0.GetArg();
			pars[4] = MyOrbit0.GetLan();
		}
		void GetOrbitParameters(double* pars){
			TVector3 centroid = GetCentroid();
			TVector3 centroid_velocity = GetCentroidVelocity();
			TVector3 pos_orbit = Pos - centroid;
			TVector3 vel_orbit = Vel - centroid_velocity;
			TVector3 vec_l = pos_orbit.Cross(vel_orbit);
			TVector3 vec_n = TVector3(0,0,1).Cross(vec_l);
			double mu = Gconst * (Mother->GetMass());
			double r = pos_orbit.Mag();
			TVector3 vec_e = (1/mu)*((vel_orbit.Mag2()-mu/r)*pos_orbit - (pos_orbit*vel_orbit)*vel_orbit);

			double radi = 1./(2./r - vel_orbit.Mag2()/mu);
			double ecc = vec_e.Mag();
			double inc = acos(vec_l.Z()/vec_l.Mag());
			double arg = acos(vec_n*vec_e/(vec_n.Mag()*vec_e.Mag()));
			double lan = acos(vec_n.X()/vec_n.Mag());
			if(vec_n.Y()<0){
				lan = 2*acos(-1) - lan;
			}
			double nu = acos(vec_e*pos_orbit/(vec_e.Mag()*pos_orbit.Mag()));
			pars[0] = radi;
			pars[1] = ecc;
			pars[2] = inc*180./acos(-1);
			pars[3] = arg*180./acos(-1);
			pars[4] = lan*180./acos(-1);
			pars[5] = nu;
		}
		void PropagateRK4(double dt, vector<Field> Fields ){
			#if Debug
//				cout<<Form("Planet %d : X = (%g,%g,%g) V = (%g,%g,%g)",id,Pos.X(),Pos.Y(),Pos.Z(),Vel.X(),Vel.Y(),Vel.Z())<<endl;
			#endif
			TVector3 dXK1 = Vel; 
			TVector3 dVK1(0,0,0);
			for(auto F:Fields){
#if Debug
				cout<<Form("Planet %d : Acce = (%g,%g,%g)",id,F.GetAcceleration(Pos).X(),F.GetAcceleration(Pos).Y(),F.GetAcceleration(Pos).Z())<<endl;	
#endif
				dVK1+= F.GetAcceleration(Pos);
			}
			TVector3 dXK2 = Vel + 0.5 * dt * dVK1;	
			TVector3 dVK2(0,0,0);
			for(auto F:Fields){
				dVK2+= F.GetAcceleration(Pos+ 0.5*dt*dXK1);
			}
			TVector3 dXK3 = Vel + 0.5 * dt * dVK2;	
			TVector3 dVK3(0,0,0);
			for(auto F:Fields){
				dVK3+= F.GetAcceleration(Pos+ 0.5*dt*dXK2);
			}
			TVector3 dXK4 = Vel +  dt * dVK3;	
			TVector3 dVK4(0,0,0);
			for(auto F:Fields){
				dVK4+= F.GetAcceleration(Pos+ 0.5*dt*dXK2);
			}
			Pos+= (dXK1 + 2* dXK2 + 2 * dXK3 + dXK4)*(dt/6.);
			Vel+= (dVK1 + 2* dVK2 + 2 * dVK3 + dVK4)*(dt/6.);
			#if Debug
				cout<<Form("Planet %d : X = (%g,%g,%g) V = (%g,%g,%g)",id,Pos.X(),Pos.Y(),Pos.Z(),Vel.X(),Vel.Y(),Vel.Z())<<endl;
				cout<<Form("Acceleration = (%g,%g,%g)",dVK1.X(),dVK1.Y(),dVK1.Z())<<endl;	
			#endif	
		}
		void Update(){
			MyField.Update(Mass,Pos);
		}

};
class Spaceship : public Celestial{
	private:
		TVector3 Acceleration;
	public:
		Spaceship(TVector3 X, TVector3 V){
			id = planet_id;
			planet_id++;
			Mass = 0;
			Pos = X;
			Vel = V;
			Update();
		}
		void SetAcceleration(TVector3 A){
			Acceleration = A;
		}
		void PropagateRK4(double dt, vector<Field> Fields ){
			Celestial::PropagateRK4(dt,Fields);
		#if Debug
			cout<<Form("Spaceship %d : X = (%g,%g,%g) V = (%g,%g,%g)",id,Pos.X(),Pos.Y(),Pos.Z(),Vel.X(),Vel.Y(),Vel.Z())<<endl;
		#endif
			Vel += Acceleration * dt;
			Pos += 0.5 *  Acceleration * dt * dt ;
		}
		virtual void Engine(double t){
			TVector3 acc(0,0,0);
			if(abs(atan2(Pos.Y(),Pos.X()))<0.1){
				acc = Vel.Unit() * 1.;
			}
			SetAcceleration(acc);
			#if Debug
			cout<<Form("Acceleration = (%g,%g,%g)",acc.X(),acc.Y(),acc.Z())<<endl;
			#endif
		};

};
class World{
	protected:
		vector<Celestial*> Celestials;
		vector<Field> ExternalFields;
		double t_=0;
	public:
		World(){}
		void AddPlanet(Celestial* C){
			Celestials.push_back(C);
			cout<<"Planet added, NPlanets = "<<Celestials.size()<<endl;
		}
		void AddExternalField(Field F){
			ExternalFields.push_back(F);
		}
		double GetFieldValue(TVector3 pos){
			double FieldValue = 0;
			for(auto F:ExternalFields){
				FieldValue += F.GetFieldValue(pos);
			}
			for(auto C:Celestials){
				FieldValue += C->GetField().GetFieldValue(pos);
			}
			return FieldValue;
		}
		TVector3 GetAcceleration(TVector3 pos){
			TVector3 Acc(0,0,0);
			for(auto F:ExternalFields){
				Acc += F.GetAcceleration(pos);
			}
			for(auto C:Celestials){
				Acc += C->GetField().GetAcceleration(pos);
			}
			return Acc;
		}
		void ProcessStep(double dt){
			for(auto C:Celestials){
				vector<Field> AppliedFields;
				for(auto C2:Celestials){
					if(C->GetID() != C2->GetID()){
						auto F2 = C2->GetField();
#if Debug
				cout<<Form("Acceleration from planet %d(%g,%g,%g) to planet %d at pos (%g,%g,%g)= (%g,%g,%g)",C2->GetID(),C2->GetPosition().X(),C2->GetPosition().Y(),C2->GetPosition().Z(),C->GetID(),C->GetPosition().X(),C->GetPosition().Y(),C->GetPosition().Z(),F2.GetAcceleration(C->GetPosition()).X(),F2.GetAcceleration(C->GetPosition()).Y(),F2.GetAcceleration(C->GetPosition()).Z())<<endl;	
#endif
						AppliedFields.push_back(C2->GetField());
					}
				}
				for(auto F:ExternalFields){
					AppliedFields.push_back(F);
				}
				if(dynamic_cast<Spaceship*>(C)!= nullptr){
					((Spaceship*)C)->Engine(t_);
					((Spaceship*)C)->PropagateRK4(dt,AppliedFields);
				}
				else{
					C->PropagateRK4(dt,AppliedFields);
				}
			}
			for(auto C:Celestials){
				C->Update();
			}
			t_+=dt;
		}
};
