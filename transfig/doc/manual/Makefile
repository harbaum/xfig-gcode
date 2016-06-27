#
# TransFig makefile
#

all: trans.tex manual.ps

# translation into latex

trans.tex: trans.fig Makefile
	../../fig2dev/fig2dev -L latex trans.fig > trans.tex
clean::
	rm -f trans.tex *.aux *.log *.dvi *.ps

cleandir::

manual.ps: manual.dvi
	dvips -o $@ $^

manual.dvi: manual.tex trans.tex
	latex manual.tex
	latex manual.tex
