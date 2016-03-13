/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Line 1529 of yacc.c.  */
#line 213 "CMDgram.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE CMDlval;

