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
     VAR = 291,
     IDENT = 292,
     TYPEIDENT = 293,
     DOCBLOCK = 294,
     STRATOM = 295,
     TAGATOM = 296,
     FLTCONST = 297,
     opINTNAME = 298,
     opINTNAMER = 299,
     opMINUSMINUS = 300,
     opPLUSPLUS = 301,
     STMT_SEP = 302,
     opSHL = 303,
     opSHR = 304,
     opPLASN = 305,
     opMIASN = 306,
     opMLASN = 307,
     opDVASN = 308,
     opMODASN = 309,
     opANDASN = 310,
     opXORASN = 311,
     opORASN = 312,
     opSLASN = 313,
     opSRASN = 314,
     opCAT = 315,
     opEQ = 316,
     opNE = 317,
     opGE = 318,
     opLE = 319,
     opAND = 320,
     opOR = 321,
     opSTREQ = 322,
     opCOLONCOLON = 323,
     opNTASN = 324,
     opNDASN = 325,
     opMDASN = 326,
     opSTRNE = 327,
     UNARY = 328
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
#define VAR 291
#define IDENT 292
#define TYPEIDENT 293
#define DOCBLOCK 294
#define STRATOM 295
#define TAGATOM 296
#define FLTCONST 297
#define opINTNAME 298
#define opINTNAMER 299
#define opMINUSMINUS 300
#define opPLUSPLUS 301
#define STMT_SEP 302
#define opSHL 303
#define opSHR 304
#define opPLASN 305
#define opMIASN 306
#define opMLASN 307
#define opDVASN 308
#define opMODASN 309
#define opANDASN 310
#define opXORASN 311
#define opORASN 312
#define opSLASN 313
#define opSRASN 314
#define opCAT 315
#define opEQ 316
#define opNE 317
#define opGE 318
#define opLE 319
#define opAND 320
#define opOR 321
#define opSTREQ 322
#define opCOLONCOLON 323
#define opNTASN 324
#define opNDASN 325
#define opMDASN 326
#define opSTRNE 327
#define UNARY 328




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

extern S32 CMDGetCurrentLine();
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

#line 45 "CMDgram.y"

        /* Reserved Word Definitions */
