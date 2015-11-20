//
//  consoleCallbackHelper.h
//  Torque2D
//
//  Created by James Urquhart on 25/10/2015.
//

#ifndef _CONSOLECALLBACKHELPER_H_
#define _CONSOLECALLBACKHELPER_H_

#include "sim/simConsoleThreadExecEvent.h"
#include "console/consoleBaseType.h"

//#include "console/stringStack.h"

/// @name Marshalling
///
/// Functions for converting to/from string-based data representations.
///
/// @note This functionality is specific to the console interop.
/// @{

/// Marshal a single piece of data from native into client form.
template< typename T >
inline ConsoleValuePtr ConsoleMarshallData( const T& value )
{
   //return castConsoleTypeToConsoleValuePtr( value );
   return ConsoleValuePtr::NullValue;
}
inline ConsoleValuePtr ConsoleMarshallData( bool value )
{
   ConsoleValuePtr cvalue;
   cvalue.setValue(value);
   return cvalue;
}
inline ConsoleValuePtr ConsoleMarshallData( const char *str )
{
   // The API assumes that if you pass a plain "const char*" through it, then you are referring
   // to string storage with non-local lifetime that can be safely passed to the control layer.
   ConsoleValuePtr cvalue;
   cvalue.setString(str);
   return cvalue;
}
inline ConsoleValuePtr ConsoleMarshallData( const unsigned char* str )
{
   // The API assumes that if you pass a plain "const char*" through it, then you are referring
   // to string storage with non-local lifetime that can be safely passed to the control layer.
   ConsoleValuePtr cvalue;
   cvalue.setString((const char*)str);
   return cvalue;
}
inline ConsoleValuePtr ConsoleMarshallData( U32 value )
{
   return ConsoleMarshallData( S32( value ) );
}

/// Marshal data from native into client form stored directly in
/// client function invocation vector.
template< typename T >
inline void ConsoleMarshallData( const T& arg, S32& argc, ConsoleValuePtr *argv )
{
   argv[ argc ] = castConsoleTypeToString( arg );
   argc ++;
}
inline void ConsoleMarshallData( bool arg, S32& argc, ConsoleValuePtr *argv )
{
   if( arg )
      argv[ argc ].setValue(1);
   else
      argv[ argc ].setValue(0);
   argc ++;
}
inline void ConsoleMarshallData( S32 arg, S32& argc, ConsoleValuePtr *argv )
{
   argv[ argc ].setValue(arg);
   argc ++;
}
inline void ConsoleMarshallData( U32 arg, S32& argc, ConsoleValuePtr *argv )
{
   ConsoleMarshallData( S32( arg ), argc, argv );
}
inline void ConsoleMarshallData( F32 arg, S32& argc, ConsoleValuePtr *argv )
{
   argv[ argc ].setValue(arg);
   argc ++;
}
inline void ConsoleMarshallData( const char* arg, S32& argc, ConsoleValuePtr *argv )
{
   argv[ argc ].setValue(ConsoleStringValue::fromString(arg));
   argc ++;
}
inline void ConsoleMarshallData( char* arg, S32& argc, ConsoleValuePtr *argv )
{
   argv[ argc ].setValue(ConsoleStringValue::fromString(arg));
   argc ++;
}
inline void ConsoleMarshallData( const unsigned char* arg, S32& argc, ConsoleValuePtr *argv )
{
   argv[ argc ].setValue(ConsoleStringValue::fromString((const char*)arg));
   argc ++;
}
inline void ConsoleMarshallData( unsigned char* arg, S32& argc, ConsoleValuePtr *argv )
{
   argv[ argc ].setValue(ConsoleStringValue::fromString((const char*)arg));
   argc ++;
}
inline void ConsoleMarshallData( ConsoleValuePtr arg, S32& argc, ConsoleValuePtr *argv )
{
   argv[ argc ].setValue(arg);
   argc ++;
}

/// Unmarshal data from client form to engine form.
///
/// This is wrapped in an a struct as partial specializations on function
/// templates are not allowed in C++.
template< typename T >
struct ConsoleUnmarshallData
{/*
   T operator() (void) const
   {
      T t;
      return t;
   }
   */
   T operator()( const char *str ) const
   {
      T value;
      castConsoleTypeFromString( value, str );
      return value;
   }
};
template<>
struct ConsoleUnmarshallData< S32 >
{
   S32 operator() (void) const
   {
      return 0;
   }
   
   S32 operator()( ConsoleValuePtr &ref ) const
   {
      return (S32)ref;
   }
   
   S32 operator()( const char *str ) const
   {
      return dAtoi( str );
   }
};
template<>
struct ConsoleUnmarshallData< U32 >
{
   U32 operator() (void) const
   {
      return 0;
   }
   
   U32 operator()( ConsoleValuePtr &ref ) const
   {
      return (U32)((S32)ref);
   }
   
   U32 operator()( const char *str ) const
   {
      return dAtoi( str );
   }
};
template<>
struct ConsoleUnmarshallData< F32 >
{
   S32 operator() (void) const
   {
      return 0.0f;
   }
   
   F32 operator()( ConsoleValuePtr &ref ) const
   {
      return ref.getFloatValue();
   }
   
   F32 operator()( const char *str ) const
   {
      return dAtof( str );
   }
};
template<>
struct ConsoleUnmarshallData< U8 >
{
   U8 operator() (void) const
   {
      return 0;
   }
   
   U8 operator()( ConsoleValuePtr &ref ) const
   {
      return (U8)((S32)ref);
   }
   
   U8 operator()( const char *str ) const
   {
      return dAtoi( str );
   }
};
template<>
struct ConsoleUnmarshallData< const char* >
{
   const char* operator() (void) const
   {
      return "";
   }
   
