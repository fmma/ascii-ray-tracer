#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <fstream>
void CaptureScreen(HWND Window, const char* filename)
{
	RECT WindowBounds;
	GetWindowRect(Window, &WindowBounds);

	int Width = WindowBounds.right - WindowBounds.left;
	int Height = WindowBounds.bottom - WindowBounds.top;

	std::ofstream fOut(filename, std::ios::binary);
	if( !fOut ) return;

	BITMAPFILEHEADER BmpHeader;
	BITMAPINFOHEADER InfoHeader;

	BmpHeader.bfType = 0x4d42;
	BmpHeader.bfSize = 0;
	BmpHeader.bfReserved1 = 0;
	BmpHeader.bfReserved2 = 0;
	BmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	InfoHeader.biSize = sizeof(InfoHeader);
	InfoHeader.biWidth = Width;
	InfoHeader.biHeight = Height;
	InfoHeader.biPlanes = 1;
	InfoHeader.biBitCount = 24;
	InfoHeader.biCompression = BI_RGB;
	InfoHeader.biSizeImage = 0;
	InfoHeader.biXPelsPerMeter = 0;
	InfoHeader.biYPelsPerMeter = 0;
	InfoHeader.biClrUsed = 0;
	InfoHeader.biClrImportant = 0;

	fOut.write((char*)&BmpHeader, sizeof(BmpHeader));
	fOut.write((char*)&InfoHeader, sizeof(InfoHeader));

	BITMAPINFO BmpInfo;
	BmpInfo.bmiHeader = InfoHeader;

	HDC winDC = GetWindowDC(Window);
	HDC memDC = CreateCompatibleDC(winDC);
	BYTE* Buffer = 0;
	HBITMAP ImageDC = CreateDIBSection(winDC, &BmpInfo, DIB_RGB_COLORS, (void**)&Buffer, 0, 0);
	SelectObject(memDC, ImageDC);
	BitBlt(memDC, 0, 0, Width, Height, winDC, 0, 0, SRCCOPY);
	DeleteDC(memDC);
	ReleaseDC(Window, winDC);

	size_t Size = (((24 * Width + 31) & (~31)) / 8)*Height;
	fOut.write((const char*)Buffer, Size);

	DeleteObject(ImageDC);
}

#endif

typedef float Real;

#define USE_SSE

#ifdef USE_SSE
#include <pmmintrin.h>
// Simd Modulo
inline __m128 _mm_mod_ps2(const __m128& A, const __m128& Divisor)
{
	__m128 C = _mm_div_ps(A, Divisor);
	__m128i I = _mm_cvttps_epi32(C);
	__m128 Trunc = _mm_cvtepi32_ps(I);
	__m128 Base = _mm_mul_ps(Trunc, Divisor);
	__m128 Remainder = _mm_sub_ps(A, Base);
	return Remainder;
}
#endif

union Vec3
{
#ifdef USE_SSE
	__m128 SimdReals;
	Vec3(__m128 Vec)
		:
		SimdReals(Vec)
	{
	}
#endif
	Vec3(Real X, Real Y, Real Z)
		:
		X(X), Y(Y), Z(Z)
	{
	}
	Real Reals[3];
	struct
	{
		Real X, Y, Z;
	};
	Real Length() const
	{
#ifdef USE_SSE
		return static_cast<Real>(
			_mm_cvtss_f32(
				_mm_sqrt_ss(
					_mm_dp_ps(SimdReals, SimdReals, 0x71)
					)
				)
			);
#else
		return static_cast<Real>(
			sqrt(X*X + Y*Y + Z*Z)
			);
#endif
	}
	Vec3 Normalized() const
	{
#ifdef USE_SSE
		return Vec3{ _mm_mul_ps(
			this->SimdReals, _mm_rsqrt_ps(
				_mm_dp_ps(this->SimdReals, this->SimdReals, 0x7f)
				)
			)
		};
#else
		const Real Len = Length();
		return (*this) / Len;
#endif
	}

	Real Dot(const Vec3& Other) const
	{
#ifdef USE_SSE
		return static_cast<Real>(
			_mm_cvtss_f32(
				_mm_dp_ps(SimdReals, Other.SimdReals, 0x71)
				)
			);
#else
		return X * Other.X + Y * Other.Y + Z * Other.Z;
#endif
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
#ifdef USE_SSE
		return Vec3{
			_mm_add_ps(SimdReals, Other.SimdReals)
		};
#else
		return Vec3{
			X + Other.X,
			Y + Other.Y,
			Z + Other.Z
		};
#endif
	}
	Vec3 operator - (const Vec3& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_sub_ps(SimdReals,Other.SimdReals)
		};
#else
		return Vec3{
			X - Other.X,
			Y - Other.Y,
			Z - Other.Z
		};
#endif
	}

	Vec3 operator * (const Vec3& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_mul_ps(SimdReals,Other.SimdReals)
		};
