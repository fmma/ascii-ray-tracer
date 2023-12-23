#include <windows.h> 
#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <string>

#include "constants.h"
#include "linalg.h"
#include "console.h"
#include "shapes.h"

const char Shades0[] = "12345678";
const char Shades1[] = ".coCO0@";
const char Shades2[] = "-+=LH#E";
const char Shades3[] = "*a&%$";
const char Shades4[] = ">zxZX";
const char Shades5[] = ".::";
const char sky = ' ';
const char grass = ':';
// const char Shades[] = ".:*o#@"; // ".:*oe&#%@";

static size_t Tick = 0;

//// SCENE
Shapes::Shape Scene(const Vec3& Point)
{
	Shapes::Shape Distance = Shapes::Shape{0,0};
	// Distance = Shapes::Plane(Point, Vec3{ 0,1,0 });
	// Distance = Shapes::Sphere(Point, Real(2));

	Distance = Shapes::PlaneS(Point, Vec3{ 0,1,0 }, 5);
	Distance = Operations::Union(Distance,
		Shapes::BoxS(Point - Vec3{10,1,1}, Vec3{1,4,10}, 3));
/*
	Distance = Operations::Union(
		Distance,
		Shapes::RoundBoxS(
			Translate::RotZ(
				Point - Vec3{ -5,2,5 }, Real(15 + Tick * 15)
				),
			Vec3{ 1,2,1 }, 0.525f, 4)
		);

	Distance = Operations::Union(
		Distance,
		Shapes::CapsuleS(Point,
						Vec3{ 5,5,6 },
						Vec3{ -5,5,6 },
						Real(3.75),
						3));

	Distance = Operations::SmoothUnion(
		Distance,
		Shapes::TorusS(
			Translate::RotX(Point - Vec3{ 0,Real(0.5),12 }, 45),
			Real(0.5), Real(7),
			3), 3
		);

	for( size_t i = 1; i < 5; i++ )
	{
		Distance = Operations::SmoothUnion(
			Distance,
			Shapes::TorusS(
				Translate::RotZ(
					Translate::RotX(Point - Vec3{ 0,Real(0.5),Real(12 + i * 7) }, 85), Real(i * 30 + (Tick * 5))
					),
				Real(1), Real(6),
				3), 3);
	}
	*/
	return Distance;
}

Vec3 CalcNormal(const Vec3& Point)
{
#define EPSILON static_cast<Real>(0.001)
	return Vec3{
		Scene(Point + Vec3{ EPSILON,0,0 }).Distance - Scene(Point - Vec3{ EPSILON,0,0 }).Distance,
		Scene(Point + Vec3{ 0,EPSILON,0 }).Distance - Scene(Point - Vec3{ 0,EPSILON,0 }).Distance,
		Scene(Point + Vec3{ 0,0,EPSILON }).Distance - Scene(Point - Vec3{ 0,0,EPSILON }).Distance,
	}.Normalized();
}

Shapes::Shape March(const Vec3& Origin, const Vec3& Ray, bool* Hit)
{
	Real Distance = 0;
	int Material = 0;
	for( size_t i = 0; i < 128; i++ )
	{
		Shapes::Shape ClosestSurface = Scene(Origin + (Ray * Distance));
		if( ClosestSurface.Distance < PREC )
		{
			// "Hit" a surface
			if( Hit != nullptr )
			{
				Material = ClosestSurface.Material;
				*Hit = true;
			}
			break;
		}
		Distance += ClosestSurface.Distance * Real(0.9);
	}
	return Shapes::Shape{Distance, Material};
}

Real Shadow(const Vec3& LightPos, const Vec3& LightDir, Real Min, Real Max, Real K)
{
	Real Res = 1;
	for( Real t = Min; t < Max;)
	{
		Shapes::Shape Distance = Scene(LightPos + LightDir*t);
		if( Distance.Distance < PREC )
		{
			return 0.0;
		}
		Res = std::min(Res, K*Distance.Distance / t);
		t += Distance.Distance;
	}
	return Res;
}

