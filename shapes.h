#include <algorithm>
#include "linalg.h"

namespace Shapes
{

Real Plane(const Vec3& Position, const Vec3& Normal)
{
	return Position.Dot(Normal.Normalized());
}
Real Sphere(const Vec3& Position, Real Radius)
{
	return Position.Length() - Radius;
}
Real Box(const Vec3& Position, Vec3 Bounds)
{
	Vec3 Dist = Position.Abs() - Bounds;
	return std::min<Real>(std::max(Dist.X, std::max<Real>(Dist.Y, Dist.Z)), 0.0) +
		Vec3{ std::max<Real>(Dist.X,0.0),
		std::max<Real>(Dist.Y,0.0),
		std::max<Real>(Dist.Z,0.0) }.Length();
}
Real RoundBox(const Vec3& Position, Vec3 Bounds, Real Radius)
{
	return Box(Position, Bounds) - Radius;
}
Real Capsule(const Vec3& Position, Vec3 A, Vec3 B, Real Radius)
{
	Vec3 Pa = Position - A;
	Vec3 Ba = B - A;
	Real H = (Pa.Dot(Ba) / Ba.Dot(Ba));
	H = std::min<Real>(0, std::max<Real>(1.0, H));
	return (Pa - (Ba*H)).Length() - Radius;
}
Real Torus(const Vec3& Position, Real InRadius, Real OutRadius)
{
	Vec3 Q{ Vec3{ Position.X,Position.Z,0 }.Length() - OutRadius,Position.Y,0 };
	return Q.Length() - InRadius;
}

typedef struct {
	Real Distance;
	int Material;
} Shape;

Shape PlaneS(const Vec3& Position, const Vec3& Normal, int Material)
{
	return Shape{Plane(Position, Normal), Material};
}
Shape SphereS(const Vec3& Position, Real Radius, int Material)
{
	return Shape{Sphere(Position, Radius), Material};
}
Shape BoxS(const Vec3& Position, Vec3 Bounds, int Material)
{
	return Shape{Box(Position, Bounds), Material};
}
Shape RoundBoxS(const Vec3& Position, Vec3 Bounds, Real Radius, int Material)
{
	return Shape{RoundBox(Position, Bounds, Radius), Material};
}
Shape CapsuleS(const Vec3& Position, Vec3 A, Vec3 B, Real Radius, int Material)
{
	return Shape{Capsule(Position, A, B, Radius), Material};
}
Shape TorusS(const Vec3& Position, Real InRadius, Real OutRadius, int Material)
{
	return Shape{Torus(Position, InRadius, OutRadius), Material};
}
}

namespace Operations
{
Real Clamp(Real A, Real Min, Real Max)
{
	const Real T = A < Min ? Min : A;
	return T > Max ? Max : T;
}
Real Lerp(Real A, Real B, Real T)
{
	return std::fma(T, B, std::fma(-T, A, A));
}
Shapes::Shape Union(Shapes::Shape A, Shapes::Shape B)
{
	return A.Distance <= B.Distance ? A : B;
}
Shapes::Shape SmoothUnion(Shapes::Shape A, Shapes::Shape B, Real K = 1)
{
	Real H = Clamp(Real(0.5) + Real(0.5)*(B.Distance - A.Distance) / K, 0, 1);
	Real D = Lerp(B.Distance, A.Distance, H) - (K*H*(Real(1.0) - H));
	int M = A.Distance <= B.Distance ? A.Material : B.Material;
	return Shapes::Shape{D, M};
}
Real Intersection(Real A, Real B)
{
	return std::max(A, B);
}
Real Subtract(Real A, Real B)
{
	return Intersection(-A, B);
}
}
