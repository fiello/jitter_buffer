/**
 *  @file
 *  \brief     Main include file to use logger
 *  \details   Defines helper macros to be used for easy logging
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "logger_impl.h"
// third-party
#include <boost/current_function.hpp>

#define LOG(level) \
   if (level < logger::Log::GetLogLevel())\
      ;\
   else\
      (logger::Log(level)) << logger::Delimiter << BOOST_CURRENT_FUNCTION << logger::Delimiter

/// log message with Debug level
#define LOGDBG		LOG(logger::Debug)
/// log message with Warning log
#define LOGWRN		LOG(logger::Warning)
/// log message with Error level
#define LOGERR		LOG(logger::Error)
/// log message with Fatal level
#define LOGFTL		LOG(logger::Fatal)
/// log Empty message (log always, no matter what log level is)
#define LOGEMPTY	(logger::Log(logger::Empty)) //

#endif // LOGGER_H
