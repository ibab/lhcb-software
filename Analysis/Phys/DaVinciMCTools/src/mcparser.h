#define LPAREN 257
#define RPAREN 258
#define LBRACK 259
#define OSCIL 260
#define CCONJ 261
#define LBRACE 262
#define RBRACE 263
#define LANGLE 264
#define RANGLE 265
#define ARROW 266
#define RARROW 267
#define COMMA 268
#define EXCLAM 269
#define ELIPSIS 270
#define PP 271
#define QMARK 272
#define METAX 273
#define EQEQUAL 274
#define LESSEQUAL 275
#define GREATEREQUAL 276
#define NOTEQUAL 277
#define UP_QUARK 278
#define DOWN_QUARK 279
#define CHARM_QUARK 280
#define STRANGE_QUARK 281
#define TOP_QUARK 282
#define BOTTOM_QUARK 283
#define ANTI_UP_QUARK 284
#define ANTI_DOWN_QUARK 285
#define ANTI_CHARM_QUARK 286
#define ANTI_STRANGE_QUARK 287
#define ANTI_TOP_QUARK 288
#define ANTI_BOTTOM_QUARK 289
#define CHARGE 290
#define BARYON 291
#define LEPTON 292
#define ISOSPIN 293
#define TOTALSPIN 294
#define SPIN 295
#define ANGULAR 296
#define G_PARITY 297
#define CP_PARITY 298
#define C_PARITY 299
#define P_PARITY 300
#define CHARM 301
#define STRANGE 302
#define TOP 303
#define BOTTOM 304
#define NAME 305
#define VALUE 306
union YYSTYPE {
  char *text;
  double dval;
  MCDecayFinder::Quarks quarkval;
  MCDecayFinder::Quantums quantumval;
  MCDecayFinder::Descriptor *desc;
  MCDecayFinder::ParticleMatcher *match;
  std::vector<MCDecayFinder::ParticleMatcher *> *mvect;
} ;

