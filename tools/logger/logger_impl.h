/**
 *  @file
 *  \brief     Log class declaration
 *  \details   Declare Log class and additional enum with acceptable log levels
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef LOGGER_LOG_H
#define LOGGER_LOG_H

// third-party
#include <sstream>
#include <iostream>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

namespace logger
{

/// Log levels to be used with this class
enum LevelId
{
   /// for debug information. Most detailed level, can impact the application performance
   Debug,
   /// warning information for minor non-fatal issues
   Warning,
   /// for serious errors that are abnormal but basically do not lead to application
   /// malfunctioning. If present in a log should be investigated by designer
   Error,
   /// for fatal failures that most likely lead to application crash
   Fatal,
   /// helper type of log level, which doesn't produce additional logging information
   /// like date stamp/loglevel/thread id. Produce log always and doesn't depend 
   /// on the current log level
   Empty
};

/// delimiter to be used in logging line between sections
const std::string Delimiter = "\t";

/**
 * Log level class implementation. Class is non-copyable and should be used
 * as a temporary object on a stack, as logging is produced in class destructor
 */
class Log : public boost::noncopyable
{
public:

   /**
    * Constructor
    * @param level - log level to be used in this instance of object
    */
   Log(LevelId level);

   /**
    * Destructor
    * Print accumulated log level to the stdout / err streams and to the log file
    * with hardcoded name
    */
   ~Log();

   /**
    * Static function, can be used to set log level at any point at runtime
    * @param level - log level to be set
    */
   static void SetLogLevel(LevelId level);

   /**
    * Static function, can be used to retrieve current log level at any point at runtime
    * @returns level - current log level
    */
   static LevelId GetLogLevel();

   /**
    * Overloaded operator, can be used to pass new log sections and concatenate them
    * @param obj - new section to be concatenated with previous ones
    * @returns reference to the Log class instance
    */
   template <typename T>
   Log& operator<< (const T& obj)
   {
      m_stream << obj;
      return *this;
   }

private:
   typedef boost::lock_guard<boost::mutex> LOCK;

   /// minimum log level that is allowed for all instances of Log class in current application
   static LevelId       m_allowedLevel;
   /// mutex to provide exclusive access to log level changing methods
   static boost::mutex  m_levelAccessGuard;
   /// log file name
   static std::string   m_logFileName;
   /// log level that was requested in this instance
   LevelId              m_requestedLevel;
   /// internal stream that summarizes all log sections and represents them in string format
   std::ostringstream   m_stream;
};

} // namespace logger

#endif // LOGGER_LOG_H
