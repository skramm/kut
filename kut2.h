/**
\file kut.h
\brief All the features are contained in this file.
Contains a set of macros used for regressions tests (unit testing).
See <a href="index.html">main page</a>.
*/

//-------------------------------------------------------------------------------------------
// this is the "real" beginning of file...
#ifndef _KUT_H_
#define _KUT_H_

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>


/// current version of file, printed in logfile
#define KUT_VERSION "20151018"

//-------------------------------------------------------------------------------------------
/// \name User interface macros
//@{

/// epsilon value for testing of equality of floating-point values (see KUT_EQF)
#ifndef KUT_EPSILON
	#define KUT_EPSILON 1e-9
#endif

/// the logfile name, to be changed if (really ?) needed
#ifndef KUT_FILENAME
	#define KUT_FILENAME "kut_logfile.txt"
#endif

/// defining this to true will enable the 'verbose' mode
#ifndef KUT_VERBOSE_MODE
	#define KUT_VERBOSE_MODE true
#endif

/// a shorthand logfile
/**
This macro also increments the log file line counter, to keep track of where the failure occurs (printed at the end of all tests)
*/
#define KUT_LOG kut_line_counter++; kut_logfile

/// This one does not increment the counter
#define KUT_LOG2 kut_logfile

/// a shorthand for std::endl
#ifndef ENDL
	#define ENDL std::endl
#endif


/// User macro, if called with 'true', then a failing test will interrupt current unit-test
/**
This is mostly useful when tracking down a bug: instead of letting the whole test suite run each time,
if you stop immediatly after the failed test, you can right away check the last lines of the log file
to see your debug output, instead of having to go through (potential) thousands of debug lines.
*/
#define KUT_STOP_ON_FAIL( a ) \
	if( a ) \
	{ \
		KUT_LOG << " - Switching to mode \"StopTestOnFail\" : ON\n"; \
		kut_data.StopTestOnFail = true; \
	} \
	else \
	{ \
		KUT_LOG << " - Switching to mode \"StopTestOnFail\" : OFF\n"; \
		kut_data.StopTestOnFail = false; \
	} \

//@}

struct KUT_GLOBAL
{
	std::vector<std::string>  kut_fail_file;
	std::vector<unsigned int> kut_fail_line;
	bool                      kut_verbose = KUT_VERBOSE_MODE;
	std::ofstream             kut_logfile;
	size_t                    kut_line_counter = 0;
//	std::vector<std::function<KUT_TYPE()>> kut_vec;
};

extern KUT_GLOBAL kut_global;

/*extern std::vector<std::string>  kut_fail_file;
extern std::vector<unsigned int> kut_fail_line;
extern bool                      kut_verbose;
extern size_t                    kut_line_counter;
*/
//-------------------------------------------------------------------------------------------
/// Internal data structure used, holds several counters. Local to main() test function
struct KUT_MASTER
{
	int NbTestTot;
	int NbFailureTot;
	int NbUnitTests;
	int NbUTFailures;
	std::vector<std::string> v_failed_test_name;    ///< name of class/function whose test failed
	std::vector<int>         v_failed_test_type;    ///< 0 for a class, 1 for a function
	std::vector<size_t>      v_failed_test_logline; ///< logfile line where this failure is reported, see kut_line_counter
	KUT_MASTER()
	{
		NbTestTot    = 0;
		NbFailureTot = 0;
		NbUnitTests  = 0;
		NbUTFailures = 0;
	}

};

//-------------------------------------------------------------------------------------------
/// Internal data structure used, holds several counters related to the current unit-test.
struct KUT_TYPE
{
	int  count_fail; ///< Nb of tests that failed.
	int  count_test; ///< Total nb of tests
	int  count_test1; ///< major text counter
	int  count_test2; ///< minor text counter
	bool StopTestOnFail;
	bool DoQuit;
	bool kut_failflag; ///< used to communicate failure between different parts of macros

//-------------------------------------------------------------------------------------------
/// Constructor, initialises all the fields
	KUT_TYPE()
	{
		count_fail  = 0;
		count_test1 = 0;
		count_test2 = 0;
		count_test  = 0;
		StopTestOnFail = false;
		DoQuit = false;
		kut_failflag = false;
	}
};

