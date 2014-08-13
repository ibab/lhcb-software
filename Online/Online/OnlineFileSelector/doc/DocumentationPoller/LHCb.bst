% LHCb Bibtex file
%
% This file should be used for all LHCb publications. To report bugs,
% suggest features or provide enhancements, contact Ulrik Egede.
%
%X       History
%X       9/30/85	(HWT)	Original version, by Howard Trickey.
%X       1/29/88	(OP&HWT) Updated for BibTeX version 0.99a, Oren Patashni%           7/97  Jacques Distler Collaboration and hyperref
%           8/11  Ulrik Egede for the LHCb collaboration. Tweaked style and
%                     added DOI hyperlinks for articles.
%          01/12  Ulrik Egede Integrated mciteplus
%
% The bibtex output produced by SPIRES, while far from perfect, is pretty
% suitable for use with this style. Indeed, this style was designed with
% SPIRES in mind.

ENTRY
  { address
    author
    booktitle
    chapter
    edition
    editor
    howpublished
    institution
    journal
    key
    month
    note
    DOI
    number
    organization
    pages
    publisher
    school
    series
    title
    type
    volume
    year
    archive
    eprint
    report
    collaboration
  }
  {}
  { label }

INTEGERS { output.state before.all mid.sentence after.quote after.sentence
		after.quoted.block after.block article.inside}

FUNCTION {init.state.consts}
{ #0 'before.all :=
  #1 'mid.sentence :=
  #2 'after.quote :=
  #3 'after.sentence :=
  #4 'after.quoted.block :=
  #5 'after.block :=
  #6 'article.inside :=
}

STRINGS { s t }

FUNCTION {output.nonnull}
{ 's :=
  output.state mid.sentence =
    { ", " * write$ }
    { output.state article.inside =
	{ write$ }
    { output.state after.quote =
	{ " " * write$ }
	{ output.state after.block =
	    { add.period$ write$
	      newline$
	      "\newblock " write$
	    }
	    { output.state before.all =
		'write$
		{ output.state after.quoted.block =
		    { write$
		      newline$
		      "\newblock " write$
		    }
		    { add.period$ " " * write$ }
		  if$
		}
	      if$
	    }
	  if$
	}
      if$
      mid.sentence 'output.state :=
    }
  if$
    }
  if$
  s
}

FUNCTION {output}
{ duplicate$ empty$
    'pop$
    'output.nonnull
  if$
}

FUNCTION {output.check}
{ 't :=
  duplicate$ empty$
    { pop$ "empty " t * " in " * cite$ * warning$ }
    'output.nonnull
  if$
}

FUNCTION {output.bibitem}
{ newline$
  "\bibitem{" write$
  cite$ write$
  "}" write$
  newline$
  ""
  before.all 'output.state :=
}

FUNCTION {blank.sep}
{ after.quote 'output.state :=
}

FUNCTION {article.sep}
{ article.inside 'output.state :=
}

