#define LPAREN 257
#define RPAREN 258
#define LBRACK 259
#define OSCIL 260
#define NOSCIL 261
#define CCONJ 262
#define LBRACE 263
#define RBRACE 264
#define LANGLE 265
#define RANGLE 266
#define ARROW 267
#define RARROW 268
#define COMMA 269
#define EXCLAM 270
#define ELIPSIS 271
#define PP 272
#define QMARK 273
#define METAX 274
#define EQEQUAL 275
#define LESSEQUAL 276
#define GREATEREQUAL 277
#define NOTEQUAL 278
#define UP_QUARK 279
#define DOWN_QUARK 280
#define CHARM_QUARK 281
#define STRANGE_QUARK 282
#define TOP_QUARK 283
#define BOTTOM_QUARK 284
#define ANTI_UP_QUARK 285
#define ANTI_DOWN_QUARK 286
#define ANTI_CHARM_QUARK 287
#define ANTI_STRANGE_QUARK 288
#define ANTI_TOP_QUARK 289
#define ANTI_BOTTOM_QUARK 290
#define CHARGE 291
#define BARYON 292
#define LEPTON 293
#define ISOSPIN 294
#define TOTALSPIN 295
#define SPIN 296
#define ANGULAR 297
#define G_PARITY 298
#define CP_PARITY 299
#define C_PARITY 300
#define P_PARITY 301
#define CHARM 302
#define STRANGE 303
#define TOP 304
#define BOTTOM 305
#define NAME 306
#define VALUE 307
union YYSTYPE {
  std::string *text;
  double dval;
  Quarks quarkval;
  Quantums quantumval;
  Descriptor *desc;
  ParticleMatcher *match;
  std::vector<ParticleMatcher *> *mvect;
} ;

