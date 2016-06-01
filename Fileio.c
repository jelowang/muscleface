// ==========================================================================
//                               FILEIO_C
// =============================================================================
//
//    FUNCTION NAMES
//
//		read_polygon_indices        -- reads the polygon indices file.
//		read_polygon_line			-- read the face polyline.
//		read_muscles				-- reads the face muscles 
//		read_expression_vectors 	-- reads a vector of expressions
//		add_muscle_to_face 			-- add a muscle to the face
//		read_expression_macros		-- read in expression macro vectors

//    C SPECIFICATIONS
//
//		read_polygon_indices	( FileName, face ) 
//		read_polygon_line		( FileName, face )
//		read_muscles			( FileName, face )
//		read_expression_vectors	( FileName, face )
//		add_muscle_to_face		( m, face )
//		read_expression_vectors ( FileName, face )
//
//    DESCRIPTION
//	
//		This module is responsible for reading the face data files.  
//		This module comes as is with no warranties. 
//
//    SIDE EFFECTS
//		Unknown.
//   
//    HISTORY
//		16-Dec-94  Keith Waters (waters@crl.dec.com) Created at Digital Equipment
//			       Cambridge Research Labatory
//		10-Aug-98  Keith Waters (waters@crl.dec.com) Modified for OpenGL on W95 and 
//				   WNT at Compaq Cambridge Research Laboratory
//
// =========================================================================== 

#include <stdlib.h>
#include <stdio.h>		// C header for standard I/O                 

#include "head.h"		// local header for the face data structure  
#include "memory.h"

void add_muscle_to_face			( MUSCLE*, HEAD* ) ;
void read_muscles				( char* ,  HEAD* ) ;

// ========================================================================= 
// read_polygon_indices                                                      
// ========================================================================= 
//
//   Read in the face data file (x,y,z)
//

void read_polygon_indices ( char *FileName, HEAD *face )
 { 
   FILE *InFile ;
   int i, ii ;

   // Check the FileName
      if (( InFile = fopen ( FileName, "r" )) == 0 ) {
		fprintf ( stderr, "read_polygon_indices failed to open: %s\n Are you sure you have the data path correct?", FileName );
     exit ( 1 ) ;
   }
     
   fscanf ( InFile,"%d", &face->npindices ) ;

   //  Allocate some memory
   face->indexlist = ( int * ) malloc ( face->npindices*4 * sizeof ( int )) ; 

   for( i=0, ii=0; i<face->npindices; i++, ii+=4 )
     fscanf(InFile,"%d%d%d%d", 
	    &face->indexlist[ii],   &face->indexlist[ii+1], 
	    &face->indexlist[ii+2], &face->indexlist[ii+3] ) ;
        
   fclose( InFile ) ; 
 }

// ========================================================================= 
// read_polygon_line                                                         
// ========================================================================= 
//
//   Read in the face data file (x,y,z)
//
//

void read_polygon_line ( char *FileName, HEAD *face )
{
  FILE *InFile ;
  int i, ii ;

   /* 
    * Check the FileName.
    */
   if (( InFile = fopen ( FileName, "r" )) == 0 ) {
     fprintf ( stderr, "can't open input file: %s\n", FileName ) ;
     exit(-1) ;
   }

  fscanf ( InFile, "%d", &face->npolylinenodes ) ;

  // Allocate some memory

  face->polyline = ( float * ) malloc ( face->npolylinenodes*3 * sizeof ( float )) ; 

  for ( i=0, ii=0; i<face->npolylinenodes; i++, ii+=3 ) {
    
    fscanf ( InFile,"%f%f%f",
	    &face->polyline[ii], 
	    &face->polyline[ii+1], 
	    &face->polyline[ii+2] ) ;
  }
  
  fclose ( InFile ) ;
}

// ========================================================================= 
//   read_muscles ( FileName, face )
// ========================================================================= 
//
// This function reads in the muscles.
//

