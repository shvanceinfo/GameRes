// LogApp.cpp : 定义控制台应用程序的入口点。
//



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//http://blog.csdn.net/csfreebird/article/details/17561225


#include "stdafx.h"
 #include <stdexcept>
 #include <string>
 #include <iostream>
 #include <fstream>
 #include <functional>
 #include <boost/ref.hpp>
 #include <boost/bind.hpp>
 #include <boost/smart_ptr/shared_ptr.hpp>
 #include <boost/date_time/posix_time/posix_time.hpp>
 #include <boost/thread/thread.hpp>
 #include <boost/thread/barrier.hpp>
  
 #include <boost/log/common.hpp>
 #include <boost/log/expressions.hpp>
 #include <boost/log/attributes.hpp>
 #include <boost/log/sinks.hpp>
 #include <boost/log/sources/logger.hpp>
 #include <boost/log/utility/record_ordering.hpp>
  
 namespace logging = boost::log;
 namespace attrs = boost::log::attributes;
 namespace src = boost::log::sources;
 namespace sinks = boost::log::sinks;
 namespace expr = boost::log::expressions;
 namespace keywords = boost::log::keywords;
   
 using boost::shared_ptr;
   
 enum
 {
 	LOG_RECORDS_TO_WRITE = 5,
 	THREAD_COUNT = 2
 };
   
 BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(test_lg, src::logger_mt)
   
 //! This function is executed in multiple threads
 void thread_fun(boost::barrier& bar)
 {
 	// Wait until all threads are created
 	bar.wait();
   
 	// Here we go. First, identify the thread.
 	BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
   
 	// Now, do some logging
 	for (int i = 0; i < LOG_RECORDS_TO_WRITE; ++i)
 	{
   		BOOST_LOG(test_lg::get()) << "Log record中国 " << i;
 	}
 }
   
 int _tmain(int argc, _TCHAR* argv[])
 {
 	try
 	{
   		// Open a rotating text file
   		shared_ptr< std::ostream > strm(new std::ofstream("test.log", std::ios::app));
   		if (!strm->good())
   			throw std::runtime_error("Failed to open a text log file");
   
   		// Create a text file sink
   		typedef sinks::text_ostream_backend backend_t;
   		typedef sinks::asynchronous_sink<
   			backend_t,
   			sinks::unbounded_ordering_queue<
   			logging::attribute_value_ordering< int, std::less< int > >
   			>
   		> sink_t;
   		shared_ptr< sink_t > sink(new sink_t(
   			boost::make_shared< backend_t >(),
   			// We'll apply record ordering to ensure that records from different threads go sequentially in the file
   			keywords::order = logging::make_attr_ordering("RecordID", std::less< int >())));
   
   		sink->locked_backend()->add_stream(strm);

		//boost::function<void(std::ostream &, logging::attribute_value_view const&, std::string const&)>
   
  		sink->set_formatter
   			(
   			expr::format("[%1%][%2%] [%3%] [%4%] - %5%")
   			% expr::attr< int >("RecordID")
   			% expr::attr< boost::posix_time::ptime >("TimeStamp")
   			% expr::attr< boost::thread::id >("ThreadID")
			% expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
   			% expr::smessage
   			);
   
   		// Add it to the core
   		logging::core::get()->add_sink(sink);
   
   		// Add some attributes too
   		logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
   		logging::core::get()->add_global_attribute("RecordID", attrs::counter< int >());
		logging::core::get()->add_thread_attribute("Scope", attrs::named_scope());
   		// Create logging threads
   		boost::barrier bar(THREAD_COUNT);
   		boost::thread_group threads;
   		for (int i = 0; i < THREAD_COUNT; ++i)
   			threads.create_thread(boost::bind(&thread_fun, boost::ref(bar)));
   
   		// Wait until all action ends
   		threads.join_all();
   
   		// Flush all buffered records
   		sink->stop();
   		sink->flush();
   
   		return 0;
 	}
 	catch (std::exception& e)
 	{
   		std::cout << "FAILURE: " << e.what() << std::endl;
   		return 1;
 	}
   
 	getchar();
 	return 0;
 }





