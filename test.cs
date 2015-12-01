// TwistScript basic test

//echo("Test conditional block");

$shazbot = 1;
$fudge = 2 + 2.3;


//

RootGroup.someField = "fred";

if (RootGroup.someField $= "Fred")
{
	echo("SlotAccess string conditional passed");
}
else
{
	echo("SlotAccess string conditional failed");
}

if (RootGroup.someField !$= "fred")
{
	echo("SlotAccess string conditional failed");
}
else
{
	echo("SlotAccess string conditional passed");
}

if (RootGroup.someField !$= "george")
{
	echo("SlotAccess string conditional passed");
}
else
{
	echo("SlotAccess string conditional failed");
}

if (RootGroup.someField $= "george")
{
	echo("SlotAccess string conditional failed");
}
else
{
	echo("SlotAccess string conditional passed");
}

// Specific case
//%moduleDefinition.moduleId $= $pref::Sandbox::defaultToyId
$georgeVar = "george";
$fredVar = "fred";

if (RootGroup.someField $= $fredVar)
{
	echo("SlotAccess string global conditional passed");
}
else
{
	echo("SlotAccess string global conditional failed");
}

if (RootGroup.someField $= $georgeVar)
{
	echo("SlotAccess string global conditional failed");
}
else
{
	echo("SlotAccess string global conditional passed");
}

if (RootGroup.someField !$= $fredVar)
{
	echo("SlotAccess string global conditional failed");
}
else
{
	echo("SlotAccess string global conditional passed");
}

%groupVar = RootGroup;


if (%groupVar.someField $= $fredVar)
{
	echo("SlotAccess string local global conditional passed");
}
else
{
	echo("SlotAccess string local global conditional failed");
}


if (%groupVar.someField !$= $fredVar)
{
	echo("SlotAccess string local global conditional failed");
}
else
{
	echo("SlotAccess string local global conditional passed");
}

// 

%numVar = 4;
if (%numVar < 1 || %numVar > 5)
{
	echo("OR conditional failed");
}
else
{
	echo("OR conditional passed");
}

if ($fudge < 2)
{
    $shazbot = 2;
}
else
{
    $shazbot = 3;
}

if (!true)
{
	echo("FAIL UNARY");
}
else
{
	echo("PASS UNARY");
}


if (!false)
{
	echo("PASS UNARY");
}
else
{
	echo("FAIL UNARY");
}

new ScriptObject(Err) {};

Err.testField += 1;

Err.dump();

echo("--");
echo($shazbot);
echo("--");

%test = createArray();
$test = createArray();
%test[123] += 1;
$test[123] += 2;

echo("Should print 1 then 2");
echo(%test[123]);
echo($test[123]);

echo("--");

// should output 
// !!! 2 > 3 
// !! 2 > 3 
// ! 2 > 3 
// 2 > 3 

echo((1 > 2) ? (2 > 3 ? "2 > 3" : "! 2 > 3") : ((2+0 > 3) ? "!! 2 > 3" : "!!! 2 > 3"));
echo((1 > 2) ? (2 > 3 ? "2 > 3" : "! 2 > 3") : ((3+0 > 2) ? "!! 2 > 3" : "!!! 2 > 3"));
echo((2 > 1) ? (2 > 3 ? "2 > 3" : "! 2 > 3") : ((3+0 > 2) ? "!! 2 > 3" : "!!! 2 > 3"));
echo((2 > 1) ? (3 > 2 ? "2 > 3" : "! 2 > 3") : ((3+0 > 2) ? "!! 2 > 3" : "!!! 2 > 3"));

// should output 
// !!! 2 > 3 
// !! 2 > 3 
// ! 2 > 3 
// 2 > 3 



echo("Test conditional assignment");
%f1 = 1 && 0;
%f2 = 1 && 1;
%f3 = 0 && 1;

echo("CHK" SPC %f1 SPC %f2 SPC %f3);