#else
		return Vec3{
			X * Other.X,
			Y * Other.Y,
			Z * Other.Z
		};
#endif
	}

	Vec3 operator / (const Vec3& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_div_ps(SimdReals,Other.SimdReals)
		};
#else
		return Vec3{
			X / Other.X,
			Y / Other.Y,
			Z / Other.Z
		};
#endif
	}

	Vec3 operator % (const Vec3& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_mod_ps2(SimdReals,Other.SimdReals)
		};
#else
		return Vec3{
			fmod(X,Other.X),
			fmod(Y,Other.Y),
			fmod(Z,Other.Z)
		};
#endif
	}

	Vec3 operator + (const Real& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_add_ps(SimdReals,_mm_set1_ps(Other))
		};
#else
		return Vec3{
			X + Other,
			Y + Other,
			Z + Other
		};
#endif
	}

	Vec3 operator - (const Real& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_sub_ps(SimdReals,_mm_set1_ps(Other))
		};
#else
		return Vec3{
			X - Other,
			Y - Other,
			Z - Other
		};
#endif
	}

	Vec3 operator * (const Real& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_mul_ps(SimdReals,_mm_set1_ps(Other))
		};
#else
		return Vec3{
			X * Other,
			Y * Other,
			Z * Other
		};
#endif
	}

	Vec3 operator / (const Real& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_div_ps(SimdReals,_mm_set1_ps(Other))
		};
#else
		return Vec3{
			X / Other,
			Y / Other,
			Z / Other
		};
#endif
	}

	Vec3 operator % (const Real& Other) const
	{
#ifdef USE_SSE
		return Vec3{
			_mm_mod_ps2(SimdReals,_mm_set1_ps(Other))
		};
#else
		return Vec3{
			fmod(X,Other),
			fmod(Y , Other),
			fmod(Z , Other)
		};
#endif
	}
};

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
	Vec3 Q{ Vec3{Position.X,Position.Z,0}.Length() - OutRadius,Position.Y,0 };
	return Q.Length() - InRadius;
}
}

