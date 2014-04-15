-- Lua test script

function doError()
    error("Error occured")
end

function do_something_with_testobj(obj)
	print("Object test...")
	obj.number = 1234
	print("Number is: " .. obj.number)
	print("Message is: " .. obj.message)
    print("Object analysis is: " .. obj:getAnalysis())
	print("Testing object method with exception...")
    obj:testException()
	print("Done")
end

function main(param)
	print("We were called with: " .. tostring(param))

	do_something_with_testobj(param)

	print("Creating new object...")
	obj = TestObject()
    obj.message = "Created from lua"
	do_something_with_testobj(obj)
end



PR = nil
P0 = nil
P1 = nil

function functionWP(p1, p2)
    return p1 + p2
end

function functionNP()
    PR = P0 + P1
end

function functionRP()
    local p1 = P0
    local p2 = P1
    PR = p1 + p2
end

function scriptTest1()
    local start = os.clock()
    local count = 0
    local i = 0
    for i=1,1000000 do
        count = count + functionWP(999999, 10101010)
    end
    --print "Count: " . count
    return os.clock() - start
end

function scriptTest2()
    local start = os.clock()
    local count = 0
    local i = 0
    for i = 1,1000000 do
        P0 = 999999
        P1 = 10101010
        functionNP()
        count = count + PR
    end
    --print "Count: " . count
    return os.clock() - start
end

function scriptTest3()
    local start = os.clock()
    local count = 0
    local i = 0
    for i=1,1000000 do
        P0 = 999999
        P1 = 10101010
        functionRP()
        count = count + PR
    end
    --print "Count: " . count
    return os.clock() - start
end

function scriptTest6()
    local start = os.clock()
    local count = 0
    local i = 0
    for i=1,1000000 do
        local value = 999999 + 10101010
        count = count + value
    end
    return os.clock() - start
end

gtime = 0
function runTest(testName)
io.write(testName)
local count = 0

for i=1,5 do
load("gtime = " .. testName .. "()")()
count = count + (gtime*1000.0)
io.write(string.format(",%f", gtime*1000.0))
end

io.write(string.format(",%f\n", count / 5.0))
end


print("TEST,1,2,3,4,5,AVG")
runTest("scriptTest1")
runTest("scriptTest2")
runTest("scriptTest3")
runTest("scriptTest6")


