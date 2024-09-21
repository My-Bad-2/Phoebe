#ifndef LIBS_STYLING_H
#define LIBS_STYLING_H

#define RESET_TEXT "\033[0m"
#define BOLD_TEXT "\033[1m"
#define FAINT_TEXT "\033[2m"
#define ITALIC_TEXT "\033[3m"
#define STRIKE_OFF_TEXT "\033[9m"

#define TEXT_COLOR(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"
#define BACKGROUND_COLOR(r, g, b) "\033[48;2;" #r ";" #g ";" #b "m"

#endif // LIBS_STYLING_H