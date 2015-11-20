

# For dependencies
# FIGURES = $(wildcard figure*.pdf)
FIGURES = $()

# Force rebuild
.PHONY: .rebuild

main: report_LGBM.pdf

%.pdf: %.tex $(wildcard *.tex *.bib) $(FIGURES) .rebuild
	pdflatex $*
#	bibtex $*
#	pdflatex $*
#	pdflatex $*

clean:
	rm -rf *.aux
	rm -rf *.bbl
	rm -rf *.blg
	rm -rf *.log
	rm -rf *.pdf