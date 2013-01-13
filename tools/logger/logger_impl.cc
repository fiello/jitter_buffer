/**
 *  @file
 *  \brief     Log class implementation
 *  \details   Holds Log class implementation together with several helper functions
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#include "logger.h"
// third-party
#include <fstream>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

namespace
{

/**
 * Helper function to convert level id to appropriate name that can
 * be logged in a message
 *
 * @param level - level id we are interested in
 * @returns - log level name
 */
std::string GetLogLevelName(const logger::LevelId level)
{
   using namespace logger;
   switch(level)
   {
      case Debug: return "DBG";
      case Warning: return "WRN";
      case Error: return "ERR";
      case Fatal: return "FTL";
      default: return "";
   }
}

/**
 * Helper function to get date-time stamp for log string
 *
 * @returns - get string with date-time stamp
 */
std::string GetDateTimeStamp()
{
   boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
   return boost::posix_time::to_simple_string(now).c_str();
}

} // unnamed namespace


namespace logger
{

LevelId Log::m_allowedLevel = logger::Warning;
boost::mutex Log::m_levelAccessGuard;
std::string Log::m_logFileName = "application.log";

Log::Log(const LevelId level)
{
   m_requestedLevel = level;
   if (m_requestedLevel != Empty)
   {
      m_stream << GetDateTimeStamp() << Delimiter
         << "tid:" << boost::this_thread::get_id() << Delimiter
         << GetLogLevelName(m_requestedLevel) << Delimiter;
   }
}

Log::~Log()
{
   std::cout << m_stream.str() << std::endl;

   std::ofstream outFile(m_logFileName.c_str(), std::fstream::app);
   if (outFile.good())
   {
      outFile << m_stream.str() << std::endl;
   }
   outFile.close();
}

void Log::SetLogLevel(const LevelId level)
{
   LOCK lock(m_levelAccessGuard);
   m_allowedLevel = level;
}

LevelId Log::GetLogLevel()
{
   LOCK lock(m_levelAccessGuard);
   return m_allowedLevel;
}

} // namespace logger