% mciteplus fin.entry
%
% pushes true (1), if add.period$ would add a period to the string on the stack
% pushes false (0), otherwise
% Uses text.length$ to avoid full string comparison and two copies of string.
% Requires one copy of string on stack.
INTEGERS {would.add.period.textlen}
FUNCTION {would.add.period}
{ duplicate$
  add.period$
  text.length$
  'would.add.period.textlen :=
  duplicate$
  text.length$
  would.add.period.textlen =
    { #0 }
    { #1 }
  if$
}

FUNCTION {fin.entry}
{ would.add.period
    { "\relax" * write$ newline$
      "\mciteBstWouldAddEndPuncttrue" write$ newline$
      "\mciteSetBstMidEndSepPunct{\mcitedefaultmidpunct}"
      write$ newline$
      "{\mcitedefaultendpunct}{\mcitedefaultseppunct}\relax"
    }
    { "\relax" * write$ newline$
      "\mciteBstWouldAddEndPunctfalse" write$ newline$
      "\mciteSetBstMidEndSepPunct{\mcitedefaultmidpunct}"
      write$ newline$
      "{}{\mcitedefaultseppunct}\relax"
    }
  if$
  write$
  newline$
  "\EndOfBibitem" write$
}
% end mciteplus fin.entry


%FUNCTION {fin.entry}
%{ output.state after.quoted.block =
%    'skip$
%    'add.period$
%  if$
%  write$
%  newline$
%}

FUNCTION {new.block}
{ output.state before.all =
    'skip$
    { output.state after.quote =
	{ after.quoted.block 'output.state := }
	{ after.block 'output.state := }
      if$
    }
  if$
}

FUNCTION {new.sentence}
{ output.state after.block =
    'skip$
    { output.state before.all =
	'skip$
	{ after.sentence 'output.state := }
      if$
    }
  if$
}

FUNCTION {not}
{   { #0 }
    { #1 }
  if$
}

FUNCTION {and}
{   'skip$
    { pop$ #0 }
  if$
}

FUNCTION {or}
{   { pop$ #1 }
    'skip$
  if$
}

FUNCTION {new.block.checka}
{ empty$
    'skip$
    'new.block
  if$
}

FUNCTION {new.block.checkb}
{ empty$
  swap$ empty$
  and
    'skip$
    'new.block
  if$
}

FUNCTION {new.sentence.checka}
{ empty$
    'skip$
    'new.sentence
  if$
}

FUNCTION {field.or.null}
{ duplicate$ empty$
    { pop$ "" }
    'skip$
  if$
}

FUNCTION {emphasize}
{ duplicate$ empty$
    { pop$ "" }
    { "{\em " swap$ * "}" * }
  if$
}

INTEGERS{ l }
FUNCTION{ string.length }
{
  #1 'l :=
  {duplicate$ duplicate$ #1 l substring$ = not}
  {l #1 + 'l :=}
  while$
  pop$ l
}

STRINGS{replace find text}
INTEGERS{find_length}
FUNCTION{find.replace}
{ 'replace :=
   'find :=
   'text :=
   find string.length 'find_length :=
   ""
   { text empty$ not }
   { text #1 find_length substring$ find =
       {
	 replace *
	 text #1 find_length + global.max$ substring$ 'text :=
       }{
	 text #1 #1 substring$ *
	 text #2 global.max$ substring$ 'text :=
       }
     if$
   }
   while$
}

INTEGERS { nameptr namesleft numnames max.num.names.before.forced.et.al num.names.shown.with.forced.et.al }

FUNCTION {format.names}
{ 's :=
  #1 'nameptr :=
  #4 'max.num.names.before.forced.et.al :=
  #1 'num.names.shown.with.forced.et.al :=
  s num.names$ 'numnames :=
  numnames 'namesleft :=
    { namesleft #0 > }
    { s nameptr "{f.~}{vv~}{ll}{, jj}" format.name$ 't :=
      nameptr #1 >
        { nameptr num.names.shown.with.forced.et.al #1 + =
          numnames max.num.names.before.forced.et.al >
          and
            { "others" 't :=
              #1 'namesleft :=
            }
            { skip$ }
          if$
          namesleft #1 >
            { ", " * t * }
            { t "others" =
                { " " * "{\em et~al.}" * }
                { numnames #2 >
                    { "," * }
                    { skip$ }
                  if$
                  " and " * t *
                }
              if$
            }
          if$
        }
        't
      if$
      nameptr #1 + 'nameptr :=
      namesleft #1 - 'namesleft :=
    }
  while$

}

FUNCTION {format.authors}
{ author empty$
    { "" }
    { author format.names }
  if$
}

FUNCTION {format.eprint.newarXivid}
{ eprint #1 #6 substring$ "arXiv:" =
    {"\href{http://arxiv.org/abs/" eprint #7 #9 substring$ * "}" *
        "{{\tt " * eprint #7 #9 substring$ * "}}" *}
    {"\href{http://arxiv.org/abs/" eprint #1 #9 substring$  * "}" *
        "{{\tt arXiv:" * eprint #1 #9 substring$ * "}}" *}
  if$
}

FUNCTION {format.eprint.oldarXivid}
{ archive empty$
    {"\href{http://arxiv.org/abs/" eprint * "}" *
        "{{\tt arXiv:" * eprint * "}}" *}
    {"\href{http://arxiv.org/abs/" archive *  "/" * eprint * "}" *
        "{{\tt arXiv:" * archive * "/" * eprint * "}}" *}
  if$
}


FUNCTION {format.eprint}
{ eprint empty$
    { ""}
        { eprint #1 #6 substring$ "arXiv:" =
          eprint #5 #1 substring$ "." =
          or  
              { format.eprint.newarXivid } 
              { format.eprint.oldarXivid } 
        if$
      }
  if$
}


FUNCTION {format.eprint.paren}
{ eprint missing$ { "" } { eprint empty$ { "" } {format.eprint} 
    			   if$ 
			  } 
  if$
}

FUNCTION {format.lhcbid.internal}
{ "\href{http://cdsweb.cern.ch/search?p=" number * 
  "&f=reportnumber&action_search=Search&c=LHCb+Internal+Notes&c=LHCb+Analysis+Notes} {" * 
  number * "}" * 
}

FUNCTION {format.lhcbid.public}
{ "\href{http://cdsweb.cern.ch/search?p=" number * 
  "&f=reportnumber&action_search=Search&c=LHCb+Reports&c=LHCb+Conference+Proceedings&c=LHCb+Conference+Contributions&c=LHCb+Notes&c=LHCb+Theses&c=LHCb+Papers} {" * 
  number * "}" * 
}

FUNCTION {format.lhcbid}
{ number #6 #3 substring$ "ANA" =
  number #6 #3 substring$ "INT" =
  or
    { format.lhcbid.internal }
    { format.lhcbid.public }
  if$
}

FUNCTION {format.DOI}
{ DOI empty$
    { ""}
    { "doi:~\href{http://dx.doi.org/" doi * "}{" * doi * "}" *}
  if$
}


FUNCTION {format.report}
{ report empty$
     { ""}
     { report}
     if$
}



FUNCTION {format.editors}
{ editor empty$
    { "" }
    { editor format.names
      editor num.names$ #1 >
	{ ", eds." * }
	{ ", ed." * }
      if$
    }
  if$
}

FUNCTION {format.title}
{ title empty$
    { "" }
    { "\ifthenelse{\boolean{articletitles}}{{\it " title "t" change.case$ * "}, " *  "}{}" * }
  if$
}

FUNCTION {format.title.p}
{ title empty$
    { "" }
    { "``" title "t" change.case$ * ".''" * }
  if$
}

FUNCTION {n.dashify}
{ 't :=
  ""
    { t empty$ not }
    { t #1 #1 substring$ "-" =
	{ t #1 #2 substring$ "--" = not
	    { "--" *
	      t #2 global.max$ substring$ 't :=
	    }
	    {   { t #1 #1 substring$ "-" = }
		{ "-" *
		  t #2 global.max$ substring$ 't :=
		}
	      while$
	    }
	  if$
	}
	{ t #1 #1 substring$ *
	  t #2 global.max$ substring$ 't :=
	}
      if$
    }
  while$
}

FUNCTION {format.date}
{ year empty$
    { month empty$
	{ "" }
	{ "there's a month but no year in " cite$ * warning$
	  month
	}
      if$
    }
    { month empty$
	'year
	{ month ", " * year * }
      if$
    }
  if$
}

FUNCTION {format.date.paren}
{ year empty$
    { "" }
    {"(" year * ")" *}
  if$
}

FUNCTION {format.collaboration}
{ collaboration empty$
    { "" }
    { collaboration }
  if$
}


FUNCTION {format.btitle}
{ title emphasize
}

FUNCTION {tie.or.space.connect}
{ duplicate$ text.length$ #3 <
    { "~" }
    { " " }
  if$
  swap$ * *
}

FUNCTION {either.or.check}
{ empty$
    'pop$
    { "can't use both " swap$ * " fields in " * cite$ * warning$ }
  if$
}

FUNCTION {format.bvolume}
{ volume empty$
    { "" }
    { "vol.~" volume *
      series empty$
	'skip$
	{ " of " * series emphasize * }
      if$
      "volume and number" number either.or.check
    }
  if$
}

FUNCTION {format.number.series}
{ volume empty$
    { number empty$
	{ series field.or.null }
	{ output.state mid.sentence =
	    { "no.~" }
	    { "No.~" }
	  if$
	  number *
	  series empty$
	    { "there's a number but no series in " cite$ * warning$ }
	    { " in " * series * }
	  if$
	}
      if$
    }
    { "" }
  if$
}

FUNCTION {format.edition}
{ edition empty$
    { "" }
    { edition "l" change.case$ "~ed." * }
  if$
}

INTEGERS { multiresult }

FUNCTION {multi.page.check}
{ 't :=
  #0 'multiresult :=
    { multiresult not
      t empty$ not
      and
    }
    { t #1 #1 substring$
      duplicate$ "-" =
      swap$ duplicate$ "," =
      swap$ "+" =
      or or
	{ #1 'multiresult := }
	{ t #2 global.max$ substring$ 't := }
      if$
    }
  while$
  multiresult
}

FUNCTION {format.pages}
{ pages empty$
    { "" }
    { pages multi.page.check
	{ "pp.~" pages n.dashify * }
	{ "p.~" pages * }
      if$
    }
  if$
}

FUNCTION {format.pages.nopp}
{ pages empty$
    { "" }
    { pages multi.page.check
	{ pages n.dashify  }
	{ pages }
      if$
    }
  if$
}

FUNCTION {first.page}
{ 't :=
""
{  t empty$ not t #1 #1 substring$ "-" = not and }
{ t #1 #1 substring$ *
t #2 global.max$ substring$ 't :=
}
while$
}

FUNCTION {format.pages.norange}
{ pages empty$
    { "" }
    { pages first.page  }
  if$
}


FUNCTION {format.volume}
{ volume empty$
    { "" }
    { "{\bf " volume * "}" * }
  if$
}

FUNCTION {format.number}
{ number empty$
    { "" }
    { "no.~" number * }
  if$
}

FUNCTION {format.chapter.pages}
{ chapter empty$
    'format.pages
    { type empty$
	{ "ch.~" chapter * }
	{ type "l" change.case$ chapter tie.or.space.connect }
      if$
      pages empty$
	'skip$
	{ ", " * format.pages * }
      if$
    }
  if$
}

FUNCTION {format.in.ed.booktitle}
{ booktitle empty$
    { "" }
    { "in " booktitle emphasize *
      editor empty$
	'skip$
	{ " (" * format.editors * ")" * }
      if$
    }
  if$
}

FUNCTION {format.thesis.type}
{ type empty$
    'skip$
    { pop$
      output.state after.block =
	{ type "t" change.case$ }
	{ type "l" change.case$ }
      if$
    }
  if$
}

FUNCTION {empty.misc.check}
{ author empty$ title empty$ howpublished empty$
  month empty$ year empty$ note empty$
  and and and and and
    { "all relevant fields are empty in " cite$ * warning$ }
    'skip$
  if$
}

FUNCTION {format.tr.number}
{ type empty$
    { "Tech. Rep." }
    'type
  if$
  number empty$
    { "l" change.case$ }
    { number tie.or.space.connect }
  if$
}

FUNCTION {format.paddress}
{ address empty$
    { "" }
    { "(" address * ")" * }
  if$
}

FUNCTION {format.article.crossref}
{ key empty$
    { journal empty$
	{ "need key or journal for " cite$ * " to crossref " * crossref *
	  warning$
	  ""
	}
	{ "in { " journal * "\/}" * }
      if$
    }
    { "in " key * }
  if$
  " \cite{" * crossref * "}" *
}

FUNCTION {format.crossref.editor}
{ editor #1 "{vv~}{ll}" format.name$
  editor num.names$ duplicate$
  #2 >
    { pop$ " {\em et.~al.}" * }
    { #2 <
	'skip$
	{ editor #2 "{ff }{vv }{ll}{ jj}" format.name$ "others" =
	    { " {\em et.~al.}" * }
	    { " and " * editor #2 "{vv~}{ll}" format.name$ * }
	  if$
	}
      if$
    }
  if$
}

FUNCTION {format.book.crossref}
{ volume empty$
    { "empty volume in " cite$ * "'s crossref of " * crossref * warning$
      "In "
    }
    { "Vol.~" volume *
      " of " *
    }
  if$
  editor empty$
  editor field.or.null author field.or.null =
  or
    { key empty$
	{ series empty$
	    { "need editor, key, or series for " cite$ * " to crossref " *
	      crossref * warning$
	      "" *
	    }
	    { "{\em " * series * "\/}" * }
	  if$
	}
	{ key * }
      if$
    }
    { format.crossref.editor * }
  if$
  " \cite{" * crossref * "}" *
}

FUNCTION {format.incoll.inproc.crossref}
{ editor empty$
  editor field.or.null author field.or.null =
  or
    { key empty$
	{ booktitle empty$
	    { "need editor, key, or booktitle for " cite$ * " to crossref " *
	      crossref * warning$
	      ""
	    }
	    { "in {\em " booktitle * "\/}" * }
	  if$
	}
	{ "in " key * }
      if$
    }
    { "in " format.crossref.editor * }
  if$
  " \cite{" * crossref * "}" *
}

FUNCTION {format.articlenodoi}
{ output.bibitem
  format.collaboration output
  format.authors "author" output.check
  format.title "title" output.check
  article.sep
  crossref missing$
  { journal missing$
    { format.eprint output }
    { journal empty$ { format.eprint output } {
      journal ".\ " "." find.replace 
      ". " "." find.replace 
      "." ".\ " find.replace 
      "journal" output.check
      blank.sep
      format.volume output
      blank.sep
      format.date.paren "year" output.check
      month empty$ { format.number output }
        		  'skip$ if$ 
      blank.sep
      format.pages.norange output }
      if$
      }
    if$
    report missing$
            { journal empty$ {} { format.eprint.paren output} if$ }
            {blank.sep format.report output format.eprint.paren output}
            if$
    }
    { format.article.crossref output.nonnull
      format.pages output
      format.eprint.paren output
    }
  if$
  mid.sentence 'output.state :=
  note output
  fin.entry
}

FUNCTION {format.articledoi}
{ output.bibitem
  format.collaboration output
  format.authors "author" output.check
  format.title "title" output.check
  article.sep
  crossref missing$
  { journal missing$
    { format.eprint output }
    { journal empty$ { format.eprint output } {
      "\href{http://dx.doi.org/" doi * "}{" * "doi" output.check
      article.sep
      journal ".\ " "." find.replace 
      ". " "." find.replace 
      "." ".\ " find.replace 
      "journal" output.check
      blank.sep
      format.volume output
      blank.sep
      format.date.paren "year" output.check
      month empty$ { format.number output }
        		  'skip$ if$ 
      blank.sep
      format.pages.norange 
      "}" *
      output } 
      if$
      }
    if$
    report missing$
            { journal empty$ {} { format.eprint.paren output} if$ }
            {blank.sep format.report output format.eprint.paren output}
            if$
    }
    { format.article.crossref output.nonnull
      format.pages output
      format.eprint.paren output
    }
  if$
  mid.sentence 'output.state :=
  note output
  fin.entry
}

FUNCTION {article}
{ DOI empty$
    { format.articlenodoi}
    { format.articledoi}
  if$
}

FUNCTION {book}
{ output.bibitem
  format.collaboration output
  author empty$
    { format.editors "author and editor" output.check }
    { format.authors output.nonnull
      crossref missing$
	{ "author and editor" editor either.or.check }
	'skip$
      if$
    }
  if$
  format.btitle "title" output.check
  crossref missing$
    { format.bvolume output
%      new.block
      format.number.series output
%      new.sentence
      publisher "publisher" output.check
      address output
    }
    { new.block
      format.book.crossref output.nonnull
    }
  if$
  format.edition output
  format.date "year" output.check
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {booklet}
{ output.bibitem
  format.collaboration output
  format.authors output
  title empty$
    { "empty title in " cite$ * warning$
      howpublished new.sentence.checka
    }
    { howpublished empty$ not
      address empty$ month empty$ year empty$ and and
      or
	{ format.title output.nonnull }
	{ format.title output.nonnull }
      if$
      blank.sep
    }
  if$
  howpublished output
  address output
  format.date output
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {inbook}
{ output.bibitem
  format.collaboration output
  author empty$
    { format.editors "author and editor" output.check }
    { format.authors output.nonnull
      crossref missing$
	{ "author and editor" editor either.or.check }
	'skip$
      if$
    }
  if$
  format.btitle "title" output.check
  crossref missing$
    { format.bvolume output
      format.chapter.pages "chapter and pages" output.check
      new.block
      format.number.series output
      new.block
      publisher "publisher" output.check
      address output
    }
    { format.chapter.pages "chapter and pages" output.check
      new.block
      format.book.crossref output.nonnull
    }
  if$
  format.edition output
  format.date "year" output.check
  new.block
  format.eprint output
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {incollection}
{ output.bibitem
  format.collaboration output
  format.authors "author" output.check
  format.title "title" output.check
  blank.sep
  crossref missing$
    { format.in.ed.booktitle "booktitle" output.check
      format.bvolume output
      format.number.series output
      format.chapter.pages output
      new.block
      publisher "publisher" output.check
      address output
      format.edition output
      format.date "year" output.check
    }
    { format.incoll.inproc.crossref output.nonnull
      format.chapter.pages output
    }
  if$
  new.block
  format.eprint output
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {inproceedings}
{ output.bibitem
  format.collaboration output
  format.authors "author" output.check
  format.title "title" output.check
  blank.sep
  crossref missing$
    { format.in.ed.booktitle "booktitle" output.check
      format.bvolume output
      format.number.series output
      format.paddress output
      format.pages output
      organization output
      publisher output
      format.date "year" output.check
    }
    { format.incoll.inproc.crossref output.nonnull
      format.pages output
    }
  if$
  new.block
  format.eprint output
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {conference} { inproceedings }

FUNCTION {manual}
{ output.bibitem
  format.collaboration output
  author empty$
    { organization empty$
	'skip$
	{ organization output.nonnull
	  address output
	}
      if$
    }
    { format.authors output.nonnull }
  if$
  format.btitle "title" output.check
  author empty$
    { organization empty$
	{ address new.block.checka
	  address output
	}
	'skip$
      if$
    }
    { organization address new.block.checkb
      organization output
      address output
    }
  if$
  format.edition output
  format.date output
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {mastersthesis}
{ output.bibitem
  format.authors "author" output.check
  format.title "title" output.check
  blank.sep
  "Master's thesis" format.thesis.type output.nonnull
  school "school" output.check
  address output
  format.date "year" output.check
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {misc}
{ output.bibitem
  format.collaboration output
  format.authors output
  title empty$
    { howpublished new.sentence.checka }
    { howpublished empty$ not
      month empty$ year empty$ and
      or
	{ format.title output.nonnull }
	{ format.title output.nonnull }
      if$
      blank.sep
    }
  if$
  howpublished output
  format.date output
  new.block
  note output
  format.DOI output
  fin.entry
  empty.misc.check
}

FUNCTION {phdthesis}
{ output.bibitem
  format.authors "author" output.check
  format.btitle "title" output.check
  "PhD thesis" format.thesis.type output.nonnull
  school "school" output.check
  address output
  format.date "year" output.check
  format.eprint output
  note output
  format.DOI output
  fin.entry
}

FUNCTION {proceedings}
{ output.bibitem
  editor empty$
    { organization output }
    { format.editors output.nonnull }
  if$
  format.btitle "title" output.check
  format.bvolume output
  format.number.series output
  format.paddress output
  editor empty$
    'skip$
    { organization output }
  if$
  publisher output
  format.date "year" output.check
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {techreport}
{ output.bibitem
  format.collaboration output
  format.authors "author" output.check
  format.title "title" output.check
  blank.sep
  format.tr.number output.nonnull
  institution "institution" output.check
  address output
  format.date "year" output.check
  new.block
  note output
  format.DOI output
  fin.entry
}

FUNCTION {lhcbreport}
{ output.bibitem
  format.collaboration output
  format.authors "author" output.check
  format.title "title" output.check
  blank.sep
  format.lhcbid output.nonnull
  new.block
  note output
  fin.entry
}

FUNCTION {unpublished}
{ output.bibitem
  format.collaboration output
  format.authors "author" output.check
  format.title "title" output.check
  blank.sep
  note "note" output.check
  format.date output
  fin.entry
}

FUNCTION {default.type} { misc }

MACRO {jan} {"Jan."}

MACRO {feb} {"Feb."}

MACRO {mar} {"Mar."}

MACRO {apr} {"Apr."}

MACRO {may} {"May"}

MACRO {jun} {"June"}

MACRO {jul} {"July"}

MACRO {aug} {"Aug."}

MACRO {sep} {"Sept."}

MACRO {oct} {"Oct."}

MACRO {nov} {"Nov."}

MACRO {dec} {"Dec."}

MACRO {nup} {"Nucl. Phys."}

MACRO {cmp} {"Comm. Math. Phys."}

MACRO {prl} {"Phys. Rev. Lett."}

MACRO {pl} {"Phys. Lett."}

MACRO {rmp} {"Rev. Mod. Phys."}

MACRO {ijmp} {"Int. Jour. Mod. Phys."}

MACRO {mpl} {"Mod. Phys. Lett."}

MACRO {pr} {"Phys. Rev."}

READ

STRINGS { longest.label }

INTEGERS { number.label longest.label.width }

FUNCTION {initialize.longest.label}
{ "" 'longest.label :=
  #1 'number.label :=
  #0 'longest.label.width :=
}

FUNCTION {longest.label.pass}
{ number.label int.to.str$ 'label :=
  number.label #1 + 'number.label :=
  label width$ longest.label.width >
    { label 'longest.label :=
      label width$ 'longest.label.width :=
    }
    'skip$
  if$
}

EXECUTE {initialize.longest.label}

ITERATE {longest.label.pass}

% FUNCTION {begin.bib}
% { preamble$ empty$
%     'skip$
%     { preamble$ write$ newline$ }
%   if$
%   "\providecommand{\href}[2]{#2}"
%   "\begingroup\raggedright\begin{thebibliography}{" * longest.label  * 
%   "}" * write$ newline$ }

FUNCTION {begin.bib}
{ preamble$ empty$
    'skip$
    { preamble$ write$ newline$ }
  if$
  "\ifx\mcitethebibliography\mciteundefinedmacro"
  write$ newline$
  "\PackageError{LHCb.bst}{mciteplus.sty has not been loaded}"
  write$ newline$
  "{This bibstyle requires the use of the mciteplus package.}\fi"
  write$ newline$
  "\providecommand{\href}[2]{#2}"
  write$ newline$
  "\begin{mcitethebibliography}{"  longest.label  * "}" * write$ newline$
  "\mciteSetBstSublistMode{n}"
  write$ newline$
  "\mciteSetBstMaxWidthForm{subitem}{\alph{mcitesubitemcount})}"
  write$ newline$
  "\mciteSetBstSublistLabelBeginEnd{\mcitemaxwidthsubitemform\space}"
  write$ newline$
  "{\relax}{\relax}"
  write$ newline$
}


EXECUTE {begin.bib}

EXECUTE {init.state.consts}

ITERATE {call.type$}

% FUNCTION {end.bib}
% { newline$
%   "\end{thebibliography}\endgroup" write$ newline$
% }

FUNCTION {end.bib}
{ newline$
  "\end{mcitethebibliography}" write$ newline$
}

EXECUTE {end.bib}
