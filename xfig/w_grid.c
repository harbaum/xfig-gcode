/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish distribute, sublicense and/or sell copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 *
 */

#include "fig.h"
#include "figx.h"
#include "resources.h"
#include "mode.h"
#include "paintop.h"
#include "object.h"
#include "w_indpanel.h"
#include "w_setup.h"
#include "w_util.h"
#include "w_zoom.h"

#include "u_redraw.h"

#define null_width 32
#define null_height 32

static char	null_bits[null_width * null_height / 8] = {0};

static Pixmap	null_pm, grid_pm = 0;
static unsigned long bg, fg;



void init_grid(void)
{
    DeclareArgs(2);

    if (null_pm == 0) {
	FirstArg(XtNbackground, &bg);
	NextArg(XtNforeground, &fg);
	GetValues(canvas_sw);

	null_pm = XCreatePixmapFromBitmapData(tool_d, canvas_win,
				(char *) null_bits, null_width, null_height,
				      fg, bg, tool_dpth);
    }
}

/* grid in X11 is simply the background of the canvas */			// isometric grid

void setup_grid(void)
{
    double	    spacing;			/* grid spacing in pixels */
    double	    x, x0c, y, y0c, y1c;
    int		    grid, grid_unit;
    int		    dim;			/* size of grid pixmap in pixels */
    int 		xdim, ydim;
    double		xs, ys, yd;
    static	    prev_grid = -1;

    DeclareArgs(2);

    grid = cur_gridmode;

    if( grid == GRID_0 ) {
		FirstArg( XtNbackgroundPixmap, null_pm );
    } else if( cur_gridtype == GRID_ISO ) {
		grid_unit = cur_gridunit;
		
		/* user scale not = 1.0, use tenths of inch if inch scale */
		if (appres.userscale != 1.0 && appres.INCHES)
		    grid_unit = TENTH_UNIT;
		ys = grid_spacing[grid_unit][(grid-1)%GRID_4] * zoomscale/appres.userscale;
		
		/* if zoom is small, use larger grid */
		while( ys < 5.0 && ++grid <= GRID_4 ) {
		    ys = grid_spacing[grid_unit][(grid-1)%GRID_4] * zoomscale/appres.userscale;
		}
		
		/* round to three decimal places */
		xs = ys * cos( 30.0 / 180.0 * M_PI );
		ys = (int) ( ys * 1000.0 ) / 1000.0;
		xs = (int) ( xs * 1000.0 ) / 1000.0;
	
		if( ys <= 4.0 ) {
		    /* too small at this zoom, no grid */
		    FirstArg(XtNbackgroundPixmap, null_pm);
		    redisplay_canvas();
		} else {
			/* size of the pixmap equal to 1 inch or 2 cm to reset any 
			   error at those boundaries */
			dim = (int) round(appres.INCHES? 
					PIX_PER_INCH*zoomscale/appres.userscale:
					2*PIX_PER_CM*zoomscale/appres.userscale);
					
			/* HOWEVER, if that is a ridiculous size, limit it to approx screen size */
			if (dim > CANVAS_WD)
			    dim = CANVAS_WD;
			
			/* set both dimensions */
			xdim = ydim = dim;
	
			/* align vertical dim and spacing */
			while( fabs( (float) ydim / ys - (int) ( (float) ydim / ys ) ) > 0.01 ) {
			    ydim++;
			    if( ydim > CANVAS_HT ) break;
			}
 			/* align horizontal dim and spacing */
			while( fabs( (float) xdim / ( 2.0 * xs ) - (int) ( (float) xdim / ( 2.0 * xs ) ) ) > 0.01 ) {
			    xdim++;
			    if( xdim > CANVAS_WD ) break;
			}

			/* debug dump */
//			printf( "xs: %f\t ys: %f\n", xs, ys );
//			printf( "xdim: %i\t ydim: %i\n", xdim, ydim );

			/* clear pixmap */
			if (grid_pm) XFreePixmap(tool_d, grid_pm);
			grid_pm = XCreatePixmap(tool_d, canvas_win, xdim, ydim, tool_dpth);
			
			/* first fill the pixmap with the background color */
			XSetForeground(tool_d, grid_gc, bg);
			XFillRectangle(tool_d, grid_pm, grid_gc, 0, 0, xdim, ydim);
			
			/* calculate horizontal offset */
			x0c = -fmod( (double) zoomscale * zoomxoff, ( 2.0 * xs ) );
			if( ( 2.0 * xs ) - x0c < 0.5 ) x0c = 0.0;

			/* calculate vertical offsets */	
			y0c = fmod( -x0c * ( ys / 2.0 ) / xs - zoomscale * zoomyoff, ys );
			y1c = fmod( x0c * ( ys / 2.0 ) / xs - zoomscale * zoomyoff, ys );
//			printf( "y0c: %f\t y1c: %f\n", y0c, y1c );

			/* set grid color */
			XSetForeground(tool_d, grid_gc, grid_color);				
			
			/* draw horizontal lines */
			for( x = x0c - xs; x < xdim; x += xs )
			    XDrawLine( tool_d, grid_pm, grid_gc, (int) round(x), 0, (int) round(x), ydim );
			    
			/* draw diagonal lines */
			yd = xdim * ( ys / 2.0 ) / xs;
			for( y = y0c - (int) ( ydim / ys + 1 ) * ys; y < ydim; y += ys )
			    XDrawLine( tool_d, grid_pm, grid_gc, 0, (int) round(y), xdim, (int) round( y + yd ) );
			for( y = y1c; y < ydim + (int) ( ydim / ys + 1 ) * ys; y += ys )
			    XDrawLine( tool_d, grid_pm, grid_gc, 0, (int) round(y), xdim, (int) round( y - yd ) );
//			XDrawLine( tool_d, grid_pm, grid_gc, 0, 0, xdim, ydim );
			    
	
			FirstArg(XtNbackgroundPixmap, grid_pm);
		}
    } else {
	    grid_unit = cur_gridunit;
	    
		/* user scale not = 1.0, use tenths of inch if inch scale */
		if (appres.userscale != 1.0 && appres.INCHES)
		    grid_unit = TENTH_UNIT;
		spacing = grid_spacing[grid_unit][(grid-1)%GRID_4] * zoomscale/appres.userscale;
		
		/* if zoom is small, use larger grid */
		while (spacing < 5.0 && ++grid <= GRID_4) {
		    spacing = grid_spacing[grid_unit][(grid-1)%GRID_4] * zoomscale/appres.userscale;
		}
		/* round to three decimal places */
		spacing = (int) (spacing*1000.0) / 1000.0;
//		printf( "spacing: %f\n", spacing );
	
		if (spacing <= 4.0) {
		    /* too small at this zoom, no grid */
		    FirstArg(XtNbackgroundPixmap, null_pm);
		    redisplay_canvas();
		} else {
			/* size of the pixmap equal to 1 inch or 2 cm to reset any 
			   error at those boundaries */
			dim = (int) round(appres.INCHES? 
					PIX_PER_INCH*zoomscale/appres.userscale:
					2*PIX_PER_CM*zoomscale/appres.userscale);

//			printf( "dim: %i\t", dim );
					
			/* HOWEVER, if that is a ridiculous size, limit it to approx screen size */
			if (dim > CANVAS_WD)
			    dim = CANVAS_WD;
//			printf( "dim: %i\t", dim );
	
			/* Do something... does not make sense... */
			while (fabs((float) dim / spacing - (int) ((float) dim / spacing)) > 0.01) {
			    dim++;
			    if (dim > CANVAS_WD)
				break;
			}
//			printf( "dim: %i\n", dim );
	
			if (grid_pm) XFreePixmap(tool_d, grid_pm);
			grid_pm = XCreatePixmap(tool_d, canvas_win, dim, dim, tool_dpth);
			
			/* first fill the pixmap with the background color */
			XSetForeground(tool_d, grid_gc, bg);
			XFillRectangle(tool_d, grid_pm, grid_gc, 0, 0, dim, dim);
			
			/* now write the grid in the grid color */
			XSetForeground(tool_d, grid_gc, grid_color);
			x0c = -fmod((double) zoomscale * zoomxoff, spacing);
			y0c = -fmod((double) zoomscale * zoomyoff, spacing);
//			printf( "zxo: %f\t zyo: %f\n", (double)zoomscale * zoomxoff, (double)zoomscale * zoomyoff );
//			printf( "x0c: %f\t y0c: %f\n", x0c, y0c );

			/* does not make sense, spacing > 4 and x0c < 0 */
			if (spacing - x0c < 0.5) x0c = 0.0;
			if (spacing - y0c < 0.5) y0c = 0.0;

			/* draw the lines */
			for (x = x0c; x < dim; x += spacing)
			    XDrawLine(tool_d, grid_pm, grid_gc, (int) round(x), 0, (int) round(x), dim);
			for (y = y0c; y < dim; y += spacing)
			    XDrawLine(tool_d, grid_pm, grid_gc, 0, (int) round(y), dim, (int) round(y));
//			printf( "done\n" );
	
			FirstArg(XtNbackgroundPixmap, grid_pm);
		}
	}
    SetValues(canvas_sw);
    if (prev_grid == GRID_0 && grid == GRID_0)
	redisplay_canvas();
    prev_grid = grid;
}
