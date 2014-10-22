/**
 * TP5 - EXO_3 - Mini-Serveur de messagerie instantannée en mémoire partagée
 * Header
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @copyright (c) 2014, toumlilt
 *
 * @version 1.0
 * @package toumlilt/M1/PR
 */

#ifndef __CHAT_H__
#define __CHAT_H__

/* types de requêtes : */
#define REQ_TYPE_LOGIN 0
#define REQ_TYPE_MSG 1
#define REQ_TYPE_LOGOUT 2

/* segment requête : */
struct request {
  long type;
  char content[1024];
};

#endif