#line 56 "CMDgram.y"

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
#line 322 "CMDgram.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 335 "CMDgram.c"

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
#define YYLAST   2731

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  99
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  161
/* YYNRULES -- Number of states.  */
#define YYNSTATES  380

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    63,     2,     2,     2,    53,    52,     2,
      54,    55,    45,    43,    56,    44,    50,    46,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    57,    58,
      47,    49,    48,    95,    64,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    91,     2,    98,    61,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    59,    51,    60,    62,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    92,    93,    94,    96,    97
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    22,
      24,    27,    28,    31,    33,    35,    37,    39,    41,    43,
      46,    49,    52,    56,    59,    61,    62,    72,    73,    85,
      86,    88,    90,    94,   105,   116,   124,   137,   147,   158,
     166,   167,   170,   171,   173,   174,   177,   178,   180,   182,
     185,   188,   192,   196,   198,   206,   214,   219,   227,   233,
     235,   239,   245,   253,   259,   266,   276,   285,   294,   302,
     311,   319,   327,   334,   342,   350,   352,   354,   358,   362,
     366,   370,   374,   378,   382,   386,   390,   393,   399,   403,
     407,   411,   415,   419,   423,   427,   431,   435,   439,   443,
     447,   451,   454,   457,   459,   461,   463,   465,   467,   469,
     471,   473,   475,   480,   488,   492,   499,   503,   507,   509,
     513,   515,   517,   520,   523,   526,   529,   532,   535,   538,
     541,   544,   547,   549,   551,   553,   557,   564,   567,   573,
     576,   580,   586,   591,   598,   605,   610,   617,   618,   620,
     622,   626,   627,   629,   631,   634,   639,   645,   650,   658,
     667,   669
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     100,     0,    -1,   101,    -1,    -1,   101,   102,    -1,   106,
      -1,   107,    -1,   103,    -1,    29,    37,    59,   104,    60,
      58,    -1,   107,    -1,   104,   107,    -1,    -1,   105,   106,
      -1,   123,    -1,   124,    -1,   125,    -1,   126,    -1,   112,
      -1,   120,    -1,     7,    58,    -1,     9,    58,    -1,    13,
      58,    -1,    13,   128,    58,    -1,   127,    58,    -1,    39,
      -1,    -1,     3,    37,   108,    54,   110,    55,    59,   105,
      60,    -1,    -1,     3,    37,    90,    37,   109,    54,   110,
      55,    59,   105,    60,    -1,    -1,   111,    -1,    36,    -1,
     111,    56,    36,    -1,    24,   131,    54,   128,   114,    55,
      59,   138,    60,    58,    -1,     5,   131,    54,   115,   114,
     116,    55,    59,   117,    60,    -1,     5,   131,    54,   115,
     114,   116,    55,    -1,     5,   131,    54,    91,   115,    98,
     114,   116,    55,    59,   117,    60,    -1,     5,   131,    54,
      91,   115,    98,   114,   116,    55,    -1,     6,   131,    54,
     115,   114,   116,    55,    59,   117,    60,    -1,     6,   131,
      54,   115,   114,   116,    55,    -1,    -1,    57,    37,    -1,
      -1,   128,    -1,    -1,    56,   137,    -1,    -1,   139,    -1,
     118,    -1,   139,   118,    -1,   113,    58,    -1,   118,   113,
      58,    -1,    59,   105,    60,    -1,   106,    -1,    25,    54,
     128,    55,    59,   121,    60,    -1,    27,    54,   128,    55,
      59,   121,    60,    -1,    26,   122,    57,   105,    -1,    26,
     122,    57,   105,    19,    57,   105,    -1,    26,   122,    57,
     105,   121,    -1,   128,    -1,   122,    28,   128,    -1,    11,
      54,   128,    55,   119,    -1,    11,    54,   128,    55,   119,
       8,   119,    -1,    14,    54,   128,    55,   119,    -1,    15,
     119,    14,    54,   128,    55,    -1,    20,    54,   128,    58,
     128,    58,   128,    55,   119,    -1,    20,    54,   128,    58,
     128,    58,    55,   119,    -1,    20,    54,   128,    58,    58,
     128,    55,   119,    -1,    20,    54,   128,    58,    58,    55,
     119,    -1,    20,    54,    58,   128,    58,   128,    55,   119,
      -1,    20,    54,    58,   128,    58,    55,   119,    -1,    20,
      54,    58,    58,   128,    55,   119,    -1,    20,    54,    58,
      58,    55,   119,    -1,    21,    54,    36,    23,   128,    55,
     119,    -1,    22,    54,    36,    23,   128,    55,   119,    -1,
     133,    -1,   133,    -1,    54,   128,    55,    -1,   128,    61,
     128,    -1,   128,    53,   128,    -1,   128,    52,   128,    -1,
     128,    51,   128,    -1,   128,    43,   128,    -1,   128,    44,
     128,    -1,   128,    45,   128,    -1,   128,    46,   128,    -1,
      44,   128,    -1,   128,    95,   128,    57,   128,    -1,   128,
      47,   128,    -1,   128,    48,   128,    -1,   128,    85,   128,
      -1,   128,    86,   128,    -1,   128,    83,   128,    -1,   128,
      84,   128,    -1,   128,    88,   128,    -1,   128,    70,   128,
      -1,   128,    71,   128,    -1,   128,    87,   128,    -1,   128,
      89,   128,    -1,   128,    96,   128,    -1,   128,    64,   128,
      -1,    63,   128,    -1,    62,   128,    -1,    41,    -1,    42,
      -1,    35,    -1,     7,    -1,   129,    -1,   130,    -1,    37,
      -1,    40,    -1,    36,    -1,    36,    91,   141,    98,    -1,
       3,    54,   110,    55,    59,   105,    60,    -1,   128,    50,
      37,    -1,   128,    50,    37,    91,   141,    98,    -1,   128,
      65,   131,    -1,   128,    66,   131,    -1,    37,    -1,    54,
     128,    55,    -1,    68,    -1,    67,    -1,    72,   128,    -1,
      73,   128,    -1,    74,   128,    -1,    75,   128,    -1,    76,
     128,    -1,    77,   128,    -1,    78,   128,    -1,    79,   128,
      -1,    80,   128,    -1,    81,   128,    -1,   134,    -1,   135,
      -1,   113,    -1,    36,    49,   128,    -1,    36,    91,   141,
      98,    49,   128,    -1,    36,   132,    -1,    36,    91,   141,
      98,   132,    -1,   129,   132,    -1,   129,    49,   128,    -1,
     129,    49,    59,   137,    60,    -1,    37,    54,   136,    55,
      -1,    37,    90,    37,    54,   136,    55,    -1,   128,    50,
      37,    54,   136,    55,    -1,    32,    54,   128,    55,    -1,
      32,    54,   128,    56,    40,    55,    -1,    -1,   137,    -1,
     128,    -1,   137,    56,   128,    -1,    -1,   139,    -1,   140,
      -1,   139,   140,    -1,    37,    49,   128,    58,    -1,    38,
      37,    49,   128,    58,    -1,    24,    49,   128,    58,    -1,
      37,    91,   141,    98,    49,   128,    58,    -1,    38,    37,
      91,   141,    98,    49,   128,    58,    -1,   128,    -1,   141,
      56,   128,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   161,   161,   167,   168,   173,   175,   177,   182,   187,
     189,   195,   196,   201,   202,   203,   204,   205,   206,   207,
     209,   211,   213,   215,   217,   223,   222,   227,   226,   234,
     235,   240,   242,   247,   252,   254,   256,   258,   260,   262,
     268,   269,   275,   276,   282,   283,   289,   290,   292,   294,
     299,   301,   306,   308,   313,   315,   320,   322,   324,   329,
     331,   336,   338,   343,   345,   350,   352,   354,   356,   358,
     360,   362,   364,   369,   371,   376,   381,   383,   385,   387,
     389,   391,   393,   395,   397,   399,   401,   403,   405,   407,
     409,   411,   413,   415,   417,   419,   421,   423,   425,   427,
     429,   431,   433,   435,   437,   439,   441,   443,   445,   447,
     449,   451,   453,   455,   468,   470,   475,   477,   482,   484,
     489,   491,   493,   495,   497,   499,   501,   503,   505,   507,
     509,   511,   516,   518,   520,   522,   524,   526,   528,   530,
     532,   534,   539,   541,   543,   548,   550,   556,   557,   562,
     564,   570,   571,   576,   578,   583,   585,   587,   589,   591,
     596,   598
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
  "CHRCONST", "INTCONST", "VAR", "IDENT", "TYPEIDENT", "DOCBLOCK",
  "STRATOM", "TAGATOM", "FLTCONST", "'+'", "'-'", "'*'", "'/'", "'<'",
  "'>'", "'='", "'.'", "'|'", "'&'", "'%'", "'('", "')'", "','", "':'",
  "';'", "'{'", "'}'", "'^'", "'~'", "'!'", "'@'", "opINTNAME",
  "opINTNAMER", "opMINUSMINUS", "opPLUSPLUS", "STMT_SEP", "opSHL", "opSHR",
  "opPLASN", "opMIASN", "opMLASN", "opDVASN", "opMODASN", "opANDASN",
  "opXORASN", "opORASN", "opSLASN", "opSRASN", "opCAT", "opEQ", "opNE",
  "opGE", "opLE", "opAND", "opOR", "opSTREQ", "opCOLONCOLON", "'['",
  "opNTASN", "opNDASN", "opMDASN", "'?'", "opSTRNE", "UNARY", "']'",
  "$accept", "start", "decl_list", "decl", "package_decl", "fn_decl_list",
  "statement_list", "stmt", "fn_decl_stmt", "@1", "@2", "var_list_decl",
  "var_list", "datablock_decl", "object_decl", "parent_block",
  "object_name", "object_args", "object_declare_block", "object_decl_list",
  "stmt_block", "switch_stmt", "case_block", "case_expr", "if_stmt",
  "while_stmt", "for_stmt", "foreach_stmt", "expression_stmt", "expr",
  "slot_acc", "intslot_acc", "class_name_expr", "assign_op_struct",
  "stmt_expr", "funcall_expr", "assert_expr", "expr_list_decl",
  "expr_list", "slot_assign_list_opt", "slot_assign_list", "slot_assign",
  "aidx_expr", 0
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
     295,   296,   297,    43,    45,    42,    47,    60,    62,    61,
      46,   124,    38,    37,    40,    41,    44,    58,    59,   123,
     125,    94,   126,    33,    64,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,    91,   324,   325,   326,    63,   327,   328,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    99,   100,   101,   101,   102,   102,   102,   103,   104,
     104,   105,   105,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   108,   107,   109,   107,   110,
     110,   111,   111,   112,   113,   113,   113,   113,   113,   113,
     114,   114,   115,   115,   116,   116,   117,   117,   117,   117,
     118,   118,   119,   119,   120,   120,   121,   121,   121,   122,
     122,   123,   123,   124,   124,   125,   125,   125,   125,   125,
     125,   125,   125,   126,   126,   127,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   129,   129,   130,   130,   131,   131,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   134,   134,   134,   135,   135,   136,   136,   137,
     137,   138,   138,   139,   139,   140,   140,   140,   140,   140,
     141,   141
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     6,     1,
       2,     0,     2,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     3,     2,     1,     0,     9,     0,    11,     0,
       1,     1,     3,    10,    10,     7,    12,     9,    10,     7,
       0,     2,     0,     1,     0,     2,     0,     1,     1,     2,
       2,     3,     3,     1,     7,     7,     4,     7,     5,     1,
       3,     5,     7,     5,     6,     9,     8,     8,     7,     8,
       7,     7,     6,     7,     7,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     5,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     7,     3,     6,     3,     3,     1,     3,
       1,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     3,     6,     2,     5,     2,
       3,     5,     4,     6,     6,     4,     6,     0,     1,     1,
       3,     0,     1,     1,     2,     4,     5,     4,     7,     8,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,   106,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   105,   111,   109,    24,   110,   103,   104,     0,     0,
       0,     0,     4,     7,     5,     6,    17,   134,    18,    13,
      14,    15,    16,     0,     0,   107,   108,    76,   132,   133,
      25,    29,   118,     0,     0,     0,    19,    20,     0,     0,
     106,    21,     0,    76,     0,    11,    53,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   121,   120,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     137,   147,     0,    86,     0,   102,   101,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   139,     0,     0,    31,     0,    30,
       0,    42,    42,     0,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   135,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   160,     0,
     149,     0,   148,     0,    77,    82,    83,    84,    85,    88,
      89,   114,    81,    80,    79,    78,   100,   116,   117,    95,
      96,    92,    93,    90,    91,    97,    94,    98,     0,    99,
       0,   140,    27,    29,     0,     0,   119,    42,    40,    43,
      40,     0,     0,    52,    12,     0,     0,     0,     0,     0,
       0,    40,     0,     0,     0,     0,     9,   145,     0,     0,
     112,   142,     0,   147,   147,     0,     0,     0,     0,     0,
      11,    32,     0,     0,    44,    44,    61,    63,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      10,     0,   161,     0,   138,   150,     0,     0,     0,    87,
     141,    29,     0,     0,    40,    41,     0,     0,     0,     0,
      64,    72,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     8,   146,   136,   143,   144,   115,
       0,    11,   113,    44,    45,    35,    39,    62,    71,    70,
       0,    68,     0,     0,     0,    73,    74,   151,     0,    59,
      54,    55,     0,     0,     0,    46,    46,    69,    67,    66,
       0,     0,     0,     0,     0,   152,   153,     0,    11,    11,
      26,    37,     0,     0,    48,    47,     0,    65,     0,     0,
       0,     0,     0,   154,    60,    56,     0,    46,    50,    34,
       0,    49,    38,     0,     0,     0,     0,     0,    33,     0,
      58,    28,     0,    51,   157,   155,     0,     0,     0,    11,
      36,     0,   156,     0,    57,     0,     0,   158,     0,   159
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    32,    33,   215,   136,    66,    35,   126,
     228,   128,   129,    36,    37,   234,   198,   267,   333,   334,
      67,    38,   282,   308,    39,    40,    41,    42,    43,    44,
      45,    46,    54,    90,    63,    48,    49,   161,   162,   324,
     335,   326,   159
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -298
static const yytype_int16 yypact[] =
{
    -298,    43,   203,  -298,   -16,    -6,    -6,   -13,   -11,    -5,
     805,     3,   435,    12,    14,    35,    -6,    38,    39,    15,
      40,  -298,  2615,   -37,  -298,  -298,  -298,  -298,   997,   997,
     997,   997,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
    -298,  -298,  -298,    37,  2352,  2635,  -298,    41,  -298,  -298,
      10,    62,  -298,   997,    50,    57,  -298,  -298,   997,    60,
    -298,  -298,  1056,  -298,   997,  -298,  -298,   107,   829,    81,
      88,    72,   997,   997,    70,   997,   997,  -298,  -298,   997,
     997,   997,   997,   997,   997,   997,   997,   997,   997,   997,
    -298,   997,    94,    53,  1110,    53,    53,  -298,   997,   997,
     997,   997,   997,   997,    96,   997,   997,   997,   997,   997,
      -6,    -6,   997,   997,   997,   997,   997,   997,   997,   997,
     997,   997,   997,   243,  -298,    98,    76,  -298,    89,   104,
    1164,    19,   997,  1218,  -298,  1272,   548,   100,   869,  1326,
     127,   129,   997,  1380,  1434,   162,  1002,  2352,  2352,  2352,
    2352,  2352,  2352,  2352,  2352,  2352,  2352,  2352,  2352,   -40,
    2352,   115,   116,   117,  -298,    93,    93,    53,    53,  2609,
    2609,   -24,  2493,  2551,    53,  2522,   438,  -298,  -298,   473,
     473,  2580,  2580,  2609,  2609,  2464,  2435,   438,  1488,   438,
     997,  2352,  -298,    62,   114,   139,  -298,   997,   123,  2352,
     123,   435,   435,  -298,  -298,   997,   285,  1542,   893,   997,
     997,  1596,   126,   128,   151,    26,  -298,  -298,   149,   997,
    2650,  -298,   997,   997,   997,   997,   997,   -19,   136,   137,
    -298,  -298,    95,   154,   138,   138,   187,  -298,  1650,   435,
    1704,   933,   957,  1758,  1812,  1866,   143,   171,   171,   144,
    -298,   146,  2352,   997,  -298,  2352,   148,   152,   -36,  2406,
    -298,    62,   156,   615,   123,  -298,   997,   158,   164,   435,
    -298,  -298,   435,   435,  1920,   435,  1974,   973,   435,   435,
     161,   997,   166,   169,  -298,  -298,  2352,  -298,  -298,  -298,
     167,  -298,  -298,   138,   116,   172,   174,  -298,  -298,  -298,
     435,  -298,   435,   435,  2028,  -298,  -298,   103,     8,  2352,
    -298,  -298,   175,   677,   181,    85,    85,  -298,  -298,  -298,
     435,   192,   -22,   184,   191,   103,  -298,   997,  -298,  -298,
    -298,   193,   153,   194,    29,    85,   195,  -298,   997,   997,
     997,   -21,   198,  -298,  2352,   142,   721,    85,  -298,  -298,
     200,    29,  -298,  2082,  2136,   -23,   997,   997,  -298,   196,
    -298,  -298,   199,  -298,  -298,  -298,   211,  2190,   -10,  -298,
    -298,   997,  -298,   213,   765,  2244,   997,  -298,  2298,  -298
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -298,  -298,  -298,  -298,  -298,  -298,  -227,     0,  -130,  -298,
    -298,  -189,  -298,  -298,  -219,  -188,  -118,  -222,  -297,   -71,
    -195,  -298,  -239,  -298,  -298,  -298,  -298,  -298,  -298,   305,
    -298,  -298,     2,   -44,    -2,  -298,  -298,  -184,  -179,  -298,
     -39,  -293,  -220
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -76
static const yytype_int16 yytable[] =
{
      47,   124,    34,   263,   229,   258,   236,   237,    55,   283,
      47,   227,   235,   268,   200,   216,   219,    91,    71,   336,
     219,    50,    59,   246,     5,     6,    60,   339,   356,   214,
     224,    52,   343,   219,     5,     6,   327,   222,    51,   256,
     257,   260,   343,     3,   271,    56,   219,    57,    53,    58,
     362,    20,    74,    92,    21,    22,    23,    64,   220,    25,
      26,    27,   289,    28,   313,   328,    68,   225,    69,   340,
     357,   314,   290,    29,   297,   366,   293,   298,   299,   232,
     301,    30,    31,   305,   306,   250,   249,   294,   373,    70,
       5,     6,    72,    73,    75,    97,   332,   332,   127,   -75,
     125,   345,   346,   104,   131,   317,   360,   318,   319,   321,
     197,   132,   177,   178,    51,   350,   332,   140,   110,   111,
     355,   137,   322,   323,   141,   337,   142,   321,   332,   145,
     193,   163,   350,   171,    47,   192,   204,   368,   100,   101,
     322,   323,   374,   104,   194,    59,   107,     5,     6,     7,
     209,     8,   210,     9,   205,    10,    11,    12,   110,   111,
     195,   359,    13,    14,    15,   214,    16,    17,   281,    18,
     221,   223,   222,   230,    20,   231,   254,    21,    22,    23,
     233,    24,    25,    26,    27,   247,    28,   248,    50,   251,
     261,   265,   262,   264,   266,   269,    29,   281,   280,    47,
      47,   285,   284,   287,    30,    31,     4,   288,     5,     6,
       7,   348,     8,   295,     9,   291,    10,    11,    12,   296,
     307,   341,   312,    13,    14,    15,   310,    16,    17,   311,
      18,   315,    19,   316,   329,    20,   331,    47,    21,    22,
      23,   338,    24,    25,    26,    27,    59,    28,     5,     6,
      60,   342,   347,   369,   349,   352,   358,    29,   363,   370,
     371,    47,   376,   204,   351,    30,    31,    47,   325,     0,
      47,    47,     0,    47,     0,    20,    47,    47,    21,    22,
      23,     0,     0,    25,    26,    27,     0,    28,    59,     0,
       5,     6,    60,     0,     0,     0,     0,    29,    47,     0,
      47,    47,   190,     0,     0,    30,    31,     0,     0,     0,
       0,    47,     0,   204,     0,    62,     0,    20,    47,     0,
      21,    22,    23,     0,     0,    25,    26,    27,     0,    28,
       0,     0,     0,    93,    94,    95,    96,     0,     0,    29,
     239,     0,     0,    47,    47,   204,   204,    30,    31,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   130,     0,
       0,     0,     0,   133,     0,     0,     0,     0,     0,   135,
       0,     0,    47,   139,   204,     0,     0,   143,   144,     0,
     146,   147,     0,     0,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,     0,   160,     0,     0,     0,
       0,     0,     0,   165,   166,   167,   168,   169,   170,     0,
     172,   173,   174,   175,   176,     0,     0,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   191,     0,
       0,     0,     0,     0,     0,     0,   199,   199,    59,     0,
       5,     6,     7,   207,     8,     0,     9,   211,    10,    11,
      12,     0,     0,     0,     0,    13,    14,    15,     0,    16,
      17,     0,    18,     0,     0,     0,     0,    20,     0,     0,
      21,    22,    23,     0,    24,    25,    26,    27,     0,    28,
       0,    98,    99,   100,   101,     0,     0,     0,   104,    29,
       0,   107,     0,     0,    65,   160,     0,    30,    31,     0,
       0,     0,   199,   110,   111,     0,     0,     0,   112,   113,
     238,   240,     0,   243,   244,   245,    98,    99,   100,   101,
       0,     0,     0,   104,   252,     0,   107,   255,   160,   160,
     158,   259,     0,     0,     0,     0,     0,     0,   110,   111,
       0,     0,     0,     0,     0,     0,   274,   276,     0,     0,
       0,    59,     0,     5,     6,     7,     0,     8,   286,     9,
       0,    10,    11,    12,     0,     0,     0,     0,    13,    14,
      15,   160,    16,    17,     0,    18,     0,     0,     0,     0,
      20,     0,   304,    21,    22,    23,   309,    24,    25,    26,
      27,     0,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,     0,     0,     0,   203,     0,
      30,    31,     0,     0,     0,     0,     0,     0,    59,     0,
       5,     6,     7,     0,     8,     0,     9,     0,    10,    11,
      12,     0,   344,     0,     0,    13,    14,    15,     0,    16,
      17,     0,    18,   353,   354,   158,     0,    20,     0,     0,
      21,    22,    23,     0,    24,    25,    26,    27,     0,    28,
       0,   367,   158,     0,     0,     0,     0,     0,     0,    29,
       0,     0,     0,     0,     0,   292,   375,    30,    31,     0,
      59,   378,     5,     6,     7,     0,     8,     0,     9,     0,
      10,    11,    12,     0,     0,     0,     0,    13,    14,    15,
       0,    16,    17,     0,    18,     0,     0,     0,     0,    20,
       0,     0,    21,    22,    23,     0,    24,    25,    26,    27,
       0,    28,     0,     0,    59,     0,     5,     6,     7,     0,
       8,    29,     9,     0,    10,    11,    12,   330,     0,    30,
      31,    13,    14,    15,     0,    16,    17,     0,    18,     0,
       0,     0,     0,    20,     0,     0,    21,    22,    23,     0,
      24,    25,    26,    27,     0,    28,     0,     0,    59,     0,
       5,     6,     7,     0,     8,    29,     9,     0,    10,    11,
      12,   361,     0,    30,    31,    13,    14,    15,     0,    16,
      17,     0,    18,     0,     0,     0,     0,    20,     0,     0,
      21,    22,    23,     0,    24,    25,    26,    27,    59,    28,
       5,     6,    60,     0,     0,     0,     0,     0,     0,    29,
       0,     0,     0,     0,     0,     0,     0,    30,    31,     0,
       0,     0,    59,     0,     5,     6,    60,    20,     0,     0,
      21,    22,    23,     0,     0,    25,    26,    27,     0,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,    20,     0,    61,    21,    22,    23,    30,    31,    25,
      26,    27,    59,    28,     5,     6,    60,     0,     0,     0,
       0,     0,     0,    29,     0,     0,     0,   138,     0,     0,
       0,    30,    31,     0,     0,     0,    59,     0,     5,     6,
      60,    20,     0,     0,    21,    22,    23,     0,     0,    25,
      26,    27,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,    20,     0,   206,    21,    22,
      23,    30,    31,    25,    26,    27,    59,    28,     5,     6,
      60,     0,     0,     0,     0,     0,     0,    29,     0,     0,
       0,   242,     0,     0,     0,    30,    31,     0,     0,     0,
      59,     0,     5,     6,    60,    20,     0,     0,    21,    22,
      23,     0,     0,    25,    26,    27,    59,    28,     5,     6,
      60,     0,     0,     0,     0,     0,     0,    29,   273,    20,
       0,     0,    21,    22,    23,    30,    31,    25,    26,    27,
      59,    28,     5,     6,    60,    20,     0,     0,    21,    22,
      23,    29,   275,    25,    26,    27,     0,    28,     0,    30,
      31,     0,     0,     0,     0,     0,     0,    29,   303,    20,
       0,     0,    21,    22,    23,    30,    31,    25,    26,    27,
       0,    28,     0,     0,     0,    98,    99,   100,   101,   102,
     103,    29,   104,   105,   106,   107,     0,   217,   218,    30,
      31,     0,     0,   108,     0,     0,   109,   110,   111,     0,
       0,     0,   112,   113,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,     0,     0,   121,   122,    98,
      99,   100,   101,   102,   103,     0,   104,   105,   106,   107,
       0,     0,     0,     0,   134,     0,     0,   108,     0,     0,
     109,   110,   111,     0,     0,     0,   112,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,     0,     0,     0,     0,
       0,   121,   122,    98,    99,   100,   101,   102,   103,     0,
     104,   105,   106,   107,     0,   164,     0,     0,     0,     0,
       0,   108,     0,     0,   109,   110,   111,     0,     0,     0,
     112,   113,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
       0,     0,     0,     0,     0,   121,   122,    98,    99,   100,
     101,   102,   103,     0,   104,   105,   106,   107,     0,   196,
       0,     0,     0,     0,     0,   108,     0,     0,   109,   110,
     111,     0,     0,     0,   112,   113,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   114,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,   121,
     122,    98,    99,   100,   101,   102,   103,     0,   104,   105,
     106,   107,     0,   201,     0,     0,     0,     0,     0,   108,
       0,     0,   109,   110,   111,     0,     0,     0,   112,   113,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,     0,     0,   121,   122,    98,    99,   100,   101,   102,
     103,     0,   104,   105,   106,   107,     0,   202,     0,     0,
       0,     0,     0,   108,     0,     0,   109,   110,   111,     0,
       0,     0,   112,   113,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,     0,     0,   121,   122,    98,
      99,   100,   101,   102,   103,     0,   104,   105,   106,   107,
       0,     0,     0,     0,   208,     0,     0,   108,     0,     0,
     109,   110,   111,     0,     0,     0,   112,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,     0,     0,     0,     0,
       0,   121,   122,    98,    99,   100,   101,   102,   103,     0,
     104,   105,   106,   107,     0,   212,     0,     0,     0,     0,
       0,   108,     0,     0,   109,   110,   111,     0,     0,     0,
     112,   113,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
       0,     0,     0,     0,     0,   121,   122,    98,    99,   100,
     101,   102,   103,     0,   104,   105,   106,   107,     0,   213,
       0,     0,     0,     0,     0,   108,     0,     0,   109,   110,
     111,     0,     0,     0,   112,   113,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   114,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,   121,
     122,    98,    99,   100,   101,   102,   103,     0,   104,   105,
     106,   107,     0,     0,     0,   226,     0,     0,     0,   108,
       0,     0,   109,   110,   111,     0,     0,     0,   112,   113,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,     0,     0,   121,   122,    98,    99,   100,   101,   102,
     103,     0,   104,   105,   106,   107,     0,     0,     0,     0,
     241,     0,     0,   108,     0,     0,   109,   110,   111,     0,
       0,     0,   112,   113,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,     0,     0,   121,   122,    98,
      99,   100,   101,   102,   103,     0,   104,   105,   106,   107,
       0,     0,     0,   233,     0,     0,     0,   108,     0,     0,
     109,   110,   111,     0,     0,     0,   112,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,     0,     0,     0,     0,
       0,   121,   122,    98,    99,   100,   101,   102,   103,     0,
     104,   105,   106,   107,     0,   270,     0,     0,     0,     0,
       0,   108,     0,     0,   109,   110,   111,     0,     0,     0,
     112,   113,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
       0,     0,     0,     0,     0,   121,   122,    98,    99,   100,
     101,   102,   103,     0,   104,   105,   106,   107,     0,   272,
       0,     0,     0,     0,     0,   108,     0,     0,   109,   110,
     111,     0,     0,     0,   112,   113,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   114,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,   121,
     122,    98,    99,   100,   101,   102,   103,     0,   104,   105,
     106,   107,     0,     0,     0,     0,   277,     0,     0,   108,
       0,     0,   109,   110,   111,     0,     0,     0,   112,   113,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,     0,     0,   121,   122,    98,    99,   100,   101,   102,
     103,     0,   104,   105,   106,   107,     0,   278,     0,     0,
       0,     0,     0,   108,     0,     0,   109,   110,   111,     0,
       0,     0,   112,   113,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,     0,     0,   121,   122,    98,
      99,   100,   101,   102,   103,     0,   104,   105,   106,   107,
       0,   279,     0,     0,     0,     0,     0,   108,     0,     0,
     109,   110,   111,     0,     0,     0,   112,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,     0,     0,     0,     0,
       0,   121,   122,    98,    99,   100,   101,   102,   103,     0,
     104,   105,   106,   107,     0,   300,     0,     0,     0,     0,
       0,   108,     0,     0,   109,   110,   111,     0,     0,     0,
     112,   113,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
       0,     0,     0,     0,     0,   121,   122,    98,    99,   100,
     101,   102,   103,     0,   104,   105,   106,   107,     0,   302,
       0,     0,     0,     0,     0,   108,     0,     0,   109,   110,
     111,     0,     0,     0,   112,   113,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   114,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,   121,
     122,    98,    99,   100,   101,   102,   103,     0,   104,   105,
     106,   107,     0,   320,     0,     0,     0,     0,     0,   108,
       0,     0,   109,   110,   111,     0,     0,     0,   112,   113,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,     0,     0,   121,   122,    98,    99,   100,   101,   102,
     103,     0,   104,   105,   106,   107,     0,     0,     0,     0,
     364,     0,     0,   108,     0,     0,   109,   110,   111,     0,
       0,     0,   112,   113,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,     0,     0,   121,   122,    98,
      99,   100,   101,   102,   103,     0,   104,   105,   106,   107,
       0,     0,     0,     0,   365,     0,     0,   108,     0,     0,
     109,   110,   111,     0,     0,     0,   112,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,     0,     0,     0,     0,
       0,   121,   122,    98,    99,   100,   101,   102,   103,     0,
     104,   105,   106,   107,     0,     0,     0,     0,   372,     0,
       0,   108,     0,     0,   109,   110,   111,     0,     0,     0,
     112,   113,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
       0,     0,     0,     0,     0,   121,   122,    98,    99,   100,
     101,   102,   103,     0,   104,   105,   106,   107,     0,     0,
       0,     0,   377,     0,     0,   108,     0,     0,   109,   110,
     111,     0,     0,     0,   112,   113,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   114,   115,   116,
     117,   118,   119,   120,     0,     0,     0,     0,     0,   121,
     122,    98,    99,   100,   101,   102,   103,     0,   104,   105,
     106,   107,     0,     0,     0,     0,   379,     0,     0,   108,
       0,     0,   109,   110,   111,     0,     0,     0,   112,   113,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,     0,     0,   121,   122,    98,    99,   100,   101,   102,
     103,     0,   104,   105,   106,   107,     0,     0,     0,     0,
       0,     0,     0,   108,     0,     0,   109,   110,   111,     0,
       0,     0,   112,   113,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,     0,     0,   121,   122,    98,
      99,   100,   101,   102,   103,     0,   104,   105,   106,   107,
       0,     0,     0,     0,     0,     0,     0,   108,     0,     0,
     109,   110,   111,     0,     0,     0,   112,   113,    98,    99,
     100,   101,   102,   103,     0,   104,   105,   106,   107,   114,
     115,   116,   117,   118,   119,   120,   108,     0,     0,   109,
     110,   111,   122,     0,     0,   112,   113,    98,    99,   100,
     101,   102,   103,     0,   104,   105,   106,   107,   114,   115,
     116,   117,   118,     0,   120,   108,     0,     0,   109,   110,
     111,   122,     0,     0,   112,   113,    98,    99,   100,   101,
     102,   103,     0,   104,     0,   106,   107,   114,   115,   116,
     117,     0,     0,   120,   108,     0,     0,   109,   110,   111,
     122,     0,     0,   112,   113,    98,    99,   100,   101,   102,
     103,     0,   104,     0,   106,   107,   114,   115,   116,   117,
       0,     0,   120,     0,     0,     0,   109,   110,   111,   122,
       0,     0,   112,   113,    98,    99,   100,   101,   102,   103,
       0,   104,     0,     0,   107,   114,   115,   116,   117,     0,
       0,   120,     0,     0,     0,   109,   110,   111,   122,     0,
       0,   112,   113,    98,    99,   100,   101,   102,   103,     0,
     104,     0,     0,   107,   114,   115,   116,   117,     0,     0,
     120,     0,     0,     0,   109,   110,   111,   122,     0,     0,
     112,   113,    98,    99,   100,   101,     0,     0,     0,   104,
       0,     0,   107,     0,    76,   116,   117,     0,     0,   120,
       0,     0,     0,   109,   110,   111,   122,     0,     0,   112,
     113,     0,    77,    78,   123,     0,     0,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,     0,   120,   253,
       0,     0,    77,    78,     0,   122,    89,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    77,    78,     0,
       0,     0,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88
};

static const yytype_int16 yycheck[] =
{
       2,    45,     2,   230,   193,   225,   201,   202,     6,   248,
      12,   190,   200,   235,   132,   145,    56,    54,    16,   316,
      56,    37,     3,   211,     5,     6,     7,    49,    49,     3,
      54,    37,   325,    56,     5,     6,    28,    56,    54,   223,
     224,    60,   335,     0,   239,    58,    56,    58,    54,    54,
     347,    32,    37,    90,    35,    36,    37,    54,    98,    40,
      41,    42,    98,    44,   291,    57,    54,    91,    54,    91,
      91,   293,   261,    54,   269,    98,   264,   272,   273,   197,
     275,    62,    63,   278,   279,   215,    60,   266,    98,    54,
       5,     6,    54,    54,    54,    58,   315,   316,    36,    58,
      90,   328,   329,    50,    54,   300,   345,   302,   303,    24,
      91,    54,   110,   111,    54,   334,   335,    36,    65,    66,
     340,    14,    37,    38,    36,   320,    54,    24,   347,    59,
      54,    37,   351,    37,   136,    37,   136,   357,    45,    46,
      37,    38,   369,    50,    55,     3,    53,     5,     6,     7,
      23,     9,    23,    11,    54,    13,    14,    15,    65,    66,
      56,    19,    20,    21,    22,     3,    24,    25,    26,    27,
      55,    54,    56,    59,    32,    36,   220,    35,    36,    37,
      57,    39,    40,    41,    42,    59,    44,    59,    37,    40,
      54,    37,    55,    98,    56,     8,    54,    26,    55,   201,
     202,    55,    58,    55,    62,    63,     3,    55,     5,     6,
       7,    58,     9,    55,    11,    59,    13,    14,    15,    55,
      59,    37,    55,    20,    21,    22,    60,    24,    25,    60,
      27,    59,    29,    59,    59,    32,    55,   239,    35,    36,
      37,    49,    39,    40,    41,    42,     3,    44,     5,     6,
       7,    60,    59,    57,    60,    60,    58,    54,    58,    60,
      49,   263,    49,   263,   335,    62,    63,   269,   307,    -1,
     272,   273,    -1,   275,    -1,    32,   278,   279,    35,    36,
      37,    -1,    -1,    40,    41,    42,    -1,    44,     3,    -1,
       5,     6,     7,    -1,    -1,    -1,    -1,    54,   300,    -1,
     302,   303,    59,    -1,    -1,    62,    63,    -1,    -1,    -1,
      -1,   313,    -1,   313,    -1,    10,    -1,    32,   320,    -1,
      35,    36,    37,    -1,    -1,    40,    41,    42,    -1,    44,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    -1,    54,
      55,    -1,    -1,   345,   346,   345,   346,    62,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,
      -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    64,
      -1,    -1,   374,    68,   374,    -1,    -1,    72,    73,    -1,
      75,    76,    -1,    -1,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    99,   100,   101,   102,   103,    -1,
     105,   106,   107,   108,   109,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   131,   132,     3,    -1,
       5,     6,     7,   138,     9,    -1,    11,   142,    13,    14,
      15,    -1,    -1,    -1,    -1,    20,    21,    22,    -1,    24,
      25,    -1,    27,    -1,    -1,    -1,    -1,    32,    -1,    -1,
      35,    36,    37,    -1,    39,    40,    41,    42,    -1,    44,
      -1,    43,    44,    45,    46,    -1,    -1,    -1,    50,    54,
      -1,    53,    -1,    -1,    59,   190,    -1,    62,    63,    -1,
      -1,    -1,   197,    65,    66,    -1,    -1,    -1,    70,    71,
     205,   206,    -1,   208,   209,   210,    43,    44,    45,    46,
      -1,    -1,    -1,    50,   219,    -1,    53,   222,   223,   224,
     225,   226,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,   241,   242,    -1,    -1,
      -1,     3,    -1,     5,     6,     7,    -1,     9,   253,    11,
      -1,    13,    14,    15,    -1,    -1,    -1,    -1,    20,    21,
      22,   266,    24,    25,    -1,    27,    -1,    -1,    -1,    -1,
      32,    -1,   277,    35,    36,    37,   281,    39,    40,    41,
      42,    -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    60,    -1,
      62,    63,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,    -1,     9,    -1,    11,    -1,    13,    14,
      15,    -1,   327,    -1,    -1,    20,    21,    22,    -1,    24,
      25,    -1,    27,   338,   339,   340,    -1,    32,    -1,    -1,
      35,    36,    37,    -1,    39,    40,    41,    42,    -1,    44,
      -1,   356,   357,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    60,   371,    62,    63,    -1,
       3,   376,     5,     6,     7,    -1,     9,    -1,    11,    -1,
      13,    14,    15,    -1,    -1,    -1,    -1,    20,    21,    22,
      -1,    24,    25,    -1,    27,    -1,    -1,    -1,    -1,    32,
      -1,    -1,    35,    36,    37,    -1,    39,    40,    41,    42,
      -1,    44,    -1,    -1,     3,    -1,     5,     6,     7,    -1,
       9,    54,    11,    -1,    13,    14,    15,    60,    -1,    62,
      63,    20,    21,    22,    -1,    24,    25,    -1,    27,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,
      39,    40,    41,    42,    -1,    44,    -1,    -1,     3,    -1,
       5,     6,     7,    -1,     9,    54,    11,    -1,    13,    14,
      15,    60,    -1,    62,    63,    20,    21,    22,    -1,    24,
      25,    -1,    27,    -1,    -1,    -1,    -1,    32,    -1,    -1,
      35,    36,    37,    -1,    39,    40,    41,    42,     3,    44,
       5,     6,     7,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    -1,
      -1,    -1,     3,    -1,     5,     6,     7,    32,    -1,    -1,
      35,    36,    37,    -1,    -1,    40,    41,    42,    -1,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    32,    -1,    58,    35,    36,    37,    62,    63,    40,
      41,    42,     3,    44,     5,     6,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    -1,    58,    -1,    -1,
      -1,    62,    63,    -1,    -1,    -1,     3,    -1,     5,     6,
       7,    32,    -1,    -1,    35,    36,    37,    -1,    -1,    40,
      41,    42,    -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    32,    -1,    58,    35,    36,
      37,    62,    63,    40,    41,    42,     3,    44,     5,     6,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    62,    63,    -1,    -1,    -1,
       3,    -1,     5,     6,     7,    32,    -1,    -1,    35,    36,
      37,    -1,    -1,    40,    41,    42,     3,    44,     5,     6,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    32,
      -1,    -1,    35,    36,    37,    62,    63,    40,    41,    42,
       3,    44,     5,     6,     7,    32,    -1,    -1,    35,    36,
      37,    54,    55,    40,    41,    42,    -1,    44,    -1,    62,
      63,    -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    32,
      -1,    -1,    35,    36,    37,    62,    63,    40,    41,    42,
      -1,    44,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    54,    50,    51,    52,    53,    -1,    55,    56,    62,
      63,    -1,    -1,    61,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    70,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,    96,    43,
      44,    45,    46,    47,    48,    -1,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    58,    -1,    -1,    61,    -1,    -1,
      64,    65,    66,    -1,    -1,    -1,    70,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,
      -1,    95,    96,    43,    44,    45,    46,    47,    48,    -1,
      50,    51,    52,    53,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      70,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      -1,    -1,    -1,    -1,    -1,    95,    96,    43,    44,    45,
      46,    47,    48,    -1,    50,    51,    52,    53,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    70,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,    95,
      96,    43,    44,    45,    46,    47,    48,    -1,    50,    51,
      52,    53,    -1,    55,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    70,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    88,    89,    -1,    -1,
      -1,    -1,    -1,    95,    96,    43,    44,    45,    46,    47,
      48,    -1,    50,    51,    52,    53,    -1,    55,    -1,    -1,
      -1,    -1,    -1,    61,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    70,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,    96,    43,
      44,    45,    46,    47,    48,    -1,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    58,    -1,    -1,    61,    -1,    -1,
      64,    65,    66,    -1,    -1,    -1,    70,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,
      -1,    95,    96,    43,    44,    45,    46,    47,    48,    -1,
      50,    51,    52,    53,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      70,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      -1,    -1,    -1,    -1,    -1,    95,    96,    43,    44,    45,
      46,    47,    48,    -1,    50,    51,    52,    53,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    70,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,    95,
      96,    43,    44,    45,    46,    47,    48,    -1,    50,    51,
      52,    53,    -1,    -1,    -1,    57,    -1,    -1,    -1,    61,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    70,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    88,    89,    -1,    -1,
      -1,    -1,    -1,    95,    96,    43,    44,    45,    46,    47,
      48,    -1,    50,    51,    52,    53,    -1,    -1,    -1,    -1,
      58,    -1,    -1,    61,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    70,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,    96,    43,
      44,    45,    46,    47,    48,    -1,    50,    51,    52,    53,
      -1,    -1,    -1,    57,    -1,    -1,    -1,    61,    -1,    -1,
      64,    65,    66,    -1,    -1,    -1,    70,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,
      -1,    95,    96,    43,    44,    45,    46,    47,    48,    -1,
      50,    51,    52,    53,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      70,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      -1,    -1,    -1,    -1,    -1,    95,    96,    43,    44,    45,
      46,    47,    48,    -1,    50,    51,    52,    53,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    70,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,    95,
      96,    43,    44,    45,    46,    47,    48,    -1,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    58,    -1,    -1,    61,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    70,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    88,    89,    -1,    -1,
      -1,    -1,    -1,    95,    96,    43,    44,    45,    46,    47,
      48,    -1,    50,    51,    52,    53,    -1,    55,    -1,    -1,
      -1,    -1,    -1,    61,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    70,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,    96,    43,
      44,    45,    46,    47,    48,    -1,    50,    51,    52,    53,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      64,    65,    66,    -1,    -1,    -1,    70,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,
      -1,    95,    96,    43,    44,    45,    46,    47,    48,    -1,
      50,    51,    52,    53,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      70,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      -1,    -1,    -1,    -1,    -1,    95,    96,    43,    44,    45,
      46,    47,    48,    -1,    50,    51,    52,    53,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    70,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,    95,
      96,    43,    44,    45,    46,    47,    48,    -1,    50,    51,
      52,    53,    -1,    55,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    70,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    88,    89,    -1,    -1,
      -1,    -1,    -1,    95,    96,    43,    44,    45,    46,    47,
      48,    -1,    50,    51,    52,    53,    -1,    -1,    -1,    -1,
      58,    -1,    -1,    61,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    70,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,    96,    43,
      44,    45,    46,    47,    48,    -1,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    58,    -1,    -1,    61,    -1,    -1,
      64,    65,    66,    -1,    -1,    -1,    70,    71,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,
      -1,    95,    96,    43,    44,    45,    46,    47,    48,    -1,
      50,    51,    52,    53,    -1,    -1,    -1,    -1,    58,    -1,
      -1,    61,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      70,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      -1,    -1,    -1,    -1,    -1,    95,    96,    43,    44,    45,
      46,    47,    48,    -1,    50,    51,    52,    53,    -1,    -1,
      -1,    -1,    58,    -1,    -1,    61,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    70,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,    95,
      96,    43,    44,    45,    46,    47,    48,    -1,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    58,    -1,    -1,    61,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    70,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    85,    86,    87,    88,    89,    -1,    -1,
      -1,    -1,    -1,    95,    96,    43,    44,    45,    46,    47,
      48,    -1,    50,    51,    52,    53,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    70,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    95,    96,    43,
      44,    45,    46,    47,    48,    -1,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      64,    65,    66,    -1,    -1,    -1,    70,    71,    43,    44,
      45,    46,    47,    48,    -1,    50,    51,    52,    53,    83,
      84,    85,    86,    87,    88,    89,    61,    -1,    -1,    64,
      65,    66,    96,    -1,    -1,    70,    71,    43,    44,    45,
      46,    47,    48,    -1,    50,    51,    52,    53,    83,    84,
      85,    86,    87,    -1,    89,    61,    -1,    -1,    64,    65,
      66,    96,    -1,    -1,    70,    71,    43,    44,    45,    46,
      47,    48,    -1,    50,    -1,    52,    53,    83,    84,    85,
      86,    -1,    -1,    89,    61,    -1,    -1,    64,    65,    66,
      96,    -1,    -1,    70,    71,    43,    44,    45,    46,    47,
      48,    -1,    50,    -1,    52,    53,    83,    84,    85,    86,
      -1,    -1,    89,    -1,    -1,    -1,    64,    65,    66,    96,
      -1,    -1,    70,    71,    43,    44,    45,    46,    47,    48,
      -1,    50,    -1,    -1,    53,    83,    84,    85,    86,    -1,
      -1,    89,    -1,    -1,    -1,    64,    65,    66,    96,    -1,
      -1,    70,    71,    43,    44,    45,    46,    47,    48,    -1,
      50,    -1,    -1,    53,    83,    84,    85,    86,    -1,    -1,
      89,    -1,    -1,    -1,    64,    65,    66,    96,    -1,    -1,
      70,    71,    43,    44,    45,    46,    -1,    -1,    -1,    50,
      -1,    -1,    53,    -1,    49,    85,    86,    -1,    -1,    89,
      -1,    -1,    -1,    64,    65,    66,    96,    -1,    -1,    70,
      71,    -1,    67,    68,    49,    -1,    -1,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    -1,    89,    49,
      -1,    -1,    67,    68,    -1,    96,    91,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    67,    68,    -1,
      -1,    -1,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   100,   101,     0,     3,     5,     6,     7,     9,    11,
      13,    14,    15,    20,    21,    22,    24,    25,    27,    29,
      32,    35,    36,    37,    39,    40,    41,    42,    44,    54,
      62,    63,   102,   103,   106,   107,   112,   113,   120,   123,
     124,   125,   126,   127,   128,   129,   130,   133,   134,   135,
      37,    54,    37,    54,   131,   131,    58,    58,    54,     3,
       7,    58,   128,   133,    54,    59,   106,   119,    54,    54,
      54,   131,    54,    54,    37,    54,    49,    67,    68,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    91,
     132,    54,    90,   128,   128,   128,   128,    58,    43,    44,
      45,    46,    47,    48,    50,    51,    52,    53,    61,    64,
      65,    66,    70,    71,    83,    84,    85,    86,    87,    88,
      89,    95,    96,    49,   132,    90,   108,    36,   110,   111,
     128,    54,    54,   128,    58,   128,   105,    14,    58,   128,
      36,    36,    54,   128,   128,    59,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   141,
     128,   136,   137,    37,    55,   128,   128,   128,   128,   128,
     128,    37,   128,   128,   128,   128,   128,   131,   131,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
      59,   128,    37,    54,    55,    56,    55,    91,   115,   128,
     115,    55,    55,    60,   106,    54,    58,   128,    58,    23,
      23,   128,    55,    55,     3,   104,   107,    55,    56,    56,
      98,    55,    56,    54,    54,    91,    57,   137,   109,   110,
      59,    36,   115,    57,   114,   114,   119,   119,   128,    55,
     128,    58,    58,   128,   128,   128,   114,    59,    59,    60,
     107,    40,   128,    49,   132,   128,   136,   136,   141,   128,
      60,    54,    55,   105,    98,    37,    56,   116,   116,     8,
      55,   119,    55,    55,   128,    55,   128,    58,    55,    55,
      55,    26,   121,   121,    58,    55,   128,    55,    55,    98,
     110,    59,    60,   114,   137,    55,    55,   119,   119,   119,
      55,   119,    55,    55,   128,   119,   119,    59,   122,   128,
      60,    60,    55,   105,   116,    59,    59,   119,   119,   119,
      55,    24,    37,    38,   138,   139,   140,    28,    57,    59,
      60,    55,   113,   117,   118,   139,   117,   119,    49,    49,
      91,    37,    60,   140,   128,   105,   105,    59,    58,    60,
     113,   118,    60,   128,   128,   141,    49,    91,    58,    19,
     121,    60,   117,    58,    58,    58,    98,   128,   141,    57,
      60,    49,    58,    98,   105,   128,    49,    58,   128,    58
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
    { (yyval.stmt) = StrConstNode::alloc( (yyvsp[(1) - (1)].str).lineNumber, (yyvsp[(1) - (1)].str).value, false, true ); ;}
    break;

  case 25:
#line 223 "CMDgram.y"
    {  FunctionDeclStmtNode::pushVariableScope(); ;}
    break;

  case 26:
#line 225 "CMDgram.y"
    { (yyval.stmt) = FunctionDeclStmtNode::alloc( (yyvsp[(1) - (9)].i).lineNumber, (yyvsp[(2) - (9)].s).value, NULL, (yyvsp[(5) - (9)].var), (yyvsp[(8) - (9)].stmt) );  FunctionDeclStmtNode::popVariableScope(); ;}
    break;

  case 27:
#line 227 "CMDgram.y"
    {  FunctionDeclStmtNode::pushVariableScope(); ;}
    break;

  case 28:
#line 229 "CMDgram.y"
    { (yyval.stmt) = FunctionDeclStmtNode::alloc( (yyvsp[(1) - (11)].i).lineNumber, (yyvsp[(4) - (11)].s).value, (yyvsp[(2) - (11)].s).value, (yyvsp[(7) - (11)].var), (yyvsp[(10) - (11)].stmt) );  FunctionDeclStmtNode::popVariableScope(); ;}
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
    { (yyval.expr) = StrConstNode::alloc( CMDGetCurrentLine(), "", false); ;}
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
    { (yyval.expr) = ConditionalExprNode::alloc( (yyvsp[(1) - (5)].expr)->dbgLineNumber, (yyvsp[(1) - (5)].expr), (yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].expr)); ;}
    break;

  case 88:
#line 406 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 89:
#line 408 "CMDgram.y"
    { (yyval.expr) = IntBinaryExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(2) - (3)].i).value, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
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
    { (yyval.expr) = StreqExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), true); ;}
    break;

  case 99:
#line 428 "CMDgram.y"
    { (yyval.expr) = StreqExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), false); ;}
    break;

  case 100:
#line 430 "CMDgram.y"
    { (yyval.expr) = StrcatExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), (yyvsp[(2) - (3)].i).value); ;}
    break;

  case 101:
#line 432 "CMDgram.y"
    { (yyval.expr) = IntUnaryExprNode::alloc((yyvsp[(1) - (2)].i).lineNumber, (yyvsp[(1) - (2)].i).value, (yyvsp[(2) - (2)].expr)); ;}
    break;

  case 102:
#line 434 "CMDgram.y"
    { (yyval.expr) = IntUnaryExprNode::alloc((yyvsp[(1) - (2)].i).lineNumber, (yyvsp[(1) - (2)].i).value, (yyvsp[(2) - (2)].expr)); ;}
    break;

  case 103:
#line 436 "CMDgram.y"
    { (yyval.expr) = StrConstNode::alloc( (yyvsp[(1) - (1)].str).lineNumber, (yyvsp[(1) - (1)].str).value, true); ;}
    break;

  case 104:
#line 438 "CMDgram.y"
    { (yyval.expr) = FloatNode::alloc( (yyvsp[(1) - (1)].f).lineNumber, (yyvsp[(1) - (1)].f).value ); ;}
    break;

  case 105:
#line 440 "CMDgram.y"
    { (yyval.expr) = IntNode::alloc( (yyvsp[(1) - (1)].i).lineNumber, (yyvsp[(1) - (1)].i).value ); ;}
    break;

  case 106:
#line 442 "CMDgram.y"
    { (yyval.expr) = ConstantNode::alloc( (yyvsp[(1) - (1)].i).lineNumber, StringTable->insert("break")); ;}
    break;

  case 107:
#line 444 "CMDgram.y"
    { (yyval.expr) = SlotAccessNode::alloc( (yyvsp[(1) - (1)].slot).lineNumber, (yyvsp[(1) - (1)].slot).object, (yyvsp[(1) - (1)].slot).array, (yyvsp[(1) - (1)].slot).slotName ); ;}
    break;

  case 108:
#line 446 "CMDgram.y"
    { (yyval.expr) = InternalSlotAccessNode::alloc( (yyvsp[(1) - (1)].intslot).lineNumber, (yyvsp[(1) - (1)].intslot).object, (yyvsp[(1) - (1)].intslot).slotExpr, (yyvsp[(1) - (1)].intslot).recurse); ;}
    break;

  case 109:
#line 448 "CMDgram.y"
    { (yyval.expr) = ConstantNode::alloc( (yyvsp[(1) - (1)].s).lineNumber, (yyvsp[(1) - (1)].s).value ); ;}
    break;

  case 110:
#line 450 "CMDgram.y"
    { (yyval.expr) = StrConstNode::alloc( (yyvsp[(1) - (1)].str).lineNumber, (yyvsp[(1) - (1)].str).value, false); ;}
    break;

  case 111:
#line 452 "CMDgram.y"
    { (yyval.expr) = (ExprNode*)VarNode::alloc( (yyvsp[(1) - (1)].s).lineNumber, (yyvsp[(1) - (1)].s).value, NULL); ;}
    break;

  case 112:
#line 454 "CMDgram.y"
    { (yyval.expr) = (ExprNode*)VarNode::alloc( (yyvsp[(1) - (4)].s).lineNumber, (yyvsp[(1) - (4)].s).value, (yyvsp[(3) - (4)].expr) ); ;}
    break;

  case 113:
#line 456 "CMDgram.y"
    {
         const U32 bufLen = 64;
         UTF8 buffer[bufLen];
         dSprintf(buffer, bufLen, "__anonymous_function%d", gAnonFunctionID++);
         StringTableEntry fName = StringTable->insert(buffer);
         StmtNode *fndef = FunctionDeclStmtNode::alloc((yyvsp[(1) - (7)].i).lineNumber, fName, NULL, (yyvsp[(3) - (7)].var), (yyvsp[(6) - (7)].stmt));

         (yyval.expr) = StrConstNode::alloc( (yyvsp[(1) - (7)].i).lineNumber, (UTF8*)fName, false );
      ;}
    break;

  case 114:
#line 469 "CMDgram.y"
    { (yyval.slot).lineNumber = (yyvsp[(1) - (3)].expr)->dbgLineNumber; (yyval.slot).object = (yyvsp[(1) - (3)].expr); (yyval.slot).slotName = (yyvsp[(3) - (3)].s).value; (yyval.slot).array = NULL; ;}
    break;

  case 115:
#line 471 "CMDgram.y"
    { (yyval.slot).lineNumber = (yyvsp[(1) - (6)].expr)->dbgLineNumber; (yyval.slot).object = (yyvsp[(1) - (6)].expr); (yyval.slot).slotName = (yyvsp[(3) - (6)].s).value; (yyval.slot).array = (yyvsp[(5) - (6)].expr); ;}
    break;

  case 116:
#line 476 "CMDgram.y"
    { (yyval.intslot).lineNumber = (yyvsp[(1) - (3)].expr)->dbgLineNumber; (yyval.intslot).object = (yyvsp[(1) - (3)].expr); (yyval.intslot).slotExpr = (yyvsp[(3) - (3)].expr); (yyval.intslot).recurse = false; ;}
    break;

  case 117:
#line 478 "CMDgram.y"
    { (yyval.intslot).lineNumber = (yyvsp[(1) - (3)].expr)->dbgLineNumber; (yyval.intslot).object = (yyvsp[(1) - (3)].expr); (yyval.intslot).slotExpr = (yyvsp[(3) - (3)].expr); (yyval.intslot).recurse = true; ;}
    break;

  case 118:
