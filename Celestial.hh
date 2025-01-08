#include "Field.hh"
static int planet_id = 0;
class Celestial{
	protected:
		double Mass;
		TVector3 Pos;
		TVector3 Vel;
		int id;
		Field MyField;
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
		Field GetField(){
			return MyField;
		}
		TVector3 GetPosition(){
			return Pos;
		}
		TVector3 GetVelocity(){
			return Vel;
		}
		int GetID(){
			return id;
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