//  #include <iostream>
//  
//  #include <boost/log/common.hpp>
//  #include <boost/log/expressions.hpp>
//  
//  #include <boost/log/utility/setup/file.hpp>
//  #include <boost/log/utility/setup/console.hpp>
//  #include <boost/log/utility/setup/common_attributes.hpp>
//  
//  #include <boost/log/attributes/timer.hpp>
//  #include <boost/log/attributes/named_scope.hpp>
//  
//  #include <boost/log/sources/logger.hpp>
//  
//  #include <boost/log/support/date_time.hpp>
//  
//  namespace logging = boost::log;
//  namespace sinks = boost::log::sinks;
//  namespace attrs = boost::log::attributes;
//  namespace src = boost::log::sources;
//  namespace expr = boost::log::expressions;
//  namespace keywords = boost::log::keywords;
//  
//  using boost::shared_ptr;
//  
//  // Here we define our application severity levels.
//  enum severity_level
//  {
//  	normal,
//  	notification,
//  	warning,
//  	error,
//  	critical
//  };
//  
//  // The formatting logic for the severity level
//  template< typename CharT, typename TraitsT >
//  inline std::basic_ostream< CharT, TraitsT >& operator<< (
//  	std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
//  {
//  	static const char* const str[] =
//  	{
//  		"normal",
//  		"notification",
//  		"warning",
//  		"error",
//  		"critical"
//  	};
//  	if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
//  		strm << str[lvl];
//  	else
//  		strm << static_cast< int >(lvl);
//  	return strm;
//  }
//  
//  int main(int argc, char* argv[])
//  {
//  	// This is a simple tutorial/example of Boost.Log usage
//  
//  	// The first thing we have to do to get using the library is
//  	// to set up the logging sinks - i.e. where the logs will be written to.
//  	logging::add_console_log(std::clog, keywords::format = "%TimeStamp%: %Message%");
//  
//  	// One can also use lambda expressions to setup filters and formatters
// 	logging::add_file_log
// 		(
// 		"sample.log",
// 		keywords::filter = expr::attr< severity_level >("Severity") >= warning,
// 		keywords::rotation_size = 10 * 1024 * 1024,
// 		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0,0,0),
//  		keywords::format = expr::stream
//  		<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
//  		//<< " [" << expr::format_date_time< attrs::timer::value_type >("Uptime", "%O:%M:%S")
//  		//<< "] [" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
//  		//<< "] <" << expr::attr< severity_level >("Severity")
// 		<< "<" << expr::attr< severity_level >("Severity")
//  		<< "> " << expr::message
//  		/*
//  		keywords::format = expr::format("%1% [%2%] [%3%] <%4%> %5%")
//  		% expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
//  		% expr::format_date_time< attrs::timer::value_type >("Uptime", "%O:%M:%S")
//  		% expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
//  		% expr::attr< severity_level >("Severity")
//  		% expr::message
//  		*/
//  		);
//  
//  	// Also let's add some commonly used attributes, like timestamp and record counter.
//  	logging::add_common_attributes();
//  	logging::core::get()->add_thread_attribute("Scope", attrs::named_scope());
//  
//  	BOOST_LOG_FUNCTION();
//  
//  	// Now our logs will be written both to the console and to the file.
//  	// Let's do a quick test and output something. We have to create a logger for this.
//  	src::logger lg;
//  
//  	// And output...
//  	BOOST_LOG(lg) << "Hello, World!";
//  
//  	// Now, let's try logging with severity
//  	src::severity_logger< severity_level > slg;
//  
//  	// Let's pretend we also want to profile our code, so add a special timer attribute.
//  	slg.add_attribute("Uptime", attrs::timer());
//  
//  	BOOST_LOG_SEV(slg, normal) << "A normal severity message, will not pass to the file";
//  	BOOST_LOG_SEV(slg, warning) << "A warning severity message, will pass to the file";
//  	BOOST_LOG_SEV(slg, error) << "An error severity message, will pass to the file";
//  
//  
//  	getchar();
//  	return 0;
//  }




