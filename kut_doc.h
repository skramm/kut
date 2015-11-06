/**
\file kut.h
\brief All the features are contained in this file.
Contains a set of macros used for regressions tests (unit testing).
See <a href="index.html">main page</a>.

\mainpage kut : A basic C++ unit-testing tool

\section features Features
This software provided a minimalistic framework for unit testing of C++ classes and functions (regression tests).
What you have to do:
- write your tests, using the provided macros (see below), and create a test 'main()'.

What this code does for you :
- run all the tests, and log information on which one failed, and which one succeded
(text log file).

It also is totally transparent: no overhead at all in your production build.

It is solely based on preprocessor (macros), no need for linking with anything, no class inheritance.
Only one header file to add to your project.

Two modes of logging are possible, "full" logging, by enabling the verbose mode, or reduced mode, where there is
only the number of tests and failures printed for each unit test. This is done
by defining the symbol KUT_VERBOSE_MODE to true or false, see around line 400

\section auth Author, licence, release
- author : Sebastien Kramm, mail is firstname DOT lastname AT univ-rouen DOT fr. Feel free to write.
- Licence : LGPL, version 3. see included file lgpl.txt, along with gpl.txt
- Release: see KUT_VERSION and \ref changelog

\section usage Usage
Detailed usage steps:
- create a new build configuration to your project, named 'test' (or whatever, but 'test' is nice, isn't it ?)
- in your project/makefile, define the symbol TESTMODE, only in 'test' build
- add to this build a new file named 'testfile.cpp' (or whatever...)
- in this file, include kut.h, and create a 'main()' function, that will be the main test function
- inside this function, call all the classes test member functions, using the provided macros.
see file demo/testfile.cpp as an example, or \ref main.

The test app will return as an int value the total number of failures, so it can be driven by an automated script.

Short I/O will be on stdout/cout (screen by default) for a quick overview of the result (one line per unit-test of a class or a function),
and in a redirected stderr/cerr file (stderr_kut.txt), where detailed information on each test will be printed.

There are no classes nor namespaces to create, everything is added to your own code. The only data type is basically a counting structure (KUT_TYPE).
No dependencies other than standard C++ libraries.

See the following pages for details:
- \subpage main
- \subpage class_test
- \subpage function_test
- \subpage macros
- \subpage iterative

\section requi Requirements and limitations
-# This framework assumes all header files have some common header file, that is included everywhere.
Then, all you need to do is to add to THIS file the following lines:
\verbatim
#ifdef TESTMODE
	#include "kut.h"
#endif
\endverbatim
Alternatively, you can also manually add these lines to all your headers (but this is rather tedious.)
-# The tested classes need to have a default constructor (no arguments).
This can be a limitation in some situations.
-# You need to manually write the 'main()' function that calls all the unit tests (see \ref main).
While this is ok for a medium-sized projects,
it will get complicated for large-sized applications with hundreds of classes...
-# You can not use this for testing a function whose definition lies in the same file as the application 'main()' function.
This is because you can not have two 'main()' functions in a program, so the file where the "regular" main() lies can not be
linked to the test application.
Workaround: move that function to some other file.
-# You cannot use it to test exception throwing on constructors (for testing exceptions in a general situation, see KUT_TRY_THROW and KUT_TRY_NOTHROW)
-# It \b cannot be used to test templated classes.


\section motiv Motivation
This started when I realised the importance of regression test for some project, while not really having time to get into
existing libraries. So I started putting together some macros to simplify writing the tests. A couple of weeks later, after continuous mods,
I came up with this, that I consider being usable for other people.

The aim was to have something simple, usable, but with no static linking to external lib, nor class inheritance. All of this lies in a single file.
It is less powerful than other c++ test frameworks, but I use it on some projects, and find it ok for my needs.
If you want something with more power (and more hassles...), I suggest you try
<a href="http://code.google.com/p/googletest/" target="gt">Googletest</a>
or
<a href="http://sourceforge.net/apps/mediawiki/cppunit" target="cppunit">cppunit</a>.
If you want more information about C++ unit testing, you can
start reading <a href="http://en.wikipedia.org/wiki/Unit_testing" target="w">this</a>.


\section misc Miscellenaous

- This framework assumes the << operator of each class prints a end-of-line

\section history Release history

- 20120518:
 - grouped master counting variables under struct KUT_MASTER

- 20110429:
 - added KUT_FAILED and moved the fail flag to KUT_TYPE

- 20110303:
 - added 'dec' flag to cerr
 - included flag 'StopTestOnFail' into the KUT_TYPE struct, so user can order to exit if a test fails.
 - Added macro KUT_EXECFUNC, for adding local functions to the test functions
 - completed doc
 - Added "loop" macros (see \ref iterative)
 - Added "verbose" on/off functionality (see \ref verbose)

 - 20100109 : first release


\section errors Test building potential errors

- If you get the following compiler error message (for gcc):
\verbatim
'class XXXXXX' has no member named 'KUT_CUTM'
\endverbatim
This means you tried to build the main testfile without having defined the symbol TESTMODE in you makefile/project file

- If you get the following linker error message:
\verbatim
"undefined reference to XXXXXX:KUT_CUTM()'
\endverbatim
this means you forgot to define the test function. See \ref class_test.

- If you get:
\verbatim
error: no match for 'operator<<' in ...
\endverbatim
This means you attempted to use a test macro that uses the stream operator, and that the tested class does not have one.
Use the _NS macro version instead.

- If you get:
\verbatim
error: expected constructor, destructor, or type conversion before ‘(’ token
\endverbatim
This means you probably forgot to include the class declaration inside the test code.

*/
//--------------------------------------------------------------------------------------------
/**
\page changelog Changelog
release:
- 2010-10 : first release
- 2011-04-29:
 - added iterative test functionality, see \ref iterative.
- 2011-12-03:
 - logfile is now separated from cerr, to make debugging in test build more easy. stderr/cerr is automatically logged into another file.
- 2012-11-20:
 - added a global log file line counter, so that at the end, the user can know at which logfile line he needs to check
 to get information on what failure happened.
 - added two macros (KUT_TRY_THROW and KUT_TRY_NOTHROW) to test function that may throw an exception.

*/