void read_muscles ( char* FileName, HEAD* face )
{
  FILE *Infile;
  int i, nm ;
  MUSCLE *m ;

  // Open the file to be read
  if((Infile = fopen(FileName,"r")) == 0) {
      fprintf(stderr,"Opening error on file:%10s\n", FileName) ;
      exit(0);
    }
  fscanf ( Infile, "%d", &nm ) ;

  for ( i=0; i < nm; i++ ) {

      m = _new ( MUSCLE ) ;

      fscanf (Infile, "%s %f %f %f %f %f %f %f %f %f %f",
	      &(*m->name),
	      &m->head[0], &m->head[1], &m->head[2],
	      &m->tail[0], &m->tail[1], &m->tail[2],
	      &m->fs, &m->fe, &m->zone, &m->clampv ) ;

      m->active = 0 ;
      m->mstat  = 0.0 ;
      
      fprintf(stderr,"%s: %d\n========================\nhx: %2.2f hy: %2.2f hz: %2.2f\ntx: %2.2f ty: %2.2f tz: %2.2f\n fall start: %2.2f\n fall end: %2.2f\n zone: %2.2f\n clampv: %2.2f mstat: %2.2f\n\n",
	     m->name, i, 
	     m->head[0], 
	     m->head[1], 
	     m->head[2],
	     m->tail[0], 
	     m->tail[1],
	     m->tail[2],
	     m->fs,
	     m->fe,
	     m->zone,
	     m->clampv,
	     m->mstat ) ;

      add_muscle_to_face ( m, face ) ;

    }

  fclose(Infile) ;
}

// ========================================================================= 
//   add_muscle_to_face ( m, face )
// ========================================================================= 
//
//   adds a muscle to the face muscle list.
//

void add_muscle_to_face ( MUSCLE* m, HEAD* face )
{
  int nn ;

  if(face->nmuscles == 0)
      face->muscle = _new_array(MUSCLE *, 50) ;
  else if(face->nmuscles % 50 == 0)
      face->muscle = _resize_array(face->muscle,MUSCLE *,face->nmuscles+50) ;

  nn = face->nmuscles ;
  face->muscle[nn] = m ;

  face->nmuscles++ ;

}

// ========================================================================= 
// read_expression_macros                                                   
// ========================================================================= 
//
//   Read in the expression macros.
//
void read_expression_macros ( char* FileName, HEAD* face )
{
  FILE *InFile ;
  int i, k, l;
  EXPRESSION *e ;
  static int allocated = 0;
  char tmp2[30], tmp1[30];

   //  Check the FileName
   if (( InFile = fopen ( FileName, "r" )) == 0 ) {
     fprintf ( stderr, "can't open input file: %s\n", FileName ) ;
     exit(-1) ;
   }

  fscanf ( InFile, "%d", &face->nexpressions ) ;
//  fprintf( stderr, "Number of expressions = %d\n", face->nexpressions ) ;

  // Allocate some memory.
  if (!allocated)
    face->expression = (EXPRESSION  **)malloc( face->nexpressions*
					       sizeof(EXPRESSION  *) );
  
  for ( i=0; i<face->nexpressions; i++) {
    if (allocated)
      e = face->expression[i];
    else
      e = face->expression[i] = _new(EXPRESSION) ;

    fscanf ( InFile, "%s\n", &(*e->name) ) ;
    
	for ( k=0, l=0; k < 9; k++, l+=2) {
      
	  fscanf (InFile, "%s %f %s %f", &(*tmp1), &e->m[l], &(*tmp2), &e->m[l+1] );
//      fprintf (stderr,"%d %d %s %f \t%s %f\n", k,l,tmp1, e->m[l], tmp2, e->m[l+1] ) ;
    }
//    fprintf (stderr, "\n") ; 
  }
  
  fclose ( InFile ) ;

  allocated = 1;
}


