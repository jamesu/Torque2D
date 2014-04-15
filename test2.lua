-- Lua test script

function bridge1Test()
	local obj = SimObject()
	local count = 0
    local start = os.clock()

	for i=1,1000000 do
		obj:getId()
	end

    return os.clock() - start
end

function bridge2Test()
	local obj = SimObject()
	local count = 0
    local start = os.clock()

	for i=1,1000000 do
		obj.prop = '123'
	end
	
    return os.clock() - start
end

function bridge3Test()
	local obj = SimObject()
	local count = 0
    local start = os.clock()

	for i=1,1000000 do
		obj:setFieldValue('prop', '123')
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
runTest("bridge1Test")
runTest("bridge2Test")
runTest("bridge3Test")


