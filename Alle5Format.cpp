#include <map>

#include "Alle5Format.h"
#include "Log.h"

#include "FileStream.h"
#include "UTF-8/UTF-8.h"

std::map<wchar_t, wchar_t> specialCharacters = {
    {0x246D, L'♂'},
    {0x246E, L'♀'},
    {0xFFFE, L'\n'},
};

#define COMMAND_HEADER (wchar_t)0xF000
std::map<wchar_t, std::wstring> commands = {
    {0x0100, L"TRNAME"},
    {0x0101, L"PKNAME"},
    {0x0102, L"PKNICK"},
    {0x0103, L"TYPE"},
    {0x0105, L"LOCATION"},
    {0x0106, L"ABILITY"},
    {0x0107, L"MOVE"},
    {0x0108, L"ITEM1"},
    {0x0109, L"ITEM2"},
    {0x010A, L"DRESSUPPROP"},
    {0x010B, L"BOX"},
    {0x010C, L"BATTLEPK"},
    {0x010D, L"STAT"},
    {0x010E, L"TRCLASS"},
    {0x010F, L"HOBBY"},
    {0x0110, L"PASSPOWER"},
    {0x0112, L"BAGPOCKET"},
    {0x0113, L"SURVEYRESULT"},
    {0x011C, L"GENERIC"},
    {0x0122, L"DRESSUPSHOWNAME"},
    {0x0123, L"DRESSUPSHOWFEELING"},
    {0x0124, L"COUNTRY"},
    {0x0125, L"PROVINCE"},
    {0x0131, L"DRESSUPBODYPART"},
    {0x0132, L"DECORNAME"},
    {0x0133, L"DRESSUPAUDIENCE"},
    {0x0134, L"MIINAME"},
    {0x0135, L"MEDAL"},
    {0x0136, L"MEDALISTRANK"},
    {0x0137, L"JOINAVINPUT"},
    {0x013B, L"TOURNAMENT"},
    {0x013C, L"BATTLEMODE"},
    {0x013D, L"INSTTITLE"},
    {0x013E, L"WEATHER"},
    {0x013F, L"MOVIENAME"},
    {0x0140, L"FUNFESTMISSION"},
    {0x0142, L"JOINAVRANK"},
    {0x0143, L"ENTRALINKLVL"},
    {0x0189, L"TRNICK"},
    {0x018A, L"1stchrTR"},
    {0x018B, L"SHOUTOUT"},
    {0x018E, L"BERRY"},
    {0x018F, L"REMFEEL"},
    {0x0190, L"REMQUAL"},
    {0x0191, L"WEBSITE"},
    {0x019C, L"CHOICECOS"},
    {0x01A1, L"GSYNCID"},
    {0x0192, L"PRVIDSAY"},
    {0x0193, L"BTLTEST"},
    {0x0195, L"GENLOC"},
    {0x0199, L"CHOICEFOOD"},
    {0x019A, L"HOTELITEM"},
    {0x019B, L"TAXISTOP"},
    {0x019F, L"MAISTITLE"},
    {0x1000, L"ITEMPLUR0"},
    {0x1001, L"ITEMPLUR1"},
    {0x1100, L"GENDBR"},
    {0x1101, L"NUMBRNCH"},
    {0x1302, L"iCOLOR2"},
    {0x1303, L"iCOLOR3"},
    {0x0200, L"NUM1"},
    {0x0201, L"NUM2"},
    {0x0202, L"NUM3"},
    {0x0203, L"NUM4"},
    {0x0204, L"NUM5"},
    {0x0205, L"NUM6"},
    {0x0206, L"NUM7"},
    {0x0207, L"NUM8"},
    {0x0208, L"NUM9"},
    {0xBD00, L"COLOREX"},
    {0xBD01, L"COLORRESET"},
    {0xBD02, L"CENTER"},
    {0xBD03, L"RIGHT"},
    {0xBD04, L"SKIPPIXELS"},
    {0xBD05, L"SETXPOS"},
    {0xBDFF, L"BLANK"},
    {0xBE00, L"SCROLL"},
    {0xBE01, L"CLEAR"},
    {0xBE02, L"WAIT"},
    {0xBE09, L"SPEED"},
    {0xFF00, L"COLOR"},
};