#line 483 "CMDgram.y"
    { (yyval.expr) = ConstantNode::alloc( (yyvsp[(1) - (1)].s).lineNumber, (yyvsp[(1) - (1)].s).value ); ;}
    break;

  case 119:
#line 485 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;

  case 120:
#line 490 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (1)].i).lineNumber; (yyval.asn).token = '+'; (yyval.asn).expr = FloatNode::alloc( (yyvsp[(1) - (1)].i).lineNumber, 1 ); ;}
    break;

  case 121:
#line 492 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (1)].i).lineNumber; (yyval.asn).token = '-'; (yyval.asn).expr = FloatNode::alloc( (yyvsp[(1) - (1)].i).lineNumber, 1 ); ;}
    break;

  case 122:
#line 494 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '+'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 123:
#line 496 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '-'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 124:
#line 498 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '*'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 125:
#line 500 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '/'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 126:
#line 502 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '%'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 127:
#line 504 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '&'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 128:
#line 506 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '^'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 129:
#line 508 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = '|'; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 130:
#line 510 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = opSHL; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 131:
#line 512 "CMDgram.y"
    { (yyval.asn).lineNumber = (yyvsp[(1) - (2)].i).lineNumber; (yyval.asn).token = opSHR; (yyval.asn).expr = (yyvsp[(2) - (2)].expr); ;}
    break;

  case 132:
#line 517 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 133:
#line 519 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 134:
#line 521 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].od); ;}
    break;

  case 135:
#line 523 "CMDgram.y"
    { (yyval.expr) = AssignExprNode::alloc( (yyvsp[(1) - (3)].s).lineNumber, (yyvsp[(1) - (3)].s).value, NULL, (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 136:
#line 525 "CMDgram.y"
    { (yyval.expr) = AssignExprNode::alloc( (yyvsp[(1) - (6)].s).lineNumber, (yyvsp[(1) - (6)].s).value, (yyvsp[(3) - (6)].expr), (yyvsp[(6) - (6)].expr)); ;}
    break;

  case 137:
#line 527 "CMDgram.y"
    { (yyval.expr) = AssignOpExprNode::alloc( (yyvsp[(1) - (2)].s).lineNumber, (yyvsp[(1) - (2)].s).value, NULL, (yyvsp[(2) - (2)].asn).expr, (yyvsp[(2) - (2)].asn).token); ;}
    break;

  case 138:
#line 529 "CMDgram.y"
    { (yyval.expr) = AssignOpExprNode::alloc( (yyvsp[(1) - (5)].s).lineNumber, (yyvsp[(1) - (5)].s).value, (yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].asn).expr, (yyvsp[(5) - (5)].asn).token); ;}
    break;

  case 139:
#line 531 "CMDgram.y"
    { (yyval.expr) = SlotAssignOpNode::alloc( (yyvsp[(1) - (2)].slot).lineNumber, (yyvsp[(1) - (2)].slot).object, (yyvsp[(1) - (2)].slot).slotName, (yyvsp[(1) - (2)].slot).array, (yyvsp[(2) - (2)].asn).token, (yyvsp[(2) - (2)].asn).expr); ;}
    break;

  case 140:
#line 533 "CMDgram.y"
    { (yyval.expr) = SlotAssignNode::alloc( (yyvsp[(1) - (3)].slot).lineNumber, (yyvsp[(1) - (3)].slot).object, (yyvsp[(1) - (3)].slot).array, (yyvsp[(1) - (3)].slot).slotName, (yyvsp[(3) - (3)].expr)); ;}
    break;

  case 141:
#line 535 "CMDgram.y"
    { (yyval.expr) = SlotAssignNode::alloc( (yyvsp[(1) - (5)].slot).lineNumber, (yyvsp[(1) - (5)].slot).object, (yyvsp[(1) - (5)].slot).array, (yyvsp[(1) - (5)].slot).slotName, (yyvsp[(4) - (5)].expr)); ;}
    break;

  case 142:
#line 540 "CMDgram.y"
    { (yyval.expr) = FuncCallExprNode::alloc( (yyvsp[(1) - (4)].s).lineNumber, (yyvsp[(1) - (4)].s).value, NULL, (yyvsp[(3) - (4)].expr), false); ;}
    break;

  case 143:
#line 542 "CMDgram.y"
    { (yyval.expr) = FuncCallExprNode::alloc( (yyvsp[(1) - (6)].s).lineNumber, (yyvsp[(3) - (6)].s).value, (yyvsp[(1) - (6)].s).value, (yyvsp[(5) - (6)].expr), false); ;}
    break;

  case 144:
#line 544 "CMDgram.y"
    { (yyvsp[(1) - (6)].expr)->append((yyvsp[(5) - (6)].expr)); (yyval.expr) = FuncCallExprNode::alloc( (yyvsp[(1) - (6)].expr)->dbgLineNumber, (yyvsp[(3) - (6)].s).value, NULL, (yyvsp[(1) - (6)].expr), true); ;}
    break;

  case 145:
#line 549 "CMDgram.y"
    { (yyval.expr) = AssertCallExprNode::alloc( (yyvsp[(1) - (4)].i).lineNumber, (yyvsp[(3) - (4)].expr), NULL ); ;}
    break;

  case 146:
#line 551 "CMDgram.y"
    { (yyval.expr) = AssertCallExprNode::alloc( (yyvsp[(1) - (6)].i).lineNumber, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].str).value ); ;}
    break;

  case 147:
#line 556 "CMDgram.y"
    { (yyval.expr) = NULL; ;}
    break;

  case 148:
#line 558 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 149:
#line 563 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 150:
#line 565 "CMDgram.y"
    { ((yyvsp[(1) - (3)].expr))->append((yyvsp[(3) - (3)].expr)); (yyval.expr) = (yyvsp[(1) - (3)].expr); ;}
    break;

  case 151:
#line 570 "CMDgram.y"
    { (yyval.slist) = NULL; ;}
    break;

  case 152:
#line 572 "CMDgram.y"
    { (yyval.slist) = (yyvsp[(1) - (1)].slist); ;}
    break;

  case 153:
#line 577 "CMDgram.y"
    { (yyval.slist) = (yyvsp[(1) - (1)].slist); ;}
    break;

  case 154:
#line 579 "CMDgram.y"
    { (yyvsp[(1) - (2)].slist)->append((yyvsp[(2) - (2)].slist)); (yyval.slist) = (yyvsp[(1) - (2)].slist); ;}
    break;

  case 155:
#line 584 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (4)].s).lineNumber, NULL, NULL, (yyvsp[(1) - (4)].s).value, (yyvsp[(3) - (4)].expr)); ;}
    break;

  case 156:
#line 586 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (5)].i).lineNumber, NULL, NULL, (yyvsp[(2) - (5)].s).value, (yyvsp[(4) - (5)].expr), (yyvsp[(1) - (5)].i).value); ;}
    break;

  case 157:
#line 588 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (4)].i).lineNumber, NULL, NULL, StringTable->insert("datablock"), (yyvsp[(3) - (4)].expr)); ;}
    break;

  case 158:
#line 590 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (7)].s).lineNumber, NULL, (yyvsp[(3) - (7)].expr), (yyvsp[(1) - (7)].s).value, (yyvsp[(6) - (7)].expr)); ;}
    break;

  case 159:
#line 592 "CMDgram.y"
    { (yyval.slist) = SlotAssignNode::alloc( (yyvsp[(1) - (8)].i).lineNumber, NULL, (yyvsp[(4) - (8)].expr), (yyvsp[(2) - (8)].s).value, (yyvsp[(7) - (8)].expr), (yyvsp[(1) - (8)].i).value); ;}
    break;

  case 160:
#line 597 "CMDgram.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 161:
#line 599 "CMDgram.y"
    { (yyval.expr) = CommaCatExprNode::alloc( (yyvsp[(1) - (3)].expr)->dbgLineNumber, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3127 "CMDgram.c"
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


#line 601 "CMDgram.y"