int main()
{
	system("clear");
	Real AngleX = Real(0.0);
	Real AngleY = Real(0.0);
	int Running = 1;
	HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);  // handle to read console

	double uvz = Real(1 / 0.7265425280053608858);

    DWORD Events = 0;     // Event count
    DWORD EventsRead = 0; // Events read from console

	Vec3 LightDir = Vec3{ 1,1,-1 }.Normalized();
	Vec3 EyePos = Vec3{ 0,2,-6 };

	std::string Screen;
	Screen.reserve(WIDTH * HEIGHT);
	do
	{
		// gets the systems current "event" count
        GetNumberOfConsoleInputEvents(rhnd, &Events);

        if(Events != 0){ // if something happened we will handle the events we want

            // create event buffer the size of how many Events
            INPUT_RECORD eventBuffer[Events];

            // fills the event buffer with the events and saves count in EventsRead
            ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);

            // loop through the event buffer using the saved count
            for(DWORD i = 0; i < EventsRead; ++i){

                // check if event[i] is a key event && if so is a press not a release
                if(eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown){
                    // check if the key press was an arrow key
                    switch(eventBuffer[i].Event.KeyEvent.wVirtualKeyCode){
                        case VK_LEFT:
							// EyePos.X -= Real(0.25);
							AngleX -= Real(3.0);
							break;
                        case VK_RIGHT:
							// EyePos.X += Real(0.25);
							AngleX += Real(3.0);
							break;
                        case VK_UP:
							AngleY += Real(3.0);
							// EyePos.Z += Real(0.25);
							break;
                        case VK_DOWN:   // if any arrow key was pressed break here
							AngleY -= Real(3.0);
							// 
							break;

                        case VK_ESCAPE: // if escape key was pressed end program loop
                            Running = false;
                            break;
						case 0x57: // w
							EyePos = EyePos + Translate::RotY(Vec3{0,0,Real(0.25)}, AngleX); // Real(0.25);
							break;
						case 0x41: // a
							EyePos = EyePos + Translate::RotY(Vec3{Real(-0.25),0,0}, AngleX); // Real(0.25);
							break;
						case 0x53: // s
							EyePos = EyePos + Translate::RotY(Vec3{0,0,Real(-0.25)}, AngleX); // Real(0.25);
							break;
						case 0x44: // d
							EyePos = EyePos + Translate::RotY(Vec3{Real(0.25),0,0}, AngleX); // Real(0.25);
							break;

                        default:        // no handled cases where pressed 
                            break;
                    }
                }

            } // end EventsRead loop

        }

		// EyePos.Z += Real(0.25);
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

				UV.Z = uvz;
				UV = Translate::RotY(UV, AngleX);
				UV = Translate::RotX(UV, AngleY);

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
				UV = UV.Normalized();

				bool Hit = false;
				Shapes::Shape Distance = March(EyePos, UV, &Hit);

				Vec3 Point = EyePos + (UV * Distance.Distance);

				if( Hit )
				{
					Vec3 Normal = CalcNormal(Point);
					Real Diffuse = Normal.Dot(LightDir);
					Diffuse *= 0.5;
					Diffuse += 0.5;

					Diffuse *= Diffuse;

					Diffuse *= Shadow(Point, LightDir, Real(0.5), 10, 10);

					switch(Distance.Material) {
						case 0:
							Screen += Shades0[static_cast<size_t>(Diffuse*(sizeof(Shades0) - 2))];
							break;
						case 1:
							Screen += Shades1[static_cast<size_t>(Diffuse*(sizeof(Shades1) - 2))];
							break;
						case 2:
							Screen += Shades2[static_cast<size_t>(Diffuse*(sizeof(Shades2) - 2))];
							break;
						case 3:
							Screen += Shades3[static_cast<size_t>(Diffuse*(sizeof(Shades3) - 2))];
							break;
						case 4:
							Screen += Shades4[static_cast<size_t>(Diffuse*(sizeof(Shades4) - 2))];
							break;
						case 5:
							Screen += Shades5[static_cast<size_t>(Diffuse*(sizeof(Shades5) - 2))];
							break;
					}
				}
				else
				{
					if(y > HEIGHT/2)
						Screen += grass;
					else
						Screen += sky;
				}
			}
			Screen += "\n";
		}
		printf("%s", Screen.c_str());
		ResetCursor();
#ifdef _WIN32
		//CaptureScreen(GetForegroundWindow(), (std::to_string(Tick) + ".bmp").c_str());
#endif
		Screen.clear();
	} while( Running );
	system("clear");
	return 0;
}