#if defined(WIN32)
#pragma warning(disable:4200)
#endif

#include <math.h>
#include <iostream>
#include "Kernel.h"
#include "error.h"

using namespace std;

static const string libDir(NJ_LIB);

nj::Kernel *nj::Kernel::singleton = 0;

jl_value_t *nj::Kernel::get_preserve_array() throw(JuliaException)
{
   if(!nj_module) nj_module = load();

   jl_sym_t *preserveSym = jl_symbol("preserve");
   jl_value_t *preserve = jl_get_global(nj_module,preserveSym);
   jl_value_t *ex = jl_exception_occurred();

   if(ex) throw getJuliaException(ex);
   return preserve;
}

jl_value_t *nj::Kernel::invoke(const string &functionName) throw(JuliaException)
{
   if(!nj_module) nj_module = load();

   jl_function_t *func = jl_get_function(nj_module,functionName.c_str());

   if(!func) throw getJuliaException("Could not locate function nj." + functionName);

   jl_value_t *res = jl_call0(func);
   jl_value_t *ex = jl_exception_occurred();

   if(ex) throw getJuliaException(ex);
   return res;
}

jl_value_t *nj::Kernel::invoke(const string &functionName,jl_value_t *arg) throw(JuliaException)
{
   if(!nj_module) nj_module = load();

   jl_function_t *func = jl_get_function(nj_module,functionName.c_str());

   if(!func) throw getJuliaException("Could not locate function nj." + functionName);

   JL_GC_PUSH1(&arg);

   jl_value_t *res = jl_call1(func,arg);
   jl_value_t *ex = jl_exception_occurred();

   JL_GC_POP();

   if(ex) throw getJuliaException(ex);
   return res;
}

jl_value_t *nj::Kernel::invoke(const string &functionName,jl_value_t *arg1,jl_value_t *arg2) throw(JuliaException)
{
   if(!nj_module) nj_module = load();

   jl_function_t *func = jl_get_function(nj_module,functionName.c_str());

   if(!func) throw getJuliaException("Could not locate function nj." + functionName);

   JL_GC_PUSH2(&arg1,&arg2);

   jl_value_t *res = jl_call2(func,arg1,arg2);
   jl_value_t *ex = jl_exception_occurred();

   JL_GC_POP();

   if(ex) throw getJuliaException(ex);
   return res;
}

jl_value_t *nj::Kernel::invoke(const std::string &functionName,std::vector<jl_value_t*> &args) throw(JuliaException)
{
   size_t numArgs = args.size();

   if(numArgs == 0) return invoke(functionName);
   if(!nj_module) nj_module = load();

   jl_function_t *func = jl_get_function(nj_module,functionName.c_str());

   if(!func) throw getJuliaException("Could not locate function nj." + functionName);

   jl_value_t **jlArgs;

   JL_GC_PUSHARGS(jlArgs,numArgs);

   for(size_t i = 0;i < numArgs;i++) jlArgs[i] = args[i];

   jl_value_t *res = jl_call(func,jlArgs,(int)numArgs);
   jl_value_t *ex = jl_exception_occurred();

   JL_GC_POP();

   if(ex) throw getJuliaException(ex);
   return res;
}

jl_module_t *nj::Kernel::load() throw(JuliaException)
{
   #if defined(JULIA_VERSION_MINOR) && JULIA_VERSION_MINOR >= 5
   string njPath = libDir + "/nj-v2.jl";
   #else
   string njPath = libDir + "/nj-v1.jl";
   #endif
   jl_function_t *func = jl_get_function(jl_core_module,"include");
   jl_value_t *ex;

   if(!func) throw getJuliaException("unable to locate Core.include");
   jl_call1(func,jl_cstr_to_string(njPath.c_str()));
   ex = jl_exception_occurred();
   if(ex) throw getJuliaException(ex);

   jl_sym_t *modName = jl_symbol("nj");
   jl_module_t *mod = (jl_module_t*)jl_get_global(jl_main_module,modName);

   ex = jl_exception_occurred();
   if(ex) throw getJuliaException(ex);

   return mod;
}

nj::Kernel *nj::Kernel::getSingleton()
{
   if(!singleton) singleton = new Kernel();
   return singleton;
}

nj::Kernel::Kernel()
{
   nj_module = 0;
   preserve_array = 0;
   freelist_start = -1;
}

