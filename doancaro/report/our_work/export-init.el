;; Emacs batch init for org -> PDF export with Vietnamese + CJK fallback
(require 'org)
(require 'ox-latex)

;; Use xelatex (Unicode + fontspec support); -shell-escape needed for minted
(setq org-latex-compiler "xelatex")
(setq org-latex-pdf-process
      '("xelatex -shell-escape -interaction=nonstopmode -output-directory=%o %f"
        "xelatex -shell-escape -interaction=nonstopmode -output-directory=%o %f"
        "xelatex -shell-escape -interaction=nonstopmode -output-directory=%o %f"))

;; Vietnamese-capable LaTeX class with CJK fallback for 五目並べ
(with-eval-after-load 'ox-latex
  (add-to-list 'org-latex-classes
               '("report-vi"
                 "\\documentclass[11pt,a4paper]{article}
\\usepackage[a4paper,margin=2.5cm]{geometry}
\\usepackage{fontspec}
\\usepackage{polyglossia}
\\setmainlanguage{vietnamese}
\\setmainfont{DejaVu Serif}
\\setsansfont{DejaVu Sans}
\\setmonofont{JetBrains Mono}[Scale=0.80]
\\newfontfamily\\cjkfont{Noto Sans CJK JP}
\\usepackage{newunicodechar}
\\newunicodechar{五}{{\\cjkfont 五}}
\\newunicodechar{目}{{\\cjkfont 目}}
\\newunicodechar{並}{{\\cjkfont 並}}
\\newunicodechar{べ}{{\\cjkfont べ}}
\\usepackage{graphicx}
\\usepackage{float}
\\usepackage{longtable}
\\usepackage{tabularx}
\\usepackage{wrapfig}
\\usepackage{rotating}
\\usepackage[normalem]{ulem}
\\usepackage{amsmath}
\\usepackage{amssymb}
\\usepackage{capt-of}
\\usepackage{hyperref}
\\hypersetup{colorlinks=true,linkcolor=blue,urlcolor=blue,bookmarks=true,bookmarksopen=true,bookmarksopenlevel=1,pdfpagelabels=true}
\\usepackage{bookmark}
\\usepackage{titlesec}
% Suppress visible section numbers but keep TOC/bookmark entries
\\titleformat{\\section}{\\Large\\bfseries}{}{0pt}{}
\\titleformat{\\subsection}{\\large\\bfseries}{}{0pt}{}
\\titleformat{\\subsubsection}{\\normalsize\\bfseries}{}{0pt}{}
\\usepackage{xcolor}
\\usepackage{fvextra}
\\DefineVerbatimEnvironment{verbatim}{Verbatim}{breaklines=true,breakanywhere=true,fontsize=\\footnotesize}
\\usepackage[cache=false]{minted}
\\usemintedstyle{friendly}
\\setminted{fontsize=\\footnotesize,breaklines=true,breakanywhere=true,frame=single,framesep=2mm,bgcolor=white,linenos=false,tabsize=2}
\\setlength{\\parskip}{0.5em}
\\setlength{\\parindent}{0pt}
\\sloppy"
                 ("\\section{%s}" . "\\section*{%s}")
                 ("\\subsection{%s}" . "\\subsection*{%s}")
                 ("\\subsubsection{%s}" . "\\subsubsection*{%s}")
                 ("\\paragraph{%s}" . "\\paragraph*{%s}")
                 ("\\subparagraph{%s}" . "\\subparagraph*{%s}"))))

;; Suppress visible section numbers but keep in TOC/bookmarks.
;; setcounter secnumdepth to -2 only suppresses part-level; we use titlesec above.

(setq org-latex-default-class "report-vi")

;; minted backend gives Pygments syntax highlighting (requires -shell-escape)
(setq org-latex-src-block-backend 'minted)
(setq org-latex-minted-options
      '(("breaklines" "true")
        ("breakanywhere" "true")
        ("fontsize" "\\footnotesize")
        ("frame" "single")
        ("framesep" "2mm")
        ("tabsize" "2")))

(setq org-export-with-smart-quotes nil)
(setq org-export-with-sub-superscripts '{})
(setq org-latex-tables-centered t)

(provide 'export-init)
