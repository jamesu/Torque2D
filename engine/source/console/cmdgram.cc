/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse CMDparse
#define yylex   CMDlex
#define yyerror CMDerror
#define yylval  CMDlval
#define yychar  CMDchar
#define yydebug CMDdebug
#define yynerrs CMDnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     rwDEFINE = 258,
     rwENDDEF = 259,
     rwDECLARE = 260,
     rwDECLARESINGLETON = 261,
     rwBREAK = 262,
     rwELSE = 263,
     rwCONTINUE = 264,
     rwGLOBAL = 265,
     rwIF = 266,
     rwNIL = 267,
     rwRETURN = 268,
     rwWHILE = 269,
     rwDO = 270,
     rwENDIF = 271,
     rwENDWHILE = 272,
     rwENDFOR = 273,
     rwDEFAULT = 274,
     rwFOR = 275,
     rwFOREACH = 276,
     rwFOREACHSTR = 277,
     rwIN = 278,
     rwDATABLOCK = 279,
     rwSWITCH = 280,
     rwCASE = 281,
     rwSWITCHSTR = 282,
     rwCASEOR = 283,
     rwPACKAGE = 284,
     rwNAMESPACE = 285,
     rwCLASS = 286,
     rwASSERT = 287,
     ILLEGAL_TOKEN = 288,
     CHRCONST = 289,
     INTCONST = 290,
     TTAG = 291,
     VAR = 292,
     IDENT = 293,
     TYPEIDENT = 294,
     DOCBLOCK = 295,
     STRATOM = 296,
     TAGATOM = 297,
     FLTCONST = 298,
     opINTNAME = 299,
     opINTNAMER = 300,
     opMINUSMINUS = 301,
     opPLUSPLUS = 302,
     STMT_SEP = 303,
     opSHL = 304,
     opSHR = 305,
     opPLASN = 306,
     opMIASN = 307,
     opMLASN = 308,
     opDVASN = 309,
     opMODASN = 310,
     opANDASN = 311,
     opXORASN = 312,
     opORASN = 313,
     opSLASN = 314,
     opSRASN = 315,
     opCAT = 316,
     opEQ = 317,
     opNE = 318,
     opGE = 319,
     opLE = 320,
     opAND = 321,
     opOR = 322,
     opSTREQ = 323,
     opCOLONCOLON = 324,
     opNTASN = 325,
     opNDASN = 326,
     opMDASN = 327,
     opSTRNE = 328,
     UNARY = 329
   };
#endif
/* Tokens.  */
#define rwDEFINE 258
#define rwENDDEF 259
#define rwDECLARE 260
#define rwDECLARESINGLETON 261
#define rwBREAK 262
#define rwELSE 263
#define rwCONTINUE 264
#define rwGLOBAL 265
#define rwIF 266
#define rwNIL 267
#define rwRETURN 268
#define rwWHILE 269
#define rwDO 270
#define rwENDIF 271
#define rwENDWHILE 272
#define rwENDFOR 273
#define rwDEFAULT 274
#define rwFOR 275
#define rwFOREACH 276
#define rwFOREACHSTR 277
#define rwIN 278
#define rwDATABLOCK 279
#define rwSWITCH 280
#define rwCASE 281
#define rwSWITCHSTR 282
#define rwCASEOR 283
#define rwPACKAGE 284
#define rwNAMESPACE 285
#define rwCLASS 286
#define rwASSERT 287
#define ILLEGAL_TOKEN 288
#define CHRCONST 289
#define INTCONST 290
#define TTAG 291
#define VAR 292
#define IDENT 293
#define TYPEIDENT 294
#define DOCBLOCK 295
#define STRATOM 296
#define TAGATOM 297
#define FLTCONST 298
#define opINTNAME 299
#define opINTNAMER 300
#define opMINUSMINUS 301
#define opPLUSPLUS 302
#define STMT_SEP 303
#define opSHL 304
#define opSHR 305
#define opPLASN 306
#define opMIASN 307
#define opMLASN 308
#define opDVASN 309
#define opMODASN 310
#define opANDASN 311
#define opXORASN 312
#define opORASN 313
#define opSLASN 314
#define opSRASN 315
#define opCAT 316
#define opEQ 317
#define opNE 318
#define opGE 319
#define opLE 320
#define opAND 321
#define opOR 322
#define opSTREQ 323
#define opCOLONCOLON 324
#define opNTASN 325
#define opNDASN 326
#define opMDASN 327
#define opSTRNE 328
#define UNARY 329




/* Copy the first part of user declarations.  */
#line 1 "CMDgram.y"


// bison --defines=cmdgram.h --verbose -o cmdgram.cpp -p CMD CMDgram.y

// Make sure we don't get gram.h twice.
#define _CMDGRAM_H_

#include <stdlib.h>
#include <stdio.h>
#include "console/console.h"
#include "console/compiler.h"
#include "console/consoleInternal.h"
#include "platform/platformString.h"

#ifndef YYDEBUG
#define YYDEBUG 0
#endif

#define YYSSIZE 350

int outtext(char *fmt, ...);
extern int serrors;

#define nil 0
#undef YY_ARGS
#define YY_ARGS(x)   x

int CMDlex();
void CMDerror(char *, ...);

#ifdef alloca
#undef alloca
#endif
#define alloca dMalloc

template< typename T >
struct Token
{
   T value;
   U32 lineNumber;
};

#line 44 "CMDgram.y"

        /* Reserved Word Definitions */
#line 55 "CMDgram.y"

        /* Constants and Identifier Definitions */