echo("Testing basic loop");
for (%i=0; %i<5; %i++)
{
    debugTimer(false);
    %count = 0;
    for (%j=0; %j<1000;%j++){//000000;%j++){//1000000; %j++) {
        //%value = 999999;
        %value = 999999 + 10101010;
        %count += %value;
        simpleCounter(false);
    }
    simpleCounter(true);
    debugTimer(true);
}

echo("Testing conditionals");

if (1 && 1)
{
    echo("[0]test AND conditional passed");
}
else
{
    echo("[0]test AND conditional passed");
}

if (1 && 0)
{
    echo("[1]test AND conditional FALED");
}
else
{
    echo("[1]test AND conditional PASSED");
}

if (0 && 1)
{
    echo("[2]test AND conditional FALED");
}
else
{
    echo("[2]test AND conditional PASSED");
}

if ("0" || "1")
{
    echo("[1]test OR conditional passed");
}
else
{
    echo("[1]test OR conditional failed");
}

if ("1" || "0")
{
    echo("[2]test OR conditional passed");
}
else
{
    echo("[2]test OR conditional failed");
}

if ("0" || "0")
{
    echo("[3]test OR conditional failed");
}
else
{
    echo("[3]test OR conditional passed");
}

echo("Test methods");
echo(RootGroup.testMethod());

echo("Defining testFunc...");
function SimObject::testFunc(%this, %a, %b, %c, %d)
{
    echo("testFunc PARAMS: " SPC %this SPC %a SPC %b SPC %c SPC %d);
}

testExecute(RootGroup);

echo("Defining new functions...");

function func1()
{
    echo("func");
    func2();
    echo("EOfunc");
}

function func2()
{
    echo("func2");
    func3();
    echo("EOfunc2");
}

function func3()
{
    echo("func3");
}

echo("calling func1");
func1();


function testFunction(%a, %b)
{
    return %a + %b;
}


echo("calling testFunction");
echo("Function call returns" SPC testFunction(1, 2) SPC testFunction(4,5));


function testReturnCond(%value)
{
	return %value;
}

if (testReturnCond(true))
{
	echo("testReturnCond PASS");
}
else
{
	echo("testReturnCond FAIL");
}

if (testReturnCond(false))
{
	echo("testReturnCond FAIL");
}
else
{
	echo("testReturnCond PASS");
}


if (!testReturnCond(true))
{
	echo("testReturnCond UNARY FAIL");
}
else
{
	echo("testReturnCond UNARY PASS");
}

if (!testReturnCond(false))
{
	echo("testReturnCond UNARY PASS");
}
else
{
	echo("testReturnCond UNARY FAIL");
}

function testOR(%val)
{
	echo("testOR" @ %val);
	return %val;
}

//



$hello = "Hello";
%hello = "Hello";

if (%hello $= %hello)
{
	echo("String check passed");
}
else
{
	echo("String check failed");
}


if (%hello !$= %hello)
{
	echo("String check failed");
}
else
{
	echo("String check passed");
}

if (true)
{
	echo("IntConst passed");
}
else
{
	echo("IntConst failed");
}

if (1<<0)
{
	echo("IntBinary passed");
}
else
{
	echo("IntBinary failed");
}

if (1<0)
{
	echo("IntBinary lt failed");
}
else
{
	echo("IntBinary lt passed");
}

if (0<1)
{
	echo("IntBinary lt passed");
}
else
{
	echo("IntBinary lt failed");
}

if (1>0)
{
	echo("IntBinary gt passed");
}
else
{
	echo("IntBinary gt passwd");
}

if(1>=0)
{
	echo("IntBinary gte passed");
}
else
{
	echo("IntBinary gte failed");
}

if(1>=1)
{
	echo("IntBinary gte passed");
}
else
{
	echo("IntBinary gte failed");
}


if(1<=0)
{
	echo("IntBinary lte failed");
}
else
{
	echo("IntBinary lte passed");
}

if(1<=1)
{
	echo("IntBinary lte passed");
}
else
{
	echo("IntBinary lte failed");
}