//-------------------------------------------------------------------------------------------
/// User needs to put this at the beginning of his main test file (global allocation)
#define KUT_ALLOC \
	KUT_GLOBAL kut_global
	std::vector<std::string>  kut_fail_file; \
	std::vector<unsigned int> kut_fail_line; \
	bool                      kut_verbose = KUT_VERBOSE_MODE; \
	std::ofstream             kut_logfile; \
	size_t                    kut_line_counter = 0; \
//	std::vector<std::function<KUT_TYPE()>> kut_vec

/// so that test function are aware of this global
extern std::ofstream kut_logfile;

//-------------------------------------------------------------------------------------------
/// \name Private macros, do not use in your code
//@{

/// Private macro
/**
\todo return statement at the end is never executed !
*/
#define KUT_P_FAILURE \
	{ \
		kut_data.count_fail++; \
		if( kut_verbose ) \
		{ \
			KUT_LOG << "FAIL (" << kut_data.count_fail << "), on line "; \
			KUT_LOG2 << __LINE__ << " of file " << __FILE__; \
		} \
		kut_data.kut_failflag = true; \
		kut_fail_file.push_back( __FILE__ ); \
		kut_fail_line.push_back( __LINE__ ); \
		if( kut_data.StopTestOnFail ) \
		{ \
			std::cout << " -premature ending of test !\n"; \
			kut_data.DoQuit = true; \
			KUT_LOG << "\n- PREMATURE ENDING of test due to failure!\n";\
			KUT_LOG << " - in function : " << __PRETTY_FUNCTION__ << ENDL; \
			KUT_LOG << " - Actual status : " << kut_data.count_test <<" tests done and " << kut_data.count_fail<<" failure(s)\n\n"; \
			exit(1); \
		} \
	}

/// Common code, to do right after the test
#define KUT_P11 \
		{ \
			if( kut_verbose ) \
				KUT_LOG2 << "PASS"; \
		} \
		else \
		{ \
			KUT_P_FAILURE \
		} \

/// streams value of the 2 arguments to log file, if test failed
#define KUT_P_STREAM_VALUES( a, b ) \
		if( kut_data.kut_failflag && kut_verbose ) \
		{ \
			KUT_LOG << "  -first value : \"" << #a << "\" = \"" << (a) << "\"" << ENDL; \
			KUT_LOG << "  -second value: \"" << #b << "\" = \"" << (b) << "\"" << ENDL; \
		}

/// prepare test
#define KUT_P2 \
		kut_data.count_test++; \
		kut_data.count_test2++; \
		if( kut_verbose ) \
		{ \
			KUT_LOG << std::dec<< " * Test " << kut_data.count_test << " (" << kut_data.count_test1 << "." << kut_data.count_test2 <<"), line: " << __LINE__  << ": "; \
		} \
		kut_data.kut_failflag = false

/// Private macro for comparing (== operator)
#define KUT_P_EQ( a, b ) \
	KUT_P2; \
	if( (a) == (b) ) \
		KUT_P11 \
	if( kut_verbose ) \
		KUT_LOG2 << ", expression: " << #a << " == " << #b << ENDL

/// Private macro for difference operator
#define KUT_P_DIFF( a, b ) \
	KUT_P2; \
	if( (a) != (b) ) \
		KUT_P11 \
	if( kut_verbose ) \
		KUT_LOG2 << ", expression: " << #a << " != " << #b << ENDL

/// Private macro for comparing (< operator)
#define KUT_P_LESS( a, b ) \
	KUT_P2; \
	if( (a) < (b) ) \
		KUT_P11 \
	if( kut_verbose ) \
		KUT_LOG2 << ", expression: " << #a << " < " << #b << ENDL

/// Private macro for 'true' test
#define KUT_P_TRUE( a ) \
	KUT_P2; \
	if( a ) \
		KUT_P11; \
	if( kut_verbose ) \
		KUT_LOG2 << ", expression: " << #a << " is TRUE" << ENDL \