#line 69 "CMDgram.y"

        /* Operator Definitions */


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 82 "CMDgram.y"
{
   Token< char >           c;
   Token< int >            i;
   Token< const char* >    s;
   Token< char* >          str;
   Token< double >         f;
   StmtNode*               stmt;
   ExprNode*               expr;
   SlotAssignNode*         slist;
   VarNode*                var;
   SlotDecl                slot;
   InternalSlotDecl        intslot;
   ObjectBlockDecl         odcl;
   ObjectDeclNode*         od;
   AssignDecl              asn;
   IfStmtNode*             ifnode;
}
/* Line 193 of yacc.c.  */
#line 323 "CMDgram.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 336 "CMDgram.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3043

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  100
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  163
/* YYNRULES -- Number of states.  */
#define YYNSTATES  388

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   329

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    64,     2,     2,     2,    54,    53,     2,
      55,    56,    46,    44,    57,    45,    51,    47,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    58,    59,
      48,    50,    49,    96,    65,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    92,     2,    99,    62,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    60,    52,    61,    63,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    93,    94,    95,    97,    98
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    22,
      24,    27,    28,    31,    33,    35,    37,    39,    41,    43,
      46,    49,    52,    56,    59,    64,    71,    73,    82,    93,
      94,    96,    98,   102,   113,   124,   132,   145,   155,   166,
     174,   175,   178,   179,   181,   182,   185,   186,   188,   190,
     193,   196,   200,   204,   206,   214,   222,   227,   235,   241,
     243,   247,   253,   261,   267,   274,   284,   293,   302,   310,
     319,   327,   335,   342,   350,   358,   360,   362,   366,   370,
     374,   378,   382,   386,   390,   394,   398,   401,   404,   406,
     412,   416,   420,   424,   428,   432,   436,   440,   444,   448,
     452,   456,   460,   464,   467,   470,   472,   474,   476,   478,
     480,   482,   484,   486,   488,   493,   501,   505,   512,   516,
     520,   522,   526,   528,   530,   533,   536,   539,   542,   545,
     548,   551,   554,   557,   560,   562,   564,   566,   570,   577,
     580,   586,   589,   593,   599,   604,   611,   618,   623,   630,
     631,   633,   635,   639,   640,   642,   644,   647,   652,   658,
     663,   671,   680,   682
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     101,     0,    -1,   102,    -1,    -1,   102,   103,    -1,   107,
      -1,   108,    -1,   104,    -1,    29,    38,    60,   105,    61,
      59,    -1,   108,    -1,   105,   108,    -1,    -1,   106,   107,
      -1,   122,    -1,   123,    -1,   124,    -1,   125,    -1,   111,
      -1,   119,    -1,     7,    59,    -1,     9,    59,    -1,    13,
      59,    -1,    13,   127,    59,    -1,   126,    59,    -1,    36,
      50,   127,    59,    -1,    36,    50,   127,    57,   127,    59,
      -1,    40,    -1,     3,    38,    55,   109,    56,    60,   106,
      61,    -1,     3,    38,    91,    38,    55,   109,    56,    60,
     106,    61,    -1,    -1,   110,    -1,    37,    -1,   110,    57,
      37,    -1,    24,   130,    55,   127,   113,    56,    60,   137,
      61,    59,    -1,     5,   130,    55,   114,   113,   115,    56,
      60,   116,    61,    -1,     5,   130,    55,   114,   113,   115,
      56,    -1,     5,   130,    55,    92,   114,    99,   113,   115,
      56,    60,   116,    61,    -1,     5,   130,    55,    92,   114,
      99,   113,   115,    56,    -1,     6,   130,    55,   114,   113,
     115,    56,    60,   116,    61,    -1,     6,   130,    55,   114,
     113,   115,    56,    -1,    -1,    58,    38,    -1,    -1,   127,
      -1,    -1,    57,   136,    -1,    -1,   138,    -1,   117,    -1,
     138,   117,    -1,   112,    59,    -1,   117,   112,    59,    -1,
      60,   106,    61,    -1,   107,    -1,    25,    55,   127,    56,
      60,   120,    61,    -1,    27,    55,   127,    56,    60,   120,
      61,    -1,    26,   121,    58,   106,    -1,    26,   121,    58,
     106,    19,    58,   106,    -1,    26,   121,    58,   106,   120,
      -1,   127,    -1,   121,    28,   127,    -1,    11,    55,   127,
      56,   118,    -1,    11,    55,   127,    56,   118,     8,   118,
      -1,    14,    55,   127,    56,   118,    -1,    15,   118,    14,
      55,   127,    56,    -1,    20,    55,   127,    59,   127,    59,
     127,    56,   118,    -1,    20,    55,   127,    59,   127,    59,
      56,   118,    -1,    20,    55,   127,    59,    59,   127,    56,
     118,    -1,    20,    55,   127,    59,    59,    56,   118,    -1,
      20,    55,    59,   127,    59,   127,    56,   118,    -1,    20,
      55,    59,   127,    59,    56,   118,    -1,    20,    55,    59,
      59,   127,    56,   118,    -1,    20,    55,    59,    59,    56,
     118,    -1,    21,    55,    37,    23,   127,    56,   118,    -1,
      22,    55,    37,    23,   127,    56,   118,    -1,   132,    -1,
     132,    -1,    55,   127,    56,    -1,   127,    62,   127,    -1,
     127,    54,   127,    -1,   127,    53,   127,    -1,   127,    52,
     127,    -1,   127,    44,   127,    -1,   127,    45,   127,    -1,
     127,    46,   127,    -1,   127,    47,   127,    -1,    45,   127,
      -1,    46,   127,    -1,    36,    -1,   127,    96,   127,    58,
     127,    -1,   127,    48,   127,    -1,   127,    49,   127,    -1,
     127,    86,   127,    -1,   127,    87,   127,    -1,   127,    84,
     127,    -1,   127,    85,   127,    -1,   127,    89,   127,    -1,
     127,    71,   127,    -1,   127,    72,   127,    -1,   127,    88,
     127,    -1,   127,    90,   127,    -1,   127,    97,   127,    -1,
     127,    65,   127,    -1,    64,   127,    -1,    63,   127,    -1,
      42,    -1,    43,    -1,    35,    -1,     7,    -1,   128,    -1,
     129,    -1,    38,    -1,    41,    -1,    37,    -1,    37,    92,
     140,    99,    -1,     3,    55,   109,    56,    60,   106,    61,
      -1,   127,    51,    38,    -1,   127,    51,    38,    92,   140,
      99,    -1,   127,    66,   130,    -1,   127,    67,   130,    -1,
      38,    -1,    55,   127,    56,    -1,    69,    -1,    68,    -1,
      73,   127,    -1,    74,   127,    -1,    75,   127,    -1,    76,
     127,    -1,    77,   127,    -1,    78,   127,    -1,    79,   127,
      -1,    80,   127,    -1,    81,   127,    -1,    82,   127,    -1,
     133,    -1,   134,    -1,   112,    -1,    37,    50,   127,    -1,
      37,    92,   140,    99,    50,   127,    -1,    37,   131,    -1,
      37,    92,   140,    99,   131,    -1,   128,   131,    -1,   128,
      50,   127,    -1,   128,    50,    60,   136,    61,    -1,    38,
      55,   135,    56,    -1,    38,    91,    38,    55,   135,    56,
      -1,   127,    51,    38,    55,   135,    56,    -1,    32,    55,
     127,    56,    -1,    32,    55,   127,    57,    41,    56,    -1,
      -1,   136,    -1,   127,    -1,   136,    57,   127,    -1,    -1,
     138,    -1,   139,    -1,   138,   139,    -1,    38,    50,   127,
      59,    -1,    39,    38,    50,   127,    59,    -1,    24,    50,
     127,    59,    -1,    38,    92,   140,    99,    50,   127,    59,
      -1,    39,    38,    92,   140,    99,    50,   127,    59,    -1,
     127,    -1,   140,    57,   127,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   161,   161,   167,   168,   173,   175,   177,   182,   187,
     189,   195,   196,   201,   202,   203,   204,   205,   206,   207,
     209,   211,   213,   215,   217,   219,   221,   226,   228,   234,
     235,   240,   242,   247,   252,   254,   256,   258,   260,   262,
     268,   269,   275,   276,   282,   283,   289,   290,   292,   294,
     299,   301,   306,   308,   313,   315,   320,   322,   324,   329,
     331,   336,   338,   343,   345,   350,   352,   354,   356,   358,
     360,   362,   364,   369,   371,   376,   381,   383,   385,   387,
     389,   391,   393,   395,   397,   399,   401,   403,   405,   407,
     409,   411,   413,   415,   417,   419,   421,   423,   425,   427,
     429,   431,   433,   435,   437,   439,   441,   443,   445,   447,
     449,   451,   453,   455,   457,   459,   472,   474,   479,   481,
     486,   488,   493,   495,   497,   499,   501,   503,   505,   507,
     509,   511,   513,   515,   520,   522,   524,   526,   528,   530,
     532,   534,   536,   538,   543,   545,   547,   552,   554,   560,
     561,   566,   568,   574,   575,   580,   582,   587,   589,   591,
     593,   595,   600,   602
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "rwDEFINE", "rwENDDEF", "rwDECLARE",
  "rwDECLARESINGLETON", "rwBREAK", "rwELSE", "rwCONTINUE", "rwGLOBAL",
  "rwIF", "rwNIL", "rwRETURN", "rwWHILE", "rwDO", "rwENDIF", "rwENDWHILE",
  "rwENDFOR", "rwDEFAULT", "rwFOR", "rwFOREACH", "rwFOREACHSTR", "rwIN",
  "rwDATABLOCK", "rwSWITCH", "rwCASE", "rwSWITCHSTR", "rwCASEOR",
  "rwPACKAGE", "rwNAMESPACE", "rwCLASS", "rwASSERT", "ILLEGAL_TOKEN",
  "CHRCONST", "INTCONST", "TTAG", "VAR", "IDENT", "TYPEIDENT", "DOCBLOCK",
  "STRATOM", "TAGATOM", "FLTCONST", "'+'", "'-'", "'*'", "'/'", "'<'",
  "'>'", "'='", "'.'", "'|'", "'&'", "'%'", "'('", "')'", "','", "':'",
  "';'", "'{'", "'}'", "'^'", "'~'", "'!'", "'@'", "opINTNAME",
  "opINTNAMER", "opMINUSMINUS", "opPLUSPLUS", "STMT_SEP", "opSHL", "opSHR",
  "opPLASN", "opMIASN", "opMLASN", "opDVASN", "opMODASN", "opANDASN",
  "opXORASN", "opORASN", "opSLASN", "opSRASN", "opCAT", "opEQ", "opNE",
  "opGE", "opLE", "opAND", "opOR", "opSTREQ", "opCOLONCOLON", "'['",
  "opNTASN", "opNDASN", "opMDASN", "'?'", "opSTRNE", "UNARY", "']'",
  "$accept", "start", "decl_list", "decl", "package_decl", "fn_decl_list",
  "statement_list", "stmt", "fn_decl_stmt", "var_list_decl", "var_list",
  "datablock_decl", "object_decl", "parent_block", "object_name",
  "object_args", "object_declare_block", "object_decl_list", "stmt_block",
  "switch_stmt", "case_block", "case_expr", "if_stmt", "while_stmt",
  "for_stmt", "foreach_stmt", "expression_stmt", "expr", "slot_acc",
  "intslot_acc", "class_name_expr", "assign_op_struct", "stmt_expr",
  "funcall_expr", "assert_expr", "expr_list_decl", "expr_list",
  "slot_assign_list_opt", "slot_assign_list", "slot_assign", "aidx_expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,    43,    45,    42,    47,    60,    62,
      61,    46,   124,    38,    37,    40,    41,    44,    58,    59,
     123,   125,    94,   126,    33,    64,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,    91,   325,   326,   327,    63,   328,   329,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   100,   101,   102,   102,   103,   103,   103,   104,   105,
     105,   106,   106,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   108,   108,   109,
     109,   110,   110,   111,   112,   112,   112,   112,   112,   112,
     113,   113,   114,   114,   115,   115,   116,   116,   116,   116,
     117,   117,   118,   118,   119,   119,   120,   120,   120,   121,
     121,   122,   122,   123,   123,   124,   124,   124,   124,   124,
     124,   124,   124,   125,   125,   126,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   128,   128,   129,   129,
     130,   130,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   133,   133,   133,   134,   134,   135,
     135,   136,   136,   137,   137,   138,   138,   139,   139,   139,
     139,   139,   140,   140
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     6,     1,
       2,     0,     2,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     3,     2,     4,     6,     1,     8,    10,     0,
       1,     1,     3,    10,    10,     7,    12,     9,    10,     7,
       0,     2,     0,     1,     0,     2,     0,     1,     1,     2,
       2,     3,     3,     1,     7,     7,     4,     7,     5,     1,
       3,     5,     7,     5,     6,     9,     8,     8,     7,     8,
       7,     7,     6,     7,     7,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     1,     5,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     7,     3,     6,     3,     3,
       1,     3,     1,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     1,     1,     3,     6,     2,
       5,     2,     3,     5,     4,     6,     6,     4,     6,     0,
       1,     1,     3,     0,     1,     1,     2,     4,     5,     4,
       7,     8,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,   108,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   107,    88,   113,   111,    26,   112,   105,   106,     0,
       0,     0,     0,     0,     4,     7,     5,     6,    17,   136,
      18,    13,    14,    15,    16,     0,     0,   109,   110,    76,
     134,   135,     0,    29,   120,     0,     0,     0,    19,    20,
       0,     0,   108,    88,    21,     0,    76,     0,    11,    53,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   123,   122,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   139,   149,     0,    86,    87,     0,
     104,   103,    23,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   141,
      29,     0,    31,     0,    30,     0,    42,    42,     0,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   137,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   162,     0,   151,     0,   150,     0,
      77,    82,    83,    84,    85,    90,    91,   116,    81,    80,
      79,    78,   102,   118,   119,    97,    98,    94,    95,    92,
      93,    99,    96,   100,     0,   101,     0,   142,     0,     0,
       0,     0,   121,    42,    40,    43,    40,     0,     0,    52,
      12,     0,     0,     0,     0,     0,     0,    40,     0,     0,
       0,     0,     9,   147,     0,     0,    24,     0,   114,   144,
       0,   149,   149,     0,     0,     0,     0,    29,    11,    32,
       0,     0,    44,    44,    61,    63,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    10,     0,
       0,   163,     0,   140,   152,     0,     0,     0,    89,   143,
      11,     0,     0,    40,    41,     0,     0,     0,     0,    64,
      72,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     8,   148,    25,   138,   145,   146,   117,
       0,     0,   115,    44,    45,    35,    39,    62,    71,    70,
       0,    68,     0,     0,     0,    73,    74,   153,     0,    59,
      54,    55,    27,    11,     0,    46,    46,    69,    67,    66,
       0,     0,     0,     0,     0,   154,   155,     0,    11,     0,
      37,     0,     0,    48,    47,     0,    65,     0,     0,     0,
       0,     0,   156,    60,    56,    28,    46,    50,    34,     0,
      49,    38,     0,     0,     0,     0,     0,    33,     0,    58,
       0,    51,   159,   157,     0,     0,     0,    11,    36,     0,
     158,     0,    57,     0,     0,   160,     0,   161
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    34,    35,   221,   141,    69,    37,   133,
     134,    38,    39,   242,   204,   276,   342,   343,    70,    40,
     291,   318,    41,    42,    43,    44,    45,    46,    47,    48,
      56,    94,    66,    50,    51,   167,   168,   334,   344,   336,
     165
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -289
static const yytype_int16 yypact[] =
{
    -289,    24,   189,  -289,   -19,     4,     4,   -22,   -16,    14,
     830,    39,   419,    44,    51,    54,     4,    58,    59,    10,
      66,  -289,    76,   186,   -40,  -289,  -289,  -289,  -289,  1226,
    1226,  1226,  1226,  1226,  -289,  -289,  -289,  -289,  -289,  -289,
    -289,  -289,  -289,  -289,  -289,    68,  2689,   462,  -289,    71,
    -289,  -289,   -14,    96,  -289,  1226,    79,    81,  -289,  -289,
    1226,    83,  -289,  -289,  -289,  1339,  -289,  1226,  -289,  -289,
     127,   874,   106,   109,    93,  1226,  1226,    94,  1226,  1226,
    1226,  -289,  -289,  1226,  1226,  1226,  1226,  1226,  1226,  1226,
    1226,  1226,  1226,  1226,  -289,  1226,   114,   -20,   -20,  1393,
     -20,   -20,  -289,  1226,  1226,  1226,  1226,  1226,  1226,   118,
    1226,  1226,  1226,  1226,  1226,     4,     4,  1226,  1226,  1226,
    1226,  1226,  1226,  1226,  1226,  1226,  1226,  1226,   918,  -289,
      96,   122,  -289,   105,   110,  1447,    20,  1226,  1501,  -289,
    1555,   551,   107,   962,  1609,   149,   150,  1226,  1663,  1717,
     171,  1231,  1285,  2689,  2689,  2689,  2689,  2689,  2689,  2689,
    2689,  2689,  2689,  2689,  2689,   -39,  2689,   119,   120,   123,
    -289,   280,   280,   -20,   -20,  2946,  2946,   -25,  2830,  2888,
     -20,  2859,   258,  -289,  -289,    78,    78,  2917,  2917,  2946,
    2946,  2801,  2772,   258,  1771,   258,  1226,  2689,   132,   136,
     133,   160,  -289,  1226,   143,  2689,   143,   419,   419,  -289,
    -289,  1226,  1006,  1825,  1050,  1226,  1226,  1879,   152,   155,
     179,    11,  -289,  -289,   178,  1226,  -289,  1226,   561,  -289,
    1226,  1226,  1226,  1226,  1226,   -17,   162,    96,  -289,  -289,
     121,   185,   176,   176,   220,  -289,  1933,   419,  1987,  1094,
    1138,  2041,  2095,  2149,   181,   212,   212,   180,  -289,   184,
    2203,  2689,  1226,  -289,  2689,   187,   190,   -29,  2743,  -289,
    -289,   191,   651,   143,  -289,  1226,   192,   193,   419,  -289,
    -289,   419,   419,  2257,   419,  2311,  1182,   419,   419,   182,
    1226,   195,   196,  -289,  -289,  -289,  2689,  -289,  -289,  -289,
     696,   198,  -289,   176,   120,   209,   213,  -289,  -289,  -289,
     419,  -289,   419,   419,  2365,  -289,  -289,    57,     6,  2689,
    -289,  -289,  -289,  -289,   194,    15,    15,  -289,  -289,  -289,
     419,   201,   -21,   203,   211,    57,  -289,  1226,  -289,   741,
     214,   216,   222,    92,    15,   223,  -289,  1226,  1226,  1226,
     -18,   218,  -289,  2689,   144,  -289,    15,  -289,  -289,   228,
      92,  -289,  2419,  2473,    -8,  1226,  1226,  -289,   230,  -289,
     229,  -289,  -289,  -289,   231,  2527,    -7,  -289,  -289,  1226,
    -289,   239,   786,  2581,  1226,  -289,  2635,  -289
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -289,  -289,  -289,  -289,  -289,  -289,  -235,    -1,  -134,  -122,
    -289,  -289,  -225,  -184,  -125,  -230,  -288,   -53,  -202,  -289,
    -247,  -289,  -289,  -289,  -289,  -289,  -289,   284,  -289,  -289,
       1,   -45,    -2,  -289,  -289,  -127,  -185,  -289,   -24,  -242,
    -229
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -76
static const yytype_int16 yytable[] =
{
      49,    36,   129,   272,   267,   244,   245,    57,   198,   292,
      49,   235,   206,   277,   220,    95,   222,    74,   227,    52,
       5,     6,   243,    61,     3,     5,     6,    62,   227,   348,
     232,   109,   365,   254,   337,   300,    53,    58,   345,   331,
     230,   130,    54,    59,   269,   280,   115,   116,    77,   227,
     227,    96,    20,   332,   333,    21,    63,    23,    24,    55,
     228,    26,    27,    28,   338,    29,    30,   233,   370,    60,
     299,   349,   257,   324,   366,    31,   307,   131,   240,   308,
     309,   331,   311,    32,    33,   315,   316,   258,   339,   303,
     304,   374,   381,   352,    67,   332,   333,     5,     6,    71,
     341,   341,   352,   354,   265,   266,    72,   369,   327,    73,
     328,   329,   203,    75,    76,   271,   183,   184,   359,   341,
     364,    78,   103,   104,   105,   106,    79,   102,   346,   109,
     -75,   341,   112,   132,   136,   359,   137,   376,    53,    49,
     210,   142,   382,   145,   115,   116,   146,    61,   147,     5,
       6,     7,   169,     8,   150,     9,   177,    10,    11,    12,
     199,   200,   211,   368,    13,    14,    15,   201,    16,    17,
     290,    18,   215,   216,   220,   229,    20,   230,   231,    21,
      22,    23,    24,   263,    25,    26,    27,    28,   236,    29,
      30,   237,     4,   238,     5,     6,     7,   239,     8,    31,
       9,   241,    10,    11,    12,    49,    49,    32,    33,    13,
      14,    15,   255,    16,    17,   256,    18,    52,    19,   259,
     273,    20,   270,   274,    21,    22,    23,    24,   278,    25,
      26,    27,    28,   275,    29,    30,    80,   289,   290,   293,
     294,   350,   317,   297,    31,    49,   298,   301,   305,   306,
     340,   347,    32,    33,    81,    82,   320,   321,   323,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,   325,
      49,   210,   351,   326,   356,   357,    49,   367,    93,    49,
      49,   379,    49,   358,   361,    49,    49,   371,   377,   384,
     378,   360,     0,   335,    65,     0,     0,     0,    49,   210,
       0,     0,   103,   104,   105,   106,     0,     0,    49,   109,
      49,    49,   112,    97,    98,    99,   100,   101,     0,     0,
       0,     0,     0,     0,   115,   116,   105,   106,    49,   117,
     118,   109,     0,     0,   112,     0,     0,    49,   210,   135,
       0,     0,     0,     0,   138,     0,   115,   116,     0,     0,
       0,   140,    49,   210,     0,   144,     0,     0,     0,   148,
     149,     0,   151,   152,   153,     0,     0,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,     0,   166,
      49,   210,     0,     0,     0,     0,     0,   171,   172,   173,
     174,   175,   176,     0,   178,   179,   180,   181,   182,     0,
       0,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   197,     0,     0,     0,     0,     0,     0,     0,
     205,   205,    61,     0,     5,     6,     7,   213,     8,     0,
       9,   217,    10,    11,    12,     0,     0,     0,     0,    13,
      14,    15,     0,    16,    17,     0,    18,     0,     0,     0,
       0,    20,     0,     0,    21,    22,    23,    24,     0,    25,
      26,    27,    28,     0,    29,    30,     0,     0,     0,     0,
       0,     0,     0,     0,    31,     0,     0,     0,     0,    68,
     166,     0,    32,    33,     0,     0,     0,   205,     0,     0,
       0,     0,     0,     0,     0,   246,   248,     0,   251,   252,
     253,     0,     0,     0,     0,     0,     0,     0,     0,   260,
       0,   261,   128,     0,   264,   166,   166,   164,   268,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      81,    82,     0,   283,   285,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,     0,   296,     0,     0,     0,
       0,     0,     0,     0,    61,     0,     5,     6,     7,   166,
       8,     0,     9,     0,    10,    11,    12,     0,     0,     0,
     314,    13,    14,    15,   319,    16,    17,     0,    18,     0,
       0,     0,     0,    20,     0,     0,    21,    22,    23,    24,
       0,    25,    26,    27,    28,     0,    29,    30,     0,     0,
       0,     0,     0,     0,     0,     0,    31,     0,     0,     0,
       0,   262,   209,     0,    32,    33,     0,     0,     0,     0,
       0,   353,     0,     0,     0,     0,     0,     0,     0,    81,
      82,   362,   363,   164,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,     0,     0,     0,     0,     0,   375,
     164,     0,     0,     0,    61,     0,     5,     6,     7,     0,
       8,     0,     9,   383,    10,    11,    12,     0,   386,     0,
       0,    13,    14,    15,     0,    16,    17,     0,    18,     0,
       0,     0,     0,    20,     0,     0,    21,    22,    23,    24,
       0,    25,    26,    27,    28,     0,    29,    30,     0,    61,
       0,     5,     6,     7,     0,     8,    31,     9,     0,    10,
      11,    12,   302,     0,    32,    33,    13,    14,    15,     0,
      16,    17,     0,    18,     0,     0,     0,     0,    20,     0,
       0,    21,    22,    23,    24,     0,    25,    26,    27,    28,
       0,    29,    30,     0,    61,     0,     5,     6,     7,     0,
       8,    31,     9,     0,    10,    11,    12,   322,     0,    32,
      33,    13,    14,    15,     0,    16,    17,     0,    18,     0,
       0,     0,     0,    20,     0,     0,    21,    22,    23,    24,
       0,    25,    26,    27,    28,     0,    29,    30,     0,    61,
       0,     5,     6,     7,     0,     8,    31,     9,     0,    10,
      11,    12,   355,     0,    32,    33,    13,    14,    15,     0,
      16,    17,     0,    18,     0,     0,     0,     0,    20,     0,
       0,    21,    22,    23,    24,     0,    25,    26,    27,    28,
       0,    29,    30,    61,     0,     5,     6,    62,     0,     0,
       0,    31,     0,     0,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    20,     0,     0,    21,    63,    23,    24,     0,
       0,    26,    27,    28,     0,    29,    30,    61,     0,     5,
       6,    62,     0,     0,     0,    31,     0,     0,     0,    64,
       0,     0,     0,    32,    33,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    20,     0,     0,    21,
      63,    23,    24,     0,     0,    26,    27,    28,     0,    29,
      30,    61,     0,     5,     6,    62,     0,     0,     0,    31,
       0,     0,     0,   143,     0,     0,     0,    32,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      20,     0,     0,    21,    63,    23,    24,     0,     0,    26,
      27,    28,     0,    29,    30,    61,     0,     5,     6,    62,
       0,     0,     0,    31,     0,     0,     0,     0,   196,     0,
       0,    32,    33,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    20,     0,     0,    21,    63,    23,
      24,     0,     0,    26,    27,    28,     0,    29,    30,    61,
       0,     5,     6,    62,     0,     0,     0,    31,     0,     0,
       0,   212,     0,     0,     0,    32,    33,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    20,     0,
       0,    21,    63,    23,    24,     0,     0,    26,    27,    28,
       0,    29,    30,    61,     0,     5,     6,    62,     0,     0,
       0,    31,   247,     0,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    20,     0,     0,    21,    63,    23,    24,     0,
       0,    26,    27,    28,     0,    29,    30,    61,     0,     5,
       6,    62,     0,     0,     0,    31,     0,     0,     0,   250,
       0,     0,     0,    32,    33,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    20,     0,     0,    21,
      63,    23,    24,     0,     0,    26,    27,    28,     0,    29,
      30,    61,     0,     5,     6,    62,     0,     0,     0,    31,
     282,     0,     0,     0,     0,     0,     0,    32,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      20,     0,     0,    21,    63,    23,    24,     0,     0,    26,
      27,    28,     0,    29,    30,    61,     0,     5,     6,    62,
       0,     0,     0,    31,   284,     0,     0,     0,     0,     0,
       0,    32,    33,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    20,     0,     0,    21,    63,    23,
      24,     0,     0,    26,    27,    28,     0,    29,    30,    61,
       0,     5,     6,    62,     0,     0,     0,    31,   313,     0,
       0,     0,     0,     0,     0,    32,    33,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    20,     0,
       0,    21,    63,    23,    24,     0,     0,    26,    27,    28,
       0,    29,    30,     0,     0,   103,   104,   105,   106,   107,
     108,    31,   109,   110,   111,   112,     0,   223,   224,    32,
      33,     0,     0,   113,     0,     0,   114,   115,   116,     0,
       0,     0,   117,   118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   119,   120,   121,   122,   123,
     124,   125,     0,     0,     0,     0,     0,   126,   127,   103,
     104,   105,   106,   107,   108,     0,   109,   110,   111,   112,
       0,     0,   225,     0,   226,     0,     0,   113,     0,     0,
     114,   115,   116,     0,     0,     0,   117,   118,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   119,
     120,   121,   122,   123,   124,   125,     0,     0,     0,     0,
       0,   126,   127,   103,   104,   105,   106,   107,   108,     0,
     109,   110,   111,   112,     0,     0,     0,     0,   139,     0,
       0,   113,     0,     0,   114,   115,   116,     0,     0,     0,
     117,   118,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   119,   120,   121,   122,   123,   124,   125,
       0,     0,     0,     0,     0,   126,   127,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,   112,     0,   170,
       0,     0,     0,     0,     0,   113,     0,     0,   114,   115,
     116,     0,     0,     0,   117,   118,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   119,   120,   121,
     122,   123,   124,   125,     0,     0,     0,     0,     0,   126,
     127,   103,   104,   105,   106,   107,   108,     0,   109,   110,
     111,   112,     0,   202,     0,     0,     0,     0,     0,   113,
       0,     0,   114,   115,   116,     0,     0,     0,   117,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   119,   120,   121,   122,   123,   124,   125,     0,     0,
       0,     0,     0,   126,   127,   103,   104,   105,   106,   107,
     108,     0,   109,   110,   111,   112,     0,   207,     0,     0,
       0,     0,     0,   113,     0,     0,   114,   115,   116,     0,
       0,     0,   117,   118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   119,   120,   121,   122,   123,
     124,   125,     0,     0,     0,     0,     0,   126,   127,   103,
     104,   105,   106,   107,   108,     0,   109,   110,   111,   112,
       0,   208,     0,     0,     0,     0,     0,   113,     0,     0,
     114,   115,   116,     0,     0,     0,   117,   118,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   119,
     120,   121,   122,   123,   124,   125,     0,     0,     0,     0,
       0,   126,   127,   103,   104,   105,   106,   107,   108,     0,
     109,   110,   111,   112,     0,     0,     0,     0,   214,     0,
       0,   113,     0,     0,   114,   115,   116,     0,     0,     0,
     117,   118,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   119,   120,   121,   122,   123,   124,   125,
       0,     0,     0,     0,     0,   126,   127,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,   112,     0,   218,
       0,     0,     0,     0,     0,   113,     0,     0,   114,   115,
     116,     0,     0,     0,   117,   118,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   119,   120,   121,
     122,   123,   124,   125,     0,     0,     0,     0,     0,   126,
     127,   103,   104,   105,   106,   107,   108,     0,   109,   110,
     111,   112,     0,   219,     0,     0,     0,     0,     0,   113,
       0,     0,   114,   115,   116,     0,     0,     0,   117,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   119,   120,   121,   122,   123,   124,   125,     0,     0,
       0,     0,     0,   126,   127,   103,   104,   105,   106,   107,
     108,     0,   109,   110,   111,   112,     0,     0,     0,   234,
       0,     0,     0,   113,     0,     0,   114,   115,   116,     0,
       0,     0,   117,   118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   119,   120,   121,   122,   123,
     124,   125,     0,     0,     0,     0,     0,   126,   127,   103,
     104,   105,   106,   107,   108,     0,   109,   110,   111,   112,
       0,     0,     0,     0,   249,     0,     0,   113,     0,     0,
     114,   115,   116,     0,     0,     0,   117,   118,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   119,
     120,   121,   122,   123,   124,   125,     0,     0,     0,     0,
       0,   126,   127,   103,   104,   105,   106,   107,   108,     0,
     109,   110,   111,   112,     0,     0,     0,   241,     0,     0,
       0,   113,     0,     0,   114,   115,   116,     0,     0,     0,
     117,   118,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   119,   120,   121,   122,   123,   124,   125,
       0,     0,     0,     0,     0,   126,   127,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,   112,     0,   279,
       0,     0,     0,     0,     0,   113,     0,     0,   114,   115,
     116,     0,     0,     0,   117,   118,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   119,   120,   121,
     122,   123,   124,   125,     0,     0,     0,     0,     0,   126,
     127,   103,   104,   105,   106,   107,   108,     0,   109,   110,
     111,   112,     0,   281,     0,     0,     0,     0,     0,   113,
       0,     0,   114,   115,   116,     0,     0,     0,   117,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   119,   120,   121,   122,   123,   124,   125,     0,     0,
       0,     0,     0,   126,   127,   103,   104,   105,   106,   107,
     108,     0,   109,   110,   111,   112,     0,     0,     0,     0,
     286,     0,     0,   113,     0,     0,   114,   115,   116,     0,
       0,     0,   117,   118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   119,   120,   121,   122,   123,
     124,   125,     0,     0,     0,     0,     0,   126,   127,   103,
     104,   105,   106,   107,   108,     0,   109,   110,   111,   112,
       0,   287,     0,     0,     0,     0,     0,   113,     0,     0,
     114,   115,   116,     0,     0,     0,   117,   118,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   119,
     120,   121,   122,   123,   124,   125,     0,     0,     0,     0,
       0,   126,   127,   103,   104,   105,   106,   107,   108,     0,
     109,   110,   111,   112,     0,   288,     0,     0,     0,     0,
       0,   113,     0,     0,   114,   115,   116,     0,     0,     0,
     117,   118,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   119,   120,   121,   122,   123,   124,   125,
       0,     0,     0,     0,     0,   126,   127,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,   112,     0,     0,
       0,     0,   295,     0,     0,   113,     0,     0,   114,   115,
     116,     0,     0,     0,   117,   118,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   119,   120,   121,
     122,   123,   124,   125,     0,     0,     0,     0,     0,   126,
     127,   103,   104,   105,   106,   107,   108,     0,   109,   110,
     111,   112,     0,   310,     0,     0,     0,     0,     0,   113,
       0,     0,   114,   115,   116,     0,     0,     0,   117,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   119,   120,   121,   122,   123,   124,   125,     0,     0,
       0,     0,     0,   126,   127,   103,   104,   105,   106,   107,
     108,     0,   109,   110,   111,   112,     0,   312,     0,     0,
       0,     0,     0,   113,     0,     0,   114,   115,   116,     0,
       0,     0,   117,   118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   119,   120,   121,   122,   123,
     124,   125,     0,     0,     0,     0,     0,   126,   127,   103,
     104,   105,   106,   107,   108,     0,   109,   110,   111,   112,
       0,   330,     0,     0,     0,     0,     0,   113,     0,     0,
     114,   115,   116,     0,     0,     0,   117,   118,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   119,
     120,   121,   122,   123,   124,   125,     0,     0,     0,     0,
       0,   126,   127,   103,   104,   105,   106,   107,   108,     0,
     109,   110,   111,   112,     0,     0,     0,     0,   372,     0,
       0,   113,     0,     0,   114,   115,   116,     0,     0,     0,
     117,   118,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   119,   120,   121,   122,   123,   124,   125,
       0,     0,     0,     0,     0,   126,   127,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,   112,     0,     0,
       0,     0,   373,     0,     0,   113,     0,     0,   114,   115,
     116,     0,     0,     0,   117,   118,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   119,   120,   121,
     122,   123,   124,   125,     0,     0,     0,     0,     0,   126,
     127,   103,   104,   105,   106,   107,   108,     0,   109,   110,
     111,   112,     0,     0,     0,     0,   380,     0,     0,   113,
       0,     0,   114,   115,   116,     0,     0,     0,   117,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   119,   120,   121,   122,   123,   124,   125,     0,     0,
       0,     0,     0,   126,   127,   103,   104,   105,   106,   107,
     108,     0,   109,   110,   111,   112,     0,     0,     0,     0,
     385,     0,     0,   113,     0,     0,   114,   115,   116,     0,
       0,     0,   117,   118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   119,   120,   121,   122,   123,
     124,   125,     0,     0,     0,     0,     0,   126,   127,   103,
     104,   105,   106,   107,   108,     0,   109,   110,   111,   112,
       0,     0,     0,     0,   387,     0,     0,   113,     0,     0,
     114,   115,   116,     0,     0,     0,   117,   118,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   119,
     120,   121,   122,   123,   124,   125,     0,     0,     0,     0,
       0,   126,   127,   103,   104,   105,   106,   107,   108,     0,
     109,   110,   111,   112,     0,     0,     0,     0,     0,     0,
       0,   113,     0,     0,   114,   115,   116,     0,     0,     0,
     117,   118,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   119,   120,   121,   122,   123,   124,   125,
       0,     0,     0,     0,     0,   126,   127,   103,   104,   105,
     106,   107,   108,     0,   109,   110,   111,   112,     0,     0,
       0,     0,     0,     0,     0,   113,     0,     0,   114,   115,
     116,     0,     0,     0,   117,   118,   103,   104,   105,   106,
     107,   108,     0,   109,   110,   111,   112,   119,   120,   121,
     122,   123,   124,   125,   113,     0,     0,   114,   115,   116,
     127,     0,     0,   117,   118,   103,   104,   105,   106,   107,
     108,     0,   109,   110,   111,   112,   119,   120,   121,   122,
     123,     0,   125,   113,     0,     0,   114,   115,   116,   127,
       0,     0,   117,   118,   103,   104,   105,   106,   107,   108,
       0,   109,     0,   111,   112,   119,   120,   121,   122,     0,
       0,   125,   113,     0,     0,   114,   115,   116,   127,     0,
       0,   117,   118,   103,   104,   105,   106,   107,   108,     0,
     109,     0,   111,   112,   119,   120,   121,   122,     0,     0,
     125,     0,     0,     0,   114,   115,   116,   127,     0,     0,
     117,   118,   103,   104,   105,   106,   107,   108,     0,   109,
       0,     0,   112,   119,   120,   121,   122,     0,     0,   125,
       0,     0,     0,   114,   115,   116,   127,     0,     0,   117,
     118,   103,   104,   105,   106,   107,   108,     0,   109,     0,
       0,   112,   119,   120,   121,   122,     0,     0,   125,     0,
       0,     0,   114,   115,   116,   127,     0,     0,   117,   118,
     103,   104,   105,   106,     0,     0,     0,   109,     0,     0,
     112,     0,     0,   121,   122,     0,     0,   125,     0,     0,
       0,   114,   115,   116,   127,     0,     0,   117,   118,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   125,     0,     0,     0,
       0,     0,     0,   127
};

static const yytype_int16 yycheck[] =
{
       2,     2,    47,   238,   233,   207,   208,     6,   130,   256,
      12,   196,   137,   243,     3,    55,   150,    16,    57,    38,
       5,     6,   206,     3,     0,     5,     6,     7,    57,    50,
      55,    51,    50,   217,    28,   270,    55,    59,   326,    24,
      57,    55,    38,    59,    61,   247,    66,    67,    38,    57,
      57,    91,    32,    38,    39,    35,    36,    37,    38,    55,
      99,    41,    42,    43,    58,    45,    46,    92,   356,    55,
      99,    92,    61,   303,    92,    55,   278,    91,   203,   281,
     282,    24,   284,    63,    64,   287,   288,   221,   323,   273,
     275,    99,    99,   335,    55,    38,    39,     5,     6,    55,
     325,   326,   344,   338,   231,   232,    55,   354,   310,    55,
     312,   313,    92,    55,    55,   237,   115,   116,   343,   344,
     349,    55,    44,    45,    46,    47,    50,    59,   330,    51,
      59,   356,    54,    37,    55,   360,    55,   366,    55,   141,
     141,    14,   377,    37,    66,    67,    37,     3,    55,     5,
       6,     7,    38,     9,    60,    11,    38,    13,    14,    15,
      38,    56,    55,    19,    20,    21,    22,    57,    24,    25,
      26,    27,    23,    23,     3,    56,    32,    57,    55,    35,
      36,    37,    38,   228,    40,    41,    42,    43,    56,    45,
      46,    55,     3,    60,     5,     6,     7,    37,     9,    55,
      11,    58,    13,    14,    15,   207,   208,    63,    64,    20,
      21,    22,    60,    24,    25,    60,    27,    38,    29,    41,
      99,    32,    60,    38,    35,    36,    37,    38,     8,    40,
      41,    42,    43,    57,    45,    46,    50,    56,    26,    59,
      56,    38,    60,    56,    55,   247,    56,    56,    56,    56,
      56,    50,    63,    64,    68,    69,    61,    61,    60,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    60,
     272,   272,    61,    60,    60,    59,   278,    59,    92,   281,
     282,    50,   284,    61,    61,   287,   288,    59,    58,    50,
      61,   344,    -1,   317,    10,    -1,    -1,    -1,   300,   300,
      -1,    -1,    44,    45,    46,    47,    -1,    -1,   310,    51,
     312,   313,    54,    29,    30,    31,    32,    33,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    67,    46,    47,   330,    71,
      72,    51,    -1,    -1,    54,    -1,    -1,   339,   339,    55,
      -1,    -1,    -1,    -1,    60,    -1,    66,    67,    -1,    -1,
      -1,    67,   354,   354,    -1,    71,    -1,    -1,    -1,    75,
      76,    -1,    78,    79,    80,    -1,    -1,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    95,
     382,   382,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,   107,   108,    -1,   110,   111,   112,   113,   114,    -1,
      -1,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     136,   137,     3,    -1,     5,     6,     7,   143,     9,    -1,
      11,   147,    13,    14,    15,    -1,    -1,    -1,    -1,    20,
      21,    22,    -1,    24,    25,    -1,    27,    -1,    -1,    -1,
      -1,    32,    -1,    -1,    35,    36,    37,    38,    -1,    40,
      41,    42,    43,    -1,    45,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,    60,
     196,    -1,    63,    64,    -1,    -1,    -1,   203,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   211,   212,    -1,   214,   215,
     216,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   225,
      -1,   227,    50,    -1,   230,   231,   232,   233,   234,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    69,    -1,   249,   250,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    -1,   262,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,    -1,     5,     6,     7,   275,
       9,    -1,    11,    -1,    13,    14,    15,    -1,    -1,    -1,
     286,    20,    21,    22,   290,    24,    25,    -1,    27,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    35,    36,    37,    38,
      -1,    40,    41,    42,    43,    -1,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,
      -1,    50,    61,    -1,    63,    64,    -1,    -1,    -1,    -1,
      -1,   337,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,
      69,   347,   348,   349,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    -1,    -1,    -1,    -1,    -1,   365,
     366,    -1,    -1,    -1,     3,    -1,     5,     6,     7,    -1,
       9,    -1,    11,   379,    13,    14,    15,    -1,   384,    -1,
      -1,    20,    21,    22,    -1,    24,    25,    -1,    27,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    35,    36,    37,    38,
      -1,    40,    41,    42,    43,    -1,    45,    46,    -1,     3,
      -1,     5,     6,     7,    -1,     9,    55,    11,    -1,    13,
      14,    15,    61,    -1,    63,    64,    20,    21,    22,    -1,
      24,    25,    -1,    27,    -1,    -1,    -1,    -1,    32,    -1,
      -1,    35,    36,    37,    38,    -1,    40,    41,    42,    43,
      -1,    45,    46,    -1,     3,    -1,     5,     6,     7,    -1,
       9,    55,    11,    -1,    13,    14,    15,    61,    -1,    63,
      64,    20,    21,    22,    -1,    24,    25,    -1,    27,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    35,    36,    37,    38,
      -1,    40,    41,    42,    43,    -1,    45,    46,    -1,     3,
      -1,     5,     6,     7,    -1,     9,    55,    11,    -1,    13,
      14,    15,    61,    -1,    63,    64,    20,    21,    22,    -1,
      24,    25,    -1,    27,    -1,    -1,    -1,    -1,    32,    -1,
      -1,    35,    36,    37,    38,    -1,    40,    41,    42,    43,
      -1,    45,    46,     3,    -1,     5,     6,     7,    -1,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    38,    -1,
      -1,    41,    42,    43,    -1,    45,    46,     3,    -1,     5,
       6,     7,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,
      -1,    -1,    -1,    63,    64,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    35,
      36,    37,    38,    -1,    -1,    41,    42,    43,    -1,    45,
      46,     3,    -1,     5,     6,     7,    -1,    -1,    -1,    55,
      -1,    -1,    -1,    59,    -1,    -1,    -1,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      32,    -1,    -1,    35,    36,    37,    38,    -1,    -1,    41,
      42,    43,    -1,    45,    46,     3,    -1,     5,     6,     7,
      -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,    60,    -1,
      -1,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    -1,    -1,    35,    36,    37,
      38,    -1,    -1,    41,    42,    43,    -1,    45,    46,     3,
      -1,     5,     6,     7,    -1,    -1,    -1,    55,    -1,    -1,
      -1,    59,    -1,    -1,    -1,    63,    64,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,
      -1,    35,    36,    37,    38,    -1,    -1,    41,    42,    43,
      -1,    45,    46,     3,    -1,     5,     6,     7,    -1,    -1,
      -1,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    38,    -1,
      -1,    41,    42,    43,    -1,    45,    46,     3,    -1,     5,
       6,     7,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,
      -1,    -1,    -1,    63,    64,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    35,
      36,    37,    38,    -1,    -1,    41,    42,    43,    -1,    45,
      46,     3,    -1,     5,     6,     7,    -1,    -1,    -1,    55,
      56,    -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      32,    -1,    -1,    35,    36,    37,    38,    -1,    -1,    41,
      42,    43,    -1,    45,    46,     3,    -1,     5,     6,     7,
      -1,    -1,    -1,    55,    56,    -1,    -1,    -1,    -1,    -1,
      -1,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    -1,    -1,    35,    36,    37,
      38,    -1,    -1,    41,    42,    43,    -1,    45,    46,     3,
      -1,     5,     6,     7,    -1,    -1,    -1,    55,    56,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    64,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,
      -1,    35,    36,    37,    38,    -1,    -1,    41,    42,    43,
      -1,    45,    46,    -1,    -1,    44,    45,    46,    47,    48,
      49,    55,    51,    52,    53,    54,    -1,    56,    57,    63,
      64,    -1,    -1,    62,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,
      89,    90,    -1,    -1,    -1,    -1,    -1,    96,    97,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      -1,    -1,    57,    -1,    59,    -1,    -1,    62,    -1,    -1,
      65,    66,    67,    -1,    -1,    -1,    71,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    96,    97,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    -1,    -1,    -1,    -1,    59,    -1,
      -1,    62,    -1,    -1,    65,    66,    67,    -1,    -1,    -1,
      71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    90,
      -1,    -1,    -1,    -1,    -1,    96,    97,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,    65,    66,
      67,    -1,    -1,    -1,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
      87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,    96,
      97,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,
      -1,    -1,    65,    66,    67,    -1,    -1,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    90,    -1,    -1,
      -1,    -1,    -1,    96,    97,    44,    45,    46,    47,    48,
      49,    -1,    51,    52,    53,    54,    -1,    56,    -1,    -1,
      -1,    -1,    -1,    62,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,
      89,    90,    -1,    -1,    -1,    -1,    -1,    96,    97,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      -1,    56,    -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,
      65,    66,    67,    -1,    -1,    -1,    71,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    96,    97,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    -1,    -1,    -1,    -1,    59,    -1,
      -1,    62,    -1,    -1,    65,    66,    67,    -1,    -1,    -1,
      71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    90,
      -1,    -1,    -1,    -1,    -1,    96,    97,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,    65,    66,
      67,    -1,    -1,    -1,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
      87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,    96,
      97,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,
      -1,    -1,    65,    66,    67,    -1,    -1,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    90,    -1,    -1,
      -1,    -1,    -1,    96,    97,    44,    45,    46,    47,    48,
      49,    -1,    51,    52,    53,    54,    -1,    -1,    -1,    58,
      -1,    -1,    -1,    62,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,
      89,    90,    -1,    -1,    -1,    -1,    -1,    96,    97,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    -1,
      65,    66,    67,    -1,    -1,    -1,    71,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    96,    97,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    -1,    -1,    -1,    58,    -1,    -1,
      -1,    62,    -1,    -1,    65,    66,    67,    -1,    -1,    -1,
      71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    90,
      -1,    -1,    -1,    -1,    -1,    96,    97,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,    65,    66,
      67,    -1,    -1,    -1,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
      87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,    96,
      97,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,
      -1,    -1,    65,    66,    67,    -1,    -1,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    90,    -1,    -1,
      -1,    -1,    -1,    96,    97,    44,    45,    46,    47,    48,
      49,    -1,    51,    52,    53,    54,    -1,    -1,    -1,    -1,
      59,    -1,    -1,    62,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,
      89,    90,    -1,    -1,    -1,    -1,    -1,    96,    97,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      -1,    56,    -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,
      65,    66,    67,    -1,    -1,    -1,    71,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    96,    97,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    -1,    56,    -1,    -1,    -1,    -1,
      -1,    62,    -1,    -1,    65,    66,    67,    -1,    -1,    -1,
      71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    90,
      -1,    -1,    -1,    -1,    -1,    96,    97,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    -1,    -1,
      -1,    -1,    59,    -1,    -1,    62,    -1,    -1,    65,    66,
      67,    -1,    -1,    -1,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
      87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,    96,
      97,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,
      -1,    -1,    65,    66,    67,    -1,    -1,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    90,    -1,    -1,
      -1,    -1,    -1,    96,    97,    44,    45,    46,    47,    48,
      49,    -1,    51,    52,    53,    54,    -1,    56,    -1,    -1,
      -1,    -1,    -1,    62,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,
      89,    90,    -1,    -1,    -1,    -1,    -1,    96,    97,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      -1,    56,    -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,
      65,    66,    67,    -1,    -1,    -1,    71,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    96,    97,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    -1,    -1,    -1,    -1,    59,    -1,
      -1,    62,    -1,    -1,    65,    66,    67,    -1,    -1,    -1,
      71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    90,
      -1,    -1,    -1,    -1,    -1,    96,    97,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    -1,    -1,
      -1,    -1,    59,    -1,    -1,    62,    -1,    -1,    65,    66,
      67,    -1,    -1,    -1,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
      87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,    96,
      97,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    -1,    -1,    -1,    -1,    59,    -1,    -1,    62,
      -1,    -1,    65,    66,    67,    -1,    -1,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    90,    -1,    -1,
      -1,    -1,    -1,    96,    97,    44,    45,    46,    47,    48,
      49,    -1,    51,    52,    53,    54,    -1,    -1,    -1,    -1,
      59,    -1,    -1,    62,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,
      89,    90,    -1,    -1,    -1,    -1,    -1,    96,    97,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    -1,
      65,    66,    67,    -1,    -1,    -1,    71,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    96,    97,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    62,    -1,    -1,    65,    66,    67,    -1,    -1,    -1,
      71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    90,
      -1,    -1,    -1,    -1,    -1,    96,    97,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,    65,    66,
      67,    -1,    -1,    -1,    71,    72,    44,    45,    46,    47,
      48,    49,    -1,    51,    52,    53,    54,    84,    85,    86,
      87,    88,    89,    90,    62,    -1,    -1,    65,    66,    67,
      97,    -1,    -1,    71,    72,    44,    45,    46,    47,    48,
      49,    -1,    51,    52,    53,    54,    84,    85,    86,    87,
      88,    -1,    90,    62,    -1,    -1,    65,    66,    67,    97,
      -1,    -1,    71,    72,    44,    45,    46,    47,    48,    49,
      -1,    51,    -1,    53,    54,    84,    85,    86,    87,    -1,
      -1,    90,    62,    -1,    -1,    65,    66,    67,    97,    -1,
      -1,    71,    72,    44,    45,    46,    47,    48,    49,    -1,
      51,    -1,    53,    54,    84,    85,    86,    87,    -1,    -1,
      90,    -1,    -1,    -1,    65,    66,    67,    97,    -1,    -1,
      71,    72,    44,    45,    46,    47,    48,    49,    -1,    51,
      -1,    -1,    54,    84,    85,    86,    87,    -1,    -1,    90,
      -1,    -1,    -1,    65,    66,    67,    97,    -1,    -1,    71,
      72,    44,    45,    46,    47,    48,    49,    -1,    51,    -1,
      -1,    54,    84,    85,    86,    87,    -1,    -1,    90,    -1,
      -1,    -1,    65,    66,    67,    97,    -1,    -1,    71,    72,
      44,    45,    46,    47,    -1,    -1,    -1,    51,    -1,    -1,
      54,    -1,    -1,    86,    87,    -1,    -1,    90,    -1,    -1,
      -1,    65,    66,    67,    97,    -1,    -1,    71,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,
      -1,    -1,    -1,    97
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   101,   102,     0,     3,     5,     6,     7,     9,    11,
      13,    14,    15,    20,    21,    22,    24,    25,    27,    29,
      32,    35,    36,    37,    38,    40,    41,    42,    43,    45,
      46,    55,    63,    64,   103,   104,   107,   108,   111,   112,
     119,   122,   123,   124,   125,   126,   127,   128,   129,   132,
     133,   134,    38,    55,    38,    55,   130,   130,    59,    59,
      55,     3,     7,    36,    59,   127,   132,    55,    60,   107,
     118,    55,    55,    55,   130,    55,    55,    38,    55,    50,
      50,    68,    69,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    92,   131,    55,    91,   127,   127,   127,
     127,   127,    59,    44,    45,    46,    47,    48,    49,    51,
      52,    53,    54,    62,    65,    66,    67,    71,    72,    84,
      85,    86,    87,    88,    89,    90,    96,    97,    50,   131,
      55,    91,    37,   109,   110,   127,    55,    55,   127,    59,
     127,   106,    14,    59,   127,    37,    37,    55,   127,   127,
      60,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   140,   127,   135,   136,    38,
      56,   127,   127,   127,   127,   127,   127,    38,   127,   127,
     127,   127,   127,   130,   130,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,    60,   127,   109,    38,
      56,    57,    56,    92,   114,   127,   114,    56,    56,    61,
     107,    55,    59,   127,    59,    23,    23,   127,    56,    56,
       3,   105,   108,    56,    57,    57,    59,    57,    99,    56,
      57,    55,    55,    92,    58,   136,    56,    55,    60,    37,
     114,    58,   113,   113,   118,   118,   127,    56,   127,    59,
      59,   127,   127,   127,   113,    60,    60,    61,   108,    41,
     127,   127,    50,   131,   127,   135,   135,   140,   127,    61,
      60,   109,   106,    99,    38,    57,   115,   115,     8,    56,
     118,    56,    56,   127,    56,   127,    59,    56,    56,    56,
      26,   120,   120,    59,    56,    59,   127,    56,    56,    99,
     106,    56,    61,   113,   136,    56,    56,   118,   118,   118,
      56,   118,    56,    56,   127,   118,   118,    60,   121,   127,
      61,    61,    61,    60,   115,    60,    60,   118,   118,   118,
      56,    24,    38,    39,   137,   138,   139,    28,    58,   106,
      56,   112,   116,   117,   138,   116,   118,    50,    50,    92,
      38,    61,   139,   127,   106,    61,    60,    59,    61,   112,
     117,    61,   127,   127,   140,    50,    92,    59,    19,   120,
     116,    59,    59,    59,    99,   127,   140,    58,    61,    50,
      59,    99,   106,   127,    50,    59,   127,    59
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 162 "CMDgram.y"
    { ;}
    break;

  case 3:
#line 167 "CMDgram.y"
    { (yyval.stmt) = nil; ;}
    break;

  case 4:
#line 169 "CMDgram.y"
    { if(!gStatementList) { gStatementList = (yyvsp[(2) - (2)].stmt); } else { gStatementList->append((yyvsp[(2) - (2)].stmt)); } ;}
    break;

  case 5:
#line 174 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 6:
#line 176 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 7:
#line 178 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 8:
#line 183 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(4) - (6)].stmt); for(StmtNode *walk = ((yyvsp[(4) - (6)].stmt));walk;walk = walk->getNext() ) walk->setPackage((yyvsp[(2) - (6)].s).value); ;}
    break;

  case 9:
#line 188 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 10:
#line 190 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(1) - (2)].stmt); ((yyvsp[(1) - (2)].stmt))->append((yyvsp[(2) - (2)].stmt));  ;}
    break;

  case 11:
#line 195 "CMDgram.y"
    { (yyval.stmt) = nil; ;}
    break;

  case 12:
#line 197 "CMDgram.y"
    { if(!(yyvsp[(1) - (2)].stmt)) { (yyval.stmt) = (yyvsp[(2) - (2)].stmt); } else { ((yyvsp[(1) - (2)].stmt))->append((yyvsp[(2) - (2)].stmt)); (yyval.stmt) = (yyvsp[(1) - (2)].stmt); } ;}
    break;

  case 19:
#line 208 "CMDgram.y"
    { (yyval.stmt) = BreakStmtNode::alloc( (yyvsp[(1) - (2)].i).lineNumber ); ;}
    break;

  case 20:
#line 210 "CMDgram.y"
    { (yyval.stmt) = ContinueStmtNode::alloc( (yyvsp[(1) - (2)].i).lineNumber ); ;}
    break;

  case 21:
#line 212 "CMDgram.y"
    { (yyval.stmt) = ReturnStmtNode::alloc( (yyvsp[(1) - (2)].i).lineNumber, NULL ); ;}
    break;

  case 22:
#line 214 "CMDgram.y"
    { (yyval.stmt) = ReturnStmtNode::alloc( (yyvsp[(1) - (3)].i).lineNumber, (yyvsp[(2) - (3)].expr) ); ;}
    break;

  case 23:
#line 216 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(1) - (2)].stmt); ;}
    break;

  case 24:
#line 218 "CMDgram.y"
    { (yyval.stmt) = TTagSetStmtNode::alloc( (yyvsp[(1) - (4)].s).lineNumber, (yyvsp[(1) - (4)].s).value, (yyvsp[(3) - (4)].expr), NULL ); ;}
    break;

  case 25:
#line 220 "CMDgram.y"
    { (yyval.stmt) = TTagSetStmtNode::alloc( (yyvsp[(1) - (6)].s).lineNumber, (yyvsp[(1) - (6)].s).value, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr) ); ;}
    break;

  case 26:
#line 222 "CMDgram.y"
    { (yyval.stmt) = StrConstNode::alloc( (yyvsp[(1) - (1)].str).lineNumber, (yyvsp[(1) - (1)].str).value, false, true ); ;}
    break;

  case 27:
#line 227 "CMDgram.y"
    { (yyval.stmt) = FunctionDeclStmtNode::alloc( (yyvsp[(1) - (8)].i).lineNumber, (yyvsp[(2) - (8)].s).value, NULL, (yyvsp[(4) - (8)].var), (yyvsp[(7) - (8)].stmt) ); ;}
    break;

  case 28:
#line 229 "CMDgram.y"
    { (yyval.stmt) = FunctionDeclStmtNode::alloc( (yyvsp[(1) - (10)].i).lineNumber, (yyvsp[(4) - (10)].s).value, (yyvsp[(2) - (10)].s).value, (yyvsp[(6) - (10)].var), (yyvsp[(9) - (10)].stmt) ); ;}
    break;

  case 29:
#line 234 "CMDgram.y"
    { (yyval.var) = NULL; ;}
    break;

  case 30:
#line 236 "CMDgram.y"
    { (yyval.var) = (yyvsp[(1) - (1)].var); ;}
    break;

  case 31:
#line 241 "CMDgram.y"
    { (yyval.var) = VarNode::alloc( (yyvsp[(1) - (1)].s).lineNumber, (yyvsp[(1) - (1)].s).value, NULL ); ;}
    break;

  case 32:
#line 243 "CMDgram.y"
    { (yyval.var) = (yyvsp[(1) - (3)].var); ((StmtNode*)((yyvsp[(1) - (3)].var)))->append((StmtNode*)VarNode::alloc( (yyvsp[(3) - (3)].s).lineNumber, (yyvsp[(3) - (3)].s).value, NULL ) ); ;}
    break;

  case 33:
#line 248 "CMDgram.y"
    { (yyval.stmt) = ObjectDeclNode::alloc( (yyvsp[(1) - (10)].i).lineNumber, (yyvsp[(2) - (10)].expr), (yyvsp[(4) - (10)].expr), NULL, (yyvsp[(5) - (10)].s).value, (yyvsp[(8) - (10)].slist), NULL, true, false, false); ;}
    break;

  case 34:
#line 253 "CMDgram.y"
    { (yyval.od) = ObjectDeclNode::alloc( (yyvsp[(1) - (10)].i).lineNumber, (yyvsp[(2) - (10)].expr), (yyvsp[(4) - (10)].expr), (yyvsp[(6) - (10)].expr), (yyvsp[(5) - (10)].s).value, (yyvsp[(9) - (10)].odcl).slots, (yyvsp[(9) - (10)].odcl).decls, false, false, false); ;}
    break;

  case 35:
#line 255 "CMDgram.y"
    { (yyval.od) = ObjectDeclNode::alloc( (yyvsp[(1) - (7)].i).lineNumber, (yyvsp[(2) - (7)].expr), (yyvsp[(4) - (7)].expr), (yyvsp[(6) - (7)].expr), (yyvsp[(5) - (7)].s).value, NULL, NULL, false, false, false); ;}
    break;

  case 36:
#line 257 "CMDgram.y"
    { (yyval.od) = ObjectDeclNode::alloc( (yyvsp[(1) - (12)].i).lineNumber, (yyvsp[(2) - (12)].expr), (yyvsp[(5) - (12)].expr), (yyvsp[(8) - (12)].expr), (yyvsp[(7) - (12)].s).value, (yyvsp[(11) - (12)].odcl).slots, (yyvsp[(11) - (12)].odcl).decls, false, true, false); ;}
    break;

  case 37:
#line 259 "CMDgram.y"
    { (yyval.od) = ObjectDeclNode::alloc( (yyvsp[(1) - (9)].i).lineNumber, (yyvsp[(2) - (9)].expr), (yyvsp[(5) - (9)].expr), (yyvsp[(8) - (9)].expr), (yyvsp[(7) - (9)].s).value, NULL, NULL, false, true, false); ;}
    break;

  case 38:
#line 261 "CMDgram.y"
    { (yyval.od) = ObjectDeclNode::alloc( (yyvsp[(1) - (10)].i).lineNumber, (yyvsp[(2) - (10)].expr), (yyvsp[(4) - (10)].expr), (yyvsp[(6) - (10)].expr), (yyvsp[(5) - (10)].s).value, (yyvsp[(9) - (10)].odcl).slots, (yyvsp[(9) - (10)].odcl).decls, false, false, true); ;}
    break;

  case 39:
#line 263 "CMDgram.y"
    { (yyval.od) = ObjectDeclNode::alloc( (yyvsp[(1) - (7)].i).lineNumber, (yyvsp[(2) - (7)].expr), (yyvsp[(4) - (7)].expr), (yyvsp[(6) - (7)].expr), (yyvsp[(5) - (7)].s).value, NULL, NULL, false, false, true); ;}
    break;

  case 40:
#line 268 "CMDgram.y"
    { (yyval.s).value = NULL; ;}
    break;

  case 41:
#line 270 "CMDgram.y"
    { (yyval.s) = (yyvsp[(2) - (2)].s); ;}
    break;

  case 42:
#line 275 "CMDgram.y"
    { (yyval.expr) = StrConstNode::alloc( CodeBlock::smCurrentParser->getCurrentLine(), "", false); ;}
    break;

  case 43:
#line 277 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 44:
#line 282 "CMDgram.y"
    { (yyval.expr) = NULL; ;}
    break;

  case 45:
#line 284 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 46:
#line 289 "CMDgram.y"
    { (yyval.odcl).slots = NULL; (yyval.odcl).decls = NULL; ;}
    break;

  case 47:
#line 291 "CMDgram.y"
    { (yyval.odcl).slots = (yyvsp[(1) - (1)].slist); (yyval.odcl).decls = NULL; ;}
    break;

  case 48:
#line 293 "CMDgram.y"
    { (yyval.odcl).slots = NULL; (yyval.odcl).decls = (yyvsp[(1) - (1)].od); ;}
    break;

  case 49:
#line 295 "CMDgram.y"
    { (yyval.odcl).slots = (yyvsp[(1) - (2)].slist); (yyval.odcl).decls = (yyvsp[(2) - (2)].od); ;}
    break;

  case 50:
#line 300 "CMDgram.y"
    { (yyval.od) = (yyvsp[(1) - (2)].od); ;}
    break;

  case 51:
#line 302 "CMDgram.y"
    { (yyvsp[(1) - (3)].od)->append((yyvsp[(2) - (3)].od)); (yyval.od) = (yyvsp[(1) - (3)].od); ;}
    break;

  case 52:
#line 307 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(2) - (3)].stmt); ;}
    break;

  case 53:
#line 309 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 54:
#line 314 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(6) - (7)].ifnode); (yyvsp[(6) - (7)].ifnode)->propagateSwitchExpr((yyvsp[(3) - (7)].expr), false); ;}
    break;

  case 55:
#line 316 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(6) - (7)].ifnode); (yyvsp[(6) - (7)].ifnode)->propagateSwitchExpr((yyvsp[(3) - (7)].expr), true); ;}
    break;

  case 56:
#line 321 "CMDgram.y"
    { (yyval.ifnode) = IfStmtNode::alloc( (yyvsp[(1) - (4)].i).lineNumber, (yyvsp[(2) - (4)].expr), (yyvsp[(4) - (4)].stmt), NULL, false); ;}
    break;

  case 57:
#line 323 "CMDgram.y"
    { (yyval.ifnode) = IfStmtNode::alloc( (yyvsp[(1) - (7)].i).lineNumber, (yyvsp[(2) - (7)].expr), (yyvsp[(4) - (7)].stmt), (yyvsp[(7) - (7)].stmt), false); ;}
    break;

  case 58:
#line 325 "CMDgram.y"
    { (yyval.ifnode) = IfStmtNode::alloc( (yyvsp[(1) - (5)].i).lineNumber, (yyvsp[(2) - (5)].expr), (yyvsp[(4) - (5)].stmt), (yyvsp[(5) - (5)].ifnode), true); ;}
    break;

  case 59:
#line 330 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr);;}
    break;

  case 60:
#line 332 "CMDgram.y"
    { ((yyvsp[(1) - (3)].expr))->append((yyvsp[(3) - (3)].expr)); (yyval.expr)=(yyvsp[(1) - (3)].expr); ;}
    break;

  case 61:
#line 337 "CMDgram.y"
    { (yyval.stmt) = IfStmtNode::alloc((yyvsp[(1) - (5)].i).lineNumber, (yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].stmt), NULL, false); ;}
    break;

  case 62:
