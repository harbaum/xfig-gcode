all: sample-presentation.pdf

overlay-sample-0.pdf: overlay-sample.lfig
	env PATH="../../fig2dev:${PATH}" sh ../fig2mpdf -m overlay-sample.lfig

sample-presentation.pdf: sample-presentation.tex overlay-sample-0.pdf
	pdflatex sample-presentation

clean:
	rm -f overlay-sample-?.pdf 
	rm -f *.aux *.log *.nav *.out *.snm *.toc sample-presentation.pdf