wchar_t EncryptCharacter(wchar_t character, u16& key)
{
    character = key ^ character;
    key = ((key << 3) | (key >> 13)) & 0xFFFF;
    return character;
}
wchar_t DecryptCharacter(wchar_t character, u16& key)
{
    character = character ^ key;
    key = ((key << 3) | (key >> 13)) & 0xFFFF;
    return character;
}

bool LoadAlle5File(const std::string& path, std::vector<std::string>& lines, bool strict)
{
    FileStream fileStream;
    if (!LoadFileStream(fileStream, path))
        return false;

    u16 textSections = FileStreamRead<u16>(fileStream, 0);
    u16 lineCount = FileStreamRead<u16>(fileStream, 2);
    if (lineCount == 0)
    {
        Log("WARNING: Empty text file (%s)", path.c_str());
        ReleaseFileStream(fileStream);
        return false;
    }

    u32 totalLength = FileStreamRead<u32>(fileStream, 4);
    u32 initialKey = FileStreamRead<u32>(fileStream, 8);
    u32 sectionData = FileStreamRead<u32>(fileStream, 12);

    // Some sanity checking to prevent errors.
    if (initialKey != 0)
    {
        Log("WARNING: Invalid initial key in text file (%s)", path.c_str());
        ReleaseFileStream(fileStream);
        return false;
    }
    if (strict && 
        (sectionData + totalLength != fileStream.length || textSections != 1))
    {
        Log("WARNING: Invalid text file (%s), could be a CTRMap error disable Strict Mode to continue", path.c_str());
        return false;
    }

    u32 sectionLength = FileStreamRead<u32>(fileStream, sectionData);
    if (sectionLength != totalLength)
    {
        Log("WARNING: Section size and overall size do not match in text file (%s)", path.c_str());
        ReleaseFileStream(fileStream);
        return false;
    }

    lines.clear();
    lines.reserve(lineCount);

    // Start key
    u16 key = 0x7C89;

    for (u16 lineIdx = 0; lineIdx < lineCount; ++lineIdx)
    {
        std::wstring line;
        u16 lineKey = key;

        u32 offset = FileStreamRead<u32>(fileStream, (lineIdx * 8) + sectionData + 4) + sectionData;
        u32 length = FileStreamRead<u32>(fileStream, (lineIdx * 8) + sectionData + 8);

        line.reserve(length);

        u32 start = offset;
        while (offset < start + length * 2) // loop through the entire text line
        {
            wchar_t character = DecryptCharacter(FileStreamReadUpdate<wchar_t>(fileStream, offset), lineKey);
            if (character == 0xFFFF)
                break;

            std::map<wchar_t, wchar_t>::iterator itr = specialCharacters.find(character);
            if (itr != specialCharacters.end())
            {
                line += itr->second;
                continue;
            }

            if (character == COMMAND_HEADER)
            {
                line += L'[';

                u16 type = (u16)DecryptCharacter(FileStreamReadUpdate<wchar_t>(fileStream, offset), lineKey);
                u16 paramCount = (u16)DecryptCharacter(FileStreamReadUpdate<wchar_t>(fileStream, offset), lineKey);

                std::map<wchar_t, std::wstring>::iterator cmdItr = commands.find(type);
                if (cmdItr != commands.end())
                {
                    line += cmdItr->second;
                    for (u8 paramIdx = 0; paramIdx < paramCount; ++paramIdx)
                    {
                        u16 param = (u16)DecryptCharacter(FileStreamReadUpdate<wchar_t>(fileStream, offset), lineKey);
                        line += L'(' + std::to_wstring(param) + L')';
                    }
                }

                line += L']';
                continue;
            }

            line += character; 
        }
        lines.emplace_back(WideToUtf8(line));

        // Set key for next line
        key += 0x2983;
    }

    ReleaseFileStream(fileStream);
    return true;
}

std::wstring MakeWideLine(std::string line)
{
    std::wstring output = Utf8ToWide(line);
    output.push_back((wchar_t)0xFFFF);
    return output;
}

