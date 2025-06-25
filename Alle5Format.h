#ifndef _ALLE5_FORMAT_H
#define _ALLE5_FORMAT_H

#include "defs.h"

// The text narc file code is adapted to C++ from Kaphotics xytext application 
// https://github.com/kwsch/xytext/blob/master/xytext/Form1.cs#L110
// Command documentation is taken from Hello007s CTRMapV plugin
// https://github.com/ds-pokemon-hacking/CTRMapV/blob/master/src/ctrmap/formats/pokemon/text/GenVTextVariableCode.java

bool LoadAlle5File(const std::string& path, std::vector<std::string>& lines, bool strict = false);

bool SaveAlle5File(const std::string& path, const std::vector<std::string>& lines);

#endif // _ALLE5_FORMAT_H