   const char* operator()( ConsoleValuePtr &ref ) const
   {
      return ref.getTempStringValue();
   }
   
   const char* operator()( const char *str ) const
   {
      return str;
   }
};

template<>
struct ConsoleUnmarshallData< void >
{
   void operator() (void) const {}
   void operator()( ConsoleValuePtr& ) const {}
   void operator()( const char* ) const {}
};


template<>
struct ConsoleUnmarshallData< ConsoleValuePtr >
{
   ConsoleValuePtr operator()( ConsoleValuePtr ref ) const
   {
      return ref;
   }
};

/// @}

// Internal helper for callback support in legacy console system.
struct _BaseEngineConsoleCallbackHelper
{
public:
   
   /// Matches up to storeArgs.
   static const U32 MAX_ARGUMENTS = 11;
   
   SimObject* mThis;
   S32 mInitialArgc;
   S32 mArgc;
   StringTableEntry mCallbackName;
   ConsoleValuePtr mArgv[ MAX_ARGUMENTS + 2 ];
   
   ConsoleValuePtr _exec();
   ConsoleValuePtr _execLater(SimConsoleThreadExecEvent *evt);
   
   void postEvent(SimConsoleThreadExecEvent* evt);
   
   _BaseEngineConsoleCallbackHelper() {;}
};

// Override for when first parameter is presumably a SimObject*, in which case A will be absorbed as the callback
template<typename P1> struct _EngineConsoleExecCallbackHelper : public _BaseEngineConsoleCallbackHelper
{
public:
   
   _EngineConsoleExecCallbackHelper( SimObject* pThis )
   {
      mThis = pThis;
      mArgc = mInitialArgc = 2;
      mCallbackName = NULL;
   }
   
   
   template< typename R, typename A >
   R call( A a )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+0, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B >
   R call( A a, B b )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+1, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C >
   R call( A a, B b, C c )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+2, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D >
   R call( A a, B b, C c, D d )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+3, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E >
   R call( A a, B b, C c, D d, E e )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+4, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F >
   R call( A a, B b, C c, D d, E e, F f )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+5, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G >
   R call( A a, B b, C c, D d, E e, F f, G g )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+6, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H >
   R call( A a, B b, C c, D d, E e, F f, G g, H h )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+7, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I >
   R call( A a, B b, C c, D d, E e, F f, G g, H h, I i )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+8, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J >
   R call( A a, B b, C c, D d, E e, F f, G g, H h, I i, J j )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         ConsoleMarshallData( j, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+9, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         ConsoleMarshallData( j, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K >
   R call( A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         ConsoleMarshallData( j, mArgc, mArgv );
         ConsoleMarshallData( k, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+10, NULL, false, &cb);
         
         mArgv[ 0 ].setString(a); mCallbackName = mArgv[0].getSTEStringValue();
         
         //ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         ConsoleMarshallData( j, mArgc, mArgv );
         ConsoleMarshallData( k, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
};

// Override for when first parameter is const char*
template<> struct _EngineConsoleExecCallbackHelper<const char*> : public _BaseEngineConsoleCallbackHelper
{
public:
   
   _EngineConsoleExecCallbackHelper( const char* callbackName )
   {
      mThis = NULL;
      mArgc = mInitialArgc = 1;
      mCallbackName = callbackName;
   }
   
   template< typename R>
   R call()
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   
   template< typename R, typename A >
   R call( A a )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+1, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B >
   R call( A a, B b )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+2, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C >
   R call( A a, B b, C c )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+3, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D >
   R call( A a, B b, C c, D d )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+4, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E >
   R call( A a, B b, C c, D d, E e )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+5, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F >
   R call( A a, B b, C c, D d, E e, F f )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+6, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G >
   R call( A a, B b, C c, D d, E e, F f, G g )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+7, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H >
   R call( A a, B b, C c, D d, E e, F f, G g, H h )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+8, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I >
   R call( A a, B b, C c, D d, E e, F f, G g, H h, I i )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+9, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J >
   R call( A a, B b, C c, D d, E e, F f, G g, H h, I i, J j )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         ConsoleMarshallData( j, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+10, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         ConsoleMarshallData( j, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
   template< typename R, typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K >
   R call( A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k )
   {
      if (Con::isMainThread())
      {
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         ConsoleMarshallData( j, mArgc, mArgv );
         ConsoleMarshallData( k, mArgc, mArgv );
         
         
         return R( ConsoleUnmarshallData< R >()( _exec() ) );
      }
      else
      {
         SimConsoleThreadExecCallback cb;
         SimConsoleThreadExecEvent *evt = new SimConsoleThreadExecEvent(mArgc+11, NULL, false, &cb);
         
         mArgv[ 0 ].setString(mCallbackName);
         
         ConsoleMarshallData( a, mArgc, mArgv );
         ConsoleMarshallData( b, mArgc, mArgv );
         ConsoleMarshallData( c, mArgc, mArgv );
         ConsoleMarshallData( d, mArgc, mArgv );
         ConsoleMarshallData( e, mArgc, mArgv );
         ConsoleMarshallData( f, mArgc, mArgv );
         ConsoleMarshallData( g, mArgc, mArgv );
         ConsoleMarshallData( h, mArgc, mArgv );
         ConsoleMarshallData( i, mArgc, mArgv );
         ConsoleMarshallData( j, mArgc, mArgv );
         ConsoleMarshallData( k, mArgc, mArgv );
         
         
         postEvent(evt);
         
         return R( ConsoleUnmarshallData< R >()( cb.waitForResult() ) );
      }
   }
   
};

#endif