bool SaveAlle5File(const std::string& path, const std::vector<std::string>& lines)
{
    FileStream fileStream;
    if (!LoadEmptyFileStream(fileStream))
        return false;

    // Set the header
    FileStreamPutBack<u16>(fileStream, 1);
    FileStreamPutBack<u16>(fileStream, (u16)lines.size());
    FileStreamPutBack<u32>(fileStream, ~0); // temporary value
    FileStreamPutBack<u32>(fileStream, 0);
    FileStreamPutBack<u32>(fileStream, 16);
    // Set the section data
    FileStreamPutBack<u32>(fileStream, ~0); // temporary value

    // Set the line data
    for (u16 lineIdx = 0; lineIdx < lines.size(); ++lineIdx)
    {
        // temporary values
        FileStreamPutBack<u32>(fileStream, ~0); // offset
        FileStreamPutBack<u32>(fileStream, ~0); // length
    }

    u16 key = 0x7C89;
    u32 offset = fileStream.length - 16;

    // Set the character data
    for (u16 lineIdx = 0; lineIdx < lines.size(); ++lineIdx)
    {
        // Update the line data with the actual offset
        FileStreamReplace<u32>(fileStream, (lineIdx * 8) + 16 + 4, offset);

        std::wstring line = MakeWideLine(lines[lineIdx]);
        u16 lineKey = key;

        u32 length = 0;
        for (u32 characterIdx = 0; characterIdx < line.length(); ++characterIdx)
        {
            wchar_t character = line[characterIdx];

            // Check if there is a command
            if (character == L'[')
            {
                std::wstring command;
                std::vector<u16> params;
                // Find the characters inside the command
                for (++characterIdx; characterIdx < line.length(); ++characterIdx)
                {
                    // Stop the loop if the command ended
                    if (line[characterIdx] == L']')
                        break;

                    // Check if there is a command parameter
                    if (line[characterIdx] == L'(')
                    {
                        std::wstring param;
                        for (++characterIdx; characterIdx < line.length(); ++characterIdx)
                        {
                            // Stop the loop if the command parameter ended
                            if (line[characterIdx] == L')')
                            {
                                // Advance by 1 so that the parenthesis does 
                                // not get added to the command string
                                ++characterIdx;
                                break;
                            }
                                
                            param += line[characterIdx];
                        }

                        params.push_back((u16)std::stoi(param));
                        break;
                    }

                    command += line[characterIdx];
                }

                for (std::map<wchar_t, std::wstring>::iterator itr = commands.begin();
                    itr != commands.end(); ++itr)
                {
                    if (itr->second == command)
                    {
                        FileStreamPutBack<u16>(fileStream, EncryptCharacter(COMMAND_HEADER, lineKey));
                        FileStreamPutBack<u16>(fileStream, EncryptCharacter(itr->first, lineKey));
                        FileStreamPutBack<u16>(fileStream, EncryptCharacter((wchar_t)params.size(), lineKey));

                        offset += 6;
                        length += 3;

                        for (u8 paramIdx = 0; paramIdx < params.size(); ++paramIdx)
                        {
                            FileStreamPutBack<u16>(fileStream, EncryptCharacter(params[paramIdx], lineKey));

                            offset += 2;
                            ++length;
                        }
                        break;
                    }
                }

                continue;
            }

            for (std::map<wchar_t, wchar_t>::iterator itr = specialCharacters.begin();
                itr != specialCharacters.end(); ++itr)
            {
                if (itr->second == character)
                {
                    character = itr->first;
                    break;
                }
            }

            FileStreamPutBack<u16>(fileStream, EncryptCharacter(character, lineKey));

            offset += 2;
            ++length;
        }

        // Update the line data with the actual length
        FileStreamReplace<u32>(fileStream, (lineIdx * 8) + 16 + 8, length);

        // Increment the key for the next line
        key += 0x2983;
    }

    // Update the header with the actual section lenght
    FileStreamReplace<u32>(fileStream, 4, fileStream.length - 16);
    FileStreamReplace<u32>(fileStream, 16, fileStream.length - 16);

    SaveFileStream(fileStream, path);

    ReleaseFileStream(fileStream);
    return true;
}