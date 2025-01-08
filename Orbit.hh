#include "Field.hh"
class Orbit{
	private:
		TVector3 Center;
		TVector3 Apoapsis;
		TVector3 Plane;
		double Eccentricity;
		double LongRadius;
		double Phase;
	public:
		Orbit(){}
		Orbit(TVector3 Center_,TVector3 Apoapsis_, TVector3 Plane, double Ecc, double R, double Ph = 0);
		TVector3 GetPosition(double Ph){
			double Ph0 = Apoapsis - Center
		}
		

};
