#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "public.h"


int network_init(int *sfd);
int recv_data_from_server(int sfd,struct mess *data_p,unsigned short num);
int send_data_to_server(int sfd,char type,unsigned short num);

#endif