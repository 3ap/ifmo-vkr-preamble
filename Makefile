TARGET = example.pdf
SRC    = example.tex

PARTS  = examples/text.md.tex \
	 preamble.tex \
	 example.bib

all: $(TARGET)

$(TARGET): $(SRC) $(PARTS)
	pdflatex --shell-escape $< $@
	bibtex example
	pdflatex --shell-escape $< $@
	pdflatex --shell-escape $< $@

%.md.tex: %.md
	pandoc -f markdown -t latex $< > $@

clean:
	rm -rf *.blg *.bbl *.pdf *.aux *.log *.toc *.out parts/*.md.tex parts/*.aux svg-inkscape images/*.pdf_tex images/*.pdf
