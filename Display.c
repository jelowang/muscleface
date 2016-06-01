// ==========================================================================
//                            DISPLAY_C
// ==========================================================================
//
//    FUNCTION NAMES
//
//    void paint_muscles 					-- displays the muscles.
//    paint_polygons						-- display the polygons.
//    calculate_polygon_vertex_normal 		-- calculate the vertex norms.
//    calc_normal 							-- calculate a normal.
//
//    C SPECIFICATIONS
//
//    void paint_muscles 					( HEAD *face )
//    void paint_polyline 					( HEAD *face )
//    paint_polygons      					( HEAD *face, int type, int normals )
//    calculate_polygon_vertex_normal 		( HEAD *face ) 
//    calc_normal 							( float *p1, float *p2, float *p3, 
//                                            float *norm )
//
//    DESCRIPTION
//    
//    This module is responsible for displaying the face geometry.
//    This module comes as is with no warranties.  
//
//    HISTORY
//		16-Dec-94  Keith Waters (waters@crl.dec.com) Created at Digital Equipment
//			       Cambridge Research Labatory
//		10-Aug-98  Keith Waters (waters@crl.dec.com) Modified for OpenGL on W95 and 
//				   WNT at Compaq Cambridge Research Laboratory
//
// ============================================================================ 

#include "math.h"       // C header for any math functions              
#include "stdio.h"
#include "glos.h"		// MS specifc

#include <GL/gl.h>		// OpenGL includes
#include <GL/glu.h>		
#include "glaux.h"

#include "memory.h"     // Local memory allocation macros                
#include "head.h"	    // local header for the face		     

// Forward declarations
void calc_normal				( float*, float*, float*, float* ) ;
void averaged_vertex_normals 	( HEAD*, int, float*, float*, float*  ) ; 
void paint_polygons     		( HEAD*, int, int ) ;

// ========================================================================= 
// paint_muscles                                                             
// ========================================================================= 
//
// Displays the face muscles.
//
//
#define PAINT_MUSCLES_DEBUG 0
void paint_muscles ( HEAD *face )
{
  int i,j ;
  float v1[3], v2[3] ;
  static float r ;

    for ( i=0; i<face->nmuscles; i++ ) {
    	
	// Make the current muscle wide and a diffent color
	  if ( face->current_muscle == i ) {
		  glLineWidth ( 5.0 ) ;
		  glColor3f	  ( 1.0, 0.9, 0.1 ) ;
	  } else {
		  glLineWidth   ( 3.0 ) ;
		  glColor3f     ( 0.9, 0.0, 0.0) ;
	  }

      for (j=0; j<3; j++) {
      v1[j] = face->muscle[i]->head[j] ;
      v2[j] = face->muscle[i]->tail[j] ;
    }
      
#if PAINT_MUSCLES_DEBUG
    fprintf (stderr, "head x: %f y: %f z: %f\n",   v1[0], v1[1], v1[2] ) ;
    fprintf (stderr, "tail x: %f y: %f z: %f\n\n", v2[0], v2[1], v2[2] ) ;
#endif

      glBegin ( GL_LINE_STRIP ) ;
      glVertex3f ( v1[0], v1[1], v1[2] ) ;
      glVertex3f ( v2[0], v2[1], v2[2] ) ;
      glEnd ( ) ;
  }
  glLineWidth   ( 1.0 ) ;
}

// ========================================================================= 
// paint_polygons                                                            
// ========================================================================= 
//
// Paints the polygons of the face. 
// Type indicates if they are to be 
// drawn         (type=0), 
// flat shaded   (type=1),
// smooth shaded (type=2).
// ignored		 (type=3).
//

