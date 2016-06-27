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
 *	genps.h: PostScript headers/patterns etc.
 *
 */

extern Boolean	epsflag;	/* to distinguish PS and EPS */
extern Boolean	pdfflag;	/* to distinguish PDF and PS/EPS */
extern Boolean	tiffpreview;	/* add a TIFF preview? */

extern void	gen_ps_eps_option();
extern void	genps_start();
extern int	genps_end();
extern void	genps_grid();
extern void	genps_arc();
extern void	genps_ellipse();
extern void	genps_line();
extern void	genps_spline();
extern void	genps_text();


#define		BEGIN_PROLOG1	"\
/$F2psDict 200 dict def\n\
$F2psDict begin\n\
$F2psDict /mtrx matrix put\n\
/col-1 {0 setgray} bind def\n\
"

#define		BEGIN_PROLOG2	"\
/cp {closepath} bind def\n\
/ef {eofill} bind def\n\
/gr {grestore} bind def\n\
/gs {gsave} bind def\n\
/sa {save} bind def\n\
/rs {restore} bind def\n\
/l {lineto} bind def\n\
/m {moveto} bind def\n\
/rm {rmoveto} bind def\n\
/n {newpath} bind def\n\
/s {stroke} bind def\n\
/sh {show} bind def\n\
/slc {setlinecap} bind def\n\
/slj {setlinejoin} bind def\n\
/slw {setlinewidth} bind def\n\
/srgb {setrgbcolor} bind def\n\
/rot {rotate} bind def\n\
/sc {scale} bind def\n\
/sd {setdash} bind def\n\
/ff {findfont} bind def\n\
/sf {setfont} bind def\n\
/scf {scalefont} bind def\n\
/sw {stringwidth} bind def\n\
/tr {translate} bind def\n\
/tnt {dup dup currentrgbcolor\n\
  4 -2 roll dup 1 exch sub 3 -1 roll mul add\n\
  4 -2 roll dup 1 exch sub 3 -1 roll mul add\n\
  4 -2 roll dup 1 exch sub 3 -1 roll mul add srgb}\n\
  bind def\n\
/shd {dup dup currentrgbcolor 4 -2 roll mul 4 -2 roll mul\n\
  4 -2 roll mul srgb} bind def\n\
"


