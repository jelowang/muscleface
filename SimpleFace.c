// ==========================================================================
//                            SIMPLEFACE.C
// ==========================================================================
//
// This is an OpenGL implementation of the simple face from Appendix 1 in 
// Computer Facial Animation. It demonstrates the use of the muscle model 
// to create facial expressions.
//
//
//    HISTORY
//	16-Dec-94  Keith Waters (waters@crl.dec.com) Created at Digital Equipment
//		       Cambridge Research Laboratory
//
//	10-Aug-98  Keith Waters (waters@crl.dec.com) Modified for OpenGL on W95 and 
//			   WNT at Compaq Cambridge Research Laboratory. This code attempts
//			   to be as close to the original code as possible. Module names
//			   and function definitions have been maintained for clarity. A 
//			   few extra components have been added in particular expression I/O
//			   so you can make your own expressions. In addition transparency
//			   has been added to observe the location of facial muscles.
//
// ============================================================================ 

#include "stdio.h"

#include "glos.h"		// MS specifc

#include <GL/gl.h>		// OpenGL includes
#include <GL/glu.h>		
#include "glaux.h"

#include "head.h"		// Face includes

void OpenGLInit(void);

static void CALLBACK Animate	( void );
static void CALLBACK Key_e		( void );
static void CALLBACK Key_b		( void );
static void CALLBACK Key_c		( void );
static void CALLBACK Key_C		( void );
static void CALLBACK Key_H		( void );
static void CALLBACK Key_n		( void );
static void CALLBACK Key_r		( void );
static void CALLBACK Key_R		( void );
static void CALLBACK Key_e		( void );
static void CALLBACK Key_up		( void );
static void CALLBACK Key_down	( void );
static void CALLBACK Key_right	( void );
static void CALLBACK Key_left	( void );
static void CALLBACK Key_w		( void );
static void CALLBACK ResizeWindow(GLsizei w, GLsizei h);

// Forward declarations
void calc_normal					( float*, float*, float*, float* ) ;
void calculate_polygon_vertex_normal( HEAD * ) ;
void FaceInit						( );
void paint_muscles					( HEAD* ) ;
void paint_polygons     			( HEAD*, int, int ) ;
void data_struct					( HEAD* ) ;
void read_muscles					( char* , HEAD* ) ;
void read_expression_macros			( char* , HEAD* ) ;
void activate_muscle				( HEAD*, float*, float*, float, float, float, float ) ;
void face_reset						( HEAD* ) ;
void make_expression				( HEAD*, int ) ;
HEAD *create_face					( char*, char* ) ;

GLfloat rotateX, rotateY ;

HEAD *face ;

// ======================================================================== 
// Key bindings for callback proceedures
// ========================================================================   
//
static void CALLBACK Key_e	(void)
{
	face_reset		( face ) ;
	make_expression ( face, face->current_exp ) ;
	face->current_exp++ ;
	if ( face->current_exp >= face->nexpressions ) 
		face->current_exp = 0 ;

}
static void CALLBACK Key_b	(void)
{
    face->rendermode++;
	if ( face->rendermode > 3 ) 
		face->rendermode = 0 ;
}
static void CALLBACK Key_c	(void)
{
	int cm ;

	cm = face->current_muscle ;

	// Record the muscle activation 
	face->muscle[cm]->mstat += 0.1 ;
	
	activate_muscle ( face, 
			 face->muscle[cm]->head, 
			 face->muscle[cm]->tail, 
			 face->muscle[cm]->fs,
			 face->muscle[cm]->fe,
			 face->muscle[cm]->zone,
			 0.1 ) ;
}
static void CALLBACK Key_C	(void)
{
	int cm ;

	cm = face->current_muscle ;

	// Record the muscle activation 
	face->muscle[cm]->mstat -= 0.1 ;

	
	activate_muscle ( face, 
			 face->muscle[cm]->head, 
			 face->muscle[cm]->tail, 
			 face->muscle[cm]->fs,
			 face->muscle[cm]->fe,
			 face->muscle[cm]->zone,
			 -0.1 ) ;

}
static void CALLBACK Key_n	(void)
{
	face->current_muscle++ ;
	if ( face->current_muscle >= face->nmuscles ) 
		face->current_muscle = 0 ;

}
static void CALLBACK Key_up	(void)
{
	rotateX -= 2.0f;
}
static void CALLBACK Key_down(void)
{
	rotateX += 2.0f;	
}
static void CALLBACK Key_left(void)
{
	rotateY  -= 2.0f;
}
static void CALLBACK Key_right(void)
{
	rotateY  += 2.0f;
}
static void CALLBACK Key_r	(void)
{
    face_reset ( face ) ;
}
static void CALLBACK Key_R	(void)
{
    face_reset					( face ) ;
	read_expression_macros		("../face-data/expression-macros.dat", face ) ;
	face->current_exp = 0 ;
}
static void CALLBACK Key_w	(void)
{
	FILE *OutFile ;
	int i;

	//  Check the FileName
	if (( OutFile = fopen ( "../face-data/single-expression.dat", "w+" )) == 0 ) {
	   fprintf (stderr, "Can't open:%s\n", OutFile );
	   return;
	   }

   for ( i=0; i<face->nmuscles; i++) {

	   fprintf (OutFile, "%s %2.2f\t", face->muscle[i]->name, face->muscle[i]->mstat ) ;
   }

	fclose ( OutFile ) ;
}
static void CALLBACK Key_h (void)
{
	fprintf(stderr,"\n");
	fprintf(stderr,"b:       toggles the drawing  modes (Wireframe, facet, smooth & transparent)\n");
	fprintf(stderr,"c:       contract the current facial muscle\n");
	fprintf(stderr,"C:       relax the current facial muscle\n");
	fprintf(stderr,"e:       next expression\n");
	fprintf(stderr,"h:       outputs this message\n");
	fprintf(stderr,"n:       next muscle (to select another facial muscle to manipulate)\n");
	fprintf(stderr,"r:       reset the expression\n");
	fprintf(stderr,"R:       reread the expression file (../face-data/expression-macros.dat)\n");
	fprintf(stderr,"w:           \t outputs muscles state (../face-data/single-expression.dat)\n");
	fprintf(stderr,"Up arrow:    \t rotates the face up\n");
	fprintf(stderr,"Down arrow:  \t rotates the face down\n");
	fprintf(stderr,"Right arrow: \t rotates the face right\n");
	fprintf(stderr,"Left arrow:  \t rotates the face left\n");

	fprintf(stderr,"\n");
}
// ======================================================================== 
// Animate ()																	
// ========================================================================   
//
// The big enchilda. The animation callback
//

