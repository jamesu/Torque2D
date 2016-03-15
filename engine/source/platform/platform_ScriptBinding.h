//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

/*! @defgroup PlatformFunctions Platform
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the setMouseLock function to un/lock the mouse.
    @param isLocked A boolean value.
    @return No return value
*/
ConsoleFunctionWithDocs( setMouseLock, ConsoleVoid, 2, 2, ( isLocked ))
{
    Platform::setMouseLock(dAtob(argv[1]));
}

//-----------------------------------------------------------------------------

/*! Use the getRealTime function to the computer time in milliseconds.
    @return Returns the current real time in milliseconds.
    @sa getSimTime
*/
ConsoleFunctionWithDocs( getRealTime, ConsoleInt, 1, 1, ())
{
    return Platform::getRealMilliseconds();
}

//-----------------------------------------------------------------------------

/*! Get the local time
    @return the local time formatted as: monthday/month/year hour/minute/second
*/
ConsoleFunctionWithDocs( getLocalTime, ConsoleString, 1, 1, ())
{
    char* buf = Con::getReturnBuffer(128);

    Platform::LocalTime lt;
    Platform::getLocalTime(lt);

    dSprintf(buf, 128, "%d/%d/%d %02d:%02d:%02d",
        lt.monthday,
        lt.month + 1,
        lt.year + 1900,
        lt.hour,
        lt.min,
        lt.sec);

    return buf;
}

/*! Use the getClipboard function to get the contents of the GUI clipboard.
	@return Returns a string equal to the current contents of the copy the clipboard, or a NULL strain if the copy clipboard is empty.
	@sa setClipboard
*/
ConsoleFunctionWithDocs( getClipboard, ConsoleString, 1, 1, ())
{
    return Platform::getClipboard();
};

/*!  Use the setClipboard function to Set value on clipboard to string.
	@param string The new value to place in the GUI clipboard.
	@return Returns true if successful, false otherwise.
	@sa getClipoard")
*/
ConsoleFunctionWithDocs( setClipboard, ConsoleBool, 2, 2, ( string ))
{
    return Platform::setClipboard(argv[1]);
};

/*! Creates a UUID string.
*/
ConsoleFunctionWithDocs( createUUID, ConsoleString, 1, 1, () )
{
    return Platform::createUUID();
}


//----------------------------------------------------------------

/*! Open a URL in the user's favorite web browser.
 */
ConsoleFunctionWithDocs( gotoWebPage, ConsoleVoid, 2, 2, ( address ))
{
	TORQUE_UNUSED( argc );
	char* protocolSep = dStrstr(argv[1],"://");
	
	if( protocolSep != NULL )
	{
		Platform::openWebBrowser(argv[1]);
		return;
	}
	
	// if we don't see a protocol seperator, then we know that some bullethead
	// sent us a bad url. We'll first check to see if a file inside the sandbox
	// with that name exists, then we'll just glom "http://" onto the front of
	// the bogus url, and hope for the best.
	
	char urlBuf[2048];
	if(Platform::isFile(argv[1]) || Platform::isDirectory(argv[1]))
	{
		dSprintf(urlBuf, sizeof(urlBuf), "file://%s",argv[1].getTempStringValue());
	}
	else
		dSprintf(urlBuf, sizeof(urlBuf), "http://%s",argv[1].getTempStringValue());
	
	Platform::openWebBrowser(urlBuf);
	return;
}

//----------------------------------------------------------------

/*! Use the quit function to stop the engine and quit to the command line.
 @return No return value
 */
ConsoleFunctionWithDocs(quit, ConsoleVoid, 1, 1, ())
{
	TORQUE_UNUSED( argc );
	TORQUE_UNUSED( argv );
	Platform::postQuitMessage(0);
}

/*! quitWithErrorMessage(msg)
 - Quit, showing the provided error message. This is equivalent
 to an AssertISV.
 @param Error Message
 @return No return value
 */
ConsoleFunctionWithDocs(quitWithErrorMessage, ConsoleVoid, 2, 2, (msg string))
{
	AssertISV(false, argv[1]);
}

/*! @} */ // group PlatformFunctions
