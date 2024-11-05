#ifndef STASSID
#define STASSID "wrtManMini"
#define STAPSK "1231231232023"
#endif


const char *ssid = STASSID;
const char *passPhrase = STAPSK;
String newHostName = "wemos";
String newDnsName = "wemos";
const char *passwordOTA = "admin";

#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/2"
// Это указание говорит, что стандартное поясное время имеет аббревиатуру CET
// и оно смещено на 2 часа вперёд (к востоку) от UTC;
// Летнее время имеет аббревиатуру CEST и смещено вперед от UTC на три часа (это определяется неявно).
// Часы переводятся на летнее время в последнее воскресенье марта в 2:00 CET,
// а на зимнее — в последнее воскресенье октября в 3:00 CEST.
