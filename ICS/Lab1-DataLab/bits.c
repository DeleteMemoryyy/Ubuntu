/* 
 * CS:APP Data Lab 
 * 
 * <Liu DeXin 1500017704>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 8.0.0.  Version 8.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, plus Amendment 1 (published
   2015-05-15).  */
/* We do not support C11 <threads.h>.  */
//1
/* 
 * thirdBits - return word with every third bit (starting from the LSB) set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int thirdBits(void) 
{
	int t1 = 73^(73<<9);
	return t1 ^ (t1<<18);
}
/*
 * isTmin - returns 1 if x is the minimum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmin(int x) 
{
	return !((x+x)|(!x));
}
//2
/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNotEqual(int x, int y) 
{
	return !!(x^y);
}
/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyOddBit(int x) 
{
	int t1 = (x>>16) | x;
	int t2 = (t1>>8) | t1;
	return !!(t2 & 170);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) 
{
	return (~x)+1;
}
//3
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) 
{
	int t1 = (~(!x)+1); 
	return (~t1&y)|(t1&z);
}
/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) 
{
	int j1 = (x^y);
	int j2 = ((x+((~y)+1))^x);
	return !((j1&j2)>>31);
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) 
{
    int j1 = x^y;
    int j2 = y+(~x+1);
    return !(((j1&x)|(~(j1|j2)))>>31);
}
//4
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int bitParity(int x) 
{
	int x1 = x ^ (x >> 16);
	int x2 = x1 ^ (x1 >> 8);
	int x3 = x2 ^ (x2 >> 4);
	int x4 = x3 ^ (x3 >> 2);
	int x5 = x4 ^ (x4 >> 1);
	return  (x5 & 1);
} 
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) 
{
 	int t = (x >> 31) ^ x;
	int j = (t >> 16);
	int m = ~(!j) + 1;
	int t1 = (t&m) + j;
	int j1 = (t1 >> 8);
	int m1 = ~(!j1) + 1;
	int t2 = (t1&m1) + j1;
	int j2 = (t2 >> 4);
	int m2 = ~(!j2) + 1;
	int t3 = (t2&m2) + j2;
	int j3 = (t3 >> 2);
	int m3 = ~(!j3) + 1;
	int t4 = (t3&m3) + j3;
	int j4 = (t4 >> 1);
	int m4 = ~(!j4) + 1;
	int t5 = (t4&m4) + j4;
	return (~m & 16)+(~m1 & 8)+(~m2 & 4)+(~m3 & 2)+(~m4 & 1)+t5+1; 
}
//float
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) 
{
  unsigned jS = uf & 0x80000000,jE = uf & 0x7f800000,jM = uf & 0x007fffff;
  // judge for rounding
  unsigned jR = (jM << 30) ^ 0xc0000000;
  int flag = 0;
  // NaN or infinity
  if(!(jE ^ 0x7f800000))
  {
    return uf;
  }
  else
    {
      // denormalized
      if(!jE)
      {
        flag = 1;
      }
      // normalized
      else
      {
        jE = jE + 0xff800000;
        // E == 1
        if(!jE)
        {
         jM = jM + 0x00800000;
         flag = 1; 
        }
      }
    }
    if(flag)
    {
      if(!jR)
        {
          ++jM;
        }
        jM = (jM >> 1);
    }  
    return jS | jE | jM;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) 
{
  unsigned jS = x & 0x80000000,jX,cE = 0,mE,tX,M1,M2,M3,res,flag = 0;
  if (x == 0)
  {
    return 0;
  }
  if (x < 0)
  {
    x = -x;
  }
  jX = x;
  tX = jX;
  while(tX > 1)
  {
    tX /= 2;
    ++cE;
  }
  tX = jX - (1 << cE);
  if(cE <= 23)
  {
    tX = (tX << (23 - cE));
  }
  else
  {
    flag = 1;
    // mE = cE - 24;
    // M = (((0xffffffffu >> (30 - mE))) & tX);
    // tX = (tX >> (mE+1));
    mE = cE - 24;
    // M1 = 0xffffffffu & ((tX << (32 - mE) << 1));
    // tX = (tX >> (mE - 1));

    if(!mE)
    {
      M1 = 0;
    }
    else
    {
      M1 = 0xffffffffu & ((tX << (32 - mE)));
      tX = (tX >> mE);
    }
    M2 = tX & 1;
    tX = (tX >> 1);
    M3 = tX & 1;
  }
  res = jS | ((cE + 127) << 23) | tX;
  if(flag)
  {
    // if((M > (1 << mE) && M < (2 << mE)) || (M>=(3 << mE)))
    if (M2 && (M3 || (!M3 && M1)))
    {
      ++res;
    }
  }
  return res;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) 
{
  int jS = (uf >> 31), jX = uf & 0x7fffffff, jE = ((uf & 0x7f800000) >> 23) - 127, jM = uf & 0x007fffff;
  jM |= 0x00800000;
  if(jX >= 0x4f000000)
  {
    return 0x80000000u;
  }
  if(jX < 0x3f800000)
  {
    return 0;
  }
  if(jE >= 23)
  {
    jM <<= (jE - 23);
  }
  else
  {
    jM >>= (23 - jE);
  }
  if(jS)
  {
    jM = -jM;
  }
  return jM;
}
