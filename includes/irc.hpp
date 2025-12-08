#pragma once

#ifndef IRC_HPP
# define IRC_HPP

// ======================================================================
// Archivo: irc.hpp
// Propósito: Cabecera general del proyecto: incluye librerías estándar, constantes, estructuras auxiliares y códigos de respuesta IRC.
// ======================================================================

# include <iostream>
# include <iomanip>
# include <vector>
# include <map>
# include <algorithm>
# include <string>

# include <sys/socket.h>
# include <netinet/in.h>
# include <exception>
# include <unistd.h>
# include <cstring>
# include <fcntl.h>
# include <cerrno>
# include <sys/epoll.h>
# include <arpa/inet.h>
# include <iostream>
# include <csignal>
# include <cstdio>
# include <utility>
# include <sstream>

// COLORS
# define RED     "\033[91;1m"
# define GREEN   "\033[92;1m"
# define YELLOW  "\033[93;1m"
# define BLUE    "\033[94;1m"
# define PINK    "\033[95;1m"
# define CLEAR   "\033[0m"

# define MAX_BYTES_MSG  512
# define MAX_EVENTS     128
# define NICKLEN        9
# define USERLEN        9
# define REALNAMELEN    50
# define MAX_TARGETS    10
# define MAX_PARAMS     13

# define	MAX_FDS	1024

/* CONNECT SUCCESS */
# define	RPL_WELCOME 			001
# define    RPL_YOURHOST            002
# define    RPL_CREATED             003
# define    RPL_MYINFO              004
# define    RPL_ISUPPORT            005

/* PASS */
# define 	ERR_PASSWDMISMATCH		464

/* NICK */
# define	ERR_NONICKNAMEGIVEN		431
# define	ERR_ERRONEUSNICKNAME	432
# define	ERR_NICKNAMEINUSE		433

/* USER */
# define ERR_NEEDMOREPARAMS         461
# define ERR_ALREADYREGISTERED      462
# define ERR_INSERTNICKAFERPASS		463
# define ERR_INSERTUSERAFTERPASSANDNICK 465

/* JOIN */
# define ERR_NEEDMOREPARAMS			461
# define ERR_NOSUCHCHANNEL 			403
# define ERR_ALREADYONCHANNEL		443
# define ERR_TOOMANYCHANNELS		405
# define ERR_BADCHANNELKEY			475
# define ERR_BANNEDFROMCHAN			474
# define ERR_CHANNELISFULL			471
# define ERR_INVITEONLYCHAN			473
# define ERR_BADCHANMASK			476
# define RPL_TOPIC					332
# define RPL_TOPICWHOTIME			333
# define RPL_NAMREPLY				353
# define RPL_ENDOFNAMES				366

/* PART */
# define	ERR_NOTONCHANNEL		442
# define	ERR_NOSUCHCHANNEL		403
# define	ERR_USERNOTINCHANNEL	441

/* LIST */
# define	RPL_LISTSTART			321
# define	RPL_LIST				322
# define	RPL_LISTEND				323

/* TOPIC */
# define ERR_CHANOPRIVSNEEDED		482
# define RPL_NOTOPIC				331
# define RPL_TOPIC					332
# define RPL_TOPICWHOTIME			333

/* INVITE */
# define ERR_NOSUCHNICK				401
# define ERR_USERONCHANNEL			443 
# define RPL_INVITING				341

/* MODE */
# define RPL_CHANNELMODEIS			324
# define ERR_USERSDONTMATCH			502
# define ERR_UNKNOWNMODE			472
# define RPL_BANLIST				367
# define RPL_ENDOFBANLIST			368
# define RPL_UMODEIS				221

/* PRIVMSG */
# define ERR_NOSUCHNICK             401
# define ERR_NOSUCHSERVER           402
# define ERR_CANNOTSENDTOCHAN       404
# define ERR_TOOMANYTARGETS         407
# define ERR_NORECIPIENT            411
# define ERR_NOTEXTTOSEND           412
# define ERR_NOTOPLEVEL             413
# define ERR_WILDTOPLEVEL           414
# define RPL_AWAY                   301

/* COMMANDOS */
# define 	ERR_UNKNOWNCOMMAND		421

/* UTILS */
// Actualiza el atributo correspondiente.
bool	setNonBlocking(int fd);

#endif 