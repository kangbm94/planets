#include "Field.hh"
class Orbit{
	private:
		double radi;//half-length of the semi-major axis
		double ecc;//eccentricity
		double inc;//inclination
		double arg;//argument of perihelion
		double lan;//longitude of the ascending node
		double mu;//gravitational parameter

	private:
		TVector3 Rotate(TVector3 vec){
			TVector3 vec_rot = vec;
			vec_rot.RotateZ(arg);
			vec_rot.RotateX(inc);
			vec_rot.RotateZ(lan);
			return vec_rot;
		}
	public:
		Orbit(){}
		Orbit(double M, double r, double e, double i, double a, double l){
			mu = Gconst * M;
			radi = r;
			ecc = e;
			inc = i*acos(-1)/180;
			arg = a*acos(-1)/180;
			lan = l*acos(-1)/180;
		}
		double GetRadi(){
			return radi;
		}
		double GetEcc(){
			return ecc;
		}
		double GetInc(){
			return inc*180./acos(-1);
		}
		double GetArg(){
			return arg*180./acos(-1);
		}
		double GetLan(){
			return lan*180./acos(-1);
		}
		TVector3 GetEllipsePosition(double nu){
			double r = radi*(1-ecc*ecc)/(1+ecc*cos(nu));
			TVector3 pos(r*cos(nu),r*sin(nu),0);
			return pos;
		}
		TVector3 GetEllipseVelocity(double nu){
			double v_rad = sqrt(mu /radi /(1-ecc*ecc)) * ecc * sin(nu);
			double v_tan = sqrt(mu /radi /(1-ecc*ecc)) * (1+ecc*cos(nu));
			TVector3 unit_rad(cos(nu),sin(nu),0);
			TVector3 unit_tan(-sin(nu),cos(nu),0);
			TVector3 vel = unit_rad * v_rad + unit_tan * v_tan;
			return vel;
		}
		TVector3 GetPosition(double nu){
			return Rotate(GetEllipsePosition(nu));
		}
		TVector3 GetVelocity(double nu){
			return Rotate(GetEllipseVelocity(nu));
		}
		double FindClosestNu(TVector3 Point,double& dX,double precision = 1e-9){
			double dn = 0.01;
			double damp = 0.1;
			double n0 = 0;
			double n1 = 2*acos(-1);
			dX = (Point - GetPosition(n0)).Mag();
			double nu = n0;
			while(dn > precision){
				for(int in=0;in<(n1-n0)/dn +1;++in ){
					double n = n0 + in * dn;
					double dXn = (Point - GetPosition(n)).Mag();
					if(dXn < dX){
						dX = dXn;
						nu = n;
					}
				}
				n0 = nu - dn;
				n1 = nu + dn;
				dn *= damp;
			}
			return nu;
		}

};