/// Private macro for 'false' test
#define KUT_P_FALSE( a ) \
	KUT_P2; \
	if( false == (a) ) \
		KUT_P11; \
	if( kut_verbose ) \
		KUT_LOG2 << ", expression: " << #a << " is FALSE" << ENDL \

//#define KUT_P_DOQUIT if(kut_data.DoQuit == true) exit(1)

///@}

//-------------------------------------------------------------------------------------------
/// \name Set of macros to be used inside the test function
//@{

/// A macro useful for documenting the test log file. Argument is a string. Increments counter 1
#define KUT_MSG( a ) \
	{ \
		kut_data.count_test1++; \
		if( kut_verbose ) \
		{ \
			KUT_LOG << ENDL; \
			KUT_LOG <<std::dec<< "* Test msg (" << kut_data.count_test1 << "): " << (a) << " TEMP "<< kut_line_counter << ENDL; \
			std::cerr <<std::dec<< "* Test msg (" << kut_data.count_test1 << "): " << (a) << ENDL; \
		} \
		kut_data.count_test2 = 0; \
	}

/// Testing equality of floating point values, using absolute epsilon value KUT_EPSILON.
/// You need to include math.h in your code if you want to use this macro (uses \c fabs() ).
/// Streams values in output file on fail.
/** Requirement: \c a and \c b need to have the '-' operator defined, and it needs to return a floating-point value.
*/
#define KUT_EQ_F( a, b ) \
	{ \
		KUT_P2; \
		if( fabs( (a) - (b) ) < KUT_EPSILON ) \
			KUT_P11 \
		if( kut_verbose ) \
			KUT_LOG << ", expr: " << #a << " == " << #b << ENDL; \
		KUT_P_STREAM_VALUES( a, b ); \
	}

// Testing ordering of floating point values, using '<'. You need to include math.h in your code if you want to use this macro.
// Streams values in output file on fail.
/*
#define KUT_LESS_F( a, b ) \
	{ \
		KUT_P2; \
		if( fabs( (a) < (b) ) ) \
			KUT_P11 \
		KUT_LOG << ", expr: " << #a << " < " << #b << ENDL; \
		KUT_P_STREAM_VALUES( a, b ); \
	}
*/



/// Testing equality of 2 objects, by using its '==' operator. Streams values in output file on fail.
#define KUT_EQ( a, b ) \
	{ \
		KUT_P_EQ( a, b ); \
		KUT_P_STREAM_VALUES( a, b ); \
	}

/// testing equality of 2 objects, by using the '==' operator. Does NOT streams values in output file on fail.
#define KUT_EQ_NS( a, b ) \
	{ \
		KUT_P_EQ( a, b ); \
	}

/// just an alias
#define KUT_NEQ KUT_DIFF

/// testing inequality of 2 objects, by using the '!=' operator. Streams values in output file on fail.
#define KUT_DIFF( a, b ) \
	{ \
		KUT_P_DIFF( a, b ); \
		KUT_P_STREAM_VALUES( a, b ); \
	}

/// testing inequality of 2 objects, by using the '!=' operator. Does NOT stream values in output file on fail.
#define KUT_DIFF_NS( a, b ) \
	{ \
		KUT_P_DIFF( a, b ); \
	}

/// testing ordering of 2 objects, by using the '<' operator. Streams values in output file on fail.
#define KUT_LESS( a, b ) \
	{ \
		KUT_P_LESS( a, b ); \
		KUT_P_STREAM_VALUES( a, b ); \
	}

/// testing ordering of 2 objects, by using the '<' operator. Does NOT stream values in output file on fail.
#define KUT_LESS_NS( a, b ) \
	{ \
		KUT_P_LESS( a, b ); \
	}




