/**
 *  @file
 *  \brief     Implements basic exception wrapper
 *  \details   Basic exception wrapper is capable of storing some extra information
 *             on top of std::exception class : user-defined result code, filename, function
 *             and line number where exception was re-thrown from.
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef EXCEPTION_INTERNAL_BASIC_EXCEPTION_H
#define EXCEPTION_INTERNAL_BASIC_EXCEPTION_H

#include "result_code.h"
// third-party
#include <sstream>
#include <exception>

namespace exception
{
namespace internal
{

/**
 * Helper class that can be used as a storage for additional diagnostic information
 * about the nature of exception occurred
 */
class BasicException : public std::exception
{
public:
   /**
    * Constructor
    * @param resultCode - user-defined result code
    * @param fileName - source file name where exception was thrown
    * @param lineNumber - line in the source file where exception was thrown
    * @param functionname - name of the function where exception was thrown
    */
   BasicException(
         const result_t resultCode,
         const char* fileName,
         int lineNumber,
         const char* functionName
      )
      : m_resultCode(resultCode)
   {
      std::ostringstream output;
      output << "BasicException: \nfile: " << fileName
         << "\nfunction: " << functionName
         << "\nline: " << lineNumber
         << "\nerror message: ";
      m_errorMessage = output.str();
   }

   virtual ~BasicException() throw() {};

   /**
    * Inherited from std::exception. Can be used to get error description
    * @returns Pointer to the internal string with error description
    */
   virtual const char* what() const throw()
   {
      return m_errorMessage.c_str();
   }

   /**
    * Overloaded operator which allows inserting additional information about error
    * @param obj - various objects that are passed to current instance using << operator
    */
   template <typename T>
   BasicException& operator<< (const T& obj)
   {
      std::ostringstream stream;
      stream << obj;
      m_errorMessage += stream.str();
      return *this;
   }

   operator result_t() const
   {
      return m_resultCode;
   }

private:
   /// user-defined result code
   result_t       m_resultCode;
   /// resulting text message with error description
   std::string    m_errorMessage;
};

} // namespace internal
} // namespace exception

#endif // EXCEPTION_INTERNAL_BASIC_EXCEPTION_H
