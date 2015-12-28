#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <string>

typedef float Real;

struct Vec3
{
	Real X, Y, Z;
	Real Length() const
	{
		return static_cast<Real>(
			sqrt(X*X + Y*Y + Z*Z)
			);
	}
	Vec3 Normalized() const
	{
		const Real Len = Length();
		return (*this) / Len;
	}

	Real Dot(const Vec3& Other) const
	{
		return X * Other.X + Y * Other.Y + Z * Other.Z;
	}

	Vec3 Abs() const
	{
		return Vec3{
			abs(X),
			abs(Y),
			abs(Z)
		};
	}

	Vec3 operator + (const Vec3& Other) const
	{
		return Vec3{
			X + Other.X,
			Y + Other.Y,
			Z + Other.Z };
	}
	Vec3 operator - (const Vec3& Other) const
	{
		return Vec3{
			X - Other.X,
			Y - Other.Y,
			Z - Other.Z };
	}

	Vec3 operator * (const Vec3& Other) const
	{
		return Vec3{
			X * Other.X,
			Y * Other.Y,
			Z * Other.Z };
	}

	Vec3 operator / (const Vec3& Other) const
	{
		return Vec3{
			X / Other.X,
			Y / Other.Y,
			Z / Other.Z };
	}

	Vec3 operator % (const Vec3& Other) const
	{
		return Vec3{
			fmod(X,Other.X),
			fmod(Y,Other.Y),
			fmod(Z,Other.Z) };
	}

	Vec3 operator + (const Real& Other) const
	{
		return Vec3{
			X + Other,
			Y + Other,
			Z + Other };
	}

	Vec3 operator - (const Real& Other) const
	{
		return Vec3{
			X - Other,
			Y - Other,
			Z - Other };
	}

	Vec3 operator * (const Real& Other) const
	{
		return Vec3{
			X * Other,
			Y * Other,
			Z * Other };
	}

	Vec3 operator / (const Real& Other) const
	{
		return Vec3{
			X / Other,
			Y / Other,
			Z / Other };
	}

	Vec3 operator % (const Real& Other) const
	{
		return Vec3{
			fmod(X,Other),
			fmod(Y,Other),
			fmod(Z,Other) };
	}
};

namespace Shapes
{
Real Plane(Vec3 Position, Vec3 Normal)
{
	//return Position.Dot(Normal.Normalized());
	return Position.Y;
}
Real Sphere(Vec3 Position, Real Radius)
{
	return Position.Length() - Radius;
}
Real Box(Vec3 Position, Vec3 Bounds)
{
	Vec3 Dist = Position.Abs() - Bounds;
	return std::min<Real>(std::max(Dist.X, std::max<Real>(Dist.Y, Dist.Z)), 0.0) +
		Vec3{ std::max<Real>(Dist.X,0.0),
		std::max<Real>(Dist.Y,0.0),
		std::max<Real>(Dist.Z,0.0) }.Length();
}
Real RoundBox(Vec3 Position, Vec3 Bounds, Real Radius)
{
	return Box(Position, Bounds) - Radius;
}
Real Capsule(Vec3 Position, Vec3 A, Vec3 B, Real Radius)
{
	Vec3 Pa = Position - A;
	Vec3 Ba = B - A;
	Real H = (Pa.Dot(Ba) / Ba.Dot(Ba));
	H = std::min<Real>(0, std::max<Real>(1.0, H));
	return (Pa - (Ba*H)).Length() - Radius;
}
Real Torus(Vec3 Position, Real InRadius, Real OutRadius)
{
	Vec3 Q{ Vec3{Position.X,Position.Z,0}.Length() - OutRadius,Position.Y };
	return Q.Length() - InRadius;
}
}

namespace Operations
{
Real Union(Real A, Real B)
{
	return std::min(A, B);
}
Real Intersection(Real A, Real B)
{
	return std::max(A, B);
}
Real Subtract(Real A, Real B)
{
	return Intersection(-A, B);
}
Vec3 Repeat(Vec3 Position, Vec3 Bounds)
{
	return (Position % Bounds) + (Bounds*Real(0.5));
}
}

