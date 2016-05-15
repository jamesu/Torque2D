
$shit = 1;
$fudge = 2 + 2.3;

if ($fudge < 2) // TOFIX
{
	$shit = 2;
}
else
{
	$shit = 3;
}


$poof = 123;
$err = 456 + 789;
$fudge = 12 & 0x100;

$fudge[0] = 0;

%local = "value";

/// Docblock here
new TestObject(err, 1, 2, 3)
{
	field1 = "123";
	arrayField[0] = "456";

	new Juice(y)
	{
		fresh = true;
		importedLocal = %local;

		new Melon()
		{

		};

		new Bannana()
		{

		};
	};

	new Tea()
	{

	};
};

%candy[0] = 123;
%candy[$poof] = 456;
%candy[$poof, 0] = 567;

    for (%i=0; %i<5; %i++)
    {
	    debugTimer(false);
	    %count = 0;
		for (%j=0; %j<1000000;%j++){//1000000; %j++) {
			//%value = 999999;
			%value = 999999 + 10101010;
			%count += %value;
			simpleCounter(false);
		}
			simpleCounter(true);
	    debugTimer(true);
	}

	function foo(%err)
	{
		return 123;
	}

	echo(foo());
/*

	[0] OP_LOADK 0 0 
[1] OP_LT 0 0 257 
[2] OP_JMP 19 
[3] OP_GETFUNC -> 1 
[4] OP_LOADK 2 0 
[5] OP_CALL 1 1 0 
[6] OP_LOADK 1 0 
[7] OP_LOADK 2 0 
[8] OP_LT 0 2 260 
[9] OP_JMP 6 
Compiler2::OP_ADD[10] -> (262 & 261) (999999.000000 & 10101010.000000) -> 4 
[11] OP_MOV 3 4 
Compiler2::OP_ADD[12] -> (1 & 3) (0.000000 & 0.000000) -> 1 
Compiler2::OP_ADD[13] -> (2 & 263) (0.000000 & 1.000000) -> 2 
[14] OP_LT 1 2 260 
[15] OP_JMP -6 
[16] OP_GETFUNC -> 4 
[17] OP_LOADK 5 8 
[18] OP_CALL 4 1 0 
Compiler2::OP_ADD[19] -> (0 & 263) (0.000000 & 1.000000) -> 0 
[20] OP_LT 1 0 257 
[21] OP_JMP -19 
[22] OP_LOADK 4 9 
[23] OP_SETVAR 0 266 4 
[24] OP_GETFUNC -> 5 
[25] OP_CALL 5 0 0 
[26] OP_RETURN 0 

1	[58]	GETTABUP 	0 0 -1	; _ENV "os"
	2	[58]	GETTABLE 	0 0 -2	; "clock"
	3	[58]	CALL     	0 1 2
	4	[59]	LOADK    	1 -3	; 0
	5	[60]	LOADK    	2 -3	; 0
	6	[62]	LT       	0 2 -4	; - 1000000
	7	[62]	JMP      	0 5	; to 13
	8	[63]	LOADK    	3 -5	; 999999
	9	[64]	ADD      	3 3 -6	; - 10101010
	10	[65]	ADD      	1 1 3
	11	[66]	ADD      	2 2 -7	; - 1
	12	[66]	JMP      	0 -7	; to 6
	13	[69]	GETTABUP 	3 0 -1	; _ENV "os"
	14	[69]	GETTABLE 	3 3 -2	; "clock"
	15	[69]	CALL     	3 1 2
	16	[69]	SUB      	3 3 0
	17	[69]	RETURN   	3 2
	18	[70]	RETURN   	0 1




*/
$ScriptErrorHash = 100;
quit();
//echo("Time start" SPC %startTime SPC End SPC %endTime SPC Sum SPC (%endTime - %startTime));


/*
OP_LOADK 0 0 
OP_SETVAR 0 257 0          ; $shit = 1; 
Compiler2::OP_ADD -> 2     ; fudge_local = 2 + 2.3
OP_MOV 1 2                 ; conv to var
OP_SETVAR 0 260 1          ; $fudge = 2 + 2.3 [fudge_local]
OP_LOADVAR 1 260           ; 
OP_LT 0 1 259              ; if ($fudge < 2) 
OP_JMP ...
OP_LOADK 0 5               ;
OP_SETVAR 0 257 0          ; $shit = 2;
OP_GETFUNC -> 2            ; grabs echo ref
OP_LOADVAR 0 257           ; Load $shit
OP_LOADK 3 8               ; Load test print
OP_LOADK 4 9               ; Load SPC
OP_MOV 5 0                 ; Plonk var in the right place
OP_CALL 2 1 0              ; echo(...);

; alternate echo 
OP_GETFUNC -> 2 
OP_LOADVAR 0 257 
Compiler2::OP_ADD -> 6 (0 + k2)
OP_LOADK 3 8 ; Test Print from TST
OP_LOADK 4 9 ; SPC
OP_MOV 5 6   ; result from OP_ADD
OP_CONCAT 3 3 6
OP_CALL 2 1 0 
Test print from TS2 5 


OP_RETURN 0

*/

/*$piss = "Poop";

function doShit()
{
	%err = 123;
	return %err;
}

setLogMode(2);
echo(doShit());*/

// Set log mode.
/*setLogMode(2);

// Set profiler.
//profilerEnable( true );

// Controls whether the execution or script files or compiled DSOs are echoed to the console or not.
// Being able to turn this off means far less spam in the console during typical development.
setScriptExecEcho( true );

// Controls whether all script execution is traced (echoed) to the console or not.
trace( false );

function testFunction(%this)
{
	%fudge = 1;
	%pi = 3.14 + 2;
	%shit = %fudge + %pi;
	return %fudge;
}
*/