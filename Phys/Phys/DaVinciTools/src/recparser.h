#ifndef YYERRCODE
#define YYERRCODE 256
#endif

#define NAME 257
#define VALUE 258
#define CHARGE 259
#define BARYON 260
#define LEPTON 261
#define ISOSPIN 262
#define TOTALSPIN 263
#define SPIN 264
#define ANGULAR 265
#define G_PARITY 266
#define C_PARITY 267
#define P_PARITY 268
#define CP_PARITY 269
#define CHARM 270
#define STRANGE 271
#define TOP 272
#define BOTTOM 273
#define COLON 274
#define BANG 275
#define QMARK 276
#define METAX 277
#define COMMA 278
#define CARET 279
#define LBRACK 280
#define LBRACE 281
#define LANGLE 282
#define LPAREN 283
#define CCONJ 284
#define OSCIL 285
#define NOSCIL 286
#define RBRACE 287
#define RANGLE 288
#define RPAREN 289
#define EQEQUAL 290
#define LESSEQUAL 291
#define GREATEREQUAL 292
#define NOTEQUAL 293
#define ARROW 294
#define RARROW 295
#define ELIPSIS 296
#define PP 297
#define U_Q 298
#define D_Q 299
#define C_Q 300
#define S_Q 301
#define T_Q 302
#define B_Q 303
#define AU_Q 304
#define AD_Q 305
#define AC_Q 306
#define AS_Q 307
#define AT_Q 308
#define AB_Q 309
union YYSTYPE {
  std::string *text;
  double dval;
  Quarks quarkval;
  Quantums quantumval;
  Descriptor *desc;
  ParticleMatcher *match;
  std::vector<ParticleMatcher *> *mvect;
} ;

