#include <string>

using namespace std;

string str_replace_all(string str, string to_replace, string replace_with, char ignore_following) {

  // Stores the position of to_replace within the string.
  unsigned int pos = 0;

  // Replace all to_replace with replace_with
  while (pos != string::npos) {

    // Find the replacement candidates within the string
    pos = str.find(to_replace, pos);

    // If we have a match... and it doesn't have
    // ignore_following immediately before it...
    if (pos != string::npos &&
       (pos > 0 && str[pos - 1] != ignore_following)) {

      // Replace it!
      str.replace(pos, to_replace.length(), replace_with);

      // Increment the current position by how many extra
      // characters were added in the replace, plus one so
      // we don't get any one-char infinite loops.
      pos += replace_with.length() - to_replace.length() + 1;

    }

  }

  return str;

}