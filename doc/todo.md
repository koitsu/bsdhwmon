# Short-term

- Consider making VERBOSE a macro
  - Example: `#define VERBOSE(fmt, args...) if (f_verbose) { printf("==> " fmt, ## args); }`
  - Pros: exposes printf() formatting mistakes (ex. missing formatters) in
    code at compile-time; vprintf() doesn't do a good job with this, and
    compilers (both gcc and clang) don't catch all cases; proof is in
    commit e776b2c
  - Cons: requires `f_verbose` be declared as extern in every file using
    VERBOSE(), and binary size grows (lots more assembly generated for if()
    and printf(), vs. having a real function that does it)

# Long-term

- Extend structs to support custom mathematics operations; for
  example, some boards might calculate +5V differently than others,
  depending upon the value of the resistor tied to that pin
- Replace the gigantic database of motherboards and output params with
  a text file, so that users can modify and change the output strings
  to whever they wish.  This would also decrease the program/binary
  size
- Get in contact with Tyan regarding the Thunder LE-T (S2518), which
  appears to use the Winbond W83782D, and see if they'll provide SMBus
  documentation (if it supports tie-ins).  Note: Tyan Support does not
  appear to provide information for EoL'd boards, as demonstrated in
  https://github.com/koitsu/bsdhwmon/issues/2#issuecomment-141079566