namespace Operations
{
Real Union(Real A, Real B)
{
	return std::min(A, B);
}
Real SmoothUnion(Real A, Real B, Real K = 8)
{
	/*
	Power Smooth
	A = std::pow(A, K);
	B = std::pow(B, K);
	return std::pow((A*B) / (A + B), Real(1) / K);
	*/
	Real H = std::min<Real>(0, std::max<Real>(1, Real(0.5) + Real(0.5)*(B - A) / K));
	return (B * (Real(1.0) - H) + A*H) - (K*H*(Real(1.0) - H));
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

namespace Translate
{
const static Real Pi(static_cast<Real>(3.1415926535897932384626433));
Vec3 RotX(const Vec3& Position, Real Angle)
{
	Angle *= Pi / Real(180);
	return Vec3{
		Position.X,
		Position.Y * cos(Angle) - Position.Z * sin(Angle),
		Position.Y * sin(Angle) + Position.Z * cos(Angle),
	};
}
Vec3 RotY(const Vec3& Position, Real Angle)
{
	Angle *= Pi / Real(180);
	return Vec3{
		Position.Z * sin(Angle) + Position.X * cos(Angle),
		Position.Y,
		Position.Z * cos(Angle) - Position.X * sin(Angle),
	};
}
Vec3 RotZ(const Vec3& Position, Real Angle)
{
	Angle *= Pi / Real(180);
	return Vec3{
		Position.X * cos(Angle) - Position.Y * sin(Angle),
		Position.X * sin(Angle) + Position.Y * cos(Angle),
		Position.Z,
	};
}
Vec3 Repeat(const Vec3& Position, const Vec3& Bounds)
{
	return (Position % Bounds) - (Bounds*Real(0.5));
}
Vec3 RepeatGround(const Vec3& Position, const Vec3& Bounds)
{
	Vec3 Rep{ Position };
	Rep.X = std::remainder(Rep.X, Bounds.X) - (Bounds.X * Real(0.5));
	Rep.Z = std::remainder(Rep.Z, Bounds.Z) - (Bounds.Z * Real(0.5));
	return Rep;
}
}

#define WIDTH (79)
#define HEIGHT (39)
#define PREC 0.002

static size_t Tick = 0;

//// SCENE
Real Scene(const Vec3& Point)
{
	Real Distance = 0;

	Distance = Shapes::Plane(Point, Vec3{ 0,1,0 });

	Distance = Operations::Union(
		Distance,
		Shapes::Sphere(
			Translate::RepeatGround(
				Point,
				Vec3{ 10,0,10 }),
			1)
		);

	Distance = Operations::Union(
		Distance,
		Shapes::RoundBox(
			Translate::RotZ(
				Point - Vec3{ -5,2,5 }, Real(15 + Tick * 15)
				),
			Vec3{ 1,2,1 }, 0.525f)
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
		Shapes::Torus(
			Translate::RotX(Point - Vec3{ 0,Real(0.5),12 }, 45),
			Real(1), Real(6)
			)
		);

	for( size_t i = 1; i < 20; i++ )
	{
		Distance = Operations::Union(
			Distance,
			Shapes::Torus(
				Translate::RotZ(
					Translate::RotX(Point - Vec3{ 0,Real(0.5),Real(12 + i * 7) }, 85), Real(i * 30 + (Tick * 5))
					),
				Real(1), Real(6)
				));
	}

	return Distance;
}

Vec3 CalcNormal(const Vec3& Point)
{
#define EPSILON static_cast<Real>(0.001)
	return Vec3{
		Scene(Point + Vec3{ EPSILON,0,0}) - Scene(Point - Vec3{ EPSILON,0,0 }),
		Scene(Point + Vec3{ 0,EPSILON,0 }) - Scene(Point - Vec3{ 0,EPSILON,0 }),
		Scene(Point + Vec3{ 0,0,EPSILON }) - Scene(Point - Vec3{ 0,0,EPSILON }),
	}.Normalized();
}

Real March(const Vec3& Origin, const Vec3& Ray, bool* Hit)
{
	Real Distance = 0;
	for( size_t i = 0; i < 128; i++ )
	{
		Real ClosestSurface = Scene(Origin + (Ray * Distance));
		if( ClosestSurface < PREC )
		{
			// "Hit" a surface
			if( Hit != nullptr )
			{
				*Hit = true;
			}
			break;
		}
		Distance += ClosestSurface * Real(0.75);
	}
	return Distance;
}

Real Shadow(const Vec3& LightPos, const Vec3& LightDir, Real Min, Real Max, Real K)
{
	Real Res = 1;
	for( Real t = Min; t < Max;)
	{
		Real Distance = Scene(LightPos + LightDir*t);
		if( Distance < PREC )
		{
			return 0.0;
		}
		Res = std::min(Res, K*Distance / t);
		t += Distance;
	}
	return Res;
}

const char Shades[] = ".:*oe$&#%@";

int main()
{
	Vec3 LightDir = Vec3{ 1,0,Real(-0.15) }.Normalized();
	Vec3 EyePos = Vec3{ 0,2,-6 };

	std::string Screen;
	Screen.reserve(WIDTH * HEIGHT);
	do
	{
		EyePos.Z += Real(1);
		LightDir = Translate::RotZ(LightDir, Real(0.45));
		Tick++;
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
				//UV.X *= 0.75;

				/* Fov
				15deg bisec = tan((pi/12)/2)    = 30 degrees fov = 0.1316524975873958
				30deg bisec = tan((pi/6)/2)     = 60 degrees fov = 0.26794919243112270
				36deg bisec = tan(((2pi)/5)/2)  = 72 degrees fov = 0.7265425280053608858
				45dec bisec = tan((pi/2)/2)     = 90 degrees fov = 1
				*/
				UV.Z = Real(1 / 0.7265425280053608858);
				UV = UV.Normalized();

				bool Hit = false;

				Real Distance = March(EyePos, UV, &Hit);

				Vec3 Point = EyePos + (UV * Distance);

				if( Hit )
				{
					Vec3 Normal = CalcNormal(Point);
					Real Diffuse = Normal.Dot(LightDir);
					Diffuse *= 0.5;
					Diffuse += 0.5;

					Diffuse *= Diffuse;

					Diffuse *= Shadow(Point, LightDir, Real(0.5), 10, 10);

					Screen += Shades[static_cast<size_t>(Diffuse*(sizeof(Shades) - 2))];
				}
				else
				{
					Screen += ' ';
				}
			}
			Screen += "\n";
		}
		printf("%s", Screen.c_str());
#ifdef _WIN32
		//CaptureScreen(GetForegroundWindow(), (std::to_string(Tick) + ".bmp").c_str());
#endif
		Screen.clear();
	} while( Tick < 300 );
	return 0;
}