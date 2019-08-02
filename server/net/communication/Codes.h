#ifndef H_CODES
#define H_CODES

#define CLIENT_SIGNATURE "808080"

//tokens
#define SPLIT ";+;"
#define HALT ";!;"
#define PADDED_HALT " ;!; "

//misc
#define BAD_PACKET "badpacket"
#define UPDATE_CLIENT "youneedtoupdate"

//actions - to client
#define SEND_PLAYER_ID "heresyourid"
#define SEND_PLAYER_DATA "heresdata"
#define SEND_PLAYER_COORDS "herescoords"
#define INVALID_LOGIN "invalidlogin"
#define LOGOUT_PLAYER "bye"
#define KICK_PLAYER "gtfo"
#define SHOW_PLAYER "hereis"
#define HIDE_PLAYER "hereisnt"
#define SEND_PLAYER_MAP "511"
#define SEND_INFOBOX_MSG "513"
#define PUBLIC_CHAT_FROM "515"

//actions - from client
#define LOGIN_REQUEST "logmein"
#define PLAYER_INFO_REQUEST "getmyinfo"
#define PLAYER_MOVEMENT_REQUEST "moveme"
#define PLAYER_LOGOUT_REQUEST "logmeout"
#define PLAYER_LOGIN_MAP_REQUEST "510"
#define SEND_MAP_EDIT "512"
#define SEND_PUBLIC_CHAT "514"

//actions - unsorted
#define SUCCESS_LOGIN "successlogin"
#define NEWPROFILE_LOGIN "createnewprofile"
#define PLAYER_MOVEMENT_REQUEST "moveme"

#endif
