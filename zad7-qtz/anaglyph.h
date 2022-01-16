#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>

typedef struct {
   double x,y,z;
} XYZ;
typedef struct {
   unsigned char r,g,b,a;
} PIXELA;
typedef struct {
   double r,g,b;
} COLOUR;

#define ABS(x) (x < 0 ? -(x) : (x))
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
#define SIGN(x) (x < 0 ? (-1) : 1)
#define MODULUS(p) (sqrt(p.x*p.x + p.y*p.y + p.z*p.z))
#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x

typedef struct {
   XYZ vp;              /* View position           */
   XYZ vd;              /* View direction vector   */
   XYZ vu;              /* View up direction       */
   XYZ pr;              /* Point to rotate about   */
   double focallength;  /* Focal Length along vd   */
   double aperture;     /* Camera aperture         */
   double eyesep;       /* Eye separation          */
   int screenheight,screenwidth;
} CAMERA;

void HandleDisplay(void);
void CreateEnvironment(void);
void CreateWorld(void);
void MakeMesh(void);
void MakeBox(void);
void MakeSphere(void);
void MakeKnot(void);
void MakeTriTorus(void);
void MakeLorenz(void);
XYZ  TriTorusEval(double,double);
void MakePulsar(void);
void MakeLighting(void);
void HandleKeyboard(unsigned char key,int x, int y);
void HandleSpecialKeyboard(int key,int x, int y);
void HandleMouse(int,int,int,int);
void HandleMainMenu(int);
void HandleCameraMenu(int);
void HandleGlassesMenu(int);
void HandleSpeedMenu(int);
void HandleModelMenu(int);
void HandleVisibility(int vis);
void HandleReshape(int,int);
void HandleMouseMotion(int,int);
void HandlePassiveMotion(int,int);
void HandleTimer(int);
void GiveUsage(char *);
void RotateCamera(int,int,int);
void TranslateCamera(int,int);
void CameraHome(int);
int  WindowDump(int,int);
void Normalise(XYZ *);
XYZ  CalcNormal(XYZ,XYZ,XYZ);

#define DTOR            0.0174532925
#define RTOD            57.2957795
#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322
#define ESC 27
