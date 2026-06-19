\
\ Forth driver for HD44780 LCD in 8-bit mode on 8051 MCU
\
\ Data bus = P1
\ RS = P3.0
\ RW = P3.1
\ E  = P3.2

HEX
90 CONSTANT P1      \ Data bus (SFR address)
B0 CONSTANT P3      \ Control port

01 CONSTANT RS-MASK
02 CONSTANT RW-MASK
04 CONSTANT E-MASK
DECIMAL

\ Bit helpers for RS, RW, E

: SET-BIT   ( mask addr -- )  OVER C@ OR SWAP C! ;
: CLR-BIT   ( mask addr -- )  >R R@ C@ INVERT AND R> C! ;

: RS-ON     ( -- )  RS-MASK P3 SET-BIT ;
: RS-OFF    ( -- )  RS-MASK P3 CLR-BIT ;

: RW-ON     ( -- )  RW-MASK P3 SET-BIT ;
: RW-OFF    ( -- )  RW-MASK P3 CLR-BIT ;

: E-PULSE   ( -- ) E-MASK P3 SET-BIT 
    \ add small delay here if needed 
    E-MASK P3 CLR-BIT ;

\ Delay routines

: DELAY-US  ( n -- )  0 DO LOOP ;
: DELAY-MS  ( n -- )  0 DO 250 DELAY-US LOOP ;

\ Write LCD command

: LCD-CMD   ( cmd -- )
    P1 C!        \ put command on data bus
    RS-OFF       \ RS = 0 (command)
    RW-OFF       \ RW = 0 (write)
    E-PULSE
    50 DELAY-US ;   \ wait 40–50 µs

\ Write character to LCD

: LCD-CHAR  ( c -- )
    P1 C!        \ put data on bus
    RS-ON        \ RS = 1 (data)
    RW-OFF       \ RW = 0 (write)
    E-PULSE
    50 DELAY-US ;

\ LCD 8-bit init sequence

: LCD-INIT  ( -- )
    20 DELAY-MS      \ wait for LCD power-up
    38 LCD-CMD       \ function set: 8-bit, 2-line
    0C LCD-CMD       \ display ON, cursor OFF
    01 LCD-CMD       \ clear display
    02 LCD-CMD       \ return home
    06 LCD-CMD ;     \ entry mode: increment

\ Print a string stored in memory
\
\ usage: S" HELLO" LCD-TYPE

: LCD-TYPE  ( addr len -- )
    0 DO
        DUP I + C@ LCD-CHAR
    LOOP
    DROP ;

\ Move cursor to a position

: LCD-GOTO  ( col row -- )
    \ row 0 = 0x00, row 1 = 0x40
    40 * + 80 OR LCD-CMD ;

\ To Do: HOME, CLS, CURSOR-ON etc...