#define		FILL_PROLOG1	"\
% This junk string is used by the show operators\n\
/PATsstr 1 string def\n\
/PATawidthshow { 	% cx cy cchar rx ry string\n\
  % Loop over each character in the string\n\
  {  % cx cy cchar rx ry char\n\
    % Show the character\n\
    dup				% cx cy cchar rx ry char char\n\
    PATsstr dup 0 4 -1 roll put	% cx cy cchar rx ry char (char)\n\
    false charpath		% cx cy cchar rx ry char\n\
    /clip load PATdraw\n\
    % Move past the character (charpath modified the\n\
    % current point)\n\
    currentpoint			% cx cy cchar rx ry char x y\n\
    newpath\n\
    moveto			% cx cy cchar rx ry char\n\
    % Reposition by cx,cy if the character in the string is cchar\n\
    3 index eq {			% cx cy cchar rx ry\n\
      4 index 4 index rmoveto\n\
    } if\n\
    % Reposition all characters by rx ry\n\
    2 copy rmoveto		% cx cy cchar rx ry\n\
  } forall\n\
  pop pop pop pop pop		% -\n\
  currentpoint\n\
  newpath\n\
  moveto\n\
} bind def\n\
"
#define		FILL_PROLOG2	"\
/PATcg {\n\
  7 dict dup begin\n\
    /lw currentlinewidth def\n\
    /lc currentlinecap def\n\
    /lj currentlinejoin def\n\
    /ml currentmiterlimit def\n\
    /ds [ currentdash ] def\n\
    /cc [ currentrgbcolor ] def\n\
    /cm matrix currentmatrix def\n\
  end\n\
} bind def\n\
% PATdraw - calculates the boundaries of the object and\n\
% fills it with the current pattern\n\
/PATdraw {			% proc\n\
  save exch\n\
    PATpcalc			% proc nw nh px py\n\
    5 -1 roll exec		% nw nh px py\n\
    newpath\n\
    PATfill			% -\n\
  restore\n\
} bind def\n\
"
#define		FILL_PROLOG3	"\
% PATfill - performs the tiling for the shape\n\
/PATfill { % nw nh px py PATfill -\n\
  PATDict /CurrentPattern get dup begin\n\
    setfont\n\
    % Set the coordinate system to Pattern Space\n\
    PatternGState PATsg\n\
    % Set the color for uncolored pattezns\n\
    PaintType 2 eq { PATDict /PColor get PATsc } if\n\
    % Create the string for showing\n\
    3 index string		% nw nh px py str\n\
    % Loop for each of the pattern sources\n\
    0 1 Multi 1 sub {		% nw nh px py str source\n\
	% Move to the starting location\n\
	3 index 3 index		% nw nh px py str source px py\n\
	moveto			% nw nh px py str source\n\
	% For multiple sources, set the appropriate color\n\
	Multi 1 ne { dup PC exch get PATsc } if\n\
	% Set the appropriate string for the source\n\
	0 1 7 index 1 sub { 2 index exch 2 index put } for pop\n\
	% Loop over the number of vertical cells\n\
	3 index 		% nw nh px py str nh\n\
	{			% nw nh px py str\n\
	  currentpoint		% nw nh px py str cx cy\n\
	  2 index oldshow	% nw nh px py str cx cy\n\
	  YStep add moveto	% nw nh px py str\n\
	} repeat		% nw nh px py str\n\
    } for\n\
    5 { pop } repeat\n\
  end\n\
} bind def\n\
"
#define		FILL_PROLOG4	"\
% PATkshow - kshow with the current pattezn\n\
/PATkshow {			% proc string\n\
  exch bind			% string proc\n\
  1 index 0 get			% string proc char\n\
  % Loop over all but the last character in the string\n\
  0 1 4 index length 2 sub {\n\
				% string proc char idx\n\
    % Find the n+1th character in the string\n\
    3 index exch 1 add get	% string proc char char+1\n\
    exch 2 copy			% strinq proc char+1 char char+1 char\n\
    % Now show the nth character\n\
    PATsstr dup 0 4 -1 roll put	% string proc chr+1 chr chr+1 (chr)\n\
    false charpath		% string proc char+1 char char+1\n\
    /clip load PATdraw\n\
    % Move past the character (charpath modified the current point)\n\
    currentpoint newpath moveto\n\
    % Execute the user proc (should consume char and char+1)\n\
    mark 3 1 roll		% string proc char+1 mark char char+1\n\
    4 index exec		% string proc char+1 mark...\n\
    cleartomark			% string proc char+1\n\
  } for\n\
  % Now display the last character\n\
  PATsstr dup 0 4 -1 roll put	% string proc (char+1)\n\
  false charpath		% string proc\n\
  /clip load PATdraw\n\
  neewath\n\
  pop pop			% -\n\
} bind def\n\
"
#define		FILL_PROLOG5	"\
% PATmp - the makepattern equivalent\n\
/PATmp {			% patdict patmtx PATmp patinstance\n\
  exch dup length 7 add		% We will add 6 new entries plus 1 FID\n\
  dict copy			% Create a new dictionary\n\
  begin\n\
    % Matrix to install when painting the pattern\n\
    TilingType PATtcalc\n\
    /PatternGState PATcg def\n\
    PatternGState /cm 3 -1 roll put\n\
    % Check for multi pattern sources (Level 1 fast color patterns)\n\
    currentdict /Multi known not { /Multi 1 def } if\n\
    % Font dictionary definitions\n\
    /FontType 3 def\n\
    % Create a dummy encoding vector\n\
    /Encoding 256 array def\n\
    3 string 0 1 255 {\n\
      Encoding exch dup 3 index cvs cvn put } for pop\n\
    /FontMatrix matrix def\n\
    /FontBBox BBox def\n\
    /BuildChar {\n\
	mark 3 1 roll		% mark dict char\n\
	exch begin\n\
	Multi 1 ne {PaintData exch get}{pop} ifelse  % mark [paintdata]\n\
	  PaintType 2 eq Multi 1 ne or\n\
	  { XStep 0 FontBBox aload pop setcachedevice }\n\
	  { XStep 0 setcharwidth } ifelse\n\
	  currentdict		% mark [paintdata] dict\n\
	  /PaintProc load	% mark [paintdata] dict paintproc\n\
	end\n\
	gsave\n\
	  false PATredef exec true PATredef\n\
	grestore\n\
	cleartomark		% -\n\
    } bind def\n\
    currentdict\n\
  end				% newdict\n\
  /foo exch			% /foo newlict\n\
  definefont			% newfont\n\
} bind def\n\
"
#define		FILL_PROLOG6	"\
% PATpcalc - calculates the starting point and width/height\n\
% of the tile fill for the shape\n\
/PATpcalc {	% - PATpcalc nw nh px py\n\
  PATDict /CurrentPattern get begin\n\
    gsave\n\
	% Set up the coordinate system to Pattern Space\n\
	% and lock down pattern\n\
	PatternGState /cm get setmatrix\n\
	BBox aload pop pop pop translate\n\
	% Determine the bounding box of the shape\n\
	pathbbox			% llx lly urx ury\n\
    grestore\n\
    % Determine (nw, nh) the # of cells to paint width and height\n\
    PatHeight div ceiling		% llx lly urx qh\n\
    4 1 roll				% qh llx lly urx\n\
    PatWidth div ceiling		% qh llx lly qw\n\
    4 1 roll				% qw qh llx lly\n\
    PatHeight div floor			% qw qh llx ph\n\
    4 1 roll				% ph qw qh llx\n\
    PatWidth div floor			% ph qw qh pw\n\
    4 1 roll				% pw ph qw qh\n\
    2 index sub cvi abs			% pw ph qs qh-ph\n\
    exch 3 index sub cvi abs exch	% pw ph nw=qw-pw nh=qh-ph\n\
    % Determine the starting point of the pattern fill\n\
    %(px, py)\n\
    4 2 roll				% nw nh pw ph\n\
    PatHeight mul			% nw nh pw py\n\
    exch				% nw nh py pw\n\
    PatWidth mul exch			% nw nh px py\n\
  end\n\
} bind def\n\
"
#define		FILL_PROLOG7	"\
% Save the original routines so that we can use them later on\n\
/oldfill	/fill load def\n\
/oldeofill	/eofill load def\n\
/oldstroke	/stroke load def\n\
/oldshow	/show load def\n\
/oldashow	/ashow load def\n\
/oldwidthshow	/widthshow load def\n\
/oldawidthshow	/awidthshow load def\n\
/oldkshow	/kshow load def\n\
\n\
% These defs are necessary so that subsequent procs don't bind in\n\
% the originals\n\
/fill	   { oldfill } bind def\n\
/eofill	   { oldeofill } bind def\n\
/stroke	   { oldstroke } bind def\n\
/show	   { oldshow } bind def\n\
/ashow	   { oldashow } bind def\n\
/widthshow { oldwidthshow } bind def\n\
/awidthshow { oldawidthshow } bind def\n\
/kshow 	   { oldkshow } bind def\n\
"
#define		FILL_PROLOG8	"\
/PATredef {\n\
  MyAppDict begin\n\
    {\n\
    /fill { /clip load PATdraw newpath } bind def\n\
    /eofill { /eoclip load PATdraw newpath } bind def\n\
    /stroke { PATstroke } bind def\n\
    /show { 0 0 null 0 0 6 -1 roll PATawidthshow } bind def\n\
    /ashow { 0 0 null 6 3 roll PATawidthshow }\n\
    bind def\n\
    /widthshow { 0 0 3 -1 roll PATawidthshow }\n\
    bind def\n\
    /awidthshow { PATawidthshow } bind def\n\
    /kshow { PATkshow } bind def\n\
  } {\n\
    /fill   { oldfill } bind def\n\
    /eofill { oldeofill } bind def\n\
    /stroke { oldstroke } bind def\n\
    /show   { oldshow } bind def\n\
    /ashow  { oldashow } bind def\n\
    /widthshow { oldwidthshow } bind def\n\
    /awidthshow { oldawidthshow } bind def\n\
    /kshow  { oldkshow } bind def\n\
    } ifelse\n\
  end\n\
} bind def\n\
false PATredef\n\
"
#define		FILL_PROLOG9	"\
% Conditionally define setcmykcolor if not available\n\
/setcmykcolor where { pop } {\n\
  /setcmykcolor {\n\
    1 sub 4 1 roll\n\
    3 {\n\
	3 index add neg dup 0 lt { pop 0 } if 3 1 roll\n\
    } repeat\n\
    setrgbcolor - pop\n\
  } bind def\n\
} ifelse\n\
/PATsc {		% colorarray\n\
  aload length		% c1 ... cn length\n\
    dup 1 eq { pop setgray } { 3 eq { setrgbcolor } { setcmykcolor\n\
  } ifelse } ifelse\n\
} bind def\n\
/PATsg {		% dict\n\
  begin\n\
    lw setlinewidth\n\
    lc setlinecap\n\
    lj setlinejoin\n\
    ml setmiterlimit\n\
    ds aload pop setdash\n\
    cc aload pop setrgbcolor\n\
    cm setmatrix\n\
  end\n\
} bind def\n\
"
#define		FILL_PROLOG10	"\
/PATDict 3 dict def\n\
/PATsp {\n\
  true PATredef\n\
  PATDict begin\n\
    /CurrentPattern exch def\n\
    % If it's an uncolored pattern, save the color\n\
    CurrentPattern /PaintType get 2 eq {\n\
      /PColor exch def\n\
    } if\n\
    /CColor [ currentrgbcolor ] def\n\
  end\n\
} bind def\n\
% PATstroke - stroke with the current pattern\n\
/PATstroke {\n\
  countdictstack\n\
  save\n\
  mark\n\
  {\n\
    currentpoint strokepath moveto\n\
    PATpcalc				% proc nw nh px py\n\
    clip newpath PATfill\n\
    } stopped {\n\
	(*** PATstroke Warning: Path is too complex, stroking\n\
	  with gray) =\n\
    cleartomark\n\
    restore\n\
    countdictstack exch sub dup 0 gt\n\
	{ { end } repeat } { pop } ifelse\n\
    gsave 0.5 setgray oldstroke grestore\n\
  } { pop restore pop } ifelse\n\
  newpath\n\
} bind def\n\
"
#define		FILL_PROLOG11	"\
/PATtcalc {		% modmtx tilingtype PATtcalc tilematrix\n\
  % Note: tiling types 2 and 3 are not supported\n\
  gsave\n\
    exch concat					% tilingtype\n\
    matrix currentmatrix exch			% cmtx tilingtype\n\
    % Tiling type 1 and 3: constant spacing\n\
    2 ne {\n\
	% Distort the pattern so that it occupies\n\
	% an integral number of device pixels\n\
	dup 4 get exch dup 5 get exch		% tx ty cmtx\n\
	XStep 0 dtransform\n\
	round exch round exch			% tx ty cmtx dx.x dx.y\n\
	XStep div exch XStep div exch		% tx ty cmtx a b\n\
	0 YStep dtransform\n\
	round exch round exch			% tx ty cmtx a b dy.x dy.y\n\
	YStep div exch YStep div exch		% tx ty cmtx a b c d\n\
	7 -3 roll astore			% { a b c d tx ty }\n\
    } if\n\
  grestore\n\
} bind def\n\
/PATusp {\n\
  false PATredef\n\
  PATDict begin\n\
    CColor PATsc\n\
  end\n\
} bind def\n\
"
#define		FILL_PAT01	"\
% this is the pattern fill program from the Second edition Reference Manual\n\
% with changes to call the above pattern fill\n\
% left30\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 32 16 true [ 32 0 0 -16 0 16 ]\n\
	{<c000c000300030000c000c000300030000c000c000300030\n\
	000c000c00030003c000c000300030000c000c0003000300\n\
	00c000c000300030000c000c00030003>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P1 exch def\n\
"
#define		FILL_PAT02	"\
% right30\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 32 16 true [ 32 0 0 -16 0 16 ]\n\
	{<00030003000c000c0030003000c000c0030003000c000c00\n\
	30003000c000c00000030003000c000c0030003000c000c0\n\
	030003000c000c0030003000c000c000>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P2 exch def\n\
"
#define		FILL_PAT03	"\
% crosshatch30\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 32 16 true [ 32 0 0 -16 0 16 ]\n\
	{<033003300c0c0c0c30033003c000c000300330030c0c0c0c\n\
	0330033000c000c0033003300c0c0c0c30033003c000c000\n\
	300330030c0c0c0c0330033000c000c0>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P3 exch def\n\
"
#define		FILL_PAT04	"\
% left45\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 20 20 true [ 20 0 0 -20 0 20 ]\n\
	{<8020004010002008001004000802000401000200\n\
	8001004000802000401080200040100020080010\n\
	0400080200040100020080010040008020004010>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P4 exch def\n\
"
#define		FILL_PAT05	"\
% right45\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 20 20 true [ 20 0 0 -20 0 20 ]\n\
	{<0040100080200100400200800401000802001004\n\
	0020080040100080200000401000802001004002\n\
	0080040100080200100400200800401000802000>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P5 exch def\n\
"
#define		FILL_PAT06	"\
% crosshatch45\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 20 20 true [ 20 0 0 -20 0 20 ]\n\
	{<8020004050102088201104400a02800401000a02\n\
	8011044020882040501080200040501020882011\n\
	04400a02800401000a0280110440208820405010>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P6 exch def\n\
"
#define		FILL_PAT07	"\
% bricks\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 16 16 true [ 16 0 0 -16 0 16 ]\n\
	{<008000800080008000800080\n\
	 0080ffff8000800080008000\n\
	 800080008000ffff>}\n\
        imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P7 exch def\n\
"
#define		FILL_PAT08	"\
% vertical bricks\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 16 16 true [ 16 0 0 -16 0 16 ]\n\
	{<ff8080808080808080808080\n\
	  8080808080ff808080808080\n\
	  8080808080808080> }\n\
        imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P8 exch def\n\
"
#define		FILL_PAT09	"\
% horizontal lines\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 16 8 true [ 16 0 0 -8 0 8 ]\n\
	{< ffff000000000000ffff000000000000>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P9 exch def\n\
"
#define		FILL_PAT10	"\
% vertical lines\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 8 16 true [ 8 0 0 -16 0 16 ]\n\
	{<11111111111111111111111111111111>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P10 exch def\n\
"
#define		FILL_PAT11	"\
% crosshatch lines\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 16 16 true [ 16 0 0 -16 0 16 ]\n\
	{<ffff111111111111ffff111111111111ffff111111111111\n\
	ffff111111111111>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P11 exch def\n\
"
#define		FILL_PAT12	"\
% left-shingles\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 48 48 true [ 48 0 0 -48 0 48 ]\n\
	{<000000000001000000000001000000000002000000000002\n\
	000000000004000000000004000000000008000000000008\n\
	000000000010000000000010000000000020000000000020\n\
	000000000040000000000040000000000080ffffffffffff\n\
	000000010000000000010000000000020000000000020000\n\
	000000040000000000040000000000080000000000080000\n\
	000000100000000000100000000000200000000000200000\n\
	000000400000000000400000000000800000ffffffffffff\n\
	000100000000000100000000000200000000000200000000\n\
	000400000000000400000000000800000000000800000000\n\
	001000000000001000000000002000000000002000000000\n\
	004000000000004000000000008000000000ffffffffffff>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P12 exch def\n\
"
#define		FILL_PAT13	"\
% right-shingles\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 48 48 true [ 48 0 0 -48 0 48 ]\n\
	{<000000000080000000000080000000000040000000000040\n\
	000000000020000000000020000000000010000000000010\n\
	000000000008000000000008000000000004000000000004\n\
	000000000002000000000002000000000001ffffffffffff\n\
	008000000000008000000000004000000000004000000000\n\
	002000000000002000000000001000000000001000000000\n\
	000800000000000800000000000400000000000400000000\n\
	000200000000000200000000000100000000ffffffffffff\n\
	000000800000000000800000000000400000000000400000\n\
	000000200000000000200000000000100000000000100000\n\
	000000080000000000080000000000040000000000040000\n\
	000000020000000000020000000000010000ffffffffffff>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P13 exch def\n\
"
#define		FILL_PAT14	"\
% vertical left-shingles\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 48 48 true [ 48 0 0 -48 0 48 ]\n\
	{<000100010001000100010001000100010001000100010001\n\
	000100010001000100010001000100010001000100010001\n\
	000180010001000160010001000118010001000106010001\n\
	000101810001000100610001000100190001000100070001\n\
	000100010001000100010001000100010001000100010001\n\
	000100010001000100010001000100010001000100010001\n\
	000100018001000100016001000100011801000100010601\n\
	000100010181000100010061000100010019000100010007\n\
	000100010001000100010001000100010001000100010001\n\
	000100010001000100010001000100010001000100010001\n\
	800100010001600100010001180100010001060100010001\n\
	018100010001006100010001001900010001000700010001>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P14 exch def\n\
"
#define		FILL_PAT15	"\
% vertical right-shingles\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 48 48 true [ 48 0 0 -48 0 48 ]\n\
	{<000100010001000100010001000100010001000100010001\n\
	000100010001000100010001000100010001000100010001\n\
	000100010007000100010019000100010061000100010181\n\
	000100010601000100011801000100016001000100018001\n\
	000100010001000100010001000100010001000100010001\n\
	000100010001000100010001000100010001000100010001\n\
	000100070001000100190001000100610001000101810001\n\
	000106010001000118010001000160010001000180010001\n\
	000100010001000100010001000100010001000100010001\n\
	000100010001000100010001000100010001000100010001\n\
	000700010001001900010001006100010001018100010001\n\
	060100010001180100010001600100010001800100010001>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P15 exch def\n\
"
#define		FILL_PAT16	"\
% fishscales\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  {  32 16 true [ 32 0 0 -16 0 16 ]\n\
	{<0007e000000c30000018180000700e0001c003800f0000f0\n\
	7800001ec0000003600000063000000c180000180e000070\n\
	038001c000f00f00001e78000003c000>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P16 exch def\n\
"
#define		FILL_PAT17	"\
% small fishscales\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 16 16 true [ 16 0 0 -16 0 16 ]\n\
	{<008000800080014001400220\n\
	0c187007c001800080004001\n\
	40012002180c0770>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P17 exch def\n\
"
#define		FILL_PAT18	"\
% circles\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 48 48 true [ 48 0 0 -48 0 48 ]\n\
	{<000007f000000000780f000000038000e000000c00001800\n\
	001000000400006000000300008000000080010000000040\n\
	020000000020040000000010040000000010080000000008\n\
	100000000004100000000004200000000002200000000002\n\
	200000000002400000000001400000000001400000000001\n\
	400000000001800000000000800000000000800000000000\n\
	800000000000800000000000800000000000800000000000\n\
	400000000001400000000001400000000001400000000001\n\
	200000000002200000000002200000000002100000000004\n\
	100000000004080000000008040000000010040000000010\n\
	020000000020010000000040008000000080006000000300\n\
	001000000400000c0000180000038000e0000000780f0000>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P18 exch def\n\
"
#define		FILL_PAT19	"\
% hexagons\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 60 36 true [ 60 0 0 -36 0 36 ]\n\
	{<008000040000000001000002000000000100000200000000\n\
	020000010000000002000001000000000400000080000000\n\
	040000008000000008000000400000000800000040000000\n\
	100000002000000010000000200000002000000010000000\n\
	200000001000000040000000080000004000000008000000\n\
	800000000400000080000000040000000000000003fffff0\n\
	800000000400000080000000040000004000000008000000\n\
	400000000800000020000000100000002000000010000000\n\
	100000002000000010000000200000000800000040000000\n\
	080000004000000004000000800000000400000080000000\n\
	020000010000000002000001000000000100000200000000\n\
	0100000200000000008000040000000000fffffc00000000>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P19 exch def\n\
"
#define		FILL_PAT20	"\
% octagons\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 32 32 true [ 32 0 0 -32 0 32 ]\n\
	{<003fff000040008000800040010000200200001004000008\n\
	080000041000000220000001400000008000000080000000\n\
	800000008000000080000000800000008000000080000000\n\
	800000008000000080000000800000008000000080000000\n\
	400000012000000210000004080000080400001002000020\n\
	0100004000800080>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P20 exch def\n\
"
#define		FILL_PAT21	"\
% horizontal sawtooth lines\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 32 16 true [ 32 0 0 -16 0 16 ]\n\
	{<000000000000000000000000000000000000000000000000\n\
	000000000100010002800280044004400820082010101010\n\
	20082008400440048002800200010001>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P21 exch def\n\
"
#define		FILL_PAT22	"\
% vertical sawtooth lines\n\
11 dict begin\n\
/PaintType 1 def\n\
/PatternType 1 def\n\
/TilingType 1 def\n\
/BBox [0 0 1 1] def\n\
/XStep 1 def\n\
/YStep 1 def\n\
/PatWidth 1 def\n\
/PatHeight 1 def\n\
/Multi 2 def\n\
/PaintData [\n\
  { clippath } bind\n\
  { 16 32 true [ 16 0 0 -32 0 32 ]\n\
	{<400020001000080004000200010000800100020004000800\n\
	100020004000800040002000100008000400020001000080\n\
	01000200040008001000200040008000>}\n\
     imagemask } bind\n\
] def\n\
/PaintProc {\n\
	pop\n\
	exec fill\n\
} def\n\
currentdict\n\
end\n\
/P22 exch def\n\
"

#define		SPECIAL_CHAR_1	"\
/reencdict 12 dict def /ReEncode { reencdict begin\n\
/newcodesandnames exch def /newfontname exch def /basefontname exch def\n\
/basefontdict basefontname findfont def /newfont basefontdict maxlength dict def\n\
basefontdict { exch dup /FID ne { dup /Encoding eq\n\
{ exch dup length array copy newfont 3 1 roll put }\n\
{ exch newfont 3 1 roll put } ifelse } { pop pop } ifelse } forall\n\
newfont /FontName newfontname put newcodesandnames aload pop\n\
128 1 255 { newfont /Encoding get exch /.notdef put } for\n\
newcodesandnames length 2 idiv { newfont /Encoding get 3 1 roll put } repeat\n\
newfontname newfont definefont pop end } def\n\
/isovec [\n\
"
#define		SPECIAL_CHAR_2	"\
8#055 /minus 8#200 /grave 8#201 /acute 8#202 /circumflex 8#203 /tilde\n\
8#204 /macron 8#205 /breve 8#206 /dotaccent 8#207 /dieresis\n\
8#210 /ring 8#211 /cedilla 8#212 /hungarumlaut 8#213 /ogonek 8#214 /caron\n\
8#220 /dotlessi 8#230 /oe 8#231 /OE\n\
8#240 /space 8#241 /exclamdown 8#242 /cent 8#243 /sterling\n\
8#244 /currency 8#245 /yen 8#246 /brokenbar 8#247 /section 8#250 /dieresis\n\
8#251 /copyright 8#252 /ordfeminine 8#253 /guillemotleft 8#254 /logicalnot\n\
8#255 /hyphen 8#256 /registered 8#257 /macron 8#260 /degree 8#261 /plusminus\n\
8#262 /twosuperior 8#263 /threesuperior 8#264 /acute 8#265 /mu 8#266 /paragraph\n\
8#267 /periodcentered 8#270 /cedilla 8#271 /onesuperior 8#272 /ordmasculine\n\
8#273 /guillemotright 8#274 /onequarter 8#275 /onehalf\n\
8#276 /threequarters 8#277 /questiondown 8#300 /Agrave 8#301 /Aacute\n\
8#302 /Acircumflex 8#303 /Atilde 8#304 /Adieresis 8#305 /Aring\n\
"
#define		SPECIAL_CHAR_3	"\
8#306 /AE 8#307 /Ccedilla 8#310 /Egrave 8#311 /Eacute\n\
8#312 /Ecircumflex 8#313 /Edieresis 8#314 /Igrave 8#315 /Iacute\n\
8#316 /Icircumflex 8#317 /Idieresis 8#320 /Eth 8#321 /Ntilde 8#322 /Ograve\n\
8#323 /Oacute 8#324 /Ocircumflex 8#325 /Otilde 8#326 /Odieresis 8#327 /multiply\n\
8#330 /Oslash 8#331 /Ugrave 8#332 /Uacute 8#333 /Ucircumflex\n\
8#334 /Udieresis 8#335 /Yacute 8#336 /Thorn 8#337 /germandbls 8#340 /agrave\n\
8#341 /aacute 8#342 /acircumflex 8#343 /atilde 8#344 /adieresis 8#345 /aring\n\
8#346 /ae 8#347 /ccedilla 8#350 /egrave 8#351 /eacute\n\
8#352 /ecircumflex 8#353 /edieresis 8#354 /igrave 8#355 /iacute\n\
8#356 /icircumflex 8#357 /idieresis 8#360 /eth 8#361 /ntilde 8#362 /ograve\n\
8#363 /oacute 8#364 /ocircumflex 8#365 /otilde 8#366 /odieresis 8#367 /divide\n\
8#370 /oslash 8#371 /ugrave 8#372 /uacute 8#373 /ucircumflex\n\
8#374 /udieresis 8#375 /yacute 8#376 /thorn 8#377 /ydieresis\
] def\n\
"

#define		ELLIPSE_PS	" \
/DrawEllipse {\n\
	/endangle exch def\n\
	/startangle exch def\n\
	/yrad exch def\n\
	/xrad exch def\n\
	/y exch def\n\
	/x exch def\n\
	/savematrix mtrx currentmatrix def\n\
	x y tr xrad yrad sc 0 0 1 startangle endangle arc\n\
	closepath\n\
	savematrix setmatrix\n\
	} def\n\
"
/* The original PostScript definition for adding a spline section to the
 * current path uses recursive bisection.  The following definition using the
 * curveto operator is more efficient since it executes at compiled rather
 * than interpreted code speed.  The Bezier control points are 2/3 of the way
 * from z1 (and z3) to z2.
 *
 * ---Rene Llames, 21 July 1988.
 */
#define		SPLINE_PS	" \
/DrawSplineSection {\n\
	/y3 exch def\n\
	/x3 exch def\n\
	/y2 exch def\n\
	/x2 exch def\n\
	/y1 exch def\n\
	/x1 exch def\n\
	/xa x1 x2 x1 sub 0.666667 mul add def\n\
	/ya y1 y2 y1 sub 0.666667 mul add def\n\
	/xb x3 x2 x3 sub 0.666667 mul add def\n\
	/yb y3 y2 y3 sub 0.666667 mul add def\n\
	x1 y1 lineto\n\
	xa ya xb yb x3 y3 curveto\n\
	} def\n\
"
#define		END_PROLOG	"\
/$F2psBegin {$F2psDict begin /$F2psEnteredState save def} def\n\
/$F2psEnd {$F2psEnteredState restore end} def\n\
"
