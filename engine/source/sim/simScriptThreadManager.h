//
//  simScriptThreadManager.h
//  Torque2D
//
//  Created by James Urquhart on 28/12/2015.
//

#ifndef _SCRIPT_THREADMANAGER_H_
#define _SCRIPT_THREADMANAGER_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

//-----------------------------------------------------------------------------

/*
 SimScriptThreadManager
 
 Manages a list of tickable coroutines. To keep things simple, actual execution is 
 handled in a script method, `SimScriptThreadManager::resumeRoutines(%manager, %routines)`.
 
 Each coroutine has a waitTicks value which basically counts down to the next tick. When this 
 value reaches 0, the coroutine will be passed to the resumeRoutines method which should 
 run the coroutine.
 */
class SimScriptThreadManager : public SimObject, public virtual Tickable
{
   typedef SimObject Parent;
   Vector<ConsoleValuePtr> mThreads;
   ConsoleValuePtr mThreadsToUpdate;
   
public:
   SimScriptThreadManager();
   ~SimScriptThreadManager();
   
   void cleanup();
   void onRemove();
   
   void addThread(CodeBlockCoroutineState *thread);
   void removeThread(CodeBlockCoroutineState *thread);
   
   bool saveThreads(Stream &s);
   bool loadThreads(Stream &s);
   
   void tick();
   
   void processTick( void );
   void interpolateTick( F32 delta );
   void advanceTime( F32 timeDelta );
   
   DECLARE_CONOBJECT(SimScriptThreadManager);
   
   static U32 SAVE_VERSION;
};



#endif
