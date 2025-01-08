#ifndef Field_h
#define Field_h
#include "Constants.hh"
#define Debug 0
class Field{
	private:
		double Mass;
		TVector3 Center;
	public:
		Field(){}
		Field(double M, TVector3 C){
			Mass = M;
			Center = C;
		}
		double GetFieldValue(TVector3 pos){
			TVector3 RelCenter = Center - pos;//Center direction
			double rad = RelCenter.Mag();
			return  (Mass * Gconst /rad );
		}
		TVector3 GetAcceleration(TVector3 pos){
			TVector3 RelCenter = Center - pos;//Center direction
			double rad = RelCenter.Mag();
			TVector3 acc= (Mass * Gconst /rad / rad  ) *( RelCenter.Unit());
#if Debug
		//	cout<<Form("Field from (%f,%f,%f) to (%f,%f,%f) = (%f,%f,%f)",Center.X(),Center.Y(),Center.Z(),pos.X(),pos.Y(),pos.Z(),acc.X(),acc.Y(),acc.Z())<<endl;	
#endif
			return acc;
		}
		TVector3 GetPosition(){
			return Center;
		}
		void Update(double M, TVector3 C){
			Mass = M;
			Center = C;
		}
};
#endif
