/**
 *  @file
 *  \brief     User-defined result codes
 *  \details   Enum with user-define result codes
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef RESULT_CODE_H
#define RESULT_CODE_H

typedef unsigned int result_t;

namespace result_code
{

/**
 * internally used return codes to pass results between worker functions
 * can be extended and standardized for cross-component / cross-interface usage
 */
enum
{
   sOk,

   // common errors
   eFail,
   eNotFound,
   eNotReady,
   eOutOfSpace,
   eInvalidArgument,
   eUnexpected
};

} // namespace result_code

#endif // RESULT_CODE_H
