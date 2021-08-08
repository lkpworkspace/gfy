#include "GLogger.hpp"
#include <cstddef>
#include <string>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/core/null_deleter.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(timeline, "Timeline", attrs::timer::value_type)

NS_G4Y_BEGIN

GLogger::GLogger()
{
    Init();
}

void GLogger::Init()
{
    // Setup the common formatter for all sinks
    logging::formatter fmt = expr::stream
        << std::setw(6) << std::setfill('0') << line_id << std::setfill(' ')
        << ": <" << severity << ">\t"
            << "(" << scope << ") "
            << expr::if_(expr::has_attr(tag_attr))
               [
                    expr::stream << "[" << tag_attr << "] "
               ]
            << expr::if_(expr::has_attr(timeline))
               [
                    expr::stream << "[" << timeline << "] "
               ]
            << ": "
            << expr::smessage;
    
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
    
    //sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >("g4y.log"));

    // We have to provide an empty deleter to avoid destroying the global stream object
    boost::shared_ptr< std::ostream > stream(&std::clog, boost::null_deleter());
    sink->locked_backend()->add_stream(stream);

    sink->set_formatter(fmt);
    // sink->set_filter(logging::trivial::severity >= logging::trivial::info);
    

    logging::core::get()->add_sink(sink);

    // Add attributes
    logging::add_common_attributes();
    logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
}

GLogger g_log;

void GLogger::Log(std::string msg, GlogLevel lv, std::string file, std::string function, int line)
{
	std::cout << msg << std::endl;
}

NS_G4Y_END