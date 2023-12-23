#ifndef __LINALG__
#define __LINALG__

typedef float Real;

union Vec3
{
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
			Z + Other.Z
		};
	}
	Vec3 operator - (const Vec3& Other) const
	{
		return Vec3{
			X - Other.X,
			Y - Other.Y,
			Z - Other.Z
		};
	}

	Vec3 operator * (const Vec3& Other) const
	{
		return Vec3{
			X * Other.X,
			Y * Other.Y,
			Z * Other.Z
		};
	}

	Vec3 operator / (const Vec3& Other) const
	{
		return Vec3{
			X / Other.X,
			Y / Other.Y,
			Z / Other.Z
		};
	}

	Vec3 operator % (const Vec3& Other) const
	{
		return Vec3{
			fmod(X,Other.X),
			fmod(Y,Other.Y),
			fmod(Z,Other.Z)
		};
	}

	Vec3 operator + (const Real& Other) const
	{
		return Vec3{
			X + Other,
			Y + Other,
			Z + Other
		};
	}

	Vec3 operator - (const Real& Other) const
	{
		return Vec3{
			X - Other,
			Y - Other,
			Z - Other
		};
	}

	Vec3 operator * (const Real& Other) const
	{
		return Vec3{
			X * Other,
			Y * Other,
			Z * Other
		};
	}

	Vec3 operator / (const Real& Other) const
	{
		return Vec3{
			X / Other,
			Y / Other,
			Z / Other
		};
	}

	Vec3 operator % (const Real& Other) const
	{
		return Vec3{
			fmod(X,Other),
			fmod(Y , Other),
			fmod(Z , Other)
		};
	}
};

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


#endif