#line 1 "C:\MCC18\h\string.h"
#line 1 "C:\MCC18\h\string.h"

#line 3 "C:\MCC18\h\string.h"



#line 7 "C:\MCC18\h\string.h"


#line 20 "C:\MCC18\h\string.h"
 
#line 22 "C:\MCC18\h\string.h"


#line 25 "C:\MCC18\h\string.h"
 
#line 27 "C:\MCC18\h\string.h"

 

#line 39 "C:\MCC18\h\string.h"
 
void *memcpy (auto void *s1, auto const void *s2, auto size_t n);


#line 55 "C:\MCC18\h\string.h"
 
void *memmove (auto void *s1, auto const void *s2, auto size_t n);


#line 67 "C:\MCC18\h\string.h"
 
char *strcpy (auto char *s1, auto const char *s2);


#line 83 "C:\MCC18\h\string.h"
 
char *strncpy (auto char *s1, auto const char *s2, auto size_t n);


#line 97 "C:\MCC18\h\string.h"
 
char *strcat (auto char *s1, auto const char *s2);


#line 113 "C:\MCC18\h\string.h"
 
char *strncat (auto char *s1, auto const char *s2, auto size_t n);


#line 128 "C:\MCC18\h\string.h"
 
signed char memcmp (auto const void *s1, auto const void *s2, auto size_t n);


#line 141 "C:\MCC18\h\string.h"
 
signed char strcmp (auto const char *s1, auto const char *s2);


#line 147 "C:\MCC18\h\string.h"
 


#line 161 "C:\MCC18\h\string.h"
 
signed char strncmp (auto const char *s1, auto const char *s2, auto size_t n);


#line 167 "C:\MCC18\h\string.h"
 


#line 183 "C:\MCC18\h\string.h"
 
void *memchr (auto const void *s, auto unsigned char c, auto size_t n);


#line 199 "C:\MCC18\h\string.h"
 
char *strchr (auto const char *s, auto unsigned char c);


#line 210 "C:\MCC18\h\string.h"
 
size_t strcspn (auto const char *s1, auto const char *s2);


#line 222 "C:\MCC18\h\string.h"
 
char *strpbrk (auto const char *s1, auto const char *s2);


#line 238 "C:\MCC18\h\string.h"
 
char *strrchr (auto const char *s, auto unsigned char c);


#line 249 "C:\MCC18\h\string.h"
 
size_t strspn (auto const char *s1, auto const char *s2);


#line 262 "C:\MCC18\h\string.h"
 
char *strstr (auto const char *s1, auto const char *s2);


#line 305 "C:\MCC18\h\string.h"
 
char *strtok (auto char *s1, auto const char *s2);


#line 321 "C:\MCC18\h\string.h"
 
void *memset (auto void *s, auto unsigned char c, auto size_t n);


#line 339 "C:\MCC18\h\string.h"
 
#line 341 "C:\MCC18\h\string.h"


#line 349 "C:\MCC18\h\string.h"
 
size_t strlen (auto const char *s);


#line 358 "C:\MCC18\h\string.h"
 
char *strupr (auto char *s);


#line 367 "C:\MCC18\h\string.h"
 
char *strlwr (auto char *s);



 

#line 379 "C:\MCC18\h\string.h"
 
far  rom void *memcpypgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 389 "C:\MCC18\h\string.h"
 
void *memcpypgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 398 "C:\MCC18\h\string.h"
 
far  rom void *memcpyram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 407 "C:\MCC18\h\string.h"
 
far  rom void *memmovepgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 417 "C:\MCC18\h\string.h"
 
void *memmovepgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 426 "C:\MCC18\h\string.h"
 
far  rom void *memmoveram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 434 "C:\MCC18\h\string.h"
 
far  rom char *strcpypgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 443 "C:\MCC18\h\string.h"
 
char *strcpypgm2ram (auto char *s1, auto const far  rom char *s2);


#line 451 "C:\MCC18\h\string.h"
 
far  rom char *strcpyram2pgm (auto far  rom char *s1, auto const char *s2);


#line 460 "C:\MCC18\h\string.h"
 
