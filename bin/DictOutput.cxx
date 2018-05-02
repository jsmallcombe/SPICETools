// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME bindIDictOutput

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/home/jsmallcombe/Dropbox/codes/SPICETools/include/spice_analysis_parse.h"
#include "/home/jsmallcombe/Dropbox/codes/SPICETools/include/spice_input.h"
#include "/home/jsmallcombe/Dropbox/codes/SPICETools/include/spice_calS3.h"
#include "/home/jsmallcombe/Dropbox/codes/SPICETools/include/spice_parse.h"
#include "/home/jsmallcombe/Dropbox/codes/SPICETools/include/spice_calSiLi.h"
#include "/home/jsmallcombe/Dropbox/codes/SPICETools/include/spice_enviro.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *SPICEcmdline_Dictionary();
   static void SPICEcmdline_TClassManip(TClass*);
   static void *new_SPICEcmdline(void *p = 0);
   static void *newArray_SPICEcmdline(Long_t size, void *p);
   static void delete_SPICEcmdline(void *p);
   static void deleteArray_SPICEcmdline(void *p);
   static void destruct_SPICEcmdline(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SPICEcmdline*)
   {
      ::SPICEcmdline *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SPICEcmdline));
      static ::ROOT::TGenericClassInfo 
         instance("SPICEcmdline", "spice_input.h", 39,
                  typeid(::SPICEcmdline), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SPICEcmdline_Dictionary, isa_proxy, 4,
                  sizeof(::SPICEcmdline) );
      instance.SetNew(&new_SPICEcmdline);
      instance.SetNewArray(&newArray_SPICEcmdline);
      instance.SetDelete(&delete_SPICEcmdline);
      instance.SetDeleteArray(&deleteArray_SPICEcmdline);
      instance.SetDestructor(&destruct_SPICEcmdline);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SPICEcmdline*)
   {
      return GenerateInitInstanceLocal((::SPICEcmdline*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SPICEcmdline*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SPICEcmdline_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SPICEcmdline*)0x0)->GetClass();
      SPICEcmdline_TClassManip(theClass);
   return theClass;
   }

   static void SPICEcmdline_TClassManip(TClass* theClass){
      theClass->CreateAttributeMap();
      TDictAttributeMap* attrMap( theClass->GetAttributeMap() );
      attrMap->AddProperty("file_name","include/spice_input.h");
   }

} // end of namespace ROOT

