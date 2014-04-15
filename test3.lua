function SimObject:testFunction(param)
	Torque.echo("Called testFunction with parameter " .. param)
end

function SimObject:printProperty()
	Torque.echo("Set assignedId is " .. self.assignedId)
end

function testFunction()
	Torque.echo("Errrr")

	myObject = SimObject()
	myObject.assignedId = myObject:getId()
	myObject:testFunction("candy")
	myObject:printProperty()
	myObject:dump()

	Torque.echo("Backtrace is as follows: " .. debug.traceback())
end