if (1%10)
{
	echo("IntBinary % passed");
}
else
{
	echo("IntBinary % failed");
}

if (0%10)
{
	echo("IntBinary % failed");
}
else
{
	echo("IntBinary % passed");
}

if (0==1)
{
	echo("IntBinary == failed");
}
else
{
	echo("IntBinary == passed");
}

if (1==1)
{
	echo("IntBinary == passed");
}
else
{
	echo("IntBinary == failed");
}

if (0!=1)
{
	echo("IntBinary != passed");
}
else
{
	echo("IntBinary != failed");
}

if (1!=1)
{
	echo("IntBinary != failed");
}
else
{
	echo("IntBinary != passed");
}

if (1.0)
{
	echo("FloatConst passed");
}
else
{
	echo("FloatConst failed");
}



if (1.0+0.0)
{
	echo("FloatBinary + passed");
}
else
{
	echo("FloatBinary + failed");
}

if (1.0+1.0)
{
	echo("FloatBinary + passed");
}
else
{
	echo("FloatBinary + failed");
}

if (0.0+0.0)
{
	echo("FloatBinary + failed");
}
else
{
	echo("FloatBinary + passed");
}

if (1.0-0.0)
{
	echo("FloatBinary - passed");
}
else
{
	echo("FloatBinary - failed");
}

if (1.0-1.0)
{
	echo("FloatBinary - failed");
}
else
{
	echo("FloatBinary - passed");
}

if (1.0*1.0)
{
	echo("FloatBinary * passed");
}
else
{
	echo("FloatBinary * failed");
}

if (1.0/1.0)
{
	echo("FloatBinary / passed");
}
else
{
	echo("FloatBinary / failed");
}


if (true && false)
{
	echo("&& failed");
}
else
{
	echo("&& passed");
}

if (true && true)
{
	echo("&& passed");
}
else
{
	echo("&& failed");
}

if (true && %hello $= %hello)
{
	echo("ERR");
}

//

echo("Should print 0,1");
%foo = testOR(false) || testOR(true);
echo(%foo);
echo("Should print 1");
%foo = testOR(true) || testOR(false);
echo(%foo);


// Variable assignment

$foo = 123;
echo("foo is" SPC $foo);
$foo += 1;
echo("foo is" SPC $foo);

echo("woo is " SPC $woo);

echo("jerk is" SPC %jerk);

echo("foreach loop");

foreach(%group in RootGroup)
{
    echo("WORD:" @ %group.getName());
}

echo("Making test SimGroup");




new SimGroup(Frodo, 1, 2, 3)
{
    hasRing = 1;
    bestFriend = "sam";
    movies = 1 * 3;

    internalName = "RingBearer";

    new ScriptObject(Ring)
    {
        owner = "sauron";
        internalName = "ring";
    };

    new ScriptObject(MithrilArmour)
    {
        strength = 100000;
        internalName = "armour";
    };
};

function Frodo::doThis(%this)
{
    echo("Dothis called");
    echo(%this.internalName);
    echo("Best friend is:" SPC %this.bestFriend);
    %this.newField = 123;
    echo(%this.newField);

    echo("Owner of ring is:" SPC %this-->ring.owner);
}


// not equal, equal
echo("Frodo" $= 123 ? "equal" : "not equal");
echo(123 $= "123" ? "equal" : "not equal");

// should print "not equal equal"
%var = "Frodo" $= 123 ? "equal" : "not equal";
%var2 = 123 $= "123" ? "equal" : "not equal";
echo(%var," ",%var2);

Frodo.doThis();
echo("doThis done");

Frodo.dump();

// Eval test

function testExecFunc()
{
	%var1 = 123; // (reg 1)
	%var2 = 456; // (reg 2)

	echo("Eval test");
	echo(%var1 SPC %var2);


	eval("%var1 += 1; %var3 = 4;");
	echo(%var1 SPC %var2);
	eval("%var2 += 2; echo(var3,%var3);");
	echo(%var1 SPC %var2);
}

testExecFunc();


$ScriptErrorHash = 100;
quit();