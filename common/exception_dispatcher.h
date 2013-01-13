/**
 *  @file
 *  \brief     Helper class to handle exception propagation
 *  \details   Holds implementation of ExceptionDispatcher class. Depends on Logger library and requires
 *             it to be linked with target module where this header is included
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef EXCEPTION_DISPATCHER_H
#define EXCEPTION_DISPATCHER_H

#include "exception_impl.h"
#include <logger/logger.h>
// third-party
#include <string.h>

namespace exception
{

/**
 * Helper class to handle exception propagation and holding/providing additional description
 * on some types of exception EE.
 */
class ExceptionDispatcher
{
public:

   /**
    * Static method, can be called from 'catch' section to capture and log some
    * additional information on the exception occurred
    *
    * @param description - description usually added by exception handler inside a
    *                      catch section (like BOOST_CURRENT_FUNCTION)
    */
   static result_t Dispatch(const std::string& description)
   {
      try
      {
         throw;
      }
      catch(const exception::internal::BasicException &ex)
      {
         LOGERR << ex.what();
         result_t code = (result_t)ex;
         return code;
      }
      catch(const std::exception &ex)
      {
         LOGERR << description << " : Unexpected exception : " << ex.what();
         return result_code::eUnexpected;
      }
   }
};

} // namespace exception

/// general macros to generate exception with given result code
#define THROW_BASIC_EXCEPTION(resultCode)\
   throw exception::internal::BasicException(resultCode,__FILE__,__LINE__,BOOST_CURRENT_FUNCTION)

/// helper macros to throw exception with InvalidArgument result code
/// (most common case in cross-component calls)
#define THROW_INVALID_ARGUMENT\
   THROW_BASIC_EXCEPTION(result_code::eInvalidArgument)

/// helper macros to check boolean expression and throw
/// InvalidArgument exception with extended message description
#define CHECK_ARGUMENT(booleanExpression, msg)\
   if (!(booleanExpression))\
      THROW_INVALID_ARGUMENT << msg;

#endif // EXCEPTION_DISPATCHER_H