// testing equality of floating point values \b a and \b b. You need to include math.h in your code if you want to use this macro.
// Does NOT stream values in output file on fail.
/*#define KUT_EQF_NS( a, b ) \
	{ \
		KUT_P2; \
		if( fabs( (a) - (b) ) < KUT_EPSILON ) \
			KUT_P11 \
		KUT_LOG << ", expr: " << #a << " == " << #b << ENDL; \
	}
*/

/// testing if expression \b a evaluates to true. Streams the object in output file on fail.
/**
*/
#define KUT_TRUE( a ) \
	{ \
		KUT_P_TRUE( a ); \
		if( kut_data.kut_failflag && kut_verbose ) \
		{ \
			KUT_LOG << "   - " << #a << " : " << "false" << ENDL; \
		} \
	}

/// testing if expression evaluates to true. Does NOT stream the object in output file on fail.
#define KUT_TRUE_NS( a ) \
	{ \
		KUT_P_TRUE( a ); \
	}


/// testing if expression \b a evaluates to true. Streams the second argument \b b (object) in output file on fail.
/// \todo To be edited...
#define KUT_TRUE_2( a, b ) \
	{ \
		KUT_P2; \
		if( a ) \
			KUT_P11; \
		if( kut_verbose ) \
		{ \
			KUT_LOG << ", expr: " << #a << ENDL; \
			if( kut_data.kut_failflag && kut_verbose ) \
			{ \
				KUT_LOG << "   - " << #b << " : " << (b) << ENDL; \
			} \
		} \
	}

/// testing if expression \b a evaluates to true. Prints the second argument \b b (object) in output, using its memberfunction  "Print( FILE* );"
/// \todo To be edited...
#define KUT_TRUE_2P( a, b ) \
	{ \
		KUT_P2; \
		if( a ) \
			KUT_P11; \
		if( kut_verbose ) \
		{ \
			KUT_LOG << ", expr: " << #a << ENDL; \
			if( kut_data.kut_failflag ) \
			{ \
				KUT_LOG << "   - " << #b << " : "; \
				b.Print( stderr ); \
			} \
		} \
	}

/// Testing if expression \b a evaluates to false. Streams the object in output file on fail.
#define KUT_FALSE( a ) \
	{ \
		KUT_P_FALSE( a ); \
		if( kut_data.kut_failflag ) \
		{ \
			KUT_LOG << "   - " << #a << " : " << true << ENDL; \
		} \
	}

/// Testing if expression evaluates to false. Does NOT stream the object in output file on fail.
#define KUT_FALSE_NS( a ) \
	{ \
		KUT_P_FALSE( a ); \
	}

///@}


//----------------------------------------------------------------------------
/// A macro to be included in each class to be tested
/**
Just add the following lines at the end of the class:
\verbatim
#ifdef TESTMODE
	KUT_CLASS_DECLARE
#endif
\endverbatim
\warning : no semi colon !

For an example, see file demo/classA.hpp

Inside: This macro adds a public method to the class named 'KUT_CUTM()
(which stands for Class Unit Test Method )

*/
#define KUT_CLASS_DECLARE \
	public: \
		KUT_TYPE KUT_CUTM(); \
	private:

//----------------------------------------------------------------------------
/// \name Set of macros for the testing class code
//@{

/// a macro that declares the test member function. \warning No Semicolon !
#define KUT_DEF_TEST_METHOD( a ) KUT_TYPE a::KUT_CUTM()

/// Class Test Method Start. Configures everything for unit test of class \b a (inside Test() function)
#define KUT_CTM_START(a) \
	std::string kut_class_name = #a; \
	KUT_TYPE kut_data; \
	kut_fail_file.clear(); \
	kut_fail_line.clear(); \
	KUT_LOG << "- BEGIN unit test of class " << #a << ", file: " << __FILE__ << " TEMP "<< kut_line_counter << ENDL; \
	std::cerr << "- BEGIN unit test of class " << #a << ", file: " << __FILE__ << ENDL << ENDL; \