void paint_polygons ( HEAD *face, int type, int normals )
{
  int    i, j ;
  float  v1[3], v2[3], v3[3] ;
  float  norm1[3], norm2[3], norm3[3] ;
  float  vn1[3], vn2[3], vn3[3] ;
  int    face_line ;

  glLineWidth   ( 2.0 ) ;  

  face_line = 0 ;

  // Note: this checks for transparency
  if ( face->rendermode == 3 ) type = 2 ;

  for (i=0; i<face->npolygons; i++ )
    {
      for (j=0; j<3; j++) {
	v1[j] = face->polygon[i]->vertex[0]->xyz[j] ;
	v2[j] = face->polygon[i]->vertex[1]->xyz[j] ;
	v3[j] = face->polygon[i]->vertex[2]->xyz[j] ;
      }

      if ( type == 0 ) {
	
	for (j=0; j<3; j++) {
	  norm1[j] = face->polygon[i]->vertex[0]->norm[j] ;
	  norm2[j] = face->polygon[i]->vertex[1]->norm[j] ;
	  norm3[j] = face->polygon[i]->vertex[2]->norm[j] ;
	}
	glBegin ( GL_LINE_LOOP ) ; {
	  glNormal3f ( norm1[0], norm1[1], norm1[2] ) ;
	  glVertex3f ( v1[0],    v1[1],    v1[2]    ) ;
	  glNormal3f ( norm2[0], norm2[1], norm2[2] ) ;
	  glVertex3f ( v2[0],    v2[1],    v2[2]    ) ;
	  glNormal3f ( norm3[0], norm3[1], norm3[2] ) ;
	  glVertex3f ( v3[0],    v3[1],    v3[2]    ) ;
	}  glEnd ( ) ;

      } // end if drawn 

      if ( type == 1 ) {
	
	for (j=0; j<3; j++) {
	  norm1[j] = face->polygon[i]->vertex[0]->norm[j] ;
	  norm2[j] = face->polygon[i]->vertex[1]->norm[j] ;
	  norm3[j] = face->polygon[i]->vertex[2]->norm[j] ;
	}
	glBegin ( GL_TRIANGLES ) ; {
	  glNormal3f ( norm1[0], norm1[1], norm1[2] ) ;
	  glVertex3f ( v1[0],    v1[1],    v1[2]    ) ;
	  glNormal3f ( norm2[0], norm2[1], norm2[2] ) ;
	  glVertex3f ( v2[0],    v2[1],    v2[2]    ) ;
	  glNormal3f ( norm3[0], norm3[1], norm3[2] ) ;
	  glVertex3f ( v3[0],    v3[1],    v3[2]    ) ;
	}  glEnd ( ) ;

      } // end if drawn 


      else if ( type == 1) {
	for (j=0; j<3; j++) {
	  norm1[j] = face->polygon[i]->vertex[0]->norm[j] ;
	  norm2[j] = face->polygon[i]->vertex[1]->norm[j] ;
	  norm3[j] = face->polygon[i]->vertex[2]->norm[j] ;
	}
      } // end if flat 

      else if ( type == 2 ) {

	averaged_vertex_normals ( face, i, norm1, norm2, norm3 ) ;

      } // end if smoothed 

      if ( type ) {
	glBegin ( GL_TRIANGLES ) ; {
	  glNormal3f ( norm3[0], norm3[1], norm3[2] ) ;
	  glVertex3f ( v3[0],    v3[1],    v3[2]    ) ;

	  glNormal3f ( norm2[0], norm2[1], norm2[2] ) ;
	  glVertex3f ( v2[0],    v2[1],    v2[2]    ) ;

	  glNormal3f ( norm1[0], norm1[1], norm1[2] ) ;
	  glVertex3f ( v1[0],    v1[1],    v1[2]    ) ;

	}  glEnd ( ) ;
      } // endif painted 

      if ( normals ) {
	for (j=0; j<3; j++) {
	  vn1[j] = face->polygon[i]->vertex[0]->xyz[j] + norm1[j] ;
	  vn2[j] = face->polygon[i]->vertex[1]->xyz[j] + norm2[j] ;
	  vn3[j] = face->polygon[i]->vertex[2]->xyz[j] + norm3[j] ;
	}

	glBegin ( GL_LINE_STRIP ) ; {
	  glVertex3f ( v1[0],    v1[1],    v1[2]     ) ;
	  glVertex3f ( vn1[0],  vn1[1],    vn1[2]    ) ;
	}  glEnd ( ) ;


	glBegin ( GL_LINES ) ; {
	  glVertex3f ( v2[0],    v2[1],    v2[2]     ) ;
	  glVertex3f ( vn2[0],  vn2[1],    vn2[2]    ) ;
	}  glEnd ( ) ;


	glBegin ( GL_LINES ) ; {
	  glVertex3f ( v3[0],    v3[1],    v3[2]     ) ;
	  glVertex3f ( vn3[0],  vn3[1],    vn3[2]    ) ;
	}  glEnd ( ) ;

      }
    }
  glLineWidth   ( 1.0 ) ;  
} 


// ======================================================================== 
// calculate_polygon_vertex_normal.											
// ======================================================================== 
//
// As it says.
//

void calculate_polygon_vertex_normal ( HEAD *face ) 
{
  int i,j,k ;
  float p1[3], p2[3], p3[3] ;
  float norm[3] ;
  for (i=0; i<face->npolygons; i++ )
    {
      for (j=0; j<3; j++) 
	p1[j] = face->polygon[i]->vertex[0]->xyz[j] ;
      for (j=0; j<3; j++) 
	p2[j] = face->polygon[i]->vertex[1]->xyz[j] ;
      for (j=0; j<3; j++) 
	p3[j] = face->polygon[i]->vertex[2]->xyz[j] ;

      calc_normal ( p1, p2, p3, norm ) ;

      for (j=0; j<3; j++) 
	for (k=0; k<3; k++)
	  face->polygon[i]->vertex[j]->norm[k] = norm[k] ;
    }
}
// ========================================================================= 
// calc_normal.					     			     
// ========================================================================= 
//
// Calculates the normal vector from three vertices.
//

void calc_normal ( float *p1, float *p2, float *p3, float *norm )
{

	GLdouble v1[3], v2[3], d;

	v1[0] = p2[0] - p1[0] ;
	v1[1] = p2[1] - p1[1] ;
	v1[2] = p2[2] - p1[2] ;

	v2[0] = p2[0] - p3[0] ;
	v2[1] = p2[1] - p3[1] ;
	v2[2] = p2[2] - p3[2] ;

	norm[0] = v1[1]*v2[2] - v2[1]*v1[2];
	norm[1] = v1[2]*v2[0] - v2[2]*v1[0];
	norm[2] = v1[0]*v2[1] - v2[0]*v1[1];

	d = ( norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2] );
	if ( d < (GLdouble)0.0000001 )
	{
		d = (GLdouble)100000000.0;
	}
	else
	{
		d = (GLdouble)1.0/sqrt(d);
	}

	norm[0] *= d;
	norm[1] *= d;
	norm[2] *= d;
}