jl_value_t *nj::Kernel::scriptify(jl_module_t *mod,const string &filename) throw(JuliaException)
{
   if(!nj_module) nj_module = load();

   jl_function_t *func = jl_get_function(nj_module,"scriptify");

   if(!func) throw getJuliaException("Could not locate function nj.scriptify");

   jl_value_t *val = jl_cstr_to_string(filename.c_str());

   JL_GC_PUSH2(&mod,&val);

   jl_value_t *ast = jl_call2(func,(jl_value_t*)mod,val);
   jl_value_t *ex = jl_exception_occurred();

   JL_GC_POP();

   if(ex) throw getJuliaException(ex);
   return ast;
}

jl_value_t *nj::Kernel::newRegex(jl_value_t *pattern) throw(JuliaException)
{
   if(!nj_module) nj_module = load();

   jl_function_t *func = jl_get_function(nj_module,"newRegex");

   if(!func) throw getJuliaException("Could not locate function nj.newRegex");

   JL_GC_PUSH1(&pattern);

   jl_value_t *re = jl_call1(func,pattern);
   jl_value_t *ex = jl_exception_occurred();

   JL_GC_POP();

   if(ex) throw getJuliaException(ex);
   return re;
}


jl_value_t *nj::Kernel::getPattern(jl_value_t *re) throw(JuliaException) { return invoke("getPattern",re); }
jl_datatype_t *nj::Kernel::getDateTimeType() throw(JuliaException) { return (jl_datatype_t*)invoke("getDateTimeType"); }

jl_value_t *nj::Kernel::toDate(double milliseconds) throw(JuliaException)
{
   int64_t rounded = (int64_t)round(milliseconds);
   jl_value_t *val = jl_box_int64(rounded);

   return invoke("toDate",val);
}

double nj::Kernel::toMilliseconds(jl_value_t *date) throw(JuliaException)
{
   jl_value_t *val = invoke("toMilliseconds",date);

   return jl_unbox_float64(val)*1000;
}

jl_datatype_t *nj::Kernel::getRegexType() throw(JuliaException) { return (jl_datatype_t*)invoke("getRegexType"); }
jl_value_t *nj::Kernel::getError(jl_value_t *ex,jl_value_t *bt) throw(JuliaException) { return invoke("getError",ex,bt); }

int64_t nj::Kernel::preserve(jl_value_t *val) throw(JuliaException)
{
   int64_t free_index;

   if(!preserve_array) preserve_array = get_preserve_array();

   if(freelist_start == -1)
   {
      jl_array_ptr_1d_push((jl_array_t*)preserve_array,val);
      freelist.push_back(-1);
      free_index = freelist.size() - 1;
   }
   else
   {
      free_index = freelist_start;
      freelist_start = freelist[free_index];
      freelist[free_index] = -1;
      jl_array_ptr_set(preserve_array,free_index,val);
   }

   jl_value_t *ex = jl_exception_occurred();

   if(ex) throw getJuliaException(ex);
   freelist_index[val] = free_index;
   return free_index;
}

jl_value_t *nj::Kernel::free(int64_t valIndex) throw(JuliaException)
{
   if(!preserve_array) preserve_array = get_preserve_array();

   freelist[valIndex] = freelist_start;
   freelist_start = valIndex;

   jl_value_t *val = jl_array_ptr_ref(preserve_array,valIndex);
   jl_value_t *ex = jl_exception_occurred();

   if(ex) throw getJuliaException(ex);
   freelist_index.erase(val);
   jl_array_ptr_set(preserve_array,valIndex,0);
   ex = jl_exception_occurred();
   if(ex) throw getJuliaException(ex);
   return val;
}

jl_value_t *nj::Kernel::get(int64_t valIndex) throw(JuliaException)
{
   if(!preserve_array) preserve_array = get_preserve_array();

   jl_value_t *val = jl_array_ptr_ref(preserve_array,valIndex);
   jl_value_t *ex = jl_exception_occurred();

   if(ex) throw getJuliaException(ex);
   return val;
}

int64_t nj::Kernel::get(jl_value_t *val)
{
   map<jl_value_t*,int64_t>::iterator i = freelist_index.find(val);

   if(i == freelist_index.end()) return -1;
   return i->second;
}


jl_value_t *nj::Kernel::import(const string &moduleName) throw(JuliaException)
{
   if(moduleName.length() != 0)
   {
      jl_value_t *val = jl_cstr_to_string(moduleName.c_str());

      return invoke("importModule",val);
   }
   return 0;
}

jl_value_t *nj::Kernel::newTuple(std::vector<jl_value_t*> &elements) throw(JuliaException)
{
   return invoke("newTuple",elements);
}