// #include <stdexcept>
// #include <string>
// #include <iostream>
// #include <boost/smart_ptr/shared_ptr.hpp>
// #include <boost/thread/thread.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>
// 
// #include <boost/log/common.hpp>
// #include <boost/log/expressions.hpp>
// #include <boost/log/attributes.hpp>
// #include <boost/log/sources/logger.hpp>
// #include <boost/log/sinks/sync_frontend.hpp>
// #include <boost/log/sinks/text_multifile_backend.hpp>
// 
// namespace logging = boost::log;
// namespace attrs = boost::log::attributes;
// namespace src = boost::log::sources;
// namespace sinks = boost::log::sinks;
// namespace expr = boost::log::expressions;
// namespace keywords = boost::log::keywords;
// 
// using boost::shared_ptr;
// 
// enum
// {
// 	THREAD_COUNT = 5,
// 	LOG_RECORDS_TO_WRITE = 10
// };
// 
// // Global logger declaration
// BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::logger_mt)
// 
// // This function is executed in a separate thread
// void thread_foo()
// {
// 	BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
// 	for (int i = 0; i < LOG_RECORDS_TO_WRITE; ++i)
// 	{
// 		BOOST_LOG(my_logger::get()) << "Log record " << i;
// 	}
// }
// 
// int main(int argc, char* argv[])
// {
// 	try
// 	{
// 		// Create a text file sink
// 		typedef sinks::synchronous_sink< sinks::text_multifile_backend > file_sink;
// 		shared_ptr< file_sink > sink(new file_sink);
// 
// 		// Set up how the file names will be generated
// 		sink->locked_backend()->set_file_name_composer(sinks::file::as_file_name_composer(
// 			expr::stream << "logs/" << /*expr::attr< boost::thread::id >("ThreadID") <<*/ "log.log"));
// 
// 		// Set the log record formatter
// 		sink->set_formatter
// 			(
// 			expr::format("[%1%] - %2%")
// 			/*% expr::attr< int >("RecordID")*/
// 			% expr::attr< boost::posix_time::ptime >("TimeStamp")
// 			% expr::smessage
// 			);
// 
// 		// Add it to the core
// 		logging::core::get()->add_sink(sink);
// 
// 		// Add some attributes too
// 		logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
// 		//logging::core::get()->add_global_attribute("RecordID", attrs::counter< int >());
// 
// 		// Create threads and make some logs
// 		boost::thread_group threads;
// 		for (int i = 0; i < THREAD_COUNT; ++i)
// 			threads.create_thread(&thread_foo);
// 
// 		threads.join_all();
// 
// 		return 0;
// 	}
// 	catch (std::exception& e)
// 	{
// 		std::cout << "FAILURE: " << e.what() << std::endl;
// 		return 1;
// 	}
// }


//#include <boost/log/expressions.hpp>
//#include <boost/log/sinks.hpp>
//#include <boost/log/sources/severity_feature.hpp>
//#include <boost/log/sources/severity_logger.hpp>
//#include <boost/log/utility/empty_deleter.hpp>
//
//enum severity_level
//{
//	normal,
//	warning,
//	critical
//};
//
//std::ostream& operator<<(std::ostream &strm, severity_level level)
//{
//	static std::array<std::string, 3> aLevel = {
//		"normal",
//		"warning",
//		"critical"
//	};
//
//	if (static_cast<std::size_t>(level) < aLevel.size())
//	{
//		strm << aLevel[level];
//	}
//	else
//	{
//		strm << static_cast<int>(level);
//	}
//
//	return strm;
//}
//
//int main(int, char**)
//{
//	boost::log::sources::severity_logger<severity_level> mLogger;
//
//	typedef boost::log::sinks::text_ostream_backend TBackend;
//	auto pBackend = boost::make_shared<TBackend>();
//	pBackend->auto_flush(true);
//	pBackend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::log::empty_deleter()));
//
//	typedef boost::log::sinks::synchronous_sink<TBackend> TFronted;
//	auto pSink = boost::make_shared<TFronted>(pBackend);
//
//	pSink->set_formatter(boost::log::expressions::stream<<"<"<<boost::log::expressions::attr<severity_level>("Severity")<<">\t"<<boost::log::expressions::message);
//
//	pSink->set_filter(boost::log::expressions::attr<severity_level>("Severity") > normal);
//
//	boost::log::core::get()->add_sink(pSink);
//
//	BOOST_LOG_SEV(mLogger, normal) << "normal log";
//	BOOST_LOG_SEV(mLogger, warning) << "warning log";
//	BOOST_LOG_SEV(mLogger, critical) << "critical log";
//
//	return 0;
//}