//--------------------------------------------------------------------------------------------
/**
\page main The main testfile program

It should look like the following:

\code
// here, include the needed headers (your stuff)
#include "classA.hpp"
#include "classB.hpp"

#include "kut.h"

KUT_ALLOC;	// memory allocation for some global data

// main test function
int main()
{
	KUT_MAIN_START;

	KUT_TEST_CLASS( ClassA );	// call of unit-test function of ClassA
	KUT_TEST_CLASS( ClassB );	// call of unit-test function of ClassB
	KUT_TEST_FUNC( MyTestFunc );	// call of unit-test function "MyTestFunc"
	...

	KUT_MAIN_END;
}
\endcode

<hr>
\b Navigation
- \ref index
- \ref main
- \ref class_test
- \ref function_test
- \ref macros
- \ref iterative
*/

//--------------------------------------------------------------------------------------------
/**
\page class_test How to test a class

Unit testing of a class is done in a special member functions, that will be linked
with the code only in test mode. These member functions need to be declared in \b your class. This is done with the macro KUT_CLASS_DECLARE, as shown below (\b no semi-colon!):
\code
class MyClassName
{
	... your stuff

#ifdef TESTMODE
	KUT_CLASS_DECLARE
#endif
};
\endcode

Then, you need to define this function, and write the tests. It can be written in the class implementation file,
or in another file, it's up to you.
In the example below, it lives in the classes implementation file.

\code
// file : MyClassName.cpp
#include "MyClassName.h"

... here, your stuff

//-------------------------
#ifdef TESTMODE
KUT_DEF_TEST_METHOD( MyClassName )
{
	KUT_CTM_START( MyClassName ); // Class Test Method - start

	MyClassName a1,a2;
	KUT_MSG( "First, we test if these things are equal or diff" );
	KUT_EQ( a1 , a2 );

	... go on with testing

	KUT_CTM_END;	// Class Test Method - end
}
#endif
\endcode


If you write this function in a dedicated test file, (say, inside a 'test' folder), than you can omit the
"#ifdef/#endif TESTMODE" lines. But remember to configure your makefile/project file to link this file only in "test" build.

\code
// file : test_MyClassName.cpp
#include "MyClassName.h"

KUT_DEF_TEST_METHOD( MyClassName )
{
	KUT_CTM_START( MyClassName ); // Class Test Method - start

	MyClassName a1,a2;
	KUT_MSG( "First, we test if these things are equal or diff" );
	KUT_EQ( a1 , a2 );

	... go on with testing

	KUT_CTM_END;	// Class Test Method - end
}
\endcode


<hr>
\b Navigation
- \ref index
- \ref main
- \ref class_test
- \ref function_test
- \ref macros
- \ref iterative

*/
//--------------------------------------------------------------------------------------------
/**
\page function_test How to test a function

For testing of functions of any type, you need to write a specific test function somewhere,
that can have any name, but must have the following signature:
\verbatim
KUT_TYPE MyTestFunc()
\endverbatim
It does NOT need to be declared, this is done by the calling macro KUT_TEST_FUNC() (see \ref main).

Inside this function, you can use any of the test macros. The function will be written like this:

\code
KUT_TYPE MyTestFunc()
{
	KUT_FT_START( MyFunc );

	KUT_MSG( "starting the test" );
	KUT_EQF( MyFunc(4), 2 ); // should return 2 if given 4

// here, do the other tests you want to do on function "MyFunc"


	KUT_FT_END;
}
\endcode

See demo/testfile.cpp for an example.


<hr>
\b Navigation
- \ref index
- \ref main
- \ref class_test
- \ref function_test
- \ref macros
- \ref iterative

*/