/// Class Test Method end. Prints out results of test of class, and returns nb of failures (inside Test() function)
#define KUT_CTM_END \
	KUT_LOG << "- END Unit test of class " << kut_class_name << ", " << kut_data.count_test <<" tests done and " << kut_data.count_fail <<" failure(s)" << " TEMP "<< kut_line_counter << ENDL; \
	if( kut_data.count_fail > 0 ) \
	{ \
		for( size_t kut_i=0; kut_i<kut_fail_line.size(); kut_i++ ) \
		{ \
			KUT_LOG << " - failed at line " << kut_fail_line.at(kut_i) \
				<< " of file " << kut_fail_file.at(kut_i) << ENDL; \
		} \
	} \
	KUT_LOG << ENDL; \
	return kut_data

///@}

//----------------------------------------------------------------------------
/// \name Set of macros to be used inside the test app
//@{

/// Test initialisation.

#define KUT_MAIN_START \
	KUT_MASTER kut_m; \
	std::cout << " Test : start\n"; \
	kut_logfile.open( KUT_FILENAME ); \
	if( !kut_logfile.is_open() ) \
	{ \
		std::cout << "KUT: Unable to open log file " << KUT_FILENAME <<", exiting..." << ENDL; \
		std::cerr << "KUT: Unable to open log file " << KUT_FILENAME <<", exiting..." << ENDL; \
		exit(1); \
	} \
	if( !freopen( "stderr.txt", "wt", stderr ) ) \
	{ \
		std::cout << "KUT: Unable to open stderr/cerr file, exiting..." << ENDL; \
		std::cerr << "KUT: Unable to open stderr/cerr file, exiting..." << ENDL; \
		exit(1); \
	} \
	time_t t = time(0); \
	KUT_LOG << "KUT logfile, created at " << asctime( localtime( &t ) ); \
	KUT_LOG << " - version of KUT is " << KUT_VERSION << ENDL



/// Test end. Returns the total nb of failures
#define KUT_MAIN_END \
	KUT_LOG << "*****************************************************\n"; \
	KUT_LOG << "Test end :"; \
	KUT_LOG << " - Nb of U.T. = "        << kut_m.NbUnitTests  << ENDL; \
	KUT_LOG << " - Nb U.T. Failures = "  << kut_m.NbUTFailures << ENDL; \
	KUT_LOG << " - Total Nb of tests = " << kut_m.NbTestTot    << ENDL; \
	KUT_LOG << " - Total Nb failures = " << kut_m.NbFailureTot << ENDL; \
	if( kut_m.v_failed_test_name.size() ) \
		KUT_LOG << " - List of failed tests:" << ENDL; \
	for( size_t i=0; i<kut_m.v_failed_test_name.size(); i++ ) \
	{ \
		KUT_LOG << i << ": test failed for "; \
		if( kut_m.v_failed_test_type[i]==0 ) \
		{ \
			KUT_LOG2 << "class"; \
		} \
		else \
		{ \
			KUT_LOG2 << "function"; \
		} \
		KUT_LOG2 << ": " << kut_m.v_failed_test_name[i]; \
		KUT_LOG2 << ", see at line " << kut_m.v_failed_test_logline[i] << ENDL; \
	} \
	std::cout << "\n Test end :"; \
	std::cout << "\n - Nb of U.T. = "        << kut_m.NbUnitTests; \
	std::cout << "\n - Nb Failures = "       << kut_m.NbUTFailures; \
	std::cout << "\n - Total Nb of tests = " << kut_m.NbTestTot; \
	std::cout << "\n - Total Nb failures = " << kut_m.NbFailureTot << ENDL; \
	std::cout << " See file " << KUT_FILENAME << " file\n"; \
	return kut_m.NbFailureTot


