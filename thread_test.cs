// Coroutine / Thread test

function testFunc(%c)
{
	echo("testFunc ", %c);
	%foo = 0;
	echo("test" SPC %foo SPC 0);
	%c.yield();
	%c.save("script1.dat");
	%foo++;
	echo("test" SPC %foo SPC 1);
	%c.save("script2.dat");
	%c.yield();
	%foo++;
	echo("test" SPC %foo SPC 2);
	%c.save("script3.dat");
	%c.yield();
	%foo++;
	echo("test" SPC %foo SPC 3);
	%c.save("script4.dat");
	%c.yield();
	%foo++;
	%c.save("script5.dat" SPC 4);

	return "DONE";
}

%c = Coroutine::create(testFunc);
%c.resume(%c);
%c.resume();
%c.resume();
%c.resume();
%c.resume();

echo("-- TEST LOAD COROUTINE --");
%foo = Coroutine::load("script2.dat");
echo("LOAD RESUME");
%foo.resume();
echo("LOAD RESUME");
%foo.resume();
echo("--");

echo("-- THREAD TEST --");
%threadManager = new SimScriptThreadManager();
$threadTestRunning = true;

echo("Thread test running ? " SPC $threadTestRunning);

function testThreadFunc(%c, %identifier)
{
	nativeDebugBreak();
	echo("CHECK GLOBALS ", $threadTestRunning);
	while ($threadTestRunning)
	{
		echo(%identifier SPC " Running");
		%c.yield();
	}
	echo("Thread now terminated");
}

function SimScriptThreadManager::resumeRoutines(%this, %routines)
{
	%size = len(%routines);
	echo("Ticking " SPC %size SPC "routines");
	for (%i=0; %i<%size; %i++)
	{
		%routines[%i].resume();
	}
}

%c = Coroutine::create(testThreadFunc);
%c.resume(%c, thread1wait10ticks);
%threadManager.addThread(%c);
%c.setWaitTicks(10);

%c = Coroutine::create(testThreadFunc);
%c.resume(%c, thread2wait5ticks);
%threadManager.addThread(%c);
%c.setWaitTicks(5);

%c = Coroutine::create(testThreadFunc);
%c.resume(%c, thread3defaultticks);
%threadManager.addThread(%c);

%c = Coroutine::create(testThreadFunc);
%c.resume(%c, thread3noticks);
%threadManager.addThread(%c);
%c.setNoTicks();

%tickTotal = 1000.0/16;
for (%i=0; %i<%tickTotal; %i++)
{
	echo("Tick" SPC %i);
	%threadManager.tick();
}

%state = %threadManager.saveThreads("threadtest.dat");

echo("Final tick (should remove all dead threads)");
$threadTestRunning = false;
%threadManager.tick();
%threadManager.tick();
echo("--");

$ScriptErrorHash = 100;
quit();