//--------------------------------------------------------------------------------------------
/**
\page macros How to do the tests
This package provides a set of macros to do the tests. For a complete list, see kut.h

On fail, all of these will have two different behaviors:
 - either it just logs the failure, and goes on with next test.
 - either it stops the current unit test (class or function test). This behavior is provided because in some situations, it has no meaning to go on with the other tests.

You can select between these two behaviours with the macro KUT_STOP_ON_FAIL, giving it 'true' or 'false' as argument.

Every test macro is provided in two versions:
- the regular one, that assumes the class provides a stream operator ( << ), to log it's current value. This way, the object
will be printed out, in case of test failure.
- the _NS (No Stream) version, for classes which don't have this operator.


List of macros:
- KUT_EQ_F : testing of equality of floating point values, using absolute value KUT_EPSILON
- KUT_LESS_F : testing ordering of two floating point values
- KUT_EQ : testing of equality of 2 objects, using their defined (!) == operator
- KUT_DIFF : testing of inequality of 2 objects, using their defined (!) != operator
- KUT_LESS : testing ordering of two objects, using their defined '<' operator
- KUT_TRUE : asserting an expression or an object/value

To be continued...

<hr>
\b Navigation
- \ref index
- \ref main
- \ref class_test
- \ref function_test
- \ref macros
- \ref iterative

*/

//--------------------------------------------------------------------------------------------
/**
\page iterative Iterating several tests

It is sometimes required that the validation of some code needs several tests, with several random values, for instance.
You must check that your code works fine with a number of values.
Sure, you could also use a "for" loop, and use the macros described before.
However, this has the disadvantage of creating a line in the log file for every test, and checking results will
not be easy, if you iterate hundreds or thousands of times.

So another way of doing this is provided here, through the "loop" macros.

For example, this will only count as one test in the log file, even if actually 500 testing operations are made.
\code
// let's iterate a hundred times these 2 tests
	KUT_LOOP_START( 100 )
	{
		KUT_LOOP_TRUE( SomeComputedBool() );
		KUT_LOOP_FALSE( SomeOtherComputedBool() );
		KUT_LOOP_EQU( foo1(), foo2() );
		KUT_LOOP_DIFF( foo3(), foo4() );
		KUT_LOOP_LESS( foo5(), foo6() );
	}
	KUT_LOOP_END;
\endcode

At the end of the test, a log of each tests failure rate will be printed out in the log file.
Please note that this will happen only in 'verbose' mode (see \ref verbose), if not, only one failure
will be reported at the line where KUT_LOOP_END stays. In this case, you need to re-run using verbose mode.

<hr>
\b Navigation
- \ref index
- \ref main
- \ref class_test
- \ref function_test
- \ref macros
- \ref iterative

*/

//-------------------------------------------------------------------------------------------
// eof