/// Unit test of a class. This macro is to be used in the main test program.
/**
This macro tests the class 'A', i.e. :
 - creates an object of that type
 - calls the test member function on it (whose name is KUT_CUTM, but you don't really need to know this...)
*/
#define KUT_TEST_CLASS( A ) \
	{ \
		A a; \
		kut_m.NbUnitTests++; \
		KUT_LOG << "*****************************************************\n"; \
		KUT_LOG << "* Unit test no " << kut_m.NbUnitTests << ", testing class " << #A << " TEMP "<< kut_line_counter << ENDL; \
		KUT_TYPE kut_data= a.KUT_CUTM(); \
		std::cout << kut_m.NbUnitTests << " : Unit test of class " << #A << " : " << kut_data.count_test <<  " tests : "; \
		kut_m.NbTestTot += kut_data.count_test; \
		if( kut_data.count_fail == 0 ) \
			std::cout << " success\n"; \
		else \
		{ \
			std::cout << kut_data.count_fail << " failure(s)\n"; \
			kut_m.NbFailureTot += kut_data.count_fail; \
			kut_m.NbUTFailures++; \
			kut_m.v_failed_test_name.push_back( #A ); \
			kut_m.v_failed_test_type.push_back( 0 ); \
			kut_m.v_failed_test_logline.push_back( kut_line_counter ); \
		} \
	}

/// Call of a function dedicated to the test of a global function.
/// This macro is to be used in the main test program.
/**
Please note that the function \c a DOES NOT need to be declared, the macro takes care of this.
*/
#define KUT_TEST_FUNC( a ) \
	{ \
		kut_m.NbUnitTests++; \
		KUT_LOG << "*****************************************************\n"; \
		KUT_LOG << "* Unit test no " << kut_m.NbUnitTests << ", testing function " << #a << " TEMP " << kut_line_counter << ENDL; \
		KUT_TYPE a(); \
		KUT_TYPE kut_data = a(); \
		std::cout << kut_m.NbUnitTests << " : Unit test of function " << #a << " : " << kut_data.count_test <<  " tests : "; \
		kut_m.NbTestTot += kut_data.count_test; \
		if( kut_data.count_fail == 0 ) \
			std::cout << " success\n"; \
		else \
		{ \
			std::cout << kut_data.count_fail << " failure(s)\n"; \
			kut_m.NbFailureTot += kut_data.count_fail; \
			kut_m.NbUTFailures++; \
			kut_m.v_failed_test_name.push_back( #a ); \
			kut_m.v_failed_test_type.push_back( 1 ); \
			kut_m.v_failed_test_logline.push_back( kut_line_counter ); \
		} \
	}

///@}

//----------------------------------------------------------------------------
/// \name two macros for testing of a function
//@{

/// Configures everything for test of function \c a
/** Actually, the argument is only for documentation purposes, not used at present
*/
#define KUT_FT_START(a) \
	KUT_TYPE kut_data; \
	kut_fail_file.clear(); \
	kut_fail_line.clear(); \
	KUT_LOG << "- BEGIN unit test of function '" << #a << "()' through test function "<< __FUNCTION__ << " TEMP "<< kut_line_counter << ENDL; \
	std::cerr << "- BEGIN unit test of function '" << #a << "()' through test function "<< __FUNCTION__ << ENDL;
	kut_global

/// End of test function
#define KUT_FT_END \
	KUT_LOG << "\n- END of test function, "<< kut_data.count_test <<" tests and " << kut_data.count_fail<<" failure(s)\n\n"; \
	return kut_data

/// Useful for using test functions inside a class test, see class1.cpp for an example
#define KUT_EXECFUNC( a ) \
	if( kut_verbose ) \
		KUT_LOG << "\n - Starting function : " << #a << ENDL; \
	kut_data.count_test1++; \
	kut_data.count_test2 = 0; \
	kut_data = a; \
	if( kut_data.DoQuit == true ) \
		return kut_data;
///@}

//----------------------------------------------------------------------------
/// \name Macros allowing iterating several tests (see page \ref iterative)
//@{

/// Start a loop
#define KUT_LOOP_START( nb_iter ) \
	{ \
		int kut_loop_line = __LINE__; \
		if( kut_verbose ) \
			KUT_LOG << std::dec<< " * Test " << ++kut_data.count_test << " (loop type) (" << kut_data.count_test1 << "." << kut_data.count_test2 <<")\n"; \
		std::vector<unsigned int> kut_loop_fails; \
		std::vector<std::string>  kut_loop_expr_a; \
		std::vector<std::string>  kut_loop_expr_b; \
		std::vector<std::string>  kut_loop_expr_op; \
		unsigned int kut_loop_nb_iter = nb_iter; \
		bool kut_fail_flag = false; \
		for( unsigned int kut_i=0; kut_i<nb_iter; kut_i++ ) \
		{ \
			unsigned int kut_loop_macro_count = 0;

/// End a loop
#define KUT_LOOP_END \
		} \
		if( kut_verbose ) \
		{ \
			KUT_LOG << (kut_fail_flag ? "FAIL" : "PASS") << ", " << kut_loop_nb_iter << " iterations, at line "<< kut_loop_line << ENDL; \
			for( size_t kut_i=0; kut_i<kut_loop_fails.size(); kut_i++ ) \
			{ \
				KUT_LOG << "   - subtest " << kut_data.count_test << "." << kut_i+1 << ", expr: " << kut_loop_expr_a.at(kut_i); \
				KUT_LOG2 << "  " << kut_loop_expr_op.at(kut_i) << " " << kut_loop_expr_b.at(kut_i); \
				KUT_LOG2 << ", failed: "<< kut_loop_fails.at(kut_i); \
				KUT_LOG2 << " over " << kut_loop_nb_iter << " (" << 100.0*kut_loop_fails.at(kut_i)/kut_loop_nb_iter<<" %)\n"; \
			} \
		} \
		if( kut_fail_flag == true ) \
		{ \
			kut_data.count_fail++; \
			kut_fail_file.push_back( __FILE__ ); \
			kut_fail_line.push_back( kut_loop_line ); \
			if( kut_data.StopTestOnFail ) \
			{ \
				std::cout << " -premature ending of test !\n"; \
				kut_data.DoQuit = true; \
				KUT_LOG << "\n- PREMATURE ENDING of test due to failure!\n"; \
				KUT_LOG << " - in function : " << __PRETTY_FUNCTION__ << ENDL; \
				KUT_LOG << " - Actual status : " << kut_data.count_test<<" tests done and " << kut_data.count_fail<<" failure(s)\n\n"; \
				exit(1); \
				return kut_data; \
			} \
		}\
	}

/// private macro 1.1
#define KUT_LOOP_P11( expr, op ) \
	if( kut_i == 0 ) \
	{ \
		kut_loop_expr_a.push_back( #expr ); \
		kut_loop_expr_b.push_back( "" ); \
		kut_loop_expr_op.push_back( op ); \
		kut_loop_fails.push_back( 0 ); \
	}

/// private macro 1.2
#define KUT_LOOP_P12( expr1, op, expr2 ) \
	if( kut_i == 0 ) \
	{ \
		kut_loop_expr_a.push_back( #expr1 ); \
		kut_loop_expr_b.push_back( #expr2 ); \
		kut_loop_expr_op.push_back( op ); \
		kut_loop_fails.push_back( 0 ); \
	}

/// private macro 2
#define KUT_LOOP_P2 \
	{ \
		kut_loop_fails[kut_loop_macro_count]++; \
		kut_fail_flag = true; \
	} \
	kut_loop_macro_count++ \


/// Testing if expression evaluates to true, inside a test loop
#define KUT_LOOP_TRUE( a ) \
	{ \
		KUT_LOOP_P11( a, "TRUE" ); \
		if( (a)==false ) \
			KUT_LOOP_P2; \
	}

/// Testing if expression evaluates to false, inside a test loop
#define KUT_LOOP_FALSE( a ) \
	{ \
		KUT_LOOP_P11( a, "FALSE" ); \
		if( (a)==true ) \
			KUT_LOOP_P2; \
	}


/// Testing is the two arguments are equal, using their '==' operator (needs to be defined)
#define KUT_LOOP_EQU( a, b ) \
	{ \
		KUT_LOOP_P12( a, "EQUAL", b ); \
		if( false == ((a) == (b)) ) \
			KUT_LOOP_P2; \
	}

/// Testing is the two arguments are different, using their '!=' operator (needs to be defined)
#define KUT_LOOP_DIFF( a, b ) \
	{ \
		KUT_LOOP_P12( a, "DIFF", b ); \
		if( false == ((a) != (b)) ) \
			KUT_LOOP_P2; \
	}

/// Testing ordering of 2 objects, by using the '<' operator (needs to be defined)
#define KUT_LOOP_LESS( a, b ) \
	{ \
		KUT_LOOP_P12( a, "LESS", b ); \
		if( false == ((a) < (b)) ) \
			KUT_LOOP_P2; \
	}

///@}

/// returns true if the last test failed
#define KUT_FAILED kut_data.kut_failflag

/// A macro for running a function and checking it does NOT throw an exception
/**
For example, say you have a function \c foobar(int z, XYZ t) that needs to be tested,
and that it has some "throw" statement, and that this means an error.

Then you can test it with:
\code
	KUT_TRY_THROW( foobar( a, b ) );
\endcode
and if an exception happens, it will be catched and increases the error counters.
*/

#define KUT_TRY_NOTHROW( a ) \
	KUT_P2; \
	try \
	{ \
		a; \
		KUT_LOG2 << "PASS: expression: " << #a << ": no exception throwed" << std::endl; \
	} \
	catch( const std::exception& e ) \
	{ \
		KUT_P_FAILURE \
		KUT_LOG2 << ", exception msg: " << e.what() << std::endl; \
	} \
	catch( ... ) \
	{ \
		std::cerr << "KUT: unhandled exception !!!\n"; \
		KUT_LOG << "\nKUT: unhandled exception !!!\n"; \
		KUT_P_FAILURE \
		throw; \
	}

/// similar to KUT_TRY_NOTHROW, but with a second argument that gets streamed
#define KUT_TRY_NOTHROW_2( a, b ) \
	KUT_P2; \
	try \
	{ \
		a; \
		KUT_LOG2 << "PASS: expression: " << #a << ": no exception throwed\n - arg: " << b << "\n"; \
	} \
	catch( const std::exception& e ) \
	{ \
		KUT_P_FAILURE \
		KUT_LOG2 << ", exception msg: " << e.what() << "\n - arg: " << b << "\n"; \
	} \
	catch( ... ) \
	{ \
		std::cerr << "KUT: unhandled exception !!!\n"; \
		KUT_LOG << "\nKUT: unhandled exception !!!\n"; \
		KUT_P_FAILURE \
		throw; \
	}


/// A macro for running a function and checking that it DOES throw a (text) exception
/**
If any other type of exception occurs, then it will be considered as a test-case failure,
and exception will be thrown again.
*/
#define KUT_TRY_THROW( a ) \
	KUT_P2; \
	try \
	{ \
		a; \
		KUT_P_FAILURE \
		KUT_LOG2 << ", statement did NOT throw exception\n"; \
	} \
	catch( const std::exception& e ) \
	{ \
		KUT_LOG2 << "PASS: expression: " << #a << ": handled exception throwed, msg=" << e.what() << "\n"; \
	} \
	catch( ... ) \
	{ \
		std::cerr << "KUT: unhandled exception !!!\n"; \
		KUT_LOG << "KUT: unhandled exception !!!\n"; \
		KUT_P_FAILURE \
		throw; \
	}

/// similar to KUT_TRY_THROW, but with a second argument that gets streamed
#define KUT_TRY_THROW_2( a, b ) \
	KUT_P2; \
	try \
	{ \
		a; \
		KUT_P_FAILURE \
		KUT_LOG2 << ", statement did NOT throw exception, arg: " << b << "\n"; \
	} \
	catch( const std::exception& e ) \
	{ \
		KUT_LOG2 << "PASS: expression: " << #a << ": handled exception throwed, msg=" << e.what() << "\n - arg: " << b << "\n"; \
	} \
	catch( ... ) \
	{ \
		std::cerr << "KUT: unhandled exception !!!\n"; \
		KUT_LOG << "KUT: unhandled exception !!!\n"; \
		KUT_P_FAILURE \
		throw; \
	}


//----------------------------------------------------------------------------
#endif

// eof
