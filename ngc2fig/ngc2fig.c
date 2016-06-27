/*
  ngc2fig.c
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int eagle_mode = 0;

#define PPI  1200.0   /* standard xfig value */

#define COLOR_BLACK   0
#define COLOR_BLUE    1
#define COLOR_GREEN   2
#define COLOR_CYAN    3
#define COLOR_RED     4
#define COLOR_MAGENTA 5
#define COLOR_YELLOW  6
#define COLOR_WHITE   7

#define DRILL_COL   COLOR_GREEN

typedef struct pos {
  float x,y;
  int len;
  struct pos *next;
} pos_t;

typedef struct {
  pos_t first;    /* derived from last g0 command */
  pos_t *pos;     /* derived from all g1 commands */
  float z;
  int tool;
  int len;
} chain_t;

/* default is inch scale */
float system_scale = PPI/1000.0;

#define MAX_TOOL 99
float eagle_tool[MAX_TOOL+1];

/* the global chain */
static chain_t chain;

void init_chain(void) {
  memset(&chain, 0, sizeof(chain_t));
}

void clear_chain(void) {
  pos_t **tmp, *tmp2;

  if(chain.pos) {
    /* free old chain */
    tmp = &chain.pos; 
    while(*tmp) {
      tmp2 = *tmp;
      tmp = &(*tmp)->next;
      free(tmp2);
    }
  }

  /* clear chain */
  chain.pos = NULL;
  chain.len = 0;
}

void output_chain(void) {
  pos_t *pos;

  if(chain.pos && chain.len) {
    int thickness = chain.tool;
    int color = COLOR_BLACK;

    printf("# tool %d\n", chain.tool);

    /* in standard mode the tools are directly mapped to line widths */
    /* in eagle mode the table hidden in the comments of the converted */
    /* file is being used */
    if(eagle_mode) 
      thickness = 1;

    printf("2 1 0 %d %d 7 %d -1 -1 0.000 1 1 -1 0 0 %d\n\t", thickness, color,
	   50-((int)((chain.z*100.0*system_scale/PPI)+0.5)), chain.len+1);

     /* output first point */
    printf(" %d %d", 
	   (int) (0.5 + chain.first.x * system_scale), 
	   (int) (0.5 + chain.first.y * system_scale));
    
    /* output chain */
    pos = chain.pos; 
    while(pos) {
      printf(" %d %d", 
	     (int) (0.5 + pos->x * system_scale), 
	     (int) (0.5 + pos->y * system_scale));
      
      pos = pos->next;
    }
    printf("\n");
  }

  clear_chain();
}

int parse_cmd(int line, char *cmd) {

  /* parameters/coordiates are collected until a new command is found */
  static float x_pos, y_pos, z_pos, radius;
  static int tool, command_val = 0;

  int intval, color;

  switch(cmd[0]) {

    /* if we get a new "G" command with process the last one */
    /* since all parameters must be present now */
  case 'G':

    /* process the last command incl. parameters (e.g. G01 X1 Y1) */
    switch(command_val) {
      pos_t **tmp;

    case 0:  /* rapid */
      /* output chain if there's one */
      output_chain();

      /* and set start point */
      chain.first.x = x_pos;
      chain.first.y = y_pos;
      chain.tool = tool;
      break;

    case 1:
      if(!chain.pos)
	chain.z = z_pos;

      /* g1 position is attached to chain */

      /* find end of chain */
      tmp = &chain.pos;
      while(*tmp)
	tmp = &(*tmp)->next;

      /* save the positions after a g0 and g1 command */
      *tmp = malloc(sizeof(pos_t));
      memset(*tmp, 0, sizeof(pos_t));
      chain.len++;
      
      (*tmp)->x = x_pos;
      (*tmp)->y = y_pos;
	
      break;

    case 82:
      color = COLOR_BLACK;  /* in non-eagle mode all holes are black */

      /* draw a solid filled circle defined by a radius */
      if(eagle_mode) {
	color = DRILL_COL;

	printf("# eagle tool %d = %f\n", tool, eagle_tool[tool]);
	radius = eagle_tool[tool]/2;

	if(!radius) {
	  radius = 1;
	  color = COLOR_RED;
	}
      }

      printf("1 3 0 0 0 %d %d -1 20 0.000 1 0.0000 "
	     "%d %d %d %d %d %d %d %d\n", color,
	     50-((int)((z_pos*100.0*system_scale/PPI)+0.5)),
	     (int) (0.5 + x_pos *  system_scale), 
	     (int) (0.5 + y_pos * system_scale),
	     (int) (0.5 + radius * -system_scale),
	     (int) (0.5 + radius * -system_scale),
	     (int) (0.5 + x_pos *  system_scale), 
	     (int) (0.5 + y_pos * system_scale),
	     (int) (0.5 + (x_pos + radius) *  system_scale), 
	     (int) (0.5 + y_pos * system_scale)
	     );
      break;

    }
    
    /* remember this last command */
    command_val = -1;
    
    intval = atoi(cmd+1);
    
    switch(intval) {
    case 0:
    case 1:
    case 82:
      /* these commands will be processed when all parameters */ 
      /* have been read */
      command_val = intval;
      break;

    case 20: // inch system
      system_scale = PPI;
      break;
      
    case 21: // mm system
      system_scale = PPI/25.4;
      break;

    }
    break;

  case 'R':
    radius = atof(cmd+1);
    break;

  case 'Z':
    z_pos = atof(cmd+1);
    break;

  case 'X':
    x_pos = atof(cmd+1);
    break;
    
  case 'Y':
    y_pos = atof(cmd+1);
    break;

  case 'T':
    tool = atoi(cmd+1);
    break;
  }

  return 1;
}