namespace ROOT {
   static void *new_ReFitMasterPanel(void *p = 0);
   static void *newArray_ReFitMasterPanel(Long_t size, void *p);
   static void delete_ReFitMasterPanel(void *p);
   static void deleteArray_ReFitMasterPanel(void *p);
   static void destruct_ReFitMasterPanel(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ReFitMasterPanel*)
   {
      ::ReFitMasterPanel *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::ReFitMasterPanel >(0);
      static ::ROOT::TGenericClassInfo 
         instance("ReFitMasterPanel", ::ReFitMasterPanel::Class_Version(), "spice_calSiLi.h", 86,
                  typeid(::ReFitMasterPanel), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::ReFitMasterPanel::Dictionary, isa_proxy, 4,
                  sizeof(::ReFitMasterPanel) );
      instance.SetNew(&new_ReFitMasterPanel);
      instance.SetNewArray(&newArray_ReFitMasterPanel);
      instance.SetDelete(&delete_ReFitMasterPanel);
      instance.SetDeleteArray(&deleteArray_ReFitMasterPanel);
      instance.SetDestructor(&destruct_ReFitMasterPanel);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ReFitMasterPanel*)
   {
      return GenerateInitInstanceLocal((::ReFitMasterPanel*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ReFitMasterPanel*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_silicalchan(void *p = 0);
   static void *newArray_silicalchan(Long_t size, void *p);
   static void delete_silicalchan(void *p);
   static void deleteArray_silicalchan(void *p);
   static void destruct_silicalchan(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::silicalchan*)
   {
      ::silicalchan *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::silicalchan >(0);
      static ::ROOT::TGenericClassInfo 
         instance("silicalchan", ::silicalchan::Class_Version(), "spice_calSiLi.h", 113,
                  typeid(::silicalchan), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::silicalchan::Dictionary, isa_proxy, 4,
                  sizeof(::silicalchan) );
      instance.SetNew(&new_silicalchan);
      instance.SetNewArray(&newArray_silicalchan);
      instance.SetDelete(&delete_silicalchan);
      instance.SetDeleteArray(&deleteArray_silicalchan);
      instance.SetDestructor(&destruct_silicalchan);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::silicalchan*)
   {
      return GenerateInitInstanceLocal((::silicalchan*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::silicalchan*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_SPICEenvironment(void *p = 0);
   static void *newArray_SPICEenvironment(Long_t size, void *p);
   static void delete_SPICEenvironment(void *p);
   static void deleteArray_SPICEenvironment(void *p);
   static void destruct_SPICEenvironment(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SPICEenvironment*)
   {
      ::SPICEenvironment *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::SPICEenvironment >(0);
      static ::ROOT::TGenericClassInfo 
         instance("SPICEenvironment", ::SPICEenvironment::Class_Version(), "spice_enviro.h", 67,
                  typeid(::SPICEenvironment), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::SPICEenvironment::Dictionary, isa_proxy, 4,
                  sizeof(::SPICEenvironment) );
      instance.SetNew(&new_SPICEenvironment);
      instance.SetNewArray(&newArray_SPICEenvironment);
      instance.SetDelete(&delete_SPICEenvironment);
      instance.SetDeleteArray(&deleteArray_SPICEenvironment);
      instance.SetDestructor(&destruct_SPICEenvironment);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SPICEenvironment*)
   {
      return GenerateInitInstanceLocal((::SPICEenvironment*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SPICEenvironment*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr ReFitMasterPanel::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *ReFitMasterPanel::Class_Name()
{
   return "ReFitMasterPanel";
}

//______________________________________________________________________________
const char *ReFitMasterPanel::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ReFitMasterPanel*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int ReFitMasterPanel::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ReFitMasterPanel*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *ReFitMasterPanel::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ReFitMasterPanel*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *ReFitMasterPanel::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ReFitMasterPanel*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr silicalchan::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *silicalchan::Class_Name()
{
   return "silicalchan";
}

//______________________________________________________________________________
const char *silicalchan::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::silicalchan*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int silicalchan::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::silicalchan*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *silicalchan::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::silicalchan*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *silicalchan::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::silicalchan*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr SPICEenvironment::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *SPICEenvironment::Class_Name()
{
   return "SPICEenvironment";
}

//______________________________________________________________________________
const char *SPICEenvironment::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SPICEenvironment*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int SPICEenvironment::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SPICEenvironment*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *SPICEenvironment::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SPICEenvironment*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *SPICEenvironment::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SPICEenvironment*)0x0)->GetClass(); }
   return fgIsA;
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_SPICEcmdline(void *p) {
      return  p ? new(p) ::SPICEcmdline : new ::SPICEcmdline;
   }
   static void *newArray_SPICEcmdline(Long_t nElements, void *p) {
      return p ? new(p) ::SPICEcmdline[nElements] : new ::SPICEcmdline[nElements];
   }
   // Wrapper around operator delete
   static void delete_SPICEcmdline(void *p) {
      delete ((::SPICEcmdline*)p);
   }
   static void deleteArray_SPICEcmdline(void *p) {
      delete [] ((::SPICEcmdline*)p);
   }
   static void destruct_SPICEcmdline(void *p) {
      typedef ::SPICEcmdline current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SPICEcmdline

//______________________________________________________________________________
void ReFitMasterPanel::Streamer(TBuffer &R__b)
{
   // Stream an object of class ReFitMasterPanel.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(ReFitMasterPanel::Class(),this);
   } else {
      R__b.WriteClassBuffer(ReFitMasterPanel::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_ReFitMasterPanel(void *p) {
      return  p ? new(p) ::ReFitMasterPanel : new ::ReFitMasterPanel;
   }
   static void *newArray_ReFitMasterPanel(Long_t nElements, void *p) {
      return p ? new(p) ::ReFitMasterPanel[nElements] : new ::ReFitMasterPanel[nElements];
   }
   // Wrapper around operator delete
   static void delete_ReFitMasterPanel(void *p) {
      delete ((::ReFitMasterPanel*)p);
   }
   static void deleteArray_ReFitMasterPanel(void *p) {
      delete [] ((::ReFitMasterPanel*)p);
   }
   static void destruct_ReFitMasterPanel(void *p) {
      typedef ::ReFitMasterPanel current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::ReFitMasterPanel

//______________________________________________________________________________
void silicalchan::Streamer(TBuffer &R__b)
{
   // Stream an object of class silicalchan.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(silicalchan::Class(),this);
   } else {
      R__b.WriteClassBuffer(silicalchan::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_silicalchan(void *p) {
      return  p ? new(p) ::silicalchan : new ::silicalchan;
   }
   static void *newArray_silicalchan(Long_t nElements, void *p) {
      return p ? new(p) ::silicalchan[nElements] : new ::silicalchan[nElements];
   }
   // Wrapper around operator delete
   static void delete_silicalchan(void *p) {
      delete ((::silicalchan*)p);
   }
   static void deleteArray_silicalchan(void *p) {
      delete [] ((::silicalchan*)p);
   }
   static void destruct_silicalchan(void *p) {
      typedef ::silicalchan current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::silicalchan

//______________________________________________________________________________
void SPICEenvironment::Streamer(TBuffer &R__b)
{
   // Stream an object of class SPICEenvironment.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(SPICEenvironment::Class(),this);
   } else {
      R__b.WriteClassBuffer(SPICEenvironment::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_SPICEenvironment(void *p) {
      return  p ? new(p) ::SPICEenvironment : new ::SPICEenvironment;
   }
   static void *newArray_SPICEenvironment(Long_t nElements, void *p) {
      return p ? new(p) ::SPICEenvironment[nElements] : new ::SPICEenvironment[nElements];
   }
   // Wrapper around operator delete
   static void delete_SPICEenvironment(void *p) {
      delete ((::SPICEenvironment*)p);
   }
   static void deleteArray_SPICEenvironment(void *p) {
      delete [] ((::SPICEenvironment*)p);
   }
   static void destruct_SPICEenvironment(void *p) {
      typedef ::SPICEenvironment current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SPICEenvironment

namespace ROOT {
   static TClass *vectorlEstringgR_Dictionary();
   static void vectorlEstringgR_TClassManip(TClass*);
   static void *new_vectorlEstringgR(void *p = 0);
   static void *newArray_vectorlEstringgR(Long_t size, void *p);
   static void delete_vectorlEstringgR(void *p);
   static void deleteArray_vectorlEstringgR(void *p);
   static void destruct_vectorlEstringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<string>*)
   {
      vector<string> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<string>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<string>", -2, "vector", 210,
                  typeid(vector<string>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEstringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<string>) );
      instance.SetNew(&new_vectorlEstringgR);
      instance.SetNewArray(&newArray_vectorlEstringgR);
      instance.SetDelete(&delete_vectorlEstringgR);
      instance.SetDeleteArray(&deleteArray_vectorlEstringgR);
      instance.SetDestructor(&destruct_vectorlEstringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<string> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<string>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEstringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<string>*)0x0)->GetClass();
      vectorlEstringgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEstringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEstringgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<string> : new vector<string>;
   }
   static void *newArray_vectorlEstringgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<string>[nElements] : new vector<string>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEstringgR(void *p) {
      delete ((vector<string>*)p);
   }
   static void deleteArray_vectorlEstringgR(void *p) {
      delete [] ((vector<string>*)p);
   }
   static void destruct_vectorlEstringgR(void *p) {
      typedef vector<string> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<string>

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = 0);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 210,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<double>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<double>*)0x0)->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete ((vector<double>*)p);
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] ((vector<double>*)p);
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace ROOT {
   static TClass *vectorlETGraphgR_Dictionary();
   static void vectorlETGraphgR_TClassManip(TClass*);
   static void *new_vectorlETGraphgR(void *p = 0);
   static void *newArray_vectorlETGraphgR(Long_t size, void *p);
   static void delete_vectorlETGraphgR(void *p);
   static void deleteArray_vectorlETGraphgR(void *p);
   static void destruct_vectorlETGraphgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TGraph>*)
   {
      vector<TGraph> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TGraph>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TGraph>", -2, "vector", 210,
                  typeid(vector<TGraph>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETGraphgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TGraph>) );
      instance.SetNew(&new_vectorlETGraphgR);
      instance.SetNewArray(&newArray_vectorlETGraphgR);
      instance.SetDelete(&delete_vectorlETGraphgR);
      instance.SetDeleteArray(&deleteArray_vectorlETGraphgR);
      instance.SetDestructor(&destruct_vectorlETGraphgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TGraph> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TGraph>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETGraphgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TGraph>*)0x0)->GetClass();
      vectorlETGraphgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETGraphgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETGraphgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TGraph> : new vector<TGraph>;
   }
   static void *newArray_vectorlETGraphgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TGraph>[nElements] : new vector<TGraph>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETGraphgR(void *p) {
      delete ((vector<TGraph>*)p);
   }
   static void deleteArray_vectorlETGraphgR(void *p) {
      delete [] ((vector<TGraph>*)p);
   }
   static void destruct_vectorlETGraphgR(void *p) {
      typedef vector<TGraph> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TGraph>

namespace {
  void TriggerDictionaryInitialization_DictOutput_Impl() {
    static const char* headers[] = {
"include/spice_analysis_parse.h",
"include/spice_input.h",
"include/spice_calS3.h",
"include/spice_parse.h",
"include/spice_calSiLi.h",
"include/spice_enviro.h",
0
    };
    static const char* includePaths[] = {
"/home/jsmallcombe/Dropbox/codes/SPICETools/include",
"/opt/root_v6.10.04/include",
"/home/jsmallcombe/Dropbox/codes/SPICETools/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "DictOutput dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(file_name@@@include/spice_input.h)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(pattern@@@*)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$spice_input.h")))  __attribute__((annotate("$clingAutoload$include/spice_analysis_parse.h")))  SPICEcmdline;
class __attribute__((annotate(R"ATTRDUMP(file_name@@@include/spice_calSiLi.h)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(pattern@@@*)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$include/spice_calSiLi.h")))  ReFitMasterPanel;
class __attribute__((annotate(R"ATTRDUMP(file_name@@@include/spice_calSiLi.h)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(pattern@@@*)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$include/spice_calSiLi.h")))  silicalchan;
class __attribute__((annotate(R"ATTRDUMP(file_name@@@include/spice_enviro.h)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(pattern@@@*)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$include/spice_enviro.h")))  SPICEenvironment;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "DictOutput dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "include/spice_analysis_parse.h"
#include "include/spice_input.h"
#include "include/spice_calS3.h"
#include "include/spice_parse.h"
#include "include/spice_calSiLi.h"
#include "include/spice_enviro.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"", payloadCode, "@",
"AddWave", payloadCode, "@",
"CalSiLiBi207", payloadCode, "@",
"CalSiLiBi207Single", payloadCode, "@",
"ChargeParse", payloadCode, "@",
"EnergyParse", payloadCode, "@",
"Fit207BiCal", payloadCode, "@",
"Fit207BiCalSpec", payloadCode, "@",
"MakeBlankS3cal", payloadCode, "@",
"MakeSiLiBlankCal", payloadCode, "@",
"OrDefault", payloadCode, "@",
"ReFitMasterPanel", payloadCode, "@",
"ReFitMasterPanel::fgIsA", payloadCode, "@",
"S3CalParse", payloadCode, "@",
"S3CalParseChain", payloadCode, "@",
"S3FixCalBeam", payloadCode, "@",
"SPICEcmdline", payloadCode, "@",
"SPICEenvironment", payloadCode, "@",
"SPICEenvironment::fgIsA", payloadCode, "@",
"SiLiCalParse", payloadCode, "@",
"SpiceParse", payloadCode, "@",
"ZeroChannel", payloadCode, "@",
"analysis_parse", payloadCode, "@",
"cmdinteruptflag", payloadCode, "@",
"cmdsigfunction", payloadCode, "@",
"preamp_question", payloadCode, "@",
"processchain", payloadCode, "@",
"silicalchan", payloadCode, "@",
"silicalchan::fgIsA", payloadCode, "@",
"tig10address", payloadCode, "@",
"write_preamp_to_file", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("DictOutput",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_DictOutput_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_DictOutput_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_DictOutput() {
  TriggerDictionaryInitialization_DictOutput_Impl();
}
