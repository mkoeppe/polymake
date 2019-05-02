/* Copyright (c) 1997-2018
   Ewgenij Gawrilow, Michael Joswig (Technische Universitaet Berlin, Germany)
   http://www.polymake.org

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version: http://www.gnu.org/licenses/gpl.txt.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
--------------------------------------------------------------------------------
*/

#include "polymake/perl/glue.h"
#include "polymake/Main.h"

namespace pm { namespace perl {

namespace {

glue::cached_cv application_cv{ "Polymake::User::application" },
            app_from_object_cv{ "Polymake::Main::application_from_object" },
                  new_scope_cv{ "Polymake::Main::createNewScope" },
                 set_custom_cv{ "Polymake::Main::set_custom" },
               reset_custom_cv{ "Polymake::Main::reset_custom" },
               local_custom_cv{ "Polymake::Main::local_custom" },
                   greeting_cv{ "Polymake::Main::greeting" },
               shell_enable_cv{ "Polymake::Main::shell_enable" },
              shell_execute_cv{ "Polymake::Main::shell_execute" },
             shell_complete_cv{ "Polymake::Main::shell_complete" },
         shell_context_help_cv{ "Polymake::Main::shell_context_help" };

const char Extension[]="Polymake::Core::Extension";

}

void Main::set_application(const AnyString& appname)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(1);
   mPUSHp(appname.ptr, appname.len);
   PUTBACK;
   glue::call_func_void(aTHX_ application_cv);
}

void Main::set_application_of(const Object& x)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(1);
   PUSHs(x.obj_ref);
   PUTBACK;
   glue::call_func_void(aTHX_ app_from_object_cv);
}

void Main::add_extension(const AnyString& path)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(2);
   mPUSHp(Extension, sizeof(Extension)-1);
   mPUSHp(path.ptr, path.len);
   PUTBACK;
   glue::call_method_void(aTHX_ "add");
}

void Main::include(const AnyString& path)
{
   call_app_method("include_rules", path); 
}

void Main::set_preference(const AnyString& label_exp)
{ 
   call_app_method("set_preference", label_exp); 
}

void Main::reset_preference(const AnyString& label_exp)
{ 
   call_app_method("reset_preference", label_exp); 
}

SV* Main::lookup_extension(const AnyString& path)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(2);
   mPUSHp(Extension, sizeof(Extension)-1);
   mPUSHp(path.ptr, path.len);
   PUTBACK;
   return glue::call_method_scalar(aTHX_ "lookup");
}

void Main::call_app_method(const char* method, const AnyString& arg)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(2);
   SV* const app=glue::get_current_application(aTHX);
   PUSHs(app);
   mPUSHp(arg.ptr, arg.len);
   PUTBACK;
   glue::call_method_void(aTHX_ method);
}

void Main::set_custom_var(const AnyString& name, const AnyString& key, Value& x)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(3);
   mPUSHp(name.ptr, name.len);
   if (key.ptr) mPUSHp(key.ptr, key.len);
   PUSHs(x.get_temp());
   PUTBACK;
   glue::call_func_void(aTHX_ set_custom_cv);
}

void Main::reset_custom(const AnyString& name, const AnyString& key)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(2);
   mPUSHp(name.ptr, name.len);
   if (key.ptr) mPUSHp(key.ptr, key.len);
   PUTBACK;
   glue::call_func_void(aTHX_ reset_custom_cv);
}

Scope Main::newScope()
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(0);
   return Scope(this, call_func_scalar(aTHX_ new_scope_cv));
}

void Scope::prefer_now(const AnyString& labels) const
{
   pm_main->call_app_method("prefer_now", labels);
}

void Scope::set_custom_var(const AnyString& name, const AnyString& key, Value& x) const
{
   dTHXa(pm_main->pi);
   PERL_SET_CONTEXT(pm_main->pi);
   PmStartFuncall(3);
   mPUSHp(name.ptr, name.len);
   if (key.ptr) mPUSHp(key.ptr, key.len);
   PUSHs(x.get_temp());
   PUTBACK;
   glue::call_func_void(aTHX_ local_custom_cv);
}

std::string Main::greeting(int verbose)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(1);
   mPUSHi(verbose);
   PUTBACK;
   return glue::call_func_string(aTHX_ greeting_cv);
}

void Main::shell_enable()
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(0);
   glue::call_func_void(aTHX_ shell_enable_cv);
}

Main::shell_execute_t Main::shell_execute(const std::string& input)
{
   if (input.empty())
      return shell_execute_t(true, input, input, input);

   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(1);
   mPUSHp(input.c_str(), input.size());
   PUTBACK;
   if (glue::call_func_list(aTHX_ shell_execute_cv) != 4)
      return shell_execute_t(false, "", "", "unknown error");

   SPAGAIN;
   bool executed=false;
   std::string out, err, exc;
   Value(POPs) >> exc;
   Value(POPs) >> err;
   Value(POPs) >> out;
   Value(POPs) >> executed;
   PmFinishFuncall;
   return shell_execute_t(executed, std::move(out), std::move(err), std::move(exc));
}

Main::shell_complete_t Main::shell_complete(const std::string& input)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(1);
   mPUSHp(input.c_str(), input.size());
   PUTBACK;
   int n=glue::call_func_list(aTHX_ shell_complete_cv);
   int offset=0;
   char append=0;
   std::vector<std::string> proposals(n>2 ? n-2 : 0);
   if (n >= 2) {
      dSP;
      while (--n >= 2) {
         Value(POPs) >> proposals[n-2];
      }
      Value(POPs) >> append;
      Value(POPs) >> offset;
      PmFinishFuncall;
   }
   return shell_complete_t(offset, append, std::move(proposals));
}

std::vector<std::string> Main::shell_context_help(const std::string& input, size_t pos, bool full, bool html)
{
   dTHXa(pi);
   PERL_SET_CONTEXT(pi);
   PmStartFuncall(4);
   mPUSHp(input.c_str(), input.size());
   if (pos == std::string::npos)
      pos=input.size();
   mPUSHi(pos);
   SV* bool_sv=full ? &PL_sv_yes : &PL_sv_no;
   PUSHs(bool_sv);
   bool_sv=html ? &PL_sv_yes : &PL_sv_no;
   PUSHs(bool_sv);
   PUTBACK;
   int n=glue::call_func_list(aTHX_ shell_context_help_cv);
   std::vector<std::string> results(n);
   if (n) {
      dSP;
      while (--n >= 0) {
         Value(POPs) >> results[n];
      }
      PmFinishFuncall;
   }
   return results;
}

} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