int iswhite(char c) {
  return((c==' ')||(c=='\t')||(c=='\n')||(c=='\r'));
}

int isfloat(char c) {
  return(((c>='0')&&(c<='9'))||(c=='.')||(c=='-')||(c=='+'));
}

int isint(char c) {
  return((c>='0')&&(c<='9'));
}

int parse_line(int line, char *data) {
  char cmd[32];
  
  /* parse entire line */
  while(*data) {
    char *p = cmd;

    /* skip white spaces and end parsing if there's nothing else left */
    while(*data && iswhite(*data)) data++;
    if(!*data) return 1;

    /* skip comments */
    if(*data == '(') {
      /* these may contain tool mappings of eagle files */

      if(eagle_mode) {
	data++;
	/* skip all white spaces */
	while(*data && iswhite(*data)) data++;
	if(*data=='T') {
	  int idx;

	  data++;           /* skip 'T' */
	  if(isint(*data)) {

	    idx = atoi(data); /* tool index */
	    while(*data && isint(*data)) data++;

	    while(*data && iswhite(*data)) data++;
	    
	    if(*data && (idx >= 0) && (idx <= MAX_TOOL)) 
	      eagle_tool[idx] = atof(data);
	  }
	}
      }

      while(*data && *data != ')') data++;
      
      if(!*data) {
	fprintf(stderr, "Line %d: Missing end of comment\n", line);
	return 0;
      } 

      /* skip closing brace */
      data++;
    } else {

      /* first char is cmd code */
      *p++ = toupper(*data++);
      
      /* skip what spaces if present */
      while(*data && iswhite(*data)) data++;
      if(!*data) {
	fprintf(stderr, "Line %d: No further data after command code\n", line);
	return 0;
      }
      
      /* copy numerical parameter */
      while(*data && isfloat(*data)) 
	*p++ = *data++;
      
      *p++ = 0;
      if(!parse_cmd(line, cmd)) {
	fprintf(stderr, "Line %d: Parse error\n", line);
	return 0;
      }
    }
  }
  return 1;
}

int parse_gcode(FILE *file) {
  char buffer[256];
  int line = 1;

  init_chain();

  while(fgets(buffer, sizeof(buffer), file) != NULL) {
    if(!parse_line(line, buffer))
      return 0;

    line++;
  }

  /* empty buffer if there's one ... */
  output_chain();
  return 1;
}

void usage(void) {
  fprintf(stderr, "Usage: ngc2fig [options] file.ngc [file2.ngc] ...\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "-e eagle pcb_gcode support\n");
}

int main(int argc, char **argv) {
  int c,i;
  
  opterr = 0;
  
  while ((c = getopt (argc, argv, "e")) != -1) {
    switch (c) {
    case 'e':
      eagle_mode = 1;
      for(i=0;i<MAX_TOOL+1;i++)
	eagle_tool[i] = 0.0;
      break;

    case '?':
      if (isprint (optopt))
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf (stderr,
		 "Unknown option character `\\x%x'.\n",
		 optopt);
      usage();
      return 1;
    default:
      abort ();
    }
  }

  if(argc-optind < 1) {
    usage();
    return 1;
  }

  /* write fig file header */
  printf("#FIG 3.2  Produced by ngc2fig v0.2%s\n", 
	 eagle_mode?" (eagle pcb-gcode mode)":"");
  printf("Landscape\n");
  printf("Center\n");
  printf("Inches\n");
  printf("Letter\n");
  printf("100.00\n");
  printf("Single\n");
  printf("-2\n");
  printf("%d 2\n", (int)PPI);
  
  while(argc - optind) {

    FILE *file = fopen(argv[optind], "r");
    if(!file) {
      fprintf(stderr, "Unable to open file %s for reading\n", argv[optind]);
      return 1;
    }

    printf("# file %s\n", argv[optind]);
    parse_gcode(file);

    fclose(file);

    optind++;
  }
  
  return 0;
}