#define WIDTH 79
#define HEIGHT 37
#define PREC 0.002

Real Scene(Vec3 Point)
{
	Real Distance = 0;

	Distance = Shapes::Plane(Point, Vec3{ 0,-1,0 });

	Distance = Operations::Union(
		Distance,
		Shapes::Sphere(Point, 1.0)
		);

	Distance = Operations::Union(
		Distance,
		Shapes::Sphere(Operations::Repeat(Point, Vec3{ 1,1,1 }), 0.125)
		);

	Distance = Operations::Union(
		Distance,
		Shapes::RoundBox(Point - Vec3{ -5,0,5 },
						 Vec3{ 2,3.2f,0.2f }, 0.225f)
		);

	Distance = Operations::Union(
		Distance,
		Shapes::Capsule(Point,
						Vec3{ 5,5,6 },
						Vec3{ -5,5,6 },
						Real(3.75)
						));

	Distance = Operations::Union(
		Distance,
		Shapes::Torus(Point - Vec3{ 0,Real(0.5),12 },
					  Real(0.5), Real(6)
					  ));

	return Distance;
}

Vec3 CalcNormal(Vec3 Point)
{
#define EPSILON static_cast<Real>(0.001)
	return Vec3{
		Scene(Point + Vec3{ EPSILON,0,0}) - Scene(Point - Vec3{ EPSILON,0,0 }),
		Scene(Point + Vec3{ 0,EPSILON,0 }) - Scene(Point - Vec3{ 0,EPSILON,0 }),
		Scene(Point + Vec3{ 0,0,EPSILON }) - Scene(Point - Vec3{ 0,0,EPSILON }),
	}.Normalized();
}

Real March(Vec3 Origin, Vec3 Ray, bool* Hit)
{
	Real Distance = 0;
	for( size_t i = 0; i < 64; i++ )
	{
		Vec3 Point = Origin + Ray * Distance;
		Real ClosestSurface = Scene(Point);
		if( ClosestSurface < PREC )
		{
			// "Hit" a surface
			if( Hit != nullptr )
			{
				*Hit = true;
			}
			break;
		}
		Distance += ClosestSurface;
	}
	return Distance;
}

const char Shades[] = ".:*oe&#%@";

int main()
{
	Vec3 EyePos = Vec3{ 0,2,-6 };

	std::string Screen;
	Screen.reserve(WIDTH * HEIGHT);
	do
	{
		EyePos.Z += 0.5;
		for( size_t y = 0; y < HEIGHT; y++ )
		{
			for( size_t x = 0; x < WIDTH; x++ )
			{
				Vec3 UV{
					static_cast<Real>(x),
					static_cast<Real>(y),
					1 };
				UV = UV / Vec3{
					WIDTH,
					HEIGHT,
					1 };

				// Recanonicalize [-1,1]
				UV = (UV * 2.0) - 1.0;

				// Flip Y axis
				UV.Y *= Real(-1);

				// Aspect Ratio Correction
				UV.X *= WIDTH / HEIGHT;
				//UV.X *= 0.5;

				// Fov
				UV.Z = Real(0.7);

				bool Hit = false;

				Real Distance = March(EyePos, UV.Normalized(), &Hit);

				Vec3 Point = EyePos + (UV.Normalized() * Distance);

				if( Hit )
				{
					Vec3 Normal = CalcNormal(Point);
					Real Diffuse = Normal.Dot(Vec3{ Real(1),Real(1),Real(1) }.Normalized());
					Diffuse *= 0.5;
					Diffuse += 0.5;

					Screen += Shades[static_cast<size_t>(Diffuse*(sizeof(Shades) - 2))];
				}
				else
				{
					Screen += ' ';
				}
			}
			Screen += "\n";
		}
		printf("%s\n", Screen.c_str());
		Screen.clear();
	} while( getchar() != 'q' );
	return 0;
}