#line 339 "CMDgram.y"
    { (yyval.stmt) = IfStmtNode::alloc((yyvsp[(1) - (7)].i).lineNumber, (yyvsp[(3) - (7)].expr), (yyvsp[(5) - (7)].stmt), (yyvsp[(7) - (7)].stmt), false); ;}
    break;

  case 63:
#line 344 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (5)].i).lineNumber, nil, (yyvsp[(3) - (5)].expr), nil, (yyvsp[(5) - (5)].stmt), false); ;}
    break;

  case 64:
#line 346 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(3) - (6)].i).lineNumber, nil, (yyvsp[(5) - (6)].expr), nil, (yyvsp[(2) - (6)].stmt), true); ;}
    break;

  case 65:
#line 351 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (9)].i).lineNumber, (yyvsp[(3) - (9)].expr), (yyvsp[(5) - (9)].expr), (yyvsp[(7) - (9)].expr), (yyvsp[(9) - (9)].stmt), false); ;}
    break;

  case 66:
#line 353 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (8)].i).lineNumber, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), NULL, (yyvsp[(8) - (8)].stmt), false); ;}
    break;

  case 67:
#line 355 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (8)].i).lineNumber, (yyvsp[(3) - (8)].expr), NULL, (yyvsp[(6) - (8)].expr), (yyvsp[(8) - (8)].stmt), false); ;}
    break;

  case 68:
#line 357 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (7)].i).lineNumber, (yyvsp[(3) - (7)].expr), NULL, NULL, (yyvsp[(7) - (7)].stmt), false); ;}
    break;

  case 69:
#line 359 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (8)].i).lineNumber, NULL, (yyvsp[(4) - (8)].expr), (yyvsp[(6) - (8)].expr), (yyvsp[(8) - (8)].stmt), false); ;}
    break;

  case 70:
#line 361 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (7)].i).lineNumber, NULL, (yyvsp[(4) - (7)].expr), NULL, (yyvsp[(7) - (7)].stmt), false); ;}
    break;

  case 71:
#line 363 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (7)].i).lineNumber, NULL, NULL, (yyvsp[(5) - (7)].expr), (yyvsp[(7) - (7)].stmt), false); ;}
    break;

  case 72:
#line 365 "CMDgram.y"
    { (yyval.stmt) = LoopStmtNode::alloc((yyvsp[(1) - (6)].i).lineNumber, NULL, NULL, NULL, (yyvsp[(6) - (6)].stmt), false); ;}
    break;

  case 73:
#line 370 "CMDgram.y"
    { (yyval.stmt) = IterStmtNode::alloc( (yyvsp[(1) - (7)].i).lineNumber, (yyvsp[(3) - (7)].s).value, (yyvsp[(5) - (7)].expr), (yyvsp[(7) - (7)].stmt), false ); ;}
    break;

  case 74:
#line 372 "CMDgram.y"
    { (yyval.stmt) = IterStmtNode::alloc( (yyvsp[(1) - (7)].i).lineNumber, (yyvsp[(3) - (7)].s).value, (yyvsp[(5) - (7)].expr), (yyvsp[(7) - (7)].stmt), true ); ;}
    break;

  case 75:
#line 377 "CMDgram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 76:
#line 382 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 77:
#line 384 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;

  case 78:
#line 386 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 79:
#line 388 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 80:
#line 390 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 81:
#line 392 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 82:
#line 394 "CMDgram.y"
    { (yyval.expr) = FloatBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 83:
#line 396 "CMDgram.y"
    { (yyval.expr) = FloatBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 84:
#line 398 "CMDgram.y"
    { (yyval.expr) = FloatBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 85:
#line 400 "CMDgram.y"
    { (yyval.expr) = FloatBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 86:
#line 402 "CMDgram.y"
    { (yyval.expr) = FloatUnaryExprNode::alloc( (yyvsp[(1) - (2)].i).lineNumber, (yyvsp[(1) - (2)].i).value, (yyvsp[(2) - (2)].expr)); ;}
    break;

  case 87:
#line 404 "CMDgram.y"
    { (yyval.expr) = TTagDerefNode::alloc( (yyvsp[(1) - (2)].i).lineNumber, (yyvsp[(2) - (2)].expr) ); ;}
    break;

  case 88:
#line 406 "CMDgram.y"
    { (yyval.expr) = TTagExprNode::alloc( (yyvsp[(1) - (1)].s).lineNumber, (yyvsp[(1) - (1)].s).value ); ;}
    break;

  case 89:
#line 408 "CMDgram.y"
    { (yyval.expr) = ConditionalExprNode::alloc( (yyvsp[(1) - (5)].expr)->dbgLineNumber, (yyvsp[(1) - (5)].expr), (yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].expr)); ;}
    break;

  case 90:
#line 410 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 91:
#line 412 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 92:
#line 414 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 93:
#line 416 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 94:
#line 418 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 95:
#line 420 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 96:
#line 422 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 97:
#line 424 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 98:
#line 426 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 99:
#line 428 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 100:
#line 430 "CMDgram.y"
    { (yyval.expr) = StreqExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), true); ;}
    break;

  case 101:
#line 432 "CMDgram.y"
    { (yyval.expr) = StreqExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), false); ;}
    break;

  case 102:
#line 434 "CMDgram.y"
    { (yyval.expr) = StrcatExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), (yyvsp[(2) - (3)].i).value); ;}
    break;

  case 103:
#line 436 "CMDgram.y"
    { (yyval.expr) = IntUnaryExprNode::alloc((yyvsp[(1) - (2)].i).lineNumber, (yyvsp[(1) - (2)].i).value, (yyvsp[(2) - (2)].expr)); ;}
    break;

  case 104:
#line 438 "CMDgram.y"
    { (yyval.expr) = IntUnaryExprNode::alloc((yyvsp[(1) - (2)].i).lineNumber, (yyvsp[(1) - (2)].i).value, (yyvsp[(2) - (2)].expr)); ;}
    break;

  case 105:
#line 440 "CMDgram.y"
    { (yyval.expr) = StrConstNode::alloc( (yyvsp[(1) - (1)].str).lineNumber, (yyvsp[(1) - (1)].str).value, true); ;}
    break;

  case 106:
#line 442 "CMDgram.y"
    { (yyval.expr) = FloatNode::alloc( (yyvsp[(1) - (1)].f).lineNumber, (yyvsp[(1) - (1)].f).value ); ;}
    break;

  case 107:
#line 444 "CMDgram.y"
    { (yyval.expr) = IntNode::alloc( (yyvsp[(1) - (1)].i).lineNumber, (yyvsp[(1) - (1)].i).value ); ;}
    break;

  case 108:
#line 446 "CMDgram.y"
    { (yyval.expr) = ConstantNode::alloc( (yyvsp[(1) - (1)].i).lineNumber, StringTable->insert("break")); ;}
    break;

  case 109:
#line 448 "CMDgram.y"
    { (yyval.expr) = SlotAccessNode::alloc( (yyvsp[(1) - (1)].slot).lineNumber, (yyvsp[(1) - (1)].slot).object, (yyvsp[(1) - (1)].slot).array, (yyvsp[(1) - (1)].slot).slotName ); ;}
    break;

  case 110:
#line 450 "CMDgram.y"
    { (yyval.expr) = InternalSlotAccessNode::alloc( (yyvsp[(1) - (1)].intslot).lineNumber, (yyvsp[(1) - (1)].intslot).object, (yyvsp[(1) - (1)].intslot).slotExpr, (yyvsp[(1) - (1)].intslot).recurse); ;}
    break;

  case 111:
#line 452 "CMDgram.y"
    { (yyval.expr) = ConstantNode::alloc( (yyvsp[(1) - (1)].s).lineNumber, (yyvsp[(1) - (1)].s).value ); ;}
    break;

  case 112:
#line 454 "CMDgram.y"
    { (yyval.expr) = StrConstNode::alloc( (yyvsp[(1) - (1)].str).lineNumber, (yyvsp[(1) - (1)].str).value, false); ;}
    break;

  case 113:
#line 456 "CMDgram.y"
    { (yyval.expr) = (ExprNode*)VarNode::alloc( (yyvsp[(1) - (1)].s).lineNumber, (yyvsp[(1) - (1)].s).value, NULL); ;}
    break;

  case 114:
#line 458 "CMDgram.y"
    { (yyval.expr) = (ExprNode*)VarNode::alloc( (yyvsp[(1) - (4)].s).lineNumber, (yyvsp[(1) - (4)].s).value, (yyvsp[(3) - (4)].expr) ); ;}
    break;

  case 115:
#line 460 "CMDgram.y"
    {
         const U32 bufLen = 64;
         UTF8 buffer[bufLen];
         dSprintf(buffer, bufLen, "__anonymous_function%d", gAnonFunctionID++);
         StringTableEntry fName = StringTable->insert(buffer);
         StmtNode *fndef = FunctionDeclStmtNode::alloc((yyvsp[(1) - (7)].i).lineNumber, fName, NULL, (yyvsp[(3) - (7)].var), (yyvsp[(6) - (7)].stmt));

         (yyval.expr) = StrConstNode::alloc( (yyvsp[(1) - (7)].i).lineNumber, (UTF8*)fName, false );
      ;}
    break;

  case 116:
#line 473 "CMDgram.y"
    { (yyval.slot).lineNumber = (yyvsp[(1) - (3)].expr)->dbgLineNumber; (yyval.slot).object = (yyvsp[(1) - (3)].expr); (yyval.slot).slotName = (yyvsp[(3) - (3)].s).value; (yyval.slot).array = NULL; ;}
    break;

  case 117:
#line 475 "CMDgram.y"
    { (yyval.slot).lineNumber = (yyvsp[(1) - (6)].expr)->dbgLineNumber; (yyval.slot).object = (yyvsp[(1) - (6)].expr); (yyval.slot).slotName = (yyvsp[(3) - (6)].s).value; (yyval.slot).array = (yyvsp[(5) - (6)].expr); ;}
    break;

  case 118:
#line 480 "CMDgram.y"
    { (yyval.intslot).lineNumber = (yyvsp[(1) - (3)].expr)->dbgLineNumber; (yyval.intslot).object = (yyvsp[(1) - (3)].expr); (yyval.intslot).slotExpr = (yyvsp[(3) - (3)].expr); (yyval.intslot).recurse = false; ;}
    break;

  case 119:
#line 482 "CMDgram.y"
    { (yyval.intslot).lineNumber = (yyvsp[(1) - (3)].expr)->dbgLineNumber; (yyval.intslot).object = (yyvsp[(1) - (3)].expr); (yyval.intslot).slotExpr = (yyvsp[(3) - (3)].expr); (yyval.intslot).recurse = true; ;}
    break;

  case 120:
#line 487 "CMDgram.y"
    { (yyval.expr) = ConstantNode::alloc( (yyvsp[(1) - (1)].s).lineNumber, (yyvsp[(1) - (1)].s).value ); ;}
    break;

  case 121:
#line 489 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;

  case 122:
#line 494 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (1)].i).lineNumber; (yyval.asn).token = '+'; (yyval.asn).expr = FloatNode::alloc( (yyvsp[(1) - (1)].i).lineNumber, 1 ); ;}
    break;

  case 123:
#line 496 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (1)].i).lineNumber; (yyval.asn).token = '-'; (yyval.asn).expr = FloatNode::alloc( (yyvsp[(1) - (1)].i).lineNumber, 1 ); ;}
    break;

  case 124:
#line 498 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '+'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 125:
#line 500 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '-'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 126:
#line 502 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '*'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 127:
#line 504 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '/'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 128:
#line 506 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '%'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 129:
#line 508 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '&'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 130:
#line 510 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '^'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 131:
#line 512 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '|'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 132:
#line 514 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = opSHL; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 133:
#line 516 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = opSHR; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 134:
#line 521 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 135:
#line 523 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 136:
#line 525 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].od); ;}
    break;

  case 137:
#line 527 "CMDgram.y"
    { (yyval.expr) = AssignExprNode::alloc( (yyvsp[(1) - (3)].s).lineNumber, (yyvsp[(1) - (3)].s).value, NULL, (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 138:
#line 529 "CMDgram.y"
    { (yyval.expr) = AssignExprNode::alloc( (yyvsp[(1) - (6)].s).lineNumber, (yyvsp[(1) - (6)].s).value, (yyvsp[(3) - (6)].expr), (yyvsp[(6) - (6)].expr)); ;}
    break;

  case 139:
#line 531 "CMDgram.y"
    { (yyval.expr) = AssignOpExprNode::alloc( (yyvsp[(1) - (2)].s).lineNumber, (yyvsp[(1) - (2)].s).value, NULL, (yyvsp[(2) - (2)].asn).expr, (yyvsp[(2) - (2)].asn).token); ;}
    break;

  case 140:
#line 533 "CMDgram.y"
    { (yyval.expr) = AssignOpExprNode::alloc( (yyvsp[(1) - (5)].s).lineNumber, (yyvsp[(1) - (5)].s).value, (yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].asn).expr, (yyvsp[(5) - (5)].asn).token); ;}
    break;

  case 141:
#line 535 "CMDgram.y"
    { (yyval.expr) = SlotAssignOpNode::alloc( (yyvsp[(1) - (2)].slot).lineNumber, (yyvsp[(1) - (2)].slot).object, (yyvsp[(1) - (2)].slot).slotName, (yyvsp[(1) - (2)].slot).array, (yyvsp[(2) - (2)].asn).token, (yyvsp[(2) - (2)].asn).expr); ;}
    break;

  case 142:
#line 537 "CMDgram.y"
    { (yyval.expr) = SlotAssignNode::alloc( (yyvsp[(1) - (3)].slot).lineNumber, (yyvsp[(1) - (3)].slot).object, (yyvsp[(1) - (3)].slot).array, (yyvsp[(1) - (3)].slot).slotName, (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 143:
#line 539 "CMDgram.y"
    { (yyval.expr) = SlotAssignNode::alloc( (yyvsp[(1) - (5)].slot).lineNumber, (yyvsp[(1) - (5)].slot).object, (yyvsp[(1) - (5)].slot).array, (yyvsp[(1) - (5)].slot).slotName, (yyvsp[(4) - (5)].expr)); ;}
    break;

  case 144:
#line 544 "CMDgram.y"
    { (yyval.expr) = FuncCallExprNode::alloc( (yyvsp[(1) - (4)].s).lineNumber, (yyvsp[(1) - (4)].s).value, NULL, (yyvsp[(3) - (4)].expr), false); ;}
    break;

  case 145:
#line 546 "CMDgram.y"
    { (yyval.expr) = FuncCallExprNode::alloc( (yyvsp[(1) - (6)].s).lineNumber, (yyvsp[(3) - (6)].s).value, (yyvsp[(1) - (6)].s).value, (yyvsp[(5) - (6)].expr), false); ;}
    break;

  case 146:
#line 548 "CMDgram.y"
    { (yyvsp[(1) - (6)].expr)->append((yyvsp[(5) - (6)].expr)); (yyval.expr) = FuncCallExprNode::alloc( (yyvsp[(1) - (6)].expr)->dbgLineNumber, (yyvsp[(3) - (6)].s).value, NULL, (yyvsp[(1) - (6)].expr), true); ;}
    break;

  case 147:
#line 553 "CMDgram.y"
    { (yyval.expr) = AssertCallExprNode::alloc( (yyvsp[(1) - (4)].i).lineNumber, (yyvsp[(3) - (4)].expr), NULL ); ;}
    break;

  case 148:
#line 555 "CMDgram.y"
    { (yyval.expr) = AssertCallExprNode::alloc( (yyvsp[(1) - (6)].i).lineNumber, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].str).value ); ;}
    break;

  case 149:
#line 560 "CMDgram.y"
    { (yyval.expr) = NULL; ;}
    break;

  case 150:
#line 562 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 151:
#line 567 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 152:
#line 569 "CMDgram.y"
    { ((yyvsp[(1) - (3)].expr))->append((yyvsp[(3) - (3)].expr)); (yyval.expr) = (yyvsp[(1) - (3)].expr); ;}
    break;

  case 153:
#line 574 "CMDgram.y"
    { (yyval.slist) = NULL; ;}
    break;

  case 154:
#line 576 "CMDgram.y"
    { (yyval.slist) = (yyvsp[(1) - (1)].slist); ;}
    break;

  case 155:
#line 581 "CMDgram.y"
    { (yyval.slist) = (yyvsp[(1) - (1)].slist); ;}
    break;

  case 156:
#line 583 "CMDgram.y"
    { (yyvsp[(1) - (2)].slist)->append((yyvsp[(2) - (2)].slist)); (yyval.slist) = (yyvsp[(1) - (2)].slist); ;}
    break;

  case 157:
#line 588 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (4)].s).lineNumber, NULL, NULL, (yyvsp[(1) - (4)].s).value, (yyvsp[(3) - (4)].expr)); ;}
    break;

  case 158:
#line 590 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (5)].i).lineNumber, NULL, NULL, (yyvsp[(2) - (5)].s).value, (yyvsp[(4) - (5)].expr), (yyvsp[(1) - (5)].i).value); ;}
    break;

  case 159:
#line 592 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (4)].i).lineNumber, NULL, NULL, StringTable->insert("datablock"), (yyvsp[(3) - (4)].expr)); ;}
    break;

  case 160:
#line 594 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (7)].s).lineNumber, NULL, (yyvsp[(3) - (7)].expr), (yyvsp[(1) - (7)].s).value, (yyvsp[(6) - (7)].expr)); ;}
    break;

  case 161:
#line 596 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (8)].i).lineNumber, NULL, (yyvsp[(4) - (8)].expr), (yyvsp[(2) - (8)].s).value, (yyvsp[(7) - (8)].expr), (yyvsp[(1) - (8)].i).value); ;}
    break;

  case 162:
#line 601 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 163:
#line 603 "CMDgram.y"
    { (yyval.expr) = CommaCatExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3203 "CMDgram.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 605 "CMDgram.y"


