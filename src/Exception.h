#ifndef __Exception
#define __Exception

#include <string>

namespace nj
{
   class Exception
   {
      public:

         static const int no_exception = 0;
         static const int system_exception = 1;
         static const int invalid_exception = 2;
         static const int initialization_exception = 3;
         static const int julia_error_exception = 4;
         static const int julia_method_error_exception = 5;
         static const int julia_undef_var_error_exception = 6;
         static const int julia_load_error_exception = 7;

      protected:

         int _id;
         std::string _what;

      public:

         Exception() {}
         Exception(int id,const std::string &what):_what(what) { _id = id; }

         int id() const {  return _id;  }
         std::string what() const {  return _what;  }
   };

   class SystemException:public Exception {  public: SystemException(const std::string &what);  };
   class InvalidException:public Exception {  public: InvalidException(const std::string &what):Exception(invalid_exception,what){}  };
   class InitializationException:public Exception {  public: InitializationException(std::string what):Exception(initialization_exception,what){} };
   class JuliaErrorException:public Exception {  public: JuliaErrorException(std::string what):Exception(julia_error_exception,what){} };
   class JuliaMethodError:public Exception {  public: JuliaMethodError(std::string what):Exception(julia_method_error_exception,what){} };
   class JuliaUndefVarError:public Exception {  public: JuliaUndefVarError(std::string what):Exception(julia_undef_var_error_exception,what){} };
   class JuliaLoadError:public Exception {  public: JuliaLoadError(std::string what):Exception(julia_load_error_exception,what){} };
}

#endif