static void CALLBACK Animate(void)
{
	GLfloat light_position [] = { 30.0, 70.0, 100.0, 1.0 }  ;

	// clear the redering window
    glClear			( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// clear current matrix (Modelview)
    glLoadIdentity ( );

	// back off thirty units down the Z axis
    glTranslatef	( 0.0f, 0.0f, -30.0f );

	// Use the keyboard to grab the rotations
	glRotatef		( rotateX, 1.0f, 0.0f, 0.0f );
	glRotatef		( rotateY, 0.0f, 1.0f, 0.0f );

    glPushMatrix ( ) ; 
     glLightfv		( GL_LIGHT0, GL_POSITION, light_position ) ;  
     glTranslated 	( 0.0, 0.0, 50.0 ) ;
     glEnable		( GL_LIGHTING ) ;
    glPopMatrix ( ) ;

	// Transparancy 
	if ( face->rendermode == 3 ) {
		glEnable		( GL_BLEND );
		glDepthMask		( GL_FALSE );
		glBlendFunc		( GL_SRC_ALPHA, GL_ONE );
	} else {
		glDisable		( GL_BLEND );
		glDepthMask		( GL_TRUE );
	}

	// Re-calculate the polygon normals as  
	// the face could have been distorted
	calculate_polygon_vertex_normal 	( face ) ;
	paint_polygons 						( face, face->rendermode, 0 ) ;
  
	// if the rendering mode is wireframe 
	// or transparent then display the muscles
	if ( face->rendermode == 0 || 
		 face->rendermode == 3 ) {
		glDisable						( GL_LIGHTING );
		paint_muscles					( face ) ;
	}

	// Now flush the pipeline and
	// swap the buffers
    glFlush			( );
    auxSwapBuffers	( );
}

// ======================================================================== 
// OpenGLInit ()																	
// ========================================================================   
//
// Initialize OpenGL
//

void OpenGLInit(void)
{
	glEnable    ( GL_LIGHTING   );
	glEnable    ( GL_LIGHT0     );
	glClearDepth( 1.0f			); 
	glDepthFunc ( GL_LEQUAL     );
	glEnable    ( GL_DEPTH_TEST );
}

// ======================================================================== 
// ResizeWindow ()																	
// ========================================================================   
//
// This get calls when the window gets resized.
//

static void CALLBACK ResizeWindow(GLsizei w, GLsizei h)
{
    h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;

	glViewport		( 0, 0, w, h );
    glMatrixMode	( GL_PROJECTION );
    glLoadIdentity	( );
    gluPerspective	( 40.0, (GLfloat)w/(GLfloat)h, 1.0f, 200.0f );
	
	// select the Modelview matrix
    glMatrixMode	( GL_MODELVIEW );
}

// ======================================================================== 
// Main ()																	
// ========================================================================   
//
// Setup OpenGL, setup the face, hook up callbacks and start the main loop.
//

int main( int argc, char** argv )
{
	auxInitDisplayMode	( AUX_DOUBLE | AUX_RGBA | AUX_DEPTH);
    auxInitPosition		( 0, 0, 320, 512 );
    auxInitWindow		( "Simple Face Example" );

	// Initialize OpenGL
    OpenGLInit			( );
	
	// Initialize the face
	FaceInit			( );

	// Set up callback functions
    auxKeyFunc			( AUX_UP,	 Key_up );		// Up
    auxKeyFunc			( AUX_DOWN,  Key_down );	// Down
    auxKeyFunc			( AUX_LEFT,  Key_left );	// Left
    auxKeyFunc			( AUX_RIGHT, Key_right );	// Right
	auxKeyFunc			( AUX_e, Key_e );			// Expression macro activation
	auxKeyFunc			( AUX_b, Key_b );			// Rendering toggle
	auxKeyFunc			( AUX_c, Key_c );			// Active the current muscle inc
	auxKeyFunc			( AUX_C, Key_C );			// Active the current muscle dec
	auxKeyFunc			( AUX_h, Key_h );			// Commmand print
	auxKeyFunc			( AUX_n, Key_n );			// Make the next muscle the active one
	auxKeyFunc			( AUX_r, Key_r );			// Reset the face
	auxKeyFunc			( AUX_R, Key_R );			// Reread the expression macro file
	auxKeyFunc			( AUX_w, Key_w );			// Writes the current muscle state out.
    auxReshapeFunc		( ResizeWindow );

	// Call this in main loop
	auxMainLoop			( Animate );

    return(0);
}

// ======================================================================== 
// FaceInit																	
// ======================================================================== 
//
// Read in the datafiles and initialize the face data structures.
//

void FaceInit ( void )
{
  face = create_face         ("face-data/index.dat", "face-data/faceline.dat");
  read_muscles               ("face-data/muscle.dat", face );
  read_expression_macros	 ("face-data/expression-macros.dat", face );
  data_struct                ( face );
}
