//
//  codeblockUtil.h
//  Torque2D
//
//  Created by James Urquhart on 21/11/2015.
//

#ifndef _CODEBLOCKUTIL_H_
#define _CODEBLOCKUTIL_H_

namespace CodeblockUtil
{
   bool execFile(const char* filename, bool noCalls, bool journal, bool allowCompiled);
   bool compile(const char *filename);
};

#endif