far  rom char *strncpypgm (auto far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 470 "C:\MCC18\h\string.h"
 
char *strncpypgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 479 "C:\MCC18\h\string.h"
 
far  rom char *strncpyram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 487 "C:\MCC18\h\string.h"
 
far  rom char *strcatpgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 496 "C:\MCC18\h\string.h"
 
char *strcatpgm2ram (auto char *s1, auto const far  rom char *s2);


#line 504 "C:\MCC18\h\string.h"
 
far  rom char *strcatram2pgm (auto far  rom char *s1, auto const char *s2);


#line 513 "C:\MCC18\h\string.h"
 
far  rom char *strncatpgm (auto far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 523 "C:\MCC18\h\string.h"
 
char *strncatpgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 532 "C:\MCC18\h\string.h"
 
far  rom char *strncatram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 541 "C:\MCC18\h\string.h"
 
signed char memcmppgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 551 "C:\MCC18\h\string.h"
 
signed char memcmppgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 560 "C:\MCC18\h\string.h"
 
signed char memcmpram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 568 "C:\MCC18\h\string.h"
 
signed char strcmppgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 577 "C:\MCC18\h\string.h"
 
signed char strcmppgm2ram (auto const char *s1, auto const far  rom char *s2);


#line 585 "C:\MCC18\h\string.h"
 
signed char strcmpram2pgm (auto const far  rom char *s1, auto const char *s2);


#line 594 "C:\MCC18\h\string.h"
 
signed char strncmppgm (auto const far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 604 "C:\MCC18\h\string.h"
 
signed char strncmppgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 613 "C:\MCC18\h\string.h"
 
signed char strncmpram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 622 "C:\MCC18\h\string.h"
 
far  rom char *memchrpgm (auto const far  rom char *s, auto const unsigned char c, auto sizerom_t n);


#line 631 "C:\MCC18\h\string.h"
 
far  rom char *strchrpgm (auto const far  rom char *s, auto unsigned char c);


#line 639 "C:\MCC18\h\string.h"
 
sizerom_t strcspnpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 647 "C:\MCC18\h\string.h"
 
sizerom_t strcspnpgmram (auto const far  rom char *s1, auto const char *s2);


#line 655 "C:\MCC18\h\string.h"
 
sizeram_t strcspnrampgm (auto const char *s1, auto const far  rom char *s2);


#line 663 "C:\MCC18\h\string.h"
 
far  rom char *strpbrkpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 671 "C:\MCC18\h\string.h"
 
far  rom char *strpbrkpgmram (auto const far  rom char *s1, auto const char *s2);


#line 679 "C:\MCC18\h\string.h"
 
char *strpbrkrampgm (auto const char *s1, auto const far  rom char *s2);


#line 688 "C:\MCC18\h\string.h"
 
 


#line 696 "C:\MCC18\h\string.h"
 
sizerom_t strspnpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 704 "C:\MCC18\h\string.h"
 
sizerom_t strspnpgmram (auto const far  rom char *s1, auto const char *s2);


#line 712 "C:\MCC18\h\string.h"
 
sizeram_t strspnrampgm (auto const char *s1, auto const far  rom char *s2);


#line 720 "C:\MCC18\h\string.h"
 
far  rom char *strstrpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 729 "C:\MCC18\h\string.h"
 
far  rom char *strstrpgmram (auto const far  rom char *s1, auto const char *s2);


#line 737 "C:\MCC18\h\string.h"
 
char *strstrrampgm (auto const char *s1, auto const far  rom char *s2);


#line 745 "C:\MCC18\h\string.h"
 
far  rom char *strtokpgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 754 "C:\MCC18\h\string.h"
 
char *strtokpgmram (auto char *s1, auto const far  rom char *s2);


#line 762 "C:\MCC18\h\string.h"
 
far  rom char *strtokrampgm (auto far  rom char *s1, auto const char *s2);


#line 771 "C:\MCC18\h\string.h"
 
far  rom void *memsetpgm (auto far  rom void *s, auto unsigned char c, auto sizerom_t n);


#line 778 "C:\MCC18\h\string.h"
 
far  rom char *struprpgm (auto far  rom char *s);


#line 785 "C:\MCC18\h\string.h"
 
far  rom char *strlwrpgm (auto far  rom char *s);


#line 792 "C:\MCC18\h\string.h"
 
sizerom_t strlenpgm (auto const far  rom char *s);

#line 796 "C:\MCC18\h\string.h"

#line 798 "C:\MCC18\h\string.h"

#line 805 "C:\MCC18\h\string.h"
#line 814 "C:\MCC18\h\string.h"

#line 816 "C:\MCC18\h\string.h"
