#include <ctype.h>
#include <Carbon/Carbon.h>

/**
 * Converts any lowercase letters in a Pascal string to their uppercase
 * equivalents, using the Macintosh file system rules.
 *
 * @remark Only a subset of the Roman character set (character codes with values
 * through $D8) are converted. Use this function to emulate the behavior of the
 * Macintosh file system.
 *
 * @param theString
 *      On input, this is the Pascal string to be converted. On output, this
 *      contains the string resulting from the conversion. UpperString traverses
 *      the characters in theString and converts any lowercase characters with
 *      character codes in the range 0x00 through 0xD8 into their uppercase
 *      equivalents. UpperString places the converted characters in theString.
 *
 * @param diacSensitive
 *      A flag that indicates whether the case conversion is to strip
 *      diacritical marks.
 *      If the value of this parameter is TRUE, diacritical marks are considered
 *      in the conversion; if it is FALSE, any diacritical marks are stripped.
 */
void
UpperString(
        Str255    theString,
        Boolean   diacSensitive)
{
    // HACK: Ignores diacritics
    uint8_t len = theString[0];
    uint8_t *ptr = &theString[1];
    uint8_t *end = ptr + len;
    for (; ptr < end; ptr++) {
        *ptr = toupper(*ptr);
    }
}