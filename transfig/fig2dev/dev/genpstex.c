/*
 * TransFig: Facility for Translating Fig code
 * Copyright (c) 1991 by Micah Beck
 * Parts Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2002 by Brian V. Smith
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and
 * documentation files (the "Software"), including without limitation the
 * rights to use, copy, modify, merge, publish and/or distribute copies of
 * the Software, and to permit persons who receive copies from any such 
 * party to do so, with the only requirement being that this copyright 
 * notice remain intact.
 *
 */

/* 
 *	genpstex.c : psTeX and psTeX_t drivers for fig2dev
 *
 *	Author: Jose Alberto Fernandez R /Maryland CP 9/90
 * 	It uses the LaTeX and PostScript drivers to generate 
 *      LaTeX processed text for a Postscript figure.
 *
 * The pstex_t driver is like a latex driver that only translates 
 * text defined in the default font.
 *
 * The pstex driver is like a PostScript driver that translates 
 * everything except for text in the default font.
 *
 * The pdftex_t and pdftex are drivers for combined PDF/LaTeX.
 *
 * The option '-p file' added to the pstex_t translator specifies
 * the name of the PostScript file to be called in the psfig macro.
 * If not set or its value is null then no PS file will be inserted.
 *
 * Jose Alberto.
 */

#include <sys/stat.h>
#include "fig2dev.h"
#include "genps.h"
#include "genpdf.h"
#include "object.h"
#include "texfonts.h"

extern double rad2deg;

#ifdef hpux
#define rint(a) floor((a)+0.5)     /* close enough? */
#endif

#ifdef gould
#define rint(a) floor((a)+0.5)     /* close enough? */
#endif

extern void
	genlatex_start (),
	gendev_null (),
     	geneps_option (),
	genps_start (),
	genps_arc (),
	genps_ellipse (),
	genps_line (),
	genps_spline (),
        genlatex_option (),
        genlatex_text (),
        genps_text ();
extern int
	genlatex_end (),
	genps_end ();

static char pstex_file[1000] = "";
static int    iObjectsRead = 0;
static int    iTextRead = 0;
static int    iObjectsFileNumber = 0;
static int    iStartDepth;
static int    iLastDepth;
static FILE * ptCreateFile;
static char   szFileName[1000];
static int    iLength;
static int    iPdfOutputs = 0;

#ifdef never
static double		unitlength;
static double		dash_mag = 1.0;

static int		(*translate_coordinates)() = NULL;
static int		(*translate_coordinates_d)() = NULL;
#else
extern double		unitlength;
extern double		dash_mag;

extern int		(*translate_coordinates)();
extern int		(*translate_coordinates_d)();
#endif

#define TOP		840
#define	SWAP(x,y)	{tmp=x; x=y; y=tmp;}
#define TRANS(x,y)		(*translate_coordinates)(&x,&y)
#define TRANS2(x1,y1,x2,y2)	(*translate_coordinates)(&x1,&y1); \
				(*translate_coordinates)(&x2,&y2)
#define TRANSD(x,y)		(*translate_coordinates_d)(&x,&y)

static void genpstex_p_finalize_objects(int depth);

/*************************************************************************
 *************************************************************************/
static
translate2(xp, yp)
  int	*xp, *yp;
{
	*xp = *xp + 1;
	*yp = (double)(TOP - *yp -1);
	}

static
translate1_d(xp, yp)
  double	*xp, *yp;
{
	*xp = *xp + 1.0;
	*yp = *yp + 1.0;
	}

static
translate2_d(xp, yp)
  double	*xp, *yp;
{
	*xp = *xp + 1.0;
	*yp = (double)TOP - *yp -1.0;
	}

/*************************************************************************
 *************************************************************************/
void genpstex_p_option(opt, optarg)
char opt, *optarg;
{
       if (opt == 'p')
       {
	   strcpy(pstex_file, optarg);
       }
       else
	   genlatex_option(opt, optarg);
}


/*************************************************************************
 *************************************************************************/
void genpstex_p_start(objects)
  F_compound	*objects;
{
	int	      tmp;
	struct stat   tStat;
	long          lUnitLength;

        if (*pstex_file == '\0')
        {
		put_msg("Argument -p is mandatory to pstex_p.");
		exit(1);
	}

	strncpy(szFileName, pstex_file, 900);
	iLength = strlen(szFileName);

	sprintf(szFileName + iLength, ".create");
	ptCreateFile = fopen (szFileName, "wb");

	if ( ! ptCreateFile)
        {
		put_msg("Couldn't open %s for writing", szFileName);
		exit(1);
	}
	fstat(fileno(ptCreateFile), & tStat);
	fchmod (fileno(ptCreateFile), tStat.st_mode | ((tStat.st_mode & (S_IRUSR | S_IRGRP | S_IROTH)) >> 2));
	fprintf(ptCreateFile, "#!/bin/bash\n\n", szFileName);

	fprintf(ptCreateFile, "while getopts \"r\" Option\n");
	fprintf(ptCreateFile, "do\n");
	fprintf(ptCreateFile, "  case $Option in\n");
	fprintf(ptCreateFile, "    r) iOptRemove=1;;\n");
	fprintf(ptCreateFile, "    *) echo \"illegal option -$Option\"\n");
	fprintf(ptCreateFile, "  esac\n");
	fprintf(ptCreateFile, "done\n");
	fprintf(ptCreateFile, "shift $(($OPTIND - 1))\n");

	texfontsizes[0] = texfontsizes[1] =
		TEXFONTSIZE(font_size != 0.0? font_size : DEFAULT_FONT_SIZE);

 	unitlength = mag/ppi;
	dash_mag /= unitlength*80.0;

	translate_coordinates = translate2;
	translate_coordinates_d = translate2_d;

	TRANS2(llx, lly, urx, ury);
	if (llx > urx) SWAP(llx, urx)
	if (lly > ury) SWAP(lly, ury)

	/* LaTeX start */

	/* print any whole-figure comments prefixed with "%" */
	if (objects->comments) {
	    fprintf(tfp,"%%\n");
	    print_comments("% ",objects->comments, "");
	    fprintf(tfp,"%%\n");
	}

	lUnitLength = (long) (round(4736286.72*unitlength));
	fprintf(tfp, "\\setlength{\\unitlength}{%lisp}%%\n",
				lUnitLength);
	/* define the SetFigFont macro */
	define_setfigfont(tfp);

	sprintf(szFileName + iLength, ".size");
	fprintf(ptCreateFile, "if [ \"$iOptRemove\" == \"\" ]; then\n");
	if (iPdfOutputs)
		fprintf(ptCreateFile, "  echo \"\\setlength\\pdfpagewidth{%3.2fpt}\\setlength\\pdfpageheight{%3.2fpt}\" > %s\n",
        	        (float) lUnitLength / 65536 * (urx - llx), (float) lUnitLength / 65536 * (ury - lly), szFileName);
	else
		fprintf(ptCreateFile, "echo -n\"\" > %s\n", szFileName);
	fprintf(ptCreateFile, "else\n  rm -f %s\nfi\n", szFileName);
}

/*************************************************************************
 *************************************************************************/
void genpdftex_p_start(objects)
  F_compound	*objects;
{
	iPdfOutputs = 1;
	genpstex_p_start(objects);
}


/*************************************************************************
 *************************************************************************/
int genpstex_p_end()
{
	if (iTextRead)
	{
		fprintf(tfp, "\\end{picture}%%\n");
		iTextRead = 0;
	}
	if (iObjectsRead)
	{
		genpstex_p_finalize_objects(iLastDepth);
		iObjectsRead = 0;
	}

	/* LaTeX ending */
	fprintf(tfp, "\\begin{picture}(%d,%d)\n", urx-llx, ury-lly);
	fprintf(tfp, "\\end{picture}%%\n");

	fclose(ptCreateFile);

	/* all ok */
	return 0;
}


/*************************************************************************
 *************************************************************************/
void genpstex_p_finalize_objects(depth)
int depth;
{
	char	szFileName[1000];
	int     iLength;

	strncpy(szFileName, pstex_file, 900);
	iLength = strlen(szFileName);


	sprintf(szFileName + iLength, "%03d", iObjectsFileNumber++);
	fprintf(ptCreateFile, "if [ \"$iOptRemove\" == \"\" ]; then\n");
	if (iPdfOutputs)
	{
		fprintf(ptCreateFile, "  %s -L pstex -D +%d:%d %s", prog, depth, iStartDepth, from, szFileName);
		fprintf(ptCreateFile, " | epstopdf -f > %s.pdf\n", szFileName, szFileName);
	}
	else
		fprintf(ptCreateFile, "%s -L pstex -D +%d:%d %s %s.eps\n", prog, depth, iStartDepth, from, szFileName);
	fprintf(ptCreateFile, "else\n  rm -f %s.", szFileName);
	if (iPdfOutputs)
		fprintf(ptCreateFile, "pdf");
	else
		fprintf(ptCreateFile, "eps");
	fprintf(ptCreateFile, "\nfi\n");

	fprintf(tfp, "\\begin{picture}(0,0)%%\n");
/* newer includegraphics directive suggested by Stephen Harker 1/13/99 */
#if defined(LATEX2E_GRAPHICS)
#  if defined(EPSFIG)
	fprintf(tfp, "\\epsfig{file=%s.eps}%%\n",szFileName);
#  else
	fprintf(tfp, "\\includegraphics{%s}%%\n",szFileName);
#  endif
#else
	fprintf(tfp, "\\special{psfile=%s.eps}%%\n",szFileName);
#endif
	fprintf(tfp, "\\end{picture}%%\n");
}

/*************************************************************************
 *************************************************************************/
void genpstex_p_object(depth)
int depth;
{
	if (iTextRead)
	{
		fprintf(tfp, "\\end{picture}%%\n");
		iTextRead = 0;
	}
	if (iObjectsRead == 0)
	{
		iStartDepth = depth;
		iObjectsRead = 1;
	}
	iLastDepth = depth;

}

void genpstex_p_arc(obj)
F_arc	*obj;
{
	genpstex_p_object(obj->depth);
}


void genpstex_p_ellipse(obj)
F_ellipse	*obj;
{
	genpstex_p_object(obj->depth);
}

void genpstex_p_line(obj)
F_line	*obj;
{
	genpstex_p_object(obj->depth);
}

void genpstex_p_spline(obj)
F_spline	*obj;
{
	genpstex_p_object(obj->depth);
}



/*************************************************************************
 *************************************************************************/
void genpstex_p_text(t)
F_text	*t;
{

	if (!special_text(t))
		genpstex_p_object(t->depth);
	else
	{
		if (iObjectsRead)
		{
			genpstex_p_finalize_objects(iLastDepth);
			iObjectsRead = 0;
		}
		if (iTextRead == 0)
		{
			fprintf(tfp, "\\begin{picture}(0,0)(%d,%d)\n", llx, lly);
			iTextRead = 1;
		}
		genlatex_text(t);
	}

}

/*************************************************************************
 *************************************************************************/
void genpstex_t_option(opt, optarg)
char opt, *optarg;
{
       if (opt == 'p') 
	   strcpy(pstex_file, optarg);
       else
	   genlatex_option(opt, optarg);
}


void genpstex_t_start(objects)
F_compound	*objects;
{
	/* Put PostScript Image if any*/
        if (pstex_file[0] != '\0') {
		fprintf(tfp, "\\begin{picture}(0,0)%%\n");
/* newer includegraphics directive suggested by Stephen Harker 1/13/99 */
#if defined(LATEX2E_GRAPHICS)
#  if defined(EPSFIG)
		fprintf(tfp, "\\epsfig{file=%s}%%\n",pstex_file); 
#  else
		fprintf(tfp, "\\includegraphics{%s}%%\n",pstex_file);
#  endif
#else
		fprintf(tfp, "\\special{psfile=%s}%%\n",pstex_file);
#endif
		fprintf(tfp, "\\end{picture}%%\n");
	}
        genlatex_start(objects);

}

void genpstex_t_text(t)
F_text	*t;
{

	if (!special_text(t))
	  gendev_null();
	else genlatex_text(t);
}

/*************************************************************************
 *************************************************************************/
void genpstex_text(t)
F_text	*t;
{

	if (!special_text(t))
	  genps_text(t);
	else gendev_null();
}

void genpstex_option(opt, optarg)
char opt, *optarg;
{
       if (opt != 'p')
	   genlatex_option(opt, optarg);
}

struct driver dev_pstex_p = {
  	genpstex_p_option,
	genpstex_p_start,
	gendev_null,
	genpstex_p_arc,
	genpstex_p_ellipse,
	genpstex_p_line,
	genpstex_p_spline,
	genpstex_p_text,
	genpstex_p_end,
	INCLUDE_TEXT
};

struct driver dev_pdftex_p = {
  	genpstex_p_option,
	genpdftex_p_start,
	gendev_null,
	genpstex_p_arc,
	genpstex_p_ellipse,
	genpstex_p_line,
	genpstex_p_spline,
	genpstex_p_text,
	genpstex_p_end,
	INCLUDE_TEXT
};

struct driver dev_pstex_t = {
  	genpstex_t_option,
	genpstex_t_start,
	gendev_null,
	gendev_null,
	gendev_null,
	gendev_null,
	gendev_null,
	genpstex_t_text,
	genlatex_end,
	INCLUDE_TEXT
};

struct driver dev_pdftex_t = {
  	genpstex_t_option,
	genpstex_t_start,
	gendev_null,
	gendev_null,
	gendev_null,
	gendev_null,
	gendev_null,
	genpstex_t_text,
	genlatex_end,
	INCLUDE_TEXT
};

struct driver dev_pstex = {
  	geneps_option, 	/* use eps so always exported in Portrait mode */
	genps_start,
	genps_grid,
	genps_arc,
	genps_ellipse,
	genps_line,
	genps_spline,
	genpstex_text,
	genps_end,
	INCLUDE_TEXT
};

extern void     genpdf_option();
extern void     genpdf_start();
extern int      genpdf_end();

struct driver dev_pdftex = {
  	genpdf_option,
	genpdf_start,
	genps_grid,
	genps_arc,
	genps_ellipse,
	genps_line,
	genps_spline,
	genpstex_text,
	genpdf_end,
	INCLUDE_TEXT
